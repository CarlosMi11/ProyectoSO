#ifndef memoria_principal
#define memoria_principal
#include "pthread.h"
#include "registros.h"
pthread_mutex_t acceso_memoria;
int ram[2000];

int lectura(int pos){
    if(pos <= 300){
        if((PSW/100000)%10 == 1){
            return ram[pos];
        }
        else{
            return -1;
        }
    }
    else{
        return ram[pos];
    }
    
}

bool escritura(int pos, int value){
    if(pos <= 300){
        if((PSW/100000)%10 == 1){
            ram[pos] = value;
            return true;
        }
        else return false;
    }
    else{
        ram[pos] = value;
        return true;
    }
}

#endif