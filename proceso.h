#ifndef PROCESS
#define PROCESS

#include "cabecera.h"

typedef struct {
    long psw;
    long sp;
    long rx;
    long rb;
    long rl;
    int modoEjec;
    char nombre[50];
} proceso;

typedef enum {
    NUEVO,        
    LISTO,      
    EJECUTANDO,    
    BLOQUEADO,    
    FINALIZADO  
} EstadoProceso;

typedef struct {
    int MAR;
    int MDR;
    int IR;
    int RB;
    int RL;
    int RX;
    int SP;
    int PSW;
    int AC;
    int PSW;
    char idProceso[];
    EstadoProceso estado;
} BCP;



#endif