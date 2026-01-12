#ifndef DMA_H
#define DMA_H

#include "memoriaSecundaria.h"
#include "memoriaPrincipal.h"
#include "cabecera.h"



extern pthread_mutex_t acceso_dma;
extern pthread_cond_t senal_dma;

extern flag dma_pendiente;
extern flag dma_trabajando;
extern flag finalizar_dma;
extern flag ESTADOdma;

#define DMA_LOCK pthread_mutex_lock(&acceso_dma)
#define DMA_UNLOCK pthread_mutex_unlock(&acceso_dma)

void* DMA(void *param);
void DMAON();
void creardma();
void killdma();
int set_pista(int pista);
int set_cilindro(int cilindro);
int set_sector(int sector);
int set_posmem(int posmem);
int set_io(int io);

#endif