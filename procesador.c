#include "registros.h"
#include "procesador.h"
#include "procesador_aux.c"
#include "DMA.h"

int MAR = 0;
int MDR = 0;
int IR = 0;
int RB = 0;
int RL = 0;
int RX = 0;
int SP = 0;
int PSW = 0;
int AC = 0;

void * procesador(void* parametros){

    return NULL;
}

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
	i.opcode = IR / 1000000;
	i.dir = (IR / 100000) % 10;
	return i;
}

flag accesoMemoria(Instruccion *i){
    int valor = 0;
    flag flagLectura = SUCCESS;


	if(i->dir == 0){
		valor = lectura(i->val, &flagLectura);
	}
	if(i->dir==1){
		if ((i->val/10000)==1) valor = (i->val%10000)*(-1);

		else valor = i->val;
	}
	if(i->dir==2){
		valor = lectura((i->val + AC), &flagLectura);
	}
    
    if(flagLectura == FAIL){
        //COLOCAR INTERRUPCION
    }

    i->val = valor;

    return flagLectura;
}

void execute (Instruccion i){
	
	
	int opcode, val, dir;
    opcode = i.opcode;
    val = i.val;
    dir = i.dir;

	flag flagMemoria;

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
                
                return;
            }
        
           
            AC = AC / val;
         
            if (AC == 0)      setCondCode(0);
            else if (AC < 0)  setCondCode(1);
            else              setCondCode(2);

			break;	

		case 4: // LOAD
			AC=val;
			break;	
		case 5: // STR
			
			if (dir==0){
				
				escritura(val, AC, &flagMemoria);
				
			}
			else if (dir==2){
				escritura(AC+val, AC, &flagMemoria);			
			}
            else if(dir == 1){
                //INTERRUPCION INSTRUCCION INVALIDA
            }
			break;	
		case 6: // LOADRX
			AC = lectura(RX, &flagMemoria);
			break;	
		case 7: // STRRX
			escritura(RX, AC, &flagMemoria);
			break;	
		case 8: // COMP
			if (AC==val) setCondCode(0);
			if (AC<val)  setCondCode(1);
			if (AC>val)  setCondCode(2);
			break;
		case 9: // JMPE			
			if (AC == lectura(SP, &flagMemoria)) setPC(val);	
			break;	
		case 10: // JMPNE
			if (AC != lectura(SP, &flagMemoria)) setPC(val);	
			break;	
		case 11: // JMPLT
			if (AC < lectura(SP, &flagMemoria))  setPC(val);
			break;	
		case 12: // JMPGT
			if (AC > lectura(SP, &flagMemoria))  setPC(val);
			break;			
		case 13: // SVC
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
            

			break;	
		case 18: // CHMOD
            if(val != 0 || val != 1){
                //INTERRUPCION INSTRUCCION INVALIDA
                
            }
            setOpMode(val);
			break;																				
		case 19: // LOADRB: cargar RB en AC
			AC = lectura(RB, &flagMemoria);
			break;	
		case 20: // STRRB
			escritura(RB, AC, &flagMemoria);
			break;	
		case 21: // LOADRL
			AC = lectura(RL, &flagMemoria);
			break;	
		case 22: // STRRL
			escritura(RB, RL, &flagMemoria);
			break;	
		case 23: // LOADSP
			AC = lectura(SP, &flagMemoria);
			break;	
		case 24: // STRSP
			escritura(RB, SP, &flagMemoria);
			break;	
		case 25: //PSH
            RX+=1;
            escritura(RX, AC, &flagMemoria);
			break;	
		case 26: //POP
            AC = lectura(RX, &flagMemoria);
            RX-=1;
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

	}
}