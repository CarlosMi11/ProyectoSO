#include "procesador.h"


long MAR = 0;
long MDR = 0;
long  IR = 0;
long  RB = 0;
long  RL = 0;
long  RX = 0;
long  SP = 0;
long PSW = 0;
long  AC = 0;

pthread_mutex_t mutex_cpu = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_start = PTHREAD_COND_INITIALIZER; 
pthread_cond_t cond_end   = PTHREAD_COND_INITIALIZER;

int estadoCPU;
flag valorFinalizacion;
flag finalizo;



palabra lectura(int pos, int *flag){
	MEM_LOCK;
	palabra leido;
	*flag = pmrd(pos + RB, &leido, PSW, RB, RL);
	MEM_UNLOCK;
	return leido;
}

void escritura(int pos, const palabra value, int *flag){
	MEM_LOCK;
	*flag = pmwr(pos + RB, value, PSW, RB, RL);
	MEM_UNLOCK;
}

void fetch(){
	
	int PC = getPC();

	MAR = PC;

	pmrd(MAR, &MDR, 1000000, 0, 2000);	//modo admin xd

	PC += 1;
	setPC(PC);
	IR = MDR;
}

Instruccion decode(){
	Instruccion i;
	i.val = IR % 100000;
	if ((i.val/10000)==1) i.val = (i.val%10000)*(-1);
	i.opcode = IR / 1000000;
	i.dir = (IR / 100000) % 10;
	return i;
}

flag MemoryAccess(Instruccion *i){
    int valor = 0;
    flag flagLectura = SUCCESS;


	if(i->dir == 0){
		valor = lectura(i->val, &flagLectura);
	}
	if(i->dir==1){
		valor = i->val;
	}
	if(i->dir==2){
		valor = lectura((i->val + AC), &flagLectura);
	}

    i->val = valor;

    return flagLectura;
}

flag execute (Instruccion i){
	
	int opcode, val;
    opcode = i.opcode;
    val = i.val;

	palabra valsp;

	flag flagMemoria = SUCCESS;
	flag flagInstruccion = SUCCESS;

	switch (opcode){
		case 0: // SUM
			AC = AC + val;

			if (AC==0) setCondCode(0);
			if (AC<0)  setCondCode(1);
			if (AC>0)  setCondCode(2);
			if (AC > MAX_NUMBER || AC < MIN_NUMBER) {
                setCondCode(3); 
                if (AC > MAX_NUMBER) {
                    AC = MIN_NUMBER + (AC - MAX_NUMBER - 1);
                } 
                else if (AC < MIN_NUMBER) {
                    AC = MAX_NUMBER + (AC - MIN_NUMBER + 1);
                }
            }
			break;
            
		case 1: // RES
			AC = AC - val;

			if (AC==0) setCondCode(0);
			if (AC<0)  setCondCode(1);
			if (AC>0)  setCondCode(2);
			if (AC > MAX_NUMBER || AC < MIN_NUMBER) {
                setCondCode(3); 
                if (AC > MAX_NUMBER) {
                    AC = MIN_NUMBER + (AC - MAX_NUMBER - 1);
                } 
                else if (AC < MIN_NUMBER) {
                    AC = MAX_NUMBER + (AC - MIN_NUMBER + 1);
                }
            }
			break;
		case 2: // MULT
			AC = AC * val;

			if (AC==0) setCondCode(0);
			if (AC<0)  setCondCode(1);
			if (AC>0)  setCondCode(2);
			if (AC > MAX_NUMBER || AC < MIN_NUMBER) {
                setCondCode(3); 

                long relativo = (AC - MIN_NUMBER) % (MAX_NUMBER - MIN_NUMBER + 1);

                if (relativo < 0) {
                    relativo += (MAX_NUMBER - MIN_NUMBER + 1); // Ajuste para restos negativos en C
                }

                AC = (int)(MIN_NUMBER + relativo);
            }
			break;
		case 3: // DIVI
			
            if (val == 0) {
                
                log_("CPU", "Error: Division por cero");
				flagInstruccion = FAIL;
				break;
            }
        
           
            AC = AC / val;
         
            if (AC == 0)      setCondCode(0);
            else if (AC < 0)  setCondCode(1);
            else              setCondCode(2);

			break;	

		case 4: // LOAD
			AC = val;
			break;	
		case 5: // STR
			escritura(val, AC, &flagMemoria);
			break;	
		case 6: // LOADRX
            AC = RX;
            break;  
        case 7: // STRRX
            RX = AC; 
            break;	
		case 8: // COMP
			if (AC==val) setCondCode(0);
			if (AC<val)  setCondCode(1);
			if (AC>val)  setCondCode(2);
			break;
		case 9: // JMPE			
			valsp = lectura(SP, &flagMemoria);
            if(flagMemoria == SUCCESS){
                if (AC == valsp) setPC(val);
            }
			break;
		case 10: // JMPNE
			valsp = lectura(SP, &flagMemoria);
            if(flagMemoria == SUCCESS){
                if (AC != valsp) setPC(val);
            }
			break;
		case 11: // JMPLT
			valsp = lectura(SP, &flagMemoria);
            if(flagMemoria == SUCCESS){
                if (AC < valsp) setPC(val);
            }
			break;	
		case 12: // JMPGT
			valsp = lectura(SP, &flagMemoria);
            if(flagMemoria == SUCCESS){
                if (AC > valsp) setPC(val);
            }
			break;		
		case 13: // SVC
			genInterr(2);
			break;	
		case 14: // RETRN
            //asumo que en SP esta la direccion a donde se debe retornar
            setPC(lectura(SP, &flagMemoria));
			break;	
		case 15: //HAB
            setInterruptions(1);
			break;	
		case 16: //DHAB
            setInterruptions(0);
			break;	
		case 17: //TTI
			flagMemoria = setReloj(val);
			break;	
		case 18: // CHMOD
            if(val != 0 && val != 1){

                
				flagInstruccion = FAIL;
				break;
                
            }
            setOpMode(val);
			break;																				
		case 19: // LOADRB: cargar RB en AC
			AC = RB;
			break;	
		case 20: // STRRB
			RB = AC;
			break;	
		case 21: // LOADRL
			AC = RL;
			break;	
		case 22: // STRRL
			RL = AC;
			break;	
		case 23: // LOADSP
			AC = SP;
			break;	
		case 24: // STRSP
			SP = AC;
			break;	
		case 25: //PSH
            SP+=1;
            escritura(SP, AC, &flagMemoria);
			break;	
		case 26: //POP
            AC = lectura(SP, &flagMemoria);
            SP -= 1;
			break;	
		case 27: // J
			setPC(val);
			break;	
		case 28: //SDMAP
            set_pista(val);
			break;	
		case 29: //SDMAC
            set_cilindro(val);
			break;	
		case 30: //SDMAS
            set_sector(val);
			break;	
		case 31: //SDMAIO
            set_io(val);
			break;	
		case 32: //SDMAM
            set_posmem(val);
			break;	
		case 33: //SDMAON
            DMAON();
			break;																		
		default:
			flagInstruccion = FAIL;
			break;
	}

	flag ret = SUCCESS;
	if(flagInstruccion == FAIL){
		genInterr(5);
		ret = FAIL;
	}
	if(flagMemoria == FAIL){
		genInterr(6);
		ret = FAIL;
	}

	return ret;
	
}


