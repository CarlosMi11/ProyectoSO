#include "procesador_aux.h"

#define PC_SHIFT 100000

int getPC(){
    return PSW % PC_SHIFT;
}

void setPC(int pc){
    PSW = ((PSW / PC_SHIFT) * PC_SHIFT) + pc;
}

void jmp(int pos){
    setPC(pos + RB);
}
int getInterruptions(){
    return (PSW / PC_SHIFT) % 10;
}

void setInterruptions(int interr){
    int pc = getPC();
    int aux = PSW / PC_SHIFT;
    PSW = (((aux / 10) * 10) + interr) * PC_SHIFT + pc;
}

int getCondCode() {
   
    return (PSW / PC_SHIFT / 100);
}

void setCondCode(int condicion) {
    
    int resto = PSW % (PC_SHIFT * 100);
    
    
    PSW = (condicion * PC_SHIFT * 100) + resto;
}

int getOpMode() {
   
    return (PSW / PC_SHIFT / 10) % 10;
}

void setOpMode(int modo) {
    int pc = getPC();
    int interr = getInterruptions();
    int codCond = getCondCode();

    PSW = (codCond * 100 + modo * 10 + interr) * PC_SHIFT + pc;
}
