#include "logger.h"

static FILE* LOG_F;
static pthread_mutex_t logmutex = PTHREAD_MUTEX_INITIALIZER;
#define LOG_LOCK pthread_mutex_lock(&logmutex)
#define LOG_UNLOCK pthread_mutex_unlock(&logmutex)

#define DIRECCIONDELOG "LOG.log"
const char* puntos = "......................"; 
flag log_(char* componente, char* mensaje){

    

    // Dentro de log_...
    int lenComp = strlen(componente);
    int numPuntos = 24 - lenComp;
    if (numPuntos < 0) numPuntos = 0;

    
    if (mensaje == NULL || componente == NULL) return FAIL;
    
    pthread_t id = pthread_self();

    char mensajeFinal[TAMANO_MAX_LOG];
    int n;
    if(SHOW_THREAD_ID) {
        n = snprintf(mensajeFinal, TAMANO_MAX_LOG, "[%s%.*s] Thread-%-10lu: %s\n", componente, numPuntos, puntos,(unsigned long)id, mensaje);
        
    } else {
        n = snprintf(mensajeFinal, TAMANO_MAX_LOG, "[%s%.*s]: %s\n", componente, numPuntos, puntos, mensaje);
    }
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
