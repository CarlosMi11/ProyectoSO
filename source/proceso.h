#ifndef PROCESS
#define PROCESS

#include "cabecera.h"

typedef enum {
    NUEVO,        
    LISTO,      
    EJECUTANDO,    
    DORMIDO,    
    FINALIZADO  
} EstadoProceso;

typedef struct {
    long MAR;
    long MDR;
    long IR;
    long RB;
    long RL;
    long RX;
    long SP;
    long PSW;
    long AC;
    long valorFinalizacion;
    long tiempoDeDormido;
    string idProceso;
    EstadoProceso estado;
} proceso;



#endif