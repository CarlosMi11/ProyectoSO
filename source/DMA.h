#ifndef DMA_H
#define DMA_H

#include "cabecera.h"

extern flag ESTADOdma;
typedef struct{
    int pista;
    int cilindro;
    int sector;
    int posmem;
    int io;
    int consola;
} solicitudIO;
int dmaOcupado();
void* DMA(void *param);
void DMAON();
void creardma();
void killdma();
void set_pista(int pista);
void set_cilindro(int cilindro);
void set_sector(int sector);
void set_posmem(int posmem);
void set_io(int io);
void set_id_proc(int idProc);
void set_nombre_proc(string nombre);
void creardma();
void matardma();
flag get_resultado(int* idProc, int* lectura);

#endif