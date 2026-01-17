#include "logger.h"

static FILE* LOG_F;
static pthread_mutex_t logmutex = PTHREAD_MUTEX_INITIALIZER;
#define LOG_LOCK pthread_mutex_lock(&logmutex)
#define LOG_UNLOCK pthread_mutex_unlock(&logmutex)

#define DIRECCIONDELOG "LOG.log"

flag log_(char* componente, char* mensaje){
    if (mensaje == NULL || componente == NULL) return FAIL;
    
    pthread_t id = pthread_self();

    char mensajeFinal[TAMANO_MAX_LOG];

    int n = snprintf(mensajeFinal, TAMANO_MAX_LOG, "[%s] Thread-%lu: %s\n", componente, (unsigned long)id, mensaje);
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


flag crearLogger(){
    
    LOG_F = fopen(DIRECCIONDELOG, "w");

    if (LOG_F == NULL) {
        
        return FAIL;
    }

    
    fprintf(LOG_F, "=== INICIO DEL LOG DEL SISTEMA ===\n");
    fflush(LOG_F);

    return SUCCESS;
}

void cerrarLogger() {
    LOG_LOCK;
    if (LOG_F != NULL) {
        fprintf(LOG_F, "=== FIN DE SIMULACION ===\n");
        fclose(LOG_F);
        LOG_F = NULL;
    }
    LOG_UNLOCK;
}
