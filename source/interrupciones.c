#include "interrupciones.h"
#include "procesador_data.h"
#include "logger.h"

static pthread_mutex_t acceso_interr = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t acceso_interr;

#define INTERR_LOCK pthread_mutex_lock(&acceso_interr)
#define INTERR_UNLOCK pthread_mutex_unlock(&acceso_interr)

typedef struct {
	int interrupciones[9];
} VectorInterrupciones;

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

static VectorInterrupciones v;
static Pila estado;

void guardarEstado(){
	
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
	if(interr>=0 && interr <=8){
		v.interrupciones[interr] = 1;
	}
	else{
		v.interrupciones[1] = 1;
	}
	INTERR_UNLOCK;
}

void limpiarInterrupciones(){
	for(int i = 0; i < 9; i++){
		v.interrupciones[i] = 0;
	}
}
flag manInterr(){
	INTERR_LOCK;
	guardarEstado();
	flag terminar = SUCCESS;
	char mensaje[200];
	if(v.interrupciones[0]){//codigo de llamada al sistema invalido (terminar el programa)
		snprintf(mensaje, 200, "ERROR: codigo de llamada al sistema invalido (%ld)", AC);
		log_("manejadorInterrupciones", mensaje); 

		//finalizarPrograma(); //detener programa actual
		terminar = FAIL;
	}
	if(v.interrupciones[1]){//codigo de interrupcion invalido (terminar el programa)
		log_("manejadorInterrupciones", "ERROR: codigo de interrupción invalido");

		//finalizarPrograma(); //detener programa actual
		terminar = FAIL;
	}
	if(v.interrupciones[2]){//llamada al sistema
		snprintf(mensaje, 200, "Llamada al sistema, codigo: %ld", AC);
		log_("manejadorInterrupciones", mensaje); 
	}
	if(v.interrupciones[3]){//reloj
		log_("manejadorInterrupciones", "interrupcion de reloj");
	}
	if(v.interrupciones[4]){//finalizacion E/S
		log_("manejadorInterrupciones", "operacion de Entrada/Salida finalizada");
	}
	if(v.interrupciones[5]){//instrucción invalida (terminar el programa)
		snprintf(mensaje, 200, "ERROR: Instruccion invalida (%ld)", IR / 1000000);
		log_("manejadorInterrupciones", mensaje); 

		//finalizarPrograma(); //detener programa actual
		terminar = FAIL;
	}
	if(v.interrupciones[6]){//direccionamiento invalido (terminar el programa)
		log_("manejadorInterrupciones", "ERROR: direccionamiento invalido");

		//finalizarPrograma(); //detener programa actual
		terminar = FAIL;
	}
	if(v.interrupciones[7]){//underflow
		log_("manejadorInterrupciones", "WARNING: underflow");
	}
	if(v.interrupciones[8]){//overflow
		log_("manejadorInterrupciones", "WARNING: overflow");
	}
	limpiarInterrupciones();
	restaurarEstado();
	INTERR_UNLOCK;
	return terminar;
}