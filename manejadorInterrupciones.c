include "registros.h"
include "cabecera.h"
#include <pthread.h>

pthread_mutex_t acceso_interr;
#define INTERR_LOCK pthread_mutex_lock(&acceso_interr)
#define INTERR_UNLOCK pthread_mutex_unlock(&acceso_interr)

flag banderaInterr=SUCCESS; // no hay interrupciones

typedef struct {
	int interrupciones[9];
	// interrupciones[0] codigo de llamada al sistema invalido
	// interrupciones[1] codigo de interrupcion invalido
	// interrupciones[2] llamada al sistema
	// interrupciones[3] reloj
	// interrupciones[4] finalizacion de operacion E/S
	// interrupciones[5] instruccion invalida
	// interrupciones[6] direccionamiento invalido
	// interrupciones[7] underflow
	// interrupciones[8] underflow

	// para cualquier i: interrupciones[i] = 0, no hay interrupciones
	// para cualquier i: interrupciones[i] = 1, si hay interrupciones
} VectorInterrupciones;

VectorInterrupciones v;

typedef struct {
	int MAR;
	int MDR;
	int IR;
	int RB;
	int RL;
	int RX;
	int SP;
	int PSW;
	int AC;
} Pila;

Pila estado;

void guardarEstado(){
	// guardar estado de procesador
		// guardar en pila todos los registros
	estado.MAR = MAR;
	estado.MDR = MDR;
	estado.IR = IR;
	estado.RB = RB;
	estado.RL = RL;
	estado.RX = RX;
	estado.SP = SP;
	estado.PSW = PSW;
	estado.AC = AC;
}

void restaurarEstado(){	
	// restaurar estado
		// sacar de pila los valores de los registros y volverlos a poner en los registros
	MAR = estado.MAR;
	MDR = estado.MDR;
	IR = estado.IR;
	RB = estado.RB;
	RX = estado.RX;
	RL = estado.RL;
	SP = estado.SP;
	PSW = estado.PSW;
	AC = estado.AC;
}

void genInterr(int interr){
	INTERR_LOCK;
	if(codInterr>=0 && codInterr <=8){
		v.interrupciones[codInterr] = 1;
	}
	INTERR_UNLOCK;
}


void manInterr(){
	INTERR_LOCK;
	for(int i=0; i<=8; i++){
		if(v.interrupciones[i]==1){
			banderaInterr=FAIL;
			switch(i){
				case 0:
					break;
				case 1:
					break;
				case 2:
					break;
				case 3:
					break;
				case 4:
					break;
				case 5:
					break;
				case 6:
					break;
				case 7:
					break;
				case 8:
					break;
			}
		}
	}
	INTERR_UNLOCK;
}