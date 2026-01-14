#include "DMA.h"


pthread_mutex_t data_dma = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t acceso_dma = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t senal_dma = PTHREAD_COND_INITIALIZER;

flag dma_pendiente = 0;
flag dma_trabajando = 0;
flag finalizar_dma = 0;
flag ESTADOdma = 0;

typedef struct{
    int pista;
    int cilindro;
    int sector;
    int io;
    int posmem;
} dma_data; 

dma_data DMA_DATA;

pthread_t ID_DMA;

void* DMA(void *param){
    
    while(1){
        DMA_LOCK;

        while(dma_pendiente == 0 && finalizar_dma == 0){
            pthread_cond_wait(&senal_dma, &acceso_dma);
        }
        if(finalizar_dma){
            DMA_UNLOCK;
            break;
        }
        dma_trabajando = 1;
        dma_pendiente = 0;

        DMA_UNLOCK;

        dma_data local_data;

        pthread_mutex_lock(&data_dma);
        local_data = DMA_DATA;
        pthread_mutex_unlock(&data_dma);

        flag secmemflag, primemflag;
        palabra dato;
        if(local_data.io){
            
            secmemflag = scrd(&dato, local_data.pista, local_data.cilindro, local_data.sector);

            MEM_LOCK;

            primemflag = pmwr(local_data.posmem, dato, 1000000, 0, 2000);

            MEM_UNLOCK;

        }
        else{
            MEM_LOCK;

            primemflag = pmrd(local_data.posmem, &dato, 1000000, 0, 2000);

            MEM_UNLOCK;

            secmemflag = scwr(dato, local_data.pista, local_data.cilindro, local_data.sector);
        }

        if(primemflag == SUCCESS && secmemflag == SUCCESS)ESTADOdma = SUCCESS;
        else ESTADOdma = FAIL;

        genInterr(4);

        DMA_LOCK;
        dma_trabajando = 0;
        DMA_UNLOCK;
    }

    return NULL;
}

void DMAON(){
    DMA_LOCK;
    dma_pendiente = 1;
    pthread_cond_signal(&senal_dma);
    DMA_UNLOCK;
    
}

void creardma() {
    pthread_create(&ID_DMA, NULL, DMA, NULL);
}

void killdma(){
    DMA_LOCK;
    finalizar_dma = 1;
    pthread_cond_signal(&senal_dma);
    DMA_UNLOCK;
    pthread_join(ID_DMA, NULL);
}

int set_pista(int pista){
    pthread_mutex_lock(&data_dma);
    DMA_DATA.pista = pista;
    pthread_mutex_unlock(&data_dma);
}
int set_cilindro(int cilindro){
    pthread_mutex_lock(&data_dma);
    DMA_DATA.cilindro = cilindro;
    pthread_mutex_unlock(&data_dma);
}
int set_sector(int sector){
    pthread_mutex_lock(&data_dma);
    DMA_DATA.sector = sector;
    pthread_mutex_unlock(&data_dma);
}
int set_posmem(int posmem){
    pthread_mutex_lock(&data_dma);
    DMA_DATA.posmem = posmem;
    pthread_mutex_unlock(&data_dma);
}
int set_io(int io){
    pthread_mutex_lock(&data_dma);
    DMA_DATA.io = io;
    pthread_mutex_unlock(&data_dma);
}

