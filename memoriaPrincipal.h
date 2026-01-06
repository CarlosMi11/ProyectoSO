#ifndef memoria_principal
#define memoria_principal

#include "cabecera.h"


pthread_mutex_t acceso_memoria;

#define MEM_LOCK pthread_mutex_lock(&acceso_memoria)
#define MEM_UNLOCK pthread_mutex_unlock(&acceso_memoria)

palabra ram[2000];

int pmrd(int pos, palabra* value, const int PSW, const int RB, const int RL);

int pmwr(int pos, palabra value, const int PSW, const int RB, const int RL);

#endif