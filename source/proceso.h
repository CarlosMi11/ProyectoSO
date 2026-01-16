#ifndef PROCESS
#define PROCESS

#include "cabecera.h"

typedef struct {
    long psw;
    long sp;
    long rx;
    long rb;
    long rl;
    int modoEjec;
    char nombre[50];
} proceso;

#endif