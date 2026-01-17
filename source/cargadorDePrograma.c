#include "cargadorDePrograma.h"


static char mensaje[200];

void limpiarLinea(char *line) {
    char *comment = strstr(line, "//"); 
    if (comment) {
        *comment = '\0'; 
    }
    
    line[strcspn(line, "\n")] = 0;
    line[strcspn(line, "\r")] = 0;
}
void impresion(char *linea){
    printf("%s\n", linea);
    log_("Cargador de Programa", linea);
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
    int offset = 1;         
    int punto_entrada = 0; 
    int num_palabras = 0;  
    int palabras_leidas = 0; 
    int flag_num_palabras_leido = 0; 

    snprintf(mensaje,200, "Cargando en dirección %d", rb);
    impresion(mensaje);

    while (fgets(linea, sizeof(linea), fp)) {
        
        if (flag_num_palabras_leido && palabras_leidas >= num_palabras) {
            break; 
        }

        linea[strcspn(linea, "\n")] = 0;

        
        char *comentario = strstr(linea, "//");
        if (comentario != NULL) {
            *comentario = '\0'; 
        }

        
        char *ptr = linea;
        while(isspace((unsigned char)*ptr)) ptr++;
        if (*ptr == '\0') continue; 

        
        if (strstr(linea, "_start")) {
            sscanf(linea, "_start %d", &punto_entrada);
            continue;
        }
        if (strstr(linea, ".NumeroPalabras")) {
            sscanf(linea, ".NumeroPalabras %d", &num_palabras);
            flag_num_palabras_leido = 1; 
            continue;
        }
        if (strstr(linea, ".NombreProg")) {
            sscanf(linea, ".NombreProg %s", actual->nombre);
            continue;
        }

        
        if (rb + offset >= 2000) {
            printf("ERROR: Desbordamiento de memoria RAM.\n");
            return FAIL;
        }

        long instruccion = atol(ptr);
        
        pmwr(rb + offset, instruccion, KERNEL_MODE); 

        offset++;
        palabras_leidas++;
    }

    if (flag_num_palabras_leido && palabras_leidas < num_palabras) {
        snprintf(mensaje,200, "WARNING: Archivo incompleto. %d / %d palabras leídas.", 
                palabras_leidas, num_palabras);
        impresion(mensaje);
    }
    fclose(fp);

    pmwr(rb + offset, -1, KERNEL_MODE);//final de programa
    offset++;
    
    actual->rb = rb;                 
    actual->rl = rb + offset;    
    actual->psw = 100000 + (rb + punto_entrada); // [0][0][1][PC]
    
    

    return SUCCESS; 
}
int crearProceso(proceso* actual, char *nombreArchivo, int rb, int *tamanoFinal){
    
    int tamanoDePila = 100;
    if (cargarPrograma(actual, nombreArchivo, rb) != SUCCESS) {
        return FAIL; // propaga el error si no se pudo cargar
    }
    
    actual->rx = actual->rl - actual->rb; //la pila empieza justo despues del final de programa
    actual->sp = actual->rx; //la pila empieza con tope y base iguales (vacia)
    actual->rl += tamanoDePila; //se ajusta el final de el espacio de memoria del programa para que incluya la pila

    
    *tamanoFinal = actual->rl - actual->rb + 1;

    return SUCCESS;
}
const char* NombreInstruccion(int opcode) {
    static const char* nombres[] = {
        "SUM", "RES", "MULT", "DIVI", "LOAD", "STR", "LOADRX", "STRRX", 
        "COMP", "JMPE", "JMPNE", "JMPLT", "JMPGT", "SVC", "RETRN", 
        "HAB", "DHAB", "TTI", "CHMOD", "LOADRB", "STRRB", "LOADRL", 
        "STRRL", "LOADSP", "STRSP", "PSH", "POP", "J", "SDMAP", 
        "SDMAC", "SDMAS", "SDMAIO", "SDMAM", "SDMAON"
    };
    
    if (opcode >= 0 && opcode <= 33) {
        return nombres[opcode];
    }
    return "UNKNOWN";
}
int menuDebug() {
    char comando[100];
    
    // Decodificar la instrucción que ACABA de ejecutarse
    // Nota: Como el ciclo ya terminó, el PC ya apunta a la siguiente.
    // Usamos IR para saber qué se ejecutó.
    int opcode_actual = IR / 1000000;
    int direccionamientoActual = (IR / 100000) % 10;
    int operando_actual = IR % 100000;
    
    printf("\n--- debug ---\n");
    
    if(finalizo){
        return 0;
    }
    //PC-1 porque PC ya avanzó
    printf("- Ejecutado en DIR %d:%s (Op: %d, Dir: %s, Val: %d)\n", 
        getPC() - 1, 
        NombreInstruccion(opcode_actual), 
        opcode_actual,
        (direccionamientoActual == 0 ? ("directo") : (direccionamientoActual == 1 ? "inmediato" : "indexado")), 
        operando_actual
    );

    // Resultado

    printf("- Resultado (AC): %ld\n", AC); 

    // 2. Menu de debug
    while (1) {
        printf("\n[n=next, r=ver registro, m=ver memoria, q=abortar]\n");
        printf("SO-SIM > ");
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
            printf("nombre del registro: \n");
            printf("SO-SIM > ");
            if (fgets(reg, sizeof(reg), stdin) == NULL) continue;
            reg[strcspn(reg, "\n")] = 0;
            reg[strcspn(reg, "\r")] = 0;
            
            // Comparar y mostrar
            if      (strcasecmp(reg, "AC") == 0)  printf("AC = %ld\n", AC);
            else if (strcasecmp(reg, "PC") == 0)  printf("PC = %d\n", getPC());
            else if (strcasecmp(reg, "SP") == 0)  printf("SP = %ld\n", SP);
            else if (strcasecmp(reg, "RX") == 0)  printf("RX = %ld\n", RX);
            else if (strcasecmp(reg, "RB") == 0)  printf("RB = %ld\n", RB);
            else if (strcasecmp(reg, "RL") == 0)  printf("RL = %ld\n", RL);
            else if (strcasecmp(reg, "MAR") == 0) printf("MAR = %ld\n", MAR);
            else if (strcasecmp(reg, "MDR") == 0) printf("MDR = %ld\n", MDR);
            else if (strcasecmp(reg, "IR") == 0)  printf("IR = %08ld\n", IR);
            else if (strcasecmp(reg, "PSW") == 0) printf("PSW = %08ld\n", PSW);
            else {
                printf("Registro '%s' desconocido.\n", reg);
                printf("Registros: AC, PC, MAR, MDR, RB, RL, RX, SP, IR, PSW\n");
            }
        }
        else if (strcmp(comando, "m") == 0) {
            char reg[32];
            printf("direccion de memoria (global): \n");
            printf("SO-SIM > ");
            if (fgets(reg, sizeof(reg), stdin) == NULL) continue;
            reg[strcspn(reg, "\n")] = 0;
            reg[strcspn(reg, "\r")] = 0;
            int pos = atoi(reg);
            palabra valor;
            pmrd(pos, &valor, KERNEL_MODE);
            // Comparar y mostrar
            printf("mem[%i] = %ld\n", pos, valor);
            
        }
        else {
            printf("Comando desconocido. Use 'n' para siguiente paso.\n");
        }
    }
    return 0;
}
void Ejecutar(char *nombreArchivo, int modo){
    impresion("empezando carga del programa");

    proceso actual;
    int tamanodelProceso;
    if(crearProceso(&actual, nombreArchivo, 350, &tamanodelProceso)!=SUCCESS)return;
    actual.modoEjec = modo;
    
    snprintf(mensaje,200, "Programa '%s' cargado exitosamente", actual.nombre);
    impresion(mensaje);
    snprintf(mensaje,200, "Proceso: RB=%ld, RL=%ld, SP=%ld, RX=%ld, PSW=%ld", actual.rb, actual.rl, actual.sp, actual.rx, actual.psw);
    impresion(mensaje);
    //establecer registros del procesador
    RB = actual.rb;
    RL = actual.rl;
    SP = actual.sp;
    RX = actual.rx;
    PSW = actual.psw;
    
    IR = 0;
    AC = 0;
    finalizo = 0;
    
    impresion("iniciando ejecución");
    if(modo == 2){
        while(!finalizo){
            activarProcesador(modo);
            int ret = menuDebug(&actual);
            if(ret == 0)break;
        }
    }
    else{
        activarProcesador(modo);
    }
    snprintf(mensaje,200, "- '%s' FINALIZO de forma %s",actual.nombre, (valorFinalizacion == SUCCESS ? "EXITOSA" : "ERRONEA"));
    impresion(mensaje);
    snprintf(mensaje,200, "- Resultado (AC): %ld", AC);
    impresion(mensaje); 

}
