#include "DMA.h"


static pthread_mutex_t data_dma = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t acceso_dma = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t senal_dma = PTHREAD_COND_INITIALIZER;

#define DMA_LOCK pthread_mutex_lock(&acceso_dma)
#define DMA_UNLOCK pthread_mutex_unlock(&acceso_dma)

static pthread_t ID_DMA;

static flag dma_pendiente = 0;
static flag dma_trabajando = 0;
static flag finalizar_dma = 0;

flag ESTADOdma = 0;

typedef struct{
    int pista;
    int cilindro;
    int sector;
    int io;
    int posmem;
} dma_data; 

static dma_data DMA_DATA;



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

            

            primemflag = pmwr(local_data.posmem, dato, KERNEL_MODE);

            

        }
        else{
            

            primemflag = pmrd(local_data.posmem, &dato, KERNEL_MODE);

            

            secmemflag = scwr(dato, local_data.pista, local_data.cilindro, local_data.sector);
        }

        if(primemflag == SUCCESS && secmemflag == SUCCESS)ESTADOdma = SUCCESS;
        else ESTADOdma = FAIL;
        string mensaje;
        sprintf(mensaje, "finalizada tarea de io, ESTADOdma: %s", (ESTADOdma == SUCCESS ? "SUCCESS" : "FAIL"));
        log_("DMA", mensaje);
        genInterr(FINIO);

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
    log_("DMA", "encendiendo DMA");
    DMA_UNLOCK;
    
}

void set_pista(int pista){
    pthread_mutex_lock(&data_dma);
    DMA_DATA.pista = pista;
    string mensaje;
    sprintf(mensaje, "colocando pista en %i", pista);
    log_("DMA", mensaje);
    pthread_mutex_unlock(&data_dma);
}
void set_cilindro(int cilindro){
    pthread_mutex_lock(&data_dma);
    DMA_DATA.cilindro = cilindro;
    pthread_mutex_unlock(&data_dma);
}
void set_sector(int sector){
    pthread_mutex_lock(&data_dma);
    DMA_DATA.sector = sector;
    string mensaje;
    sprintf(mensaje, "colocando sector en %i", sector);
    log_("DMA", mensaje);
    pthread_mutex_unlock(&data_dma);
}
void set_posmem(int posmem){
    pthread_mutex_lock(&data_dma);
    DMA_DATA.posmem = posmem;
    string mensaje;
    sprintf(mensaje, "colocando posicion de memoria en %i", posmem);
    log_("DMA", mensaje);
    pthread_mutex_unlock(&data_dma);
}
void set_io(int io){
    pthread_mutex_lock(&data_dma);
    DMA_DATA.io = io;
    string mensaje;
    sprintf(mensaje, "colocando io en %i", io);
    log_("DMA", mensaje);
    pthread_mutex_unlock(&data_dma);
}

void creardma() {
    log_("DMA", "CREANDO DMA");
    pthread_create(&ID_DMA, NULL, DMA, NULL);
    log_("DMA", "DMA CREADO");
}

void matardma(){
    log_("DMA", "APAGANDO DMA");
    DMA_LOCK;
    finalizar_dma = 1;
    pthread_cond_signal(&senal_dma);
    DMA_UNLOCK;
    pthread_join(ID_DMA, NULL);
    log_("DMA", "DMA APAGADO");
}
