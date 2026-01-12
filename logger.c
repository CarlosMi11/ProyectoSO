#include "logger.h"

FILE* LOG_F;
pthread_mutex_t logmutex;

flag log(char* componente, char* mensaje){
    if (mensaje == NULL || componente == NULL) return FAIL;
    
    pthread_t id = pthread_self();

    char mensajeFinal[TAMANO_MAX_LOG];

    int n = snprintf(mensaje, TAMANO_MAX_LOG, "%s (%lu): %s \n", componente, (unsigned long)id, mensaje);
    if (n < 0 || n >= TAMANO_MAX_LOG) return FAIL;

    LOG_LOCK;

    if (fprintf(LOG_F, "%s", mensajeFinal) < 0 ) {
        LOG_UNLOCK; 
        return FAIL;
    }
    if (fflush(LOG_F) != 0) {
        LOG_UNLOCK;
        return FAIL;
    }

    LOG_UNLOCK;

    return SUCCESS;
}
