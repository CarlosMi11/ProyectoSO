#ifndef PROCESADOR_AUX_H
#define PROCESADOR_AUX_H

#include "procesador_data.h"

int getPC(void);
void setPC(int pc);
int getInterruptions(void);
void setInterruptions(int interr);
int getCondCode(void);
void setCondCode(int condicion);
int getOpMode(void);
void setOpMode(int modo);

#endif