#include "cargadorDePrograma.h"




static void impresion(char *linea){
    printf("%s\n", linea);
    log_("Cargador de Programa", linea);
}

flag Ejecutar(char *primerArchivo){
    
    cantProc = maxProc = 0;
    impresion("empezando carga de programas");

    char *actual = primerArchivo;
    crearProceso(actual);
    
    actual = strtok(NULL, " ");

    while(actual != NULL && actual != '\n') {
        
        crearProceso(actual);
        actual = strtok(NULL, " ");
    }
    
    activarProcesador();

    return SUCCESS;    
}
