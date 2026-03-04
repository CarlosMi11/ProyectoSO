#include "memoriaSecundaria.h"

typedef char sector_t[10];

static sector_t disco[10][10][100];

int scwr(palabra word, int pista, int cilindro, int sector){
    
    if(word < 0){
        word = 100000000 + (-1) * word; //si es negativa, se guarda con un 1 en el noveno digito
    }
    snprintf(disco[pista][cilindro][sector], 10, "%08ld", word);

    string mensaje;
    snprintf(mensaje, 200, "escritura de %ld exitosa en disco[%i][%i][%i]", word, pista, cilindro, sector);
    log_("memoriaSecundaria", mensaje);
    return SUCCESS;
}

int scrd(palabra *word, int pista, int cilindro, int sector){
    *word = atol(disco[pista][cilindro][sector]);
    if(*word / 100000000 == 1){ //si tiene un 1 en el 9no digito
        *word = (*word % 100000000) * (-1); //la palabra son los primeros 8 digitos *-1
    }

    string mensaje;
    snprintf(mensaje, 200, "lectura de %ld exitosa en disco[%i][%i][%i]", *word, pista, cilindro, sector);
    log_("memoriaSecundaria", mensaje);

    return SUCCESS;
}

flag nextPosMemSec(int* pista, int* cilindro, int* sector, int jump) {
    
    
    int posLineal = (*pista * 10 * 100) + (*cilindro * 100) + (*sector) + jump;

    
    if (posLineal >= 10000 || posLineal < 0) {
        log_("memoriaSecundaria", "ERROR: Desbordamiento de dirección de disco (Jump fuera de rango)");
        return FAIL;
    }

    // Traducimos la nueva posición lineal de vuelta a coordenadas CHS
    // Esto es mucho más eficiente que hacer múltiples IF en cascada
    *pista = posLineal / (10 * 100);
    int resto = posLineal % (10 * 100);
    
    *cilindro = resto / 100;
    *sector = resto % 100;

    return SUCCESS;
}