#include "procesador.h"
#include "logger.h"

pthread_t ID_PROC;
pthread_t ID_DMA;

void crearProcesador(){
    estadoCPU = CPU_ESPERA;
	pthread_create(&ID_PROC, NULL, procesador, NULL);

}

void matarProcesador(){
	pthread_mutex_lock(&mutex_cpu);
    estadoCPU = CPU_APAGAR;
    
    pthread_cond_signal(&cond_start);
    
    pthread_mutex_unlock(&mutex_cpu);

    pthread_join(ID_PROC, NULL);
}

void creardma() {
    pthread_create(&ID_DMA, NULL, DMA, NULL);
}

void matardma(){
    DMA_LOCK;
    finalizar_dma = 1;
    pthread_cond_signal(&senal_dma);
    DMA_UNLOCK;
    pthread_join(ID_DMA, NULL);
}

flag crearLogger(){
    
    LOG_F = fopen("LOG.log", "w");

    if (LOG_F == NULL) {
        
        return FAIL;
    }

    
    fprintf(LOG_F, "=== INICIO DEL LOG DEL SISTEMA ===\n");
    fflush(LOG_F);

    return SUCCESS;
}

void cerrarLogger() {
    LOG_LOCK;
    if (LOG_F != NULL) {
        fprintf(LOG_F, "=== FIN DE SIMULACION ===\n");
        fclose(LOG_F);
        LOG_F = NULL;
    }
    LOG_UNLOCK;
}

void limpiarLinea(char *line) {
    char *comment = strstr(line, "//"); 
    if (comment) {
        *comment = '\0'; 
    }
    
    line[strcspn(line, "\n")] = 0;
    line[strcspn(line, "\r")] = 0;
}

