#ifndef PROCESADOR
#define PROCESADOR
#include "cabecera.h"
#include "registros.h"
#include "memoriaPrincipal.h"


#define MAX_NUMBER 9999999
#define MIN_NUMBER -9999999

typedef struct {
    int opcode;
    int dir;
    int val;
} Instruccion;

void fetch(int *dirPSW);

Instruccion decode(int IR);

int accesoMemoria(Instruccion i);

flag execute (Instruccion i);

#endif