#include "procesador.h"

#include "queue.h"

#define CPU_ESPERA   0  
#define CPU_EJECUTAR_NORMAL   1  
#define CPU_EJECUTAR_DEBUG  2  
#define CPU_APAGAR   4  

long MAR = 0;
long MDR = 0;
long  IR = 0;
long  RB = 0;
long  RL = 0;
long  RX = 0;
long  SP = 0;
long PSW = 0;
long  AC = 0;
int PROC_IND = 0;
solicitudIO IO_DATA;
static pthread_mutex_t mutex_cpu = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond_start = PTHREAD_COND_INITIALIZER; 
static pthread_cond_t cond_end   = PTHREAD_COND_INITIALIZER;

static pthread_t ID_PROC;
static int estadoCPU;

flag valorFinalizacion;
flag finalizo;

static string mensaje;

void fetch(){
	
	int PC = getPC();

	MAR = PC;
	
	
	int flag;
	MDR = lectura(MAR, &flag);
	snprintf(mensaje,200, "FETCH: PC=%i, MAR=%li, MDR=%li", PC, MAR, MDR);
    log_("procesador", mensaje);
	PC += 1;
	setPC(PC);
	IR = MDR;

	
}

Instruccion decode(){
	snprintf(mensaje,200, "decodificando IR=%ld", IR);
    log_("procesador", mensaje);

	Instruccion i;
	
	i.val = IR % 100000;
	if ((i.val/10000)==1) i.val = (i.val%10000)*(-1);
	i.opcode = IR / 1000000;
	i.dir = (IR / 100000) % 10;


	snprintf(mensaje,200, "opcode=%i, dir=%i, valor=%i", i.opcode, i.dir, i.val);
	log_("procesador", mensaje);
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
	snprintf(mensaje,200, "valor %i cargado de forma: %s", 
		i->val, 
		(i->dir == 0 ? ("directo") : (i->dir == 1 ? "inmediato" : "indexado")));
    log_("procesador", mensaje);
    return flagLectura;
}

flag execute (Instruccion i){
	
	int opcode, val;
    opcode = i.opcode;
    val = i.val;

	palabra valsp;

	snprintf(mensaje,200, "ejecutando instrucción %i con val=%i", opcode, val);
    log_("procesador", mensaje);

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
					genInterr(OVERFLOW);
                } 
                else if (AC < MIN_NUMBER) {
                    AC = MAX_NUMBER + (AC - MIN_NUMBER + 1);
					genInterr(UNDERFLOW);
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
					genInterr(OVERFLOW);
                } 
                else if (AC < MIN_NUMBER) {
                    AC = MAX_NUMBER + (AC - MIN_NUMBER + 1);
					genInterr(UNDERFLOW);
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
				if (AC > MAX_NUMBER) {
                    genInterr(OVERFLOW);
                } 
                else if (AC < MIN_NUMBER) {
                    genInterr(UNDERFLOW);
                }
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
                if (AC == valsp) jmp(val);
            }
			break;
		case 10: // JMPNE
			valsp = lectura(SP, &flagMemoria);
            if(flagMemoria == SUCCESS){
                if (AC != valsp) jmp(val);
            }
			break;
		case 11: // JMPLT
			valsp = lectura(SP, &flagMemoria);
            if(flagMemoria == SUCCESS){
                if (AC < valsp) jmp(val);
            }
			snprintf(mensaje, 200, "jmplt con: %li < %li", AC, valsp);
    		log_("procesador", mensaje);
			
			break;	
		case 12: // JMPGT
			valsp = lectura(SP, &flagMemoria);
            if(flagMemoria == SUCCESS){
                if (AC > valsp) jmp(val);
            }
			break;		
		case 13: // SVC
			if(AC < 1 || AC > 4){
				genInterr(LLAMADAINVALIDA);
				break;
			}
			genInterr(LLAMADASISTEMA);
			break;	
		case 14: // RETRN
            //asumo que en SP esta la direccion a donde se debe retornar
			if(SP - 1 < RX){ // si intenta hacer pop de la base de la pila, no puede
				flagMemoria = FAIL;
				break;
			}
            setPC(lectura(SP, &flagMemoria));
            SP -= 1;
            
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
            flagMemoria = push(AC);
			break;	
		case 26: //POP
			flagMemoria = pop(&AC);
			break;	
		case 27: // J
			jmp(val);
			break;	
		case 28: //SDMAP
            IO_DATA.pista = val;
			break;	
		case 29: //SDMAC
            IO_DATA.cilindro = val;
			break;	
		case 30: //SDMAS
            IO_DATA.sector = val;
			break;	
		case 31: //SDMAIO
            IO_DATA.io = val;
			break;	
		case 32: //SDMAM
            IO_DATA.posmem = RB + val;
			break;	
		case 33: //SDMAON
            //DMAON();
			colocarSolicitudDeIO(0, PROC_IND);
			break;																		
		default:
			flagInstruccion = FAIL;
			break;
	}

	flag ret = SUCCESS;
	if(flagInstruccion == FAIL){
		genInterr(INSTRUCCIONINVALIDA);
		ret = FAIL;
	}
	if(flagMemoria == FAIL){
		ret = FAIL;
	}
	
	snprintf(mensaje, 200, "instruccion ejecutada con: %s", (ret == SUCCESS ? "EXITO" : "ERROR"));
    log_("procesador", mensaje);
	return ret;
	
}

