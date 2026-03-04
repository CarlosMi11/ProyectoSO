#include "tablaDeProgramas.h"
#include "memoriaSecundaria.h"
#include "logger.h"

#define MAX_PROGRAMAS_EN_DISCO 50


typedef struct {
    string nombreArchivo; 
    string nombreProg;
    int pista;
    int cilindro;
    int sector;
} RegistroDisco;


static RegistroDisco tablaDeProgramas[MAX_PROGRAMAS_EN_DISCO];

static int cantArchDisco = 0;
static void impresion(char *linea){
    printf("%s\n", linea);
    log_("Tabla de Programas", linea);
}

flag antesLeido(const char* ruta, char* nombre, int* pista, int* cilindro, int* sector) {

    for (int i = 0; i < cantArchDisco; i++) {

        if (strcmp(tablaDeProgramas[i].nombreArchivo, ruta) == 0) {
            strcpy(nombre, tablaDeProgramas[i].nombreProg);
            *pista = tablaDeProgramas[i].pista;
            *cilindro = tablaDeProgramas[i].cilindro;
            *sector = tablaDeProgramas[i].sector;
            
            return SUCCESS; 
        }
    }
    return FAIL; 
}

flag getNextCodeSpace(int* pista, int* cilindro, int* sector){
    
    if(cantArchDisco == 0){
        *pista = 0;
        *cilindro = 0;
        *sector = 0;
        return SUCCESS;
    }
    else {
        
        int ultimo = cantArchDisco - 1;
        *pista = tablaDeProgramas[ultimo].pista;
        *cilindro = tablaDeProgramas[ultimo].cilindro;
        *sector = tablaDeProgramas[ultimo].sector;
        
        
        nextPosMemSec(pista, cilindro, sector, 1); 
        palabra tamanoViejo;
        scrd(&tamanoViejo, *pista, *cilindro, *sector); 

        
        return nextPosMemSec(pista, cilindro, sector, tamanoViejo + 1);
    }
}
flag leerLinea(FILE *fp, string resultado){
    if (fgets(resultado, 255, fp) == NULL) {
        return FAIL; 
    }

    char *comentario = strstr(resultado, "//");
    if (comentario != NULL) {
        *comentario = '\0'; 
    }

    resultado[strcspn(resultado, "\n")] = 0;
    resultado[strcspn(resultado, "\r")] = 0;

    int i = 0;
    while(isspace((unsigned char)resultado[i])) {
        i++;
    }
    if (i > 0) {
        
        memmove(resultado, resultado + i, strlen(resultado) - i + 1);
    }

    return SUCCESS;
}
void registrarEnDisco(const char* ruta, const char* nombre, int p, int c, int s) {
    if (cantArchDisco < MAX_PROGRAMAS_EN_DISCO) {
        strcpy(tablaDeProgramas[cantArchDisco].nombreArchivo, ruta);
        strcpy(tablaDeProgramas[cantArchDisco].nombreProg, nombre);
        tablaDeProgramas[cantArchDisco].pista = p;
        tablaDeProgramas[cantArchDisco].cilindro = c;
        tablaDeProgramas[cantArchDisco].sector = s;
        cantArchDisco++;
    }
}

flag cargarArchivoAMemoriaSecundaria(const char* archivo, string nombre, int* pista_, int* cilindro_, int* sector_, int* tam_){
    string mensaje;
    FILE *fp = fopen(archivo, "r");
    if (fp == NULL) {
        snprintf(mensaje, TAM_MENSAJE, "ERROR: archivo '%s' no encontrado.", archivo, MAX_TAM_PROG);
        impresion(mensaje);
        return FAIL;
    }

    string linea; 
    int punto_entrada = 0;
    int num_palabras = 0;
    
    
    leerLinea(fp, linea);
    sscanf(linea, "_start %d", &punto_entrada);
    
    leerLinea(fp, linea);
    sscanf(linea, ".NumeroPalabras %d", &num_palabras);

    if(num_palabras > MAX_TAM_PROG){
        snprintf(mensaje, TAM_MENSAJE, "ERROR: programa %s excede el tamano limite: %i", archivo, MAX_TAM_PROG);
        impresion(mensaje);
        return FAIL;
    }
    
    leerLinea(fp, linea);
    sscanf(linea, ".NombreProg %s", nombre); 

    int pista, cilindro, sector;
    if(getNextCodeSpace(&pista, &cilindro, &sector) == FAIL){
        impresion("ERROR: Memoria secundaria llena");
        fclose(fp);
        return FAIL;
    }

    *pista_ = pista;
    *cilindro_ = cilindro;
    *sector_ = sector;
    *tam_ = num_palabras;

    scwr(punto_entrada, pista, cilindro, sector);
    nextPosMemSec(&pista, &cilindro, &sector, 1);
    scwr(num_palabras, pista, cilindro, sector);
    nextPosMemSec(&pista, &cilindro, &sector, 1);

    while (leerLinea(fp, linea) == SUCCESS) {
        if (linea[0] == '\0') continue; 
        
        long instruccion = atol(linea);
        scwr(instruccion, pista, cilindro, sector);
        nextPosMemSec(&pista, &cilindro, &sector, 1);
    }
    
    fclose(fp);

    registrarEnDisco(archivo, nombre, *pista_, *cilindro_, *sector_);

    return SUCCESS;
}

void limpiarTablaDeProgramas(){
    
    cantArchDisco = 0;
    log_("Tabla de Programas", "Tabla limpia.");
}