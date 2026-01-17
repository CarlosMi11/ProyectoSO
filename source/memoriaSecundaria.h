#ifndef MEMORIA_SECUNDARIA
#define MEMORIA_SECUNDARIA

#include "cabecera.h"
#include "logger.h"

int scwr(palabra word, int pista, int cilindro, int sector);

int scrd(palabra *word, int pista, int cilindro, int sector);


#endif