int cargarPrograma(proceso* actual, char *nombreArchivo, int rb) {
    
    FILE *fp = fopen(nombreArchivo, "r");
    if (fp == NULL) {
        return FAIL; 
    }
    if (rb < 300) {
        fclose(fp);
        return FAIL;
    }

    char linea[128];
    int offset = 0;        
    int punto_entrada = 0; 
    int num_palabras = 0;  
    char nombre_prog[50];  

    printf("Cargando en dirección %d...\n", rb);

    while (fgets(linea, sizeof(linea), fp)) {
        
        
        linea[strcspn(linea, "\n")] = 0;

        
        char *comentario = strstr(linea, "//");
        if (comentario != NULL) {
            *comentario = '\0'; 
        }

        if (strlen(linea) == 0) continue; 

        
        if (strstr(linea, "_start")) {
            sscanf(linea, "_start %d", &punto_entrada);
            continue;
        }
        if (strstr(linea, ".NumeroPalabras")) {
            sscanf(linea, ".NumeroPalabras %d", &num_palabras);
            continue;
        }
        if (strstr(linea, ".NombreProg")) {
            sscanf(linea, ".NombreProg %s", actual->nombre);
            continue;
        }

        char *ptr = linea;
        while(isspace((unsigned char)*ptr)) ptr++;
        
        if(*ptr == '\0') continue; 

        // Verificamos protección de memoria antes de escribir
        if (rb + offset >= 2000) {
            printf("ERROR: Desbordamiento de memoria RAM.\n");
            return FAIL;
        }

        long instruccion = atol(ptr);

        pmwr(rb + offset, instruccion, 1000000, 0, 2000);
        
        offset++;
    }

    fclose(fp);

    pmwr(rb + offset, -1, 1000000, 0, 2000);//final de programa
    offset++;
    
    actual->rb = rb;                 
    actual->rl = rb + offset - 1;    
    actual->psw = 100000 + (rb + punto_entrada - 1); // [0][0][1][PC]
    
    printf("Programa '%s' cargado exitosamente.\n", actual->nombre);

    return SUCCESS; 
}
int crearProceso(proceso* actual, char *nombreArchivo, int rb, int *tamanoFinal){
    
    int tamanoDePila = 100;
    if (cargarPrograma(actual, nombreArchivo, rb) != SUCCESS) {
        return FAIL; // propaga el error si no se pudo cargar
    }
    
    actual->rx = 0;
    actual->sp = actual->rx; //la pila empieza con tope y base iguales (vacia)
    actual->rl += tamanoDePila; //se ajusta el final de el espacio de memoria del programa para que incluya la pila

    
    *tamanoFinal = actual->rl - actual->rb + 1;

    return SUCCESS;
}
const char* NombreInstruccion(int opcode) {
    static const char* mnemonicos[] = {
        "SUM", "RES", "MULT", "DIVI", "LOAD", "STR", "LOADRX", "STRRX", 
        "COMP", "JMPE", "JMPNE", "JMPLT", "JMPGT", "SVC", "RETRN", 
        "HAB", "DHAB", "TTI", "CHMOD", "LOADRB", "STRRB", "LOADRL", 
        "STRRL", "LOADSP", "STRSP", "PSH", "POP", "J", "SDMAP", 
        "SDMAC", "SDMAS", "SDMAIO", "SDMAM", "SDMAON"
    };
    
    if (opcode >= 0 && opcode <= 33) {
        return mnemonicos[opcode];
    }
    return "UNKNOWN";
}
int menuDebug(proceso *p) {
    char comando[100];
    
    // 1. Decodificar la instrucción que ACABA de ejecutarse
    // Nota: Como el ciclo ya terminó, el PC ya apunta a la siguiente.
    // Usamos IR para saber qué se ejecutó.
    int opcode_actual = IR / 1000000;
    int operando_actual = IR % 100000;
    
    printf("\n--- debug ---\n");
    
    if(finalizo){
        printf("-'%s' FINALIZO de forma %s\n",p->nombre, (valorFinalizacion == SUCCESS ? "EXITOSA" : "ERRONEA"));
    }
    //PC-1 porque PC ya avanzó
    printf("- Ejecutado en DIR %d:%s (Op: %d, Val: %d)\n", getPC() - 1, NombreInstruccion(opcode_actual), opcode_actual, operando_actual);

    // Resultado

    printf("- Resultado (AC): %d\n", AC); 
    while (1) {
        printf("\n[n=next, r=ver registro, q=abortar] - ");
        
        if (fgets(comando, sizeof(comando), stdin) == NULL) continue;
        comando[strcspn(comando, "\n")] = 0;
        comando[strcspn(comando, "\r")] = 0;
        
        if (strlen(comando) == 0) continue;

        if (strcmp(comando, "n") == 0) {
            return 1; 
        }
        else if (strcmp(comando, "q") == 0) {
            printf("Abortando ejecución del programa.\n");
            return 0; 
        }
        else if (strcmp(comando, "r") == 0) {
            char reg[32];
            printf("nombre del registro: \n-");
            if (fgets(reg, sizeof(reg), stdin) == NULL) continue;
            reg[strcspn(reg, "\n")] = 0;
            reg[strcspn(reg, "\r")] = 0;
            
            // Comparar y mostrar
            if (strcasecmp(reg, "AC") == 0) printf("AC = %d\n", AC);
            else if (strcasecmp(reg, "PC") == 0) printf("PC = %d\n", getPC());
            else if (strcasecmp(reg, "SP") == 0) printf("SP = %d\n", SP);
            else if (strcasecmp(reg, "RX") == 0) printf("RX = %d\n", RX);
            else if (strcasecmp(reg, "RB") == 0) printf("RB = %d\n", RB);
            else if (strcasecmp(reg, "RL") == 0) printf("RL = %d\n", RL);
            else if (strcasecmp(reg, "MAR") == 0) printf("MAR = %d\n", MAR);
            else if (strcasecmp(reg, "MDR") == 0) printf("MDR = %d\n", MDR);
            else if (strcasecmp(reg, "IR") == 0) printf("IR = %08d\n", IR);
            else {
                printf("Registro '%s' desconocido.\n", reg);
                printf("Registros: AC, PC, MAR, MDR, RB, RL, RX, SP, IR\n");
            }
        }
        else {
            printf("Comando desconocido. Use 'n' para siguiente paso.\n");
        }
    }
    return 0;
}
void Ejecutar(char *nombreArchivo, int modo){
    printf("empezando carga del programa...");
    proceso actual;
    int tamanodelProceso;
    if(crearProceso(&actual, nombreArchivo, 350, &tamanodelProceso)!=SUCCESS)return;
    actual.modoEjec = modo;
    //establecer registros del procesador

    RB = actual.rb;
    RL = actual.rl;
    SP = actual.sp;
    RX = actual.rx;
    PSW = actual.psw;
    
    IR = 0;
    AC = 0;
    finalizo = 0;
    if(modo == 2){
        while(!finalizo){
            activarProcesador(modo);
            int ret = menuDebug(&actual);
            if(ret == 0)break;
        }
    }
    else{
        activarProcesador(modo);
        printf("-'%s' FINALIZO de forma %s\n",actual.nombre, (valorFinalizacion == SUCCESS ? "EXITOSA" : "ERRONEA"));
    }
    

}

int main(int contv, char* argv[]){
    crearLogger();
    crearProcesador();
    creardma();
    

    char comando[100];
    char archivo[100];
    int modo;
    
    
    printf("=== CONSOLA DEL SISTEMA ===\n");

    while (1) { 
        printf("\nComandos:\nsalir\nejecutar\n");//<archivo> <modo (DEBUG|NORMAL)>
        printf("SO-SIM > ");
        if (fgets(comando, sizeof(comando), stdin) == NULL) continue;
        comando[strcspn(comando, "\n")] = 0;

        
        if (strcmp(comando, "salir") == 0) {
            printf("Apagando simulación...\n");
            break; 
        }
        else if (strcmp(comando, "ejecutar") == 0) {
            
            printf("\nDireccion del archivo: (sin espacios, máximo 100 caracteres)\n");
            printf("SO-SIM > ");
            if (fgets(archivo, sizeof(archivo), stdin) == NULL) continue;
            archivo[strcspn(archivo, "\n")] = 0;

            printf("\nModo de ejecución: (1 = normal, 2 = debug): \n");
            printf("SO-SIM > ");
            if (fgets(comando, sizeof(comando), stdin) == NULL) continue;
            modo = atoi(comando);
            
            if (strlen(archivo) > 0 && (modo == 1 || modo == 2)) {
                
                Ejecutar(archivo, modo); 
            } else {
                printf("Entrada inválida.\n");
            }
        }
        else {
            printf("Comando desconocido: %s\n", comando);
        }
    }

    matardma();
    matarProcesador();
    cerrarLogger();
}