#ifndef DMA_H
#define DMA_H

#include "memoriaSecundaria.h"
#include "memoriaPrincipal.h"
#include "interrupciones.h"





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
void set_pista(int pista);
void set_cilindro(int cilindro);
void set_sector(int sector);
void set_posmem(int posmem);
void set_io(int io);

#endif