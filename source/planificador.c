#include "Planificador.h"
#include "queue.h"

proceso PROCESOS[MAX_PROCESOS];
int maxProc = 0;
int cantProc = 0;


Queue LISTOq;
Queue DORMIDOq;
int todosDormidos(){
    return DORMIDOq.cantidad == cantProc;
}
static void impresion(char *linea){
    printf("%s\n", linea);
    log_("Planificador", linea);
}
void guardarContexto(){
	
	PROCESOS[PROC_IND].MAR = MAR;
	PROCESOS[PROC_IND].MDR = MDR;
	PROCESOS[PROC_IND].IR = IR;
	PROCESOS[PROC_IND].RB = RB;
	PROCESOS[PROC_IND].RL = RL;
	PROCESOS[PROC_IND].RX = RX;
	PROCESOS[PROC_IND].SP = SP;
	PROCESOS[PROC_IND].PSW = PSW;
	PROCESOS[PROC_IND].AC = AC;
}

void restaurarContexto(){	
	
	MAR = PROCESOS[PROC_IND].MAR;
	MDR = PROCESOS[PROC_IND].MDR;
	IR  = PROCESOS[PROC_IND].IR;
	RB  = PROCESOS[PROC_IND].RB;
	RX  = PROCESOS[PROC_IND].RX;
	RL  = PROCESOS[PROC_IND].RL;
	SP  = PROCESOS[PROC_IND].SP;
	PSW = PROCESOS[PROC_IND].PSW;
	AC  = PROCESOS[PROC_IND].AC;
}

void cargarProcesoEnMemoria(proceso *proces, int pista, int cilindro, int sector){
    //la memoria estara segmentada en espacios de 85 palabras (para 20 procesos)
    int posIni = 300 + MAX_TAM_PROC * maxProc;
    palabra pal;

    
    scrd(&pal, pista, cilindro, sector); //punto de entrada
    nextPosMemSec(&pista, &cilindro, &sector, 1);
    int puntoEntrada = pal;

    scrd(&pal, pista, cilindro, sector); //numero de palabras
    nextPosMemSec(&pista, &cilindro, &sector, 1);
    int n = pal;

    // MAR, MDR, IR, RB, RL, RX, SP, PSW
    proces->PSW = 100000 + puntoEntrada;
    proces->IR = 0;
    proces->MAR = 0;
    proces->MDR = 0;
    proces->RX = proces->SP =  n + 1; //la pila empieza con tope y base iguales (vacia)
    proces->RB = posIni;
    proces->RL = posIni + MAX_TAM_PROC;

    for(int i = 0; i < n; i++){
        scrd(&pal, pista, cilindro, sector); //instruccion
        nextPosMemSec(&pista, &cilindro, &sector, 1);
        pmwr(proces->RB + i + 1, pal, KERNEL_MODE);
    }
    pmwr(proces->RB + n + 1, -1, KERNEL_MODE);
}

