#include "memoriaPrincipal.h"


palabra ram[2000];
pthread_mutex_t acceso_memoria = PTHREAD_MUTEX_INITIALIZER;


int pmrd(int pos, palabra* value, const int PSW, const int RB, const int RL){
    
    
    if((pos < RB || pos >= RL || pos <= 300) && (PSW/1000000)%10 != 1){
        return FAIL;
    }
    
    *value = ram[pos];
    
    return SUCCESS;
}

int pmwr(int pos, palabra value, const int PSW, const int RB, const int RL){
    if((pos < RB || pos >= RL || pos <= 300) && (PSW/1000000)%10 != 1){
        return FAIL;
    }
    
    ram[pos] = value;
    
    return SUCCESS;
}