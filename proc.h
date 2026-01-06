#include "cabecera.h"
#include "registros.h"
#include "memoriaPrincipal.h"

int word, opcode, dir, val;

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

void instruccion (palabra word){
	val = word % 100000;
	opcode = word / 1000000;
	dir = (word / 100000) % 10;

	int aux = PSW % 10000000;

	int flagLectura;
	switch (opcode){
		case 00: // SUM
			if(dir==0){
				
				AC = AC + lectura(val, &flagLectura);
				
			}
			if(dir==1) AC = AC + val;
			if(dir==2){
				

				AC = AC + lectura(AC + val, &flagLectura);
				
			}

			if (AC==0) PSW = aux;
			if (AC<0) PSW = (1*10000000) + aux;
			if (AC>0) PSW = (2*10000000) + aux;
			if (AC>99999999) PSW = (3*10000000) + aux;  
			break;
		case 01: // RES
			if(dir==0){
				MEM_LOCK;
				AC = AC - lectura(val);
				MEM_UNLOCK;
			}
			if(dir==1) AC = AC - val;
			if(dir==2){
				MEM_LOCK;
				 AC = AC - lectura(AC+val);
				MEM_UNLOCK;
			}
			if (AC==0) PSW = aux;
			if (AC<0) PSW = (1*10000000) + aux;
			if (AC>0) PSW = (2*10000000) + aux;
			if (AC>99999999) PSW = (3*10000000) + aux;  
			break;
		case 02: // MULT
			if(dir==0){
				MEM_LOCK;
				AC = AC * lectura(val);;
				MEM_UNLOCK;
			}
			if(dir==1) AC = AC * val;
			if(dir==2){
				MEM_LOCK;
				 AC = AC * lectura(AC+val);
				MEM_UNLOCK;
			}
			if (AC==0) PSW = aux;
			if (AC<0) PSW = (1*10000000) + aux;
			if (AC>0) PSW = (2*10000000) + aux;
			if (AC>99999999) PSW = (3*10000000) + aux;  
			break;
		case 03: // DIVI
			if(dir==0){
				MEM_LOCK;
				AC = AC / lectura(val);;
				MEM_UNLOCK;
			}
			if(dir==1) AC = AC / val;
			if(dir==2){
				MEM_LOCK;
				AC = AC / lectura(AC+val);
				MEM_UNLOCK;
			}
			if (AC==0) PSW = aux;
			if (AC<0) PSW = (1*10000000) + aux;
			if (AC>0) PSW = (2*10000000) + aux;
			if (AC>99999999) PSW = (3*10000000) + aux;  
			break;	
		case 04: // LOAD
			if (dir==0){
				MEM_LOCK;
				AC = lectura(val);
				MEM_UNLOCK;				
			}
			ìf (dir==1){
				AC = val
			}
			if (dir==2){
				MEM_LOCK;
				AC = lectura(AC+val);
				MEM_UNLOCK;				
			}
			break;	
		case 05: // STR
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
			if (dir==0){
				if (AC==lectura(SP)) PC = lectura(val);	
			}
			if (dir==2){
				if (AC==lectura(SP)) PC = lectura(val);
			}
			MEM_UNLOCK;	
			break;	
		case 10: // JMPNE
			MEM_LOCK;
			if (dir==0){
				if (AC!=lectura(SP)) PC = lectura(val);		
			}
			if (dir=2){
				if (AC!=lectura(SP)) PC = lectura(val);
			}
			MEM_UNLOCK;	
			break;	
		case 11: // JMPLT
			MEM_LOCK;
			if (dir==0){
				if (AC<lectura(SP)) PC = lectura(val);		
			}
			if (dir=2){
				if (AC<lectura(SP)) PC = lectura(val);
			}
			MEM_UNLOCK;	
			break;	
			break;	
		case 12: // JMPGT
			MEM_LOCK;
			if (dir==0){
				if (AC>lectura(SP)) PC = lectura(val);		
			}
			if (dir=2){
				if (AC>lectura(SP)) PC = lectura(val);
			}
			MEM_UNLOCK;	
			break;			
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
		case 19:
			break;	
		case 20:
			break;	
		case 21:
			break;	
		case 22:
			break;	
		case 23:
			break;	
		case 24:
			break;	
		case 25:
			break;	
		case 26:
			break;	
		case 27:
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