flag crearProceso(string path){
    string mensaje;
    string nombre;
    if(cantProc > 20){
        impresion("ERROR: LIMITE DE PROCESOS ALCANZADO");
        return FAIL;
    }
    int pista, cilindro, sector, tam;
    flag leido = antesLeido(path, nombre, &pista, &cilindro, &sector);
    
    if(leido == FAIL){
        snprintf(mensaje, TAM_MENSAJE, "Cargando archivo %s", path);
        impresion(mensaje);
        if(cargarArchivoAMemoriaSecundaria(path, nombre, &pista, &cilindro, &sector, &tam) == FAIL){
            return FAIL;
        }
    }else{
        snprintf(mensaje, TAM_MENSAJE, "programa %s anteriormente leido, cargando de P(%i) C(%i) S(%i)", nombre, pista, cilindro, sector);
        impresion(mensaje);
        
    }
    
    
    PROCESOS[maxProc].estado = NUEVO;
    PROCESOS[maxProc].AC = 0;
    PROCESOS[maxProc].valorFinalizacion = FAIL;
    strcpy(PROCESOS[maxProc].idProceso, nombre);

    cargarProcesoEnMemoria(&PROCESOS[maxProc], pista, cilindro, sector);
    //en cargar proceso, se le asigna la memoria al proceso y los registros:
    //MAR, MDR, IR, RB, RL, RX, SP

    colaInsertar(&LISTOq, maxProc); 
    PROCESOS[maxProc].estado = LISTO;
    
    snprintf(mensaje, TAM_MENSAJE, "Programa '%s' cargado exitosamente", nombre);
    impresion(mensaje);
    snprintf(mensaje, TAM_MENSAJE, "Proceso: RB=%ld, RL=%ld, SP=%ld, RX=%ld, PSW=%ld", PROCESOS[maxProc].RB, PROCESOS[maxProc].RL, PROCESOS[maxProc].SP, PROCESOS[maxProc].RX, PROCESOS[maxProc].PSW);
    log_("Planificador", mensaje);
    maxProc++;
    cantProc++;
    return SUCCESS;
}
void dormirProceso(int indProc, int tiempo){
    //ninguno de estos casos deberian ocurrir... pero porsialasmoscas
    if(PROCESOS[indProc].estado == DORMIDO) return;
    if(PROCESOS[indProc].estado == FINALIZADO) return;
    if(PROCESOS[indProc].estado == NUEVO) return;

    //Este caso tampoco deberia ocurrir... pero porsialasmoscas
    if(PROCESOS[indProc].estado == LISTO){
        for(int i = LISTOq.inicio; i < LISTOq.final; i = (i+1)%20){
            if(LISTOq.cola[i] == indProc){
                eliminarDeCola(&LISTOq, i);
                break;
            }
        }
    }

    //ahora si, lo duermo
    PROCESOS[indProc].tiempoDeDormido = tiempo;
    colaInsertar(&DORMIDOq, indProc);
    PROCESOS[indProc].estado = DORMIDO;
}
char* getNombreProceso(int indProc){
    return PROCESOS[indProc].idProceso;
}
flag despertarProceso(int indProc, int ind){
    if(DORMIDOq.cantidad == 0){
        return FAIL;
    }
    //si no me dieron el indice, lo busco
    if(ind == -1){
        for(int i = DORMIDOq.inicio; i < DORMIDOq.final; i = (i+1)%20){
        int indiceDelProceso = DORMIDOq.cola[i];
            if(indProc == indiceDelProceso){
                ind = i;
            }
        }
    }
    if(ind == -1){ //si aun asi no está, retorno FAIL
        return FAIL;
    }
    eliminarDeCola(&DORMIDOq, ind);
    colaInsertar(&LISTOq, indProc); 
    PROCESOS[indProc].estado = LISTO;
    string mensaje;
    snprintf(mensaje, TAM_MENSAJE, "Proceso %s DESPERTADO", PROCESOS[maxProc].idProceso);
    log_("Planificador", mensaje);
    return SUCCESS;
}
void matarProceso(int indProc, int valorFinalizacion){
    //de nuevo, casos que no deberian ocurrir pero ajá
    if(PROCESOS[indProc].estado == FINALIZADO) return;
    if(PROCESOS[indProc].estado == NUEVO) return;

    if(PROCESOS[indProc].estado == DORMIDO){
        despertarProceso(indProc, -1);
    }
    //Este caso tampoco deberia ocurrir... pero porsialasmoscas
    if(PROCESOS[indProc].estado == LISTO){
        for(int i = LISTOq.inicio; i < LISTOq.final; i = (i+1)%20){
            if(LISTOq.cola[i] == indProc){
                eliminarDeCola(&LISTOq, i);
                break;
            }
        }
    }
    //ahora si, lo mato (no está en ninguna cola y le cambio el estado a FINALIZADO para que al planificar no lo inserten en listo)

    string mensaje;
    snprintf(mensaje, TAM_MENSAJE, "Programa '%s' finalizado de forma: %s (Cod: %i)", PROCESOS[indProc].idProceso, (valorFinalizacion == 0 ? "EXITOSA" : "FALLIDA"), valorFinalizacion);
    impresion(mensaje);
    
    PROCESOS[indProc].valorFinalizacion = valorFinalizacion;
    PROCESOS[indProc].estado = FINALIZADO;
    cantProc-=1;
}
void restarContadoresDeSleep(){
    for(int i = DORMIDOq.inicio; i < DORMIDOq.final; i = (i+1)%20){
        int indiceDelProceso = DORMIDOq.cola[i];
        if(PROCESOS[indiceDelProceso].tiempoDeDormido > 1){
            PROCESOS[indiceDelProceso].tiempoDeDormido -= 1;
        }
        else if(PROCESOS[indiceDelProceso].tiempoDeDormido == 1){
            PROCESOS[indiceDelProceso].tiempoDeDormido = 0;
            despertarProceso(indiceDelProceso, i);
        }
    }
}

flag planificar(flag codigo){
    string mensaje;
    restarContadoresDeSleep();

    if(codigo == FAIL){//codigo FAIL indica cambio de contexto
        setReloj(QUANTUM); 
        guardarContexto();

        if(PROCESOS[PROC_IND].estado == EJECUTANDO){ //solo moverlo a la cola de listos si llego acá desde EJECUTANDO
            //mover a la cola de listos
            PROCESOS[PROC_IND].estado = LISTO;
            colaInsertar(&LISTOq, PROC_IND); 
            
            snprintf(mensaje, TAM_MENSAJE, "Proceso %s movido a LISTO", PROCESOS[PROC_IND].idProceso);
            log_("Planificador", mensaje);
        }

        //colocar al siguiente        
        int indSig = colaExtraer(&LISTOq); 

        PROC_IND = indSig;
        restaurarContexto();
        PROCESOS[PROC_IND].estado = EJECUTANDO;
        snprintf(mensaje, TAM_MENSAJE, "Proceso %s colocado en EJECUCION", PROCESOS[PROC_IND].idProceso);
        log_("Planificador", mensaje);
    }
    return SUCCESS;
}
flag cargarPlanificador(){
    //se coloca el primer proceso
    int indSig = colaExtraer(&LISTOq); 

    PROC_IND = indSig;
    restaurarContexto();
    string mensaje;
    PROCESOS[PROC_IND].estado = EJECUTANDO;
    snprintf(mensaje, TAM_MENSAJE, "Proceso %s colocado en EJECUCION", PROCESOS[PROC_IND].idProceso);
    log_("Planificador", mensaje);
    return SUCCESS;
}
flag crearPlanificador(){
    log_("Planificador", "Creando colas Dormidoq y Listoq");
    crearCola(&LISTOq);
    crearCola(&DORMIDOq);
    maxProc = cantProc = 0;
    
    limpiarTablaDeProgramas();
    return SUCCESS;
}
flag reiniciarPlanificador(){
    impresion("Limpieza de procesos en el sistema.");
    crearCola(&LISTOq);
    crearCola(&DORMIDOq);
    maxProc = cantProc = 0;
    impresion("Procesos deindexados.");
    limpiarTablaDeProgramas();
    impresion("Tabla de programas reiniciada.");
    return SUCCESS;
}