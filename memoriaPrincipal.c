#include "memoriaPrincipal.h"

int pmrd(int pos, palabra* value, const int PSW, const int RB, const int RL){
    if(pos <= 300 && (PSW/100000)%10 != 1){
        return FAIL;
    }
    
    else if(pos < RB || pos >= RL){
        return FAIL;
    }
    
    *value = ram[pos];
    
    return SUCCESS;
}

int pmwr(int pos, palabra value, const int PSW, const int RB, const int RL){
    if(pos <= 300 && (PSW/100000)%10 != 1){
        return FAIL;
    }
    
    else if(pos < RB || pos >= RL){
        return FAIL;
    }
    
    ram[pos] = value;
    
    return SUCCESS;
}