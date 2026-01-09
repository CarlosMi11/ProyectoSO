#include "cabecera.h"
#include "registros.h"
#include "memoriaPrincipal.h"

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

typedef struct {
    int opcode;
    int dir;
    int val;
} Instruccion;

void fetch(int *dirPSW){
	int PSW = *dirPSW;
	int PC = PSW % 100000; // los ultimos 5 digitos de PSW son el PC
	int aux = PSW / 100000;
	MAR = PC;
	int flagLectura;
	MEM_LOCK;
	MDR = lectura(MAR, &flagLectura);	
	MEM_UNLOCK;
	PC+=1;
	*dirPSW = (aux*100000)+PC;
	IR = MDR;
}

Instruccion decode(int IR){
	struct Instruccion i;
	i.val = IR % 100000;
	i.opcode = IR / 1000000;
	i.dir = (IR / 100000) % 10;
	return i;
}

int accesoMemoria(Instruccion i){
	if(i.dir == 0){
		int flagLectura;
		MEM_LOCK;
		int valor = lectura(i.val, &flagLectura);
		MEM_UNLOCK;
		return valor;
	}
	if(i.dir==1){
		if ((i.val/10000)==1) return (i.val%10000)*(-1);
		return i.val;
	}
	if(i.dir==2){
		int flagLectura;
		MEM_LOCK;
		int valor = lectura((i.val + AC), &flagLectura);
		MEM_UNLOCK;
		return valor;
	}
}

void execute (Instruccion i){
	int opcode, dir, val, flagLectura;
	val = accesoMemoria(i);
	opcode = i.opcode;

	int aux = PSW % 10000000;

	int flagLectura;
	switch (opcode){
		case 00: // SUM
			AC = AC + val;

			if (AC==0) PSW = aux;
			if (AC<0) PSW = (1*10000000) + aux;
			if (AC>0) PSW = (2*10000000) + aux;
			if (AC>9999999 || AC < -9999999) PSW = (3*10000000) + aux;  
			break;
		case 01: // RES
			AC = AC - val;

			if (AC==0) PSW = aux;
			if (AC<0) PSW = (1*10000000) + aux;
			if (AC>0) PSW = (2*10000000) + aux;
			if (AC>9999999 || AC < -9999999) PSW = (3*10000000) + aux;  
			break;
		case 02: // MULT
			AC = AC * val;

			if (AC==0) PSW = aux;
			if (AC<0) PSW = (1*10000000) + aux;
			if (AC>0) PSW = (2*10000000) + aux;
			if (AC>9999999 || AC < -9999999) PSW = (3*10000000) + aux;  
			break;
		case 03: // DIVI
			AC = AC / val;
			
			if (AC==0) PSW = aux;
			if (AC<0) PSW = (1*10000000) + aux;
			if (AC>0) PSW = (2*10000000) + aux;
			if (AC>9999999 || AC < -9999999) PSW = (3*10000000) + aux;  
			break;	
		case 04: // LOAD
			AC=val;
			break;	
		case 05: // STR
			// REVISAR
			if (dir==0){
				MEM_LOCK;
				escritura(val, AC);
				MEM_UNLOCK;				
			}
			if (dir==2){
				MEM_LOCK;
				escritura(AC+val, AC);
				MEM_UNLOCK;				
			}
			break;	
		case 06: // LOADRX
			AC = RX;
			break;	
		case 07: // STRRX
			RX = AC;
			break;	
		case 08: // COMP
			if (AC==val) PSW = aux;
			if (AC<val) PSW = (1*10000000) + aux;
			if (AC>val) PSW = (2*10000000) + aux;
			break;
		case 09: // JMPE			
			MEM_LOCK;
			if (AC==lectura(SP, &flagLectura)) PC = val;	
			MEM_UNLOCK;	
			break;	
		case 10: // JMPNE
			MEM_LOCK;
			if (AC!=lectura(SP, &flagLectura)) PC = val;	
			MEM_UNLOCK;	
			break;	
		case 11: // JMPLT
			MEM_LOCK;
			if (AC<lectura(SP, &flagLectura)) PC = val;	
			MEM_UNLOCK;	
			break;	
		case 12: // JMPGT
			MEM_LOCK;
			if (AC>lectura(SP, &flagLectura)) PC = val;	
			MEM_UNLOCK;	
			break;			
		case 13:
			break;	
		case 14:
			break;	
		case 15:
			break;	
		case 16:
			break;	
		case 17:
			break;	
		case 18:
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
			RC = AC;
			break;	
		case 23: // LOADSP
			AC = SP;
			break;	
		case 24: // STRSP
			SP = AC;
			break;	
		case 25: 
			break;	
		case 26:
			break;	
		case 27: // J
			PC = val;
			break;	
		case 28:
			break;	
		case 29:
			break;	
		case 30:
			break;	
		case 31:
			break;	
		case 32:
			break;	
		case 33:
			break;																		

	}
}