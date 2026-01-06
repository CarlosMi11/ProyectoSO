#ifndef memoria_secundaria
#define memoria_secundaria

#include "cabecera.h"

typedef char sector_t[10];

sector_t disco[10][10][100];

int escribir(palabra *word, int pista, int cilindro, int sector);

int leer(palabra *word, int pista, int cilindro, int sector);


#endif