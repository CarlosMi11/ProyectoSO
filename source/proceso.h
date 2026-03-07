#ifndef PROCESS
#define PROCESS

#include "cabecera.h"
#include "DMA.h"
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
    solicitudIO IO_DATA;
    EstadoProceso estado;
} proceso;


#endif