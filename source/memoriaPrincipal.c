#include "memoriaPrincipal.h"
#include "interrupciones.h"
#include "logger.h"

static palabra ram[2000];
static pthread_mutex_t acceso_memoria = PTHREAD_MUTEX_INITIALIZER;


#define MEM_LOCK pthread_mutex_lock(&acceso_memoria)
#define MEM_UNLOCK pthread_mutex_unlock(&acceso_memoria)

int pmrd(int pos, palabra* value, const int PSW, const int RB, const int RL){
    
    char mensaje[200];
    if((pos < RB || pos >= RL || pos < 300) && (PSW/1000000)%10 != 1){
        genInterr(6);
        
        snprintf(mensaje, 200, 
            "ERROR: intento de lectura en %i con RB=%d y RL=%d y modo de operacion=%i", 
            pos, RB, RL, (PSW/1000000)%10);
        log_("memoriaPrincipal", mensaje);

        return FAIL;
    }

    MEM_LOCK;
    *value = ram[pos];
    MEM_UNLOCK;

    snprintf(mensaje, 200, "lectura exitosa en %i", pos);
    log_("memoriaPrincipal", mensaje);
    return SUCCESS;
}

int pmwr(int pos, palabra value, const int PSW, const int RB, const int RL){
    char mensaje[200];
    if((pos < RB || pos >= RL || pos < 300) && (PSW/1000000)%10 != 1){
        genInterr(6);
        
        snprintf(mensaje, 200, 
            "ERROR: intento de escritura en %i con RB=%d y RL=%d y modo de operacion=%i", 
            pos, RB, RL, (PSW/1000000)%10);
        log_("memoria principal", mensaje);
        return FAIL;
    }
    
    MEM_LOCK;
    ram[pos] = value;
    MEM_UNLOCK;


    snprintf(mensaje, 200, "escritura exitosa en %i", pos);
    log_("memoria principal", mensaje);
    return SUCCESS;
}