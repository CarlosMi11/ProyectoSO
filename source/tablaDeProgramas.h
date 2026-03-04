#ifndef TABLADEPROGRAMAS
#define TABLADEPROGRAMAS
#include "cabecera.h"

flag antesLeido(const char* ruta, char* nombre, int* pista, int* cilindro, int* sector);
flag cargarArchivoAMemoriaSecundaria(const char* archivo, string nombre, int* pista_, int* cilindro_, int* sector_, int* tam_);

void limpiarTablaDeProgramas();
#endif