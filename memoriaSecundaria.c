#include "memoriaSecundaria.h"

typedef char sector_t[10];

sector_t disco[10][10][100];

int scwr(palabra word, int pista, int cilindro, int sector){
    snprintf(disco[pista][cilindro][sector], 10, "%09ld", word); 
    return SUCCESS;
}

int scrd(palabra *word, int pista, int cilindro, int sector){
    word = atol(disco[pista][cilindro][sector]);
    return SUCCESS;
}