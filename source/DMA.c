#include "DMA.h"
#include "memoriaSecundaria.h"
#include "memoriaPrincipal.h"
#include "interrupciones.h"
#include "logger.h"

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
    //identificador del proceso
    int idProc;
    
    //para lectura por MEM_SEC
    int pista;
    int cilindro;
    int sector;
    int posmem;
    int io;

    //para lectura por consola
    int consola;
    string nombreProceso;
    int lectura;
} dataDMA;

dataDMA DMA_DATA;

flag IO_MEMSEC(){
    solicitudIO local_data;

    pthread_mutex_lock(&data_dma);
    local_data.cilindro = DMA_DATA.cilindro;
    local_data.posmem = DMA_DATA.posmem;
    local_data.pista = DMA_DATA.pista;
    local_data.io = DMA_DATA.io;
    local_data.sector = DMA_DATA.sector;
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
    if(primemflag == SUCCESS && secmemflag == SUCCESS)return SUCCESS;
    return FAIL;
}

flag IO_CONSOLA(){
    string mensaje;
    pthread_mutex_lock(&data_dma);
    printf("SO-SIM> %s>> ", DMA_DATA.nombreProceso);
    pthread_mutex_unlock(&data_dma);


    palabra a;
	scanf("%li", &a);


    pthread_mutex_lock(&data_dma);
    DMA_DATA.lectura = a;
    pthread_mutex_unlock(&data_dma);


	snprintf(mensaje, TAM_MENSAJE, "lectura por pantalla: %ld", a);
	log_("DMA", mensaje); 
    return SUCCESS;
}
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

        

        if(DMA_DATA.consola == 0){
            ESTADOdma = IO_MEMSEC();
        }
        else{
            ESTADOdma = IO_CONSOLA();
        }
        string mensaje;
        sprintf(mensaje, "finalizada tarea de io, ESTADOdma: %s", (ESTADOdma == SUCCESS ? "SUCCESS" : "FAIL"));
        log_("DMA", mensaje);
        genInterr(FINIO);

        
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
int dmaOcupado(){
    return dma_trabajando;
}
void set_pista(int pista){
    pthread_mutex_lock(&data_dma);
    DMA_DATA.pista = pista;
    pthread_mutex_unlock(&data_dma);

    string mensaje;
    sprintf(mensaje, "colocando pista en %i", pista);
    log_("DMA", mensaje);
    
}
void set_cilindro(int cilindro){
    pthread_mutex_lock(&data_dma);
    DMA_DATA.cilindro = cilindro;
    pthread_mutex_unlock(&data_dma);

    string mensaje;
    sprintf(mensaje, "colocando cilindro en %i", cilindro);
    log_("DMA", mensaje);
}
void set_sector(int sector){
    pthread_mutex_lock(&data_dma);
    DMA_DATA.sector = sector;
    pthread_mutex_unlock(&data_dma);

    string mensaje;
    sprintf(mensaje, "colocando sector en %i", sector);
    log_("DMA", mensaje);
   
}
void set_posmem(int posmem){
    pthread_mutex_lock(&data_dma);
    DMA_DATA.posmem = posmem;
    pthread_mutex_unlock(&data_dma);

    string mensaje;
    sprintf(mensaje, "colocando posicion de memoria en %i", posmem);
    log_("DMA", mensaje);
    
}
void set_io(int io){
    pthread_mutex_lock(&data_dma);
    DMA_DATA.io = io;
    pthread_mutex_unlock(&data_dma);

    string mensaje;
    sprintf(mensaje, "colocando io en %i", io);
    log_("DMA", mensaje);
    
}
void set_I_consola(int consola){
    pthread_mutex_lock(&data_dma);
    DMA_DATA.consola = consola;
    pthread_mutex_unlock(&data_dma);

    string mensaje;
    sprintf(mensaje, "colocando consola en %i", consola);
    log_("DMA", mensaje);
    
}
void set_id_proc(int idProc){
    pthread_mutex_lock(&data_dma);
    DMA_DATA.idProc = idProc;
    pthread_mutex_unlock(&data_dma);

    string mensaje;
    sprintf(mensaje, "colocando indice del proceso en %i", idProc);
    log_("DMA", mensaje);
    
}
void set_nombre_proc(string nombre){
    pthread_mutex_lock(&data_dma);
    strcpy(DMA_DATA.nombreProceso, nombre);
    pthread_mutex_unlock(&data_dma);

    string mensaje;
    sprintf(mensaje, "colocando nombre del proceso en %s", nombre);
    log_("DMA", mensaje);
    
}
flag get_resultado(int* idProc, int* lectura){
    pthread_mutex_lock(&data_dma);
    *idProc = DMA_DATA.idProc;
    *lectura = DMA_DATA.lectura;
    pthread_mutex_unlock(&data_dma);
    DMA_LOCK;
    dma_trabajando = 0;
    DMA_UNLOCK;
    return ESTADOdma;
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
