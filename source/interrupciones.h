#ifndef INTERRUPCIONES
#define INTERRUPCIONES

#include "cabecera.h"


#include "procesador_data.h"
#include "logger.h"

extern pthread_mutex_t acceso_interr;

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



flag manInterr();
void genInterr(int a);
void limpiarInterrupciones();
#endif