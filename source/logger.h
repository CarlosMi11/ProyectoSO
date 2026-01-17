#ifndef LOGGER
#define LOGGER

#include "cabecera.h"

#define TAMANO_MAX_LOG 200

flag log_(char* componente, char* mensaje);

flag crearLogger();

void cerrarLogger();


#endif