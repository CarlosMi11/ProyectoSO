#ifndef RELOJ
#define RELOJ

#include "interrupciones.h"
#include "cabecera.h"


flag setReloj(int tiempo);

int getReloj();

void tick();
extern int QUANTUM;
#endif