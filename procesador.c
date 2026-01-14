#include "registros.h"
#include "procesador.h"
#include "procesador_aux.c"
#include "DMA.h"
#include "reloj.h"

int MAR = 0;
int MDR = 0;
int IR = 0;
int RB = 0;
int RL = 0;
int RX = 0;
int SP = 0;
int PSW = 0;
int AC = 0;



palabra lectura(int pos, int *flag){
	MEM_LOCK;
	palabra leido;
	*flag = pmrd(pos, &leido, PSW, RB, RL);
	MEM_UNLOCK;
	return leido;
}

void escritura(int pos, const palabra value, int *flag){
	MEM_LOCK;
	*flag = pmwr(pos, value, PSW, RB, RL);
	MEM_UNLOCK;
}

void fetch(){
	
	int PC = getPC();

	MAR = PC;

	flag flagLectura;
	MDR = lectura(MAR, &flagLectura);	

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
	
	
	int opcode, val, dir;
    opcode = i.opcode;
    val = i.val;
    dir = i.dir;

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
                
                log("CPU", "Error: Division por cero");
				flagInstruccion = FAIL;
				break;
            }
        
           
            AC = AC / val;
         
            if (AC == 0)      setCondCode(0);
            else if (AC < 0)  setCondCode(1);
            else              setCondCode(2);

			break;	

		case 4: // LOAD
			AC = lectura(val, &flagMemoria);
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
			palabra valsp = lectura(SP, &flagMemoria);
            if(flagMemoria == SUCCESS){
                if (AC == valsp) setPC(val);
            }
			break;
		case 10: // JMPNE
			palabra valsp = lectura(SP, &flagMemoria);
            if(flagMemoria == SUCCESS){
                if (AC != valsp) setPC(val);
            }
			break;
		case 11: // JMPLT
			palabra valsp = lectura(SP, &flagMemoria);
            if(flagMemoria == SUCCESS){
                if (AC < valsp) setPC(val);
            }
			break;	
		case 12: // JMPGT
			palabra valsp = lectura(SP, &flagMemoria);
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

void * procesador(void* parametros){

    while(1) {
        
        
        if (interrupcion() && getInterruptions()) {
            manInterr(); 
            continue; 
        }

        
        fetch(); 
		if(IR == -1){
			//TERMINO EL PROGRAMA
			break;
		}

		
        Instruccion instr = decode();
	
		flag estado_memoria =  MemoryAccess(&instr);

        if (estado_memoria == FAIL) {
            
            genInterr(6);
            continue;
        }

        
        execute(instr);

        
        tick(); 

        
        if (debugMode) {
            
        }
    }
    return NULL;
}