flag CicloInstruccion(){
	
	fetch(); 

	if(IR == -1){
		
		pthread_mutex_lock(&mutex_cpu);
		estadoCPU = CPU_ESPERA;
        pthread_cond_signal(&cond_end); 
        pthread_mutex_unlock(&mutex_cpu);
		finalizo = 1;
		valorFinalizacion = SUCCESS; // unico lugar donde se coloca el valor de finalizacion en SUCCESS
		return SUCCESS;
	}
	
    Instruccion instr = decode();

	flag estado_memoria =  MemoryAccess(&instr);
    if (estado_memoria == FAIL) {
        
        genInterr(6);
        return FAIL;
    }
    
    execute(instr);
	return SUCCESS;
}
void * procesador(void* parametros){
	
    while(estadoCPU != CPU_APAGAR) {
        
		pthread_mutex_lock(&mutex_cpu);

        while (estadoCPU == CPU_ESPERA) { 
            pthread_cond_wait(&cond_start, &mutex_cpu);
        }

		if (estadoCPU == CPU_APAGAR) {
            pthread_mutex_unlock(&mutex_cpu);
            break; 
        }

        pthread_mutex_unlock(&mutex_cpu);
		limpiarInterrupciones();
		valorFinalizacion = FAIL;
		
		while(estadoCPU != CPU_ESPERA && estadoCPU != CPU_APAGAR){

			
			CicloInstruccion();
			
			tick(); 

			if (getInterruptions()) {
				if(manInterr()){
					pthread_mutex_lock(&mutex_cpu);
    				estadoCPU = CPU_ESPERA; 
    				pthread_cond_signal(&cond_end); 
    				pthread_mutex_unlock(&mutex_cpu);
					valorFinalizacion = FAIL;
					finalizo = 1;
				} // finalizar programa 
			}
			
			if (estadoCPU == CPU_EJECUTAR_DEBUG) {

				/*
				cuando entra aca, el cpu entra en modo espera y devuelve el control a la consola.
				La consola debe ejecutar el menu de debugging y volver a activar el proceso.
				*/
                pthread_mutex_lock(&mutex_cpu);
                
                estadoCPU = CPU_ESPERA; 
                pthread_cond_signal(&cond_end); 
                
                pthread_mutex_unlock(&mutex_cpu);
			}
		}

    }
    return NULL;
}

void activarProcesador(int modo){
	pthread_mutex_lock(&mutex_cpu);

    estadoCPU = modo; 

    pthread_cond_signal(&cond_start);

	
    while (estadoCPU != CPU_ESPERA && estadoCPU != CPU_APAGAR) {
        pthread_cond_wait(&cond_end, &mutex_cpu);
    }
    
    pthread_mutex_unlock(&mutex_cpu);
}

