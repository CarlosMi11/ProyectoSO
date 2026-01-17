#include "cargadorDePrograma.h"

int main(){
    crearLogger();
    crearProcesador();
    creardma();
    

    char comando[100];
    char archivo[100];
    int modo;
    
    
    printf("=== CONSOLA DEL SISTEMA ===\n");

    while (1) { 
        printf("\nComandos:\nsalir\nejecutar\n");
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