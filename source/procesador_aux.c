#include "procesador_aux.h"
#include "memoriaPrincipal.h"
#include "interrupciones.h"
#define PC_SHIFT 100000


palabra lectura(int pos, int *flag){
	
	palabra leido;

	if(getOpMode() != 1)pos = pos + RB;
	  
	*flag = pmrd(pos, &leido, PSW, RB, RL);
    if(*flag == FAIL)genInterr(DIRECCIONINVALIDA); //generamos la interrupción acá para que la memoria no genere interrupciones de procesos que no estan en ejecucion
	return leido;
}

void escritura(int pos, const palabra value, int *flag){
	
	if(getOpMode() != 1)pos = pos + RB;
	  
	*flag = pmwr(pos, value, PSW, RB, RL);
	if(*flag == FAIL)genInterr(DIRECCIONINVALIDA);
}

int getPC(){
    return PSW % PC_SHIFT;
}

void setPC(int pc){
    PSW = ((PSW / PC_SHIFT) * PC_SHIFT) + pc;
}

void jmp(int pos){
    setPC(pos);
}
int getInterruptions(){
    return (PSW / PC_SHIFT) % 10;
}
flag push(palabra AC){
	flag flagMemoria;
	SP+=1;
	escritura(SP, AC, &flagMemoria);
    //printf("PUSH DE %li\n", AC);
	return flagMemoria;
}
flag pop(palabra *AC){

	if(SP - 1 < RX){ // si intenta hacer pop de la base de la pila, no puede
		return FAIL;
	}
	flag flagMemoria;
    *AC = lectura(SP, &flagMemoria);
    SP -= 1;
    //printf("POP DE %li\n", *AC);
	return flagMemoria;
}
void setInterruptions(int interr){
    int pc = getPC();
    int aux = PSW / PC_SHIFT;
    PSW = (((aux / 10) * 10) + interr) * PC_SHIFT + pc;
}

int getCondCode() {
   
    return (PSW / PC_SHIFT / 100);
}

void setCondCode(int condicion) {
    
    int resto = PSW % (PC_SHIFT * 100);
    
    
    PSW = (condicion * PC_SHIFT * 100) + resto;
}

int getOpMode() {
   
    return (PSW / PC_SHIFT / 10) % 10;
}

void setOpMode(int modo) {
    int pc = getPC();
    int interr = getInterruptions();
    int codCond = getCondCode();

    PSW = (codCond * 100 + modo * 10 + interr) * PC_SHIFT + pc;
}
