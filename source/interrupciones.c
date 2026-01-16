#include "interrupciones.h"

pthread_mutex_t acceso_interr = PTHREAD_MUTEX_INITIALIZER;
VectorInterrupciones v;
Pila estado;

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

//void finalizarPrograma(){
//	
//}
void limpiarInterrupciones(){
	for(int i = 0; i < 9; i++){
		v.interrupciones[i] = 0;
	}
}
flag manInterr(){
	INTERR_LOCK;
	guardarEstado();
	flag terminar = SUCCESS;
	char mensaje_svc[100];
	if(v.interrupciones[0]){//codigo de llamada al sistema invalido (terminar el programa)
		log_("manejadorInterrupciones", "ERROR: codigo de llamada al sistema invalido");

		//finalizarPrograma(); //detener programa actual
		terminar = FAIL;
	}
	if(v.interrupciones[1]){//codigo de interrupcion invalido (terminar el programa)
		log_("manejadorInterrupciones", "ERROR: codigo de interrupción invalido");

		//finalizarPrograma(); //detener programa actual
		terminar = FAIL;
	}
	if(v.interrupciones[2]){//llamada al sistema
		snprintf(mensaje_svc, 100, "Llamada al sistema, codigo: %d", AC);
		log_("manejadorInterrupciones", mensaje_svc); 
	}
	if(v.interrupciones[3]){//reloj
		log_("manejadorInterrupciones", "interrupcion de reloj");
	}
	if(v.interrupciones[4]){//finalizacion E/S
		log_("manejadorInterrupciones", "operacion de Entrada/Salida finalizada");
	}
	if(v.interrupciones[5]){//instrucción invalida (terminar el programa)
		snprintf(mensaje_svc, 100, "ERROR: Instruccion invalida %d", IR / 1000000);
		log_("manejadorInterrupciones", mensaje_svc); 

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