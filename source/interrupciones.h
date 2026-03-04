#ifndef INTERRUPCIONES
#define INTERRUPCIONES

#include "cabecera.h"

typedef enum {
    LLAMADAINVALIDA,        
    CODIGOINTERRUPCIOINVALIDO,      
    LLAMADASISTEMA,    
    RELOJINTERR,    
    FINIO,
    INSTRUCCIONINVALIDA,
    DIRECCIONINVALIDA,
    UNDERFLOW,
    OVERFLOW  
} Interrupcion;

int manInterr();
void genInterr(Interrupcion a);
void limpiarInterrupciones();

#endif