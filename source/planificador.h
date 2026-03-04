#ifndef PLANIFICADOR
#define PLANIFICADOR

#include "proceso.h"
#include "procesador_data.h"
#include "tablaDeProgramas.h"
#include "memoriaPrincipal.h"
#include "memoriaSecundaria.h"
#include "reloj.h"



flag planificar(flag codigo);
flag crearProceso(string path);
void dormirProceso(int indProc, int tiempo);
flag despertarProceso(int indProc, int ind);
void matarProceso(int indProc, int codigo);
char* getNombreProceso(int indProc);
int todosDormidos();
flag cargarPlanificador();
flag crearPlanificador();
flag reiniciarPlanificador();
extern int cantProc;
extern int maxProc;
#endif