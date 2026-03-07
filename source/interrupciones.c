#include "interrupciones.h"
#include "procesador_aux.h"
#include "planificador.h"
#include "logger.h"

static pthread_mutex_t acceso_interr = PTHREAD_MUTEX_INITIALIZER;

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

static void guardarContexto(){
	
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

static void restaurarContexto(){	
	
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

void genInterr(Interrupcion interr){
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
flag llamadaInvalida(){
	string mensaje;
	snprintf(mensaje, TAM_MENSAJE, "ERROR: codigo de llamada al sistema invalido (%ld)", AC);
	log_("Manejador Interrupciones", mensaje); 
	return FAIL;
	//finalizarPrograma(); //detener programa actual
};
flag codInterrInvalido(){
	log_("Manejador Interrupciones", "ERROR: codigo de interrupción invalido");
	return FAIL;
	//finalizarPrograma(); //detener programa actual
};
flag llamadaSistema(palabra codigo){
	string mensaje;
	snprintf(mensaje, TAM_MENSAJE, "Llamada al sistema, codigo: %ld", AC);
	log_("Manejador Interrupciones", mensaje); 
	palabra a;
	switch(codigo){
		case 1: //fin proceso
			
			pop(&a);
			
			
			matarProceso(PROC_IND, a);
			return FAIL; //no es un fail, pero se debe planificar... entonces se manda un 1

		case 2: //imprimir por pantalla
			
			pop(&a);

			//obtener el nombre de a
			printf("SO-SIM> %s<< %li\n", getNombreProceso(PROC_IND), a);
			snprintf(mensaje, TAM_MENSAJE, "Impresion por pantalla: %ld", a);
			log_("Manejador Interrupciones", mensaje); 
			return SUCCESS;

		case 3://leer por pantalla
			colocarSolicitudDeIO(1, PROC_IND);
			return SUCCESS;

		case 4:
			
			pop(&a);
			dormirProceso(PROC_IND, a);
			return FAIL;

		default:
			//codigo de interrupcion invalido (no deberia haber llegado acá)
			matarProceso(PROC_IND, -1);
			return FAIL;
		
	}

	return SUCCESS;
};
flag relojInterr(){
	log_("Manejador Interrupciones", "interrupcion de reloj");
	return FAIL;
};
flag finIO(){
	int idProc;
	int lectura;
	flag operacion = get_resultado(&idProc, &lectura);
	terminarIO(operacion, idProc, lectura);
	//printf("INTERRUPCIONES: idProc = %i, lectura = %i, operacion = %i\n", idProc, lectura, operacion);
	log_("Manejador Interrupciones", "operacion de Entrada/Salida finalizada");
	return SUCCESS;
}
flag instrucInvalida(){
	string mensaje;
	snprintf(mensaje,TAM_MENSAJE, "ERROR: Instruccion invalida (%ld)", IR / 1000000);
	log_("Manejador Interrupciones", mensaje); 
	
	return FAIL;
}
flag direccInvalida(){
	log_("Manejador Interrupciones", "ERROR: direccionamiento invalido");

	return FAIL;
}
flag underFlow(){
	log_("Manejador Interrupciones", "WARNING: underflow");
	return SUCCESS;
}
flag overFlow(){
	log_("Manejador Interrupciones", "WARNING: overflow");
	return SUCCESS;
}
int manInterr(){
	/*
	Las interrupciones devuelven:
		FAIL si hay que hacer un cambio de contexto
		SUCCESS si no es necesario
	*/
	INTERR_LOCK;
	
	flag terminar = SUCCESS;
	
	
	//deberian terminar el proceso
	if(v.interrupciones[0]){
		terminar |= llamadaInvalida();
	}
	if(v.interrupciones[1]){
		terminar |= codInterrInvalido();
	}
	if(v.interrupciones[5]){
		terminar |= instrucInvalida();
	}
	if(v.interrupciones[6]){
		terminar |= direccInvalida();
	}

	if(terminar == 1){
		limpiarInterrupciones();
		matarProceso(PROC_IND,-1);
		INTERR_UNLOCK;
		return FAIL;
	}

	//pueden generar un cambio de contexto
	if(v.interrupciones[2]){
		terminar |= llamadaSistema(AC);
	}
	if(v.interrupciones[3]){
		terminar |= relojInterr();
	}

	//extras
	if(v.interrupciones[4]){
		terminar |= finIO();
	}
	if(v.interrupciones[7]){
		terminar |= underFlow();
	}
	if(v.interrupciones[8]){
		terminar |= overFlow();
	}

	limpiarInterrupciones();
	
	INTERR_UNLOCK;
	return terminar;
}