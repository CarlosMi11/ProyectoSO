#include "reloj.h"


static int TIEMPO;

int QUANTUM = 2;
flag setReloj(int tiempo){
    if(tiempo < 1){
        return FAIL;
    }
    TIEMPO = tiempo;
    return SUCCESS;
}

int getReloj(){
    return TIEMPO;
}

void tick(){
    TIEMPO -= 1;
    if(TIEMPO <= 0){
        genInterr(RELOJINTERR);
        TIEMPO = QUANTUM;
        
    }
}