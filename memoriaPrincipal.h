#ifndef MEMORIA_PRINCIPAL
#define MEMORIA_PRINCIPAL

#include "cabecera.h"


extern pthread_mutex_t acceso_memoria;

#define MEM_LOCK pthread_mutex_lock(&acceso_memoria)
#define MEM_UNLOCK pthread_mutex_unlock(&acceso_memoria)


int pmrd(int pos, palabra* value, const int PSW, const int RB, const int RL);

int pmwr(int pos, palabra value, const int PSW, const int RB, const int RL);

#endif