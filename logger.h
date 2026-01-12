#ifndef LOGGER
#define LOGGER

#include "cabecera.h"


extern pthread_mutex_t logmutex;
extern FILE* LOG_F;


#define LOG_LOCK pthread_mutex_lock(&logmutex)

#define LOG_UNLOCK pthread_mutex_unlock(&logmutex)

#define TAMANO_MAX_LOG 200



flag log(char* componente, char* mensaje);
#endif