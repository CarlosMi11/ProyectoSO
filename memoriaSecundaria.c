#include "memoriaSecundaria.h"

int escribir(palabra *word, int pista, int cilindro, int sector){
    snprintf(disco[pista][cilindro][sector], 10, "%09ld", word); 
    return SUCCESS;
}

int leer(palabra *word, int pista, int cilindro, int sector){
    word = atol(disco[pista][cilindro][sector]);
    return SUCCESS;
}