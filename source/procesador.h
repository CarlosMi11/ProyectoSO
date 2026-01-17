#ifndef PROCESADOR
#define PROCESADOR

#include "memoriaPrincipal.h"
#include "procesador_aux.h"
#include "DMA.h"
#include "reloj.h"
#include "logger.h"
#include "interrupciones.h"
#include "proceso.h"

#define MAX_NUMBER 9999999
#define MIN_NUMBER -9999999


typedef struct {
    int opcode;
    int dir;
    int val;
} Instruccion;


void activarProcesador(int modo);
void crearProcesador();
void matarProcesador();
#endif