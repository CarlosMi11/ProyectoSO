#include "reloj.h"


static int TIEMPO;


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
        genInterr(3);
        TIEMPO = 12;
        
    }
}