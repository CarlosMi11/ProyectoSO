#ifndef PROCESADOR_AUX_H
#define PROCESADOR_AUX_H

#include "procesador_data.h"
palabra lectura(int pos, int *flag);
void escritura(int pos, const palabra value, int *flag);
int getPC(void);
void setPC(int pc);
int getInterruptions(void);
void setInterruptions(int interr);
int getCondCode(void);
void setCondCode(int condicion);
int getOpMode(void);
void setOpMode(int modo);
void jmp(int pos);
flag push(palabra AC);
flag pop(palabra *AC);
#endif