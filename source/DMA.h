#ifndef DMA_H
#define DMA_H

#include "memoriaSecundaria.h"
#include "memoriaPrincipal.h"
#include "interrupciones.h"
#include "logger.h"

extern flag ESTADOdma;

void* DMA(void *param);
void DMAON();
void creardma();
void killdma();
void set_pista(int pista);
void set_cilindro(int cilindro);
void set_sector(int sector);
void set_posmem(int posmem);
void set_io(int io);

void creardma();
void matardma();

#endif