flag finalizarPrograma(flag valor){
	//Hacer todo el protocolo de finalizacion
	//esto para que los programas que usabamos antes funcionen

	//push del valor de retorno
	push(valor);
	
	//Cargar el valor de AC = 1
	AC = 1;
	//llamar al sistema
	genInterr(LLAMADASISTEMA);
	return SUCCESS;
}

flag CicloInstruccion(){
	
	fetch(); 

	if(IR == -1){
		return finalizarPrograma(SUCCESS);
	}
	
    Instruccion instr = decode();

	flag estado_memoria =  MemoryAccess(&instr);
    if (estado_memoria == FAIL) {
        return FAIL;
    }
    
    execute(instr);
	return SUCCESS;
}
void * procesador(void* parametros){
	
    while(estadoCPU != CPU_APAGAR) {
        
		pthread_mutex_lock(&mutex_cpu);
		log_("procesador", "DURMIENDO PROCESADOR");
        while (estadoCPU == CPU_ESPERA) { 
            pthread_cond_wait(&cond_start, &mutex_cpu);
        }
		log_("procesador", "DESPERTANDO PROCESADOR");
		if (estadoCPU == CPU_APAGAR) {
			
            pthread_mutex_unlock(&mutex_cpu);
            break; 
        }

        pthread_mutex_unlock(&mutex_cpu);
		
		snprintf(mensaje,200, "INICIANDO PROCESADOR. modo de ejecución: %s", (estadoCPU == CPU_EJECUTAR_NORMAL ? "NORMAL" : "DEBUG"));
    	log_("procesador", mensaje);
		limpiarInterrupciones();
		valorFinalizacion = FAIL;
		
		while(estadoCPU != CPU_ESPERA && estadoCPU != CPU_APAGAR){

			
			if(!todosDormidos())CicloInstruccion();
			
			tick(); 
			int acc = SUCCESS;
			if (getInterruptions()) {
				acc = manInterr();
			}
			planificar(acc); //siempre se debe planificar
			if(cantProc == 0){
				pthread_mutex_lock(&mutex_cpu);
				estadoCPU = CPU_ESPERA; 
				pthread_cond_signal(&cond_end); 
				pthread_mutex_unlock(&mutex_cpu);
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

void activarProcesador(){
	cargarPlanificador(); //se activa el planificador
	setReloj(QUANTUM);
	pthread_mutex_lock(&mutex_cpu);

    estadoCPU = CPU_EJECUTAR_NORMAL; 

    pthread_cond_signal(&cond_start);
	
	
    while (estadoCPU != CPU_ESPERA && estadoCPU != CPU_APAGAR) {
        pthread_cond_wait(&cond_end, &mutex_cpu);
    }
    
    pthread_mutex_unlock(&mutex_cpu);
}

void crearProcesador(){
	log_("procesador", "CREANDO PROCESADOR");
    estadoCPU = CPU_ESPERA;
    setReloj(QUANTUM);
	crearPlanificador();
	pthread_create(&ID_PROC, NULL, procesador, NULL);
	log_("procesador", "PROCESADOR CREADO");

}

void matarProcesador(){
	log_("procesador", "APAGANDO PROCESADOR");
	pthread_mutex_lock(&mutex_cpu);
    estadoCPU = CPU_APAGAR;
    
    pthread_cond_signal(&cond_start);
    
    pthread_mutex_unlock(&mutex_cpu);

    pthread_join(ID_PROC, NULL);
	log_("procesador", "PROCESADOR APAGADO");
}