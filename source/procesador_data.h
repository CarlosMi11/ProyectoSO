#ifndef DATA_PROC
#define DATA_PROC
#include "cabecera.h"

extern long MAR;
extern long MDR;
extern long  IR;
extern long  RB;
extern long  RL;
extern long  RX;
extern long  SP;
extern long PSW;
extern long  AC;
extern int estadoCPU; 

extern flag valorFinalizacion;
extern flag finalizo;

extern pthread_mutex_t mutex_cpu;
extern pthread_cond_t cond_start; 
extern pthread_cond_t cond_end; 


#define CPU_ESPERA   0  
#define CPU_EJECUTAR_NORMAL   1  
#define CPU_EJECUTAR_DEBUG  2  
#define CPU_APAGAR   4  
#endif