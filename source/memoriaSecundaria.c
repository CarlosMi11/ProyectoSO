#include "memoriaSecundaria.h"

typedef char sector_t[10];

static sector_t disco[10][10][100];

int scwr(palabra word, int pista, int cilindro, int sector){
    
    if(word < 0){
        word = 10000000 + (-1) * word;
    }
    snprintf(disco[pista][cilindro][sector], 10, "%08ld", word);

    char mensaje[200];
    snprintf(mensaje, 200, "escritura de %ld exitosa en disco[%i][%i][%i]", word, pista, cilindro, sector);
    log_("memoriaSecundaria", mensaje);
    return SUCCESS;
}

int scrd(palabra *word, int pista, int cilindro, int sector){
    *word = atol(disco[pista][cilindro][sector]);
    if(*word / 10000000 == 1){
        *word = (*word % 10000000) * (-1);
    }

    char mensaje[200];
    snprintf(mensaje, 200, "lectura de %ld exitosa en disco[%i][%i][%i]", *word, pista, cilindro, sector);
    log_("memoriaSecundaria", mensaje);

    return SUCCESS;
}