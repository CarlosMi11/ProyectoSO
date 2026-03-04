#include "cargadorDePrograma.h"


int main(){
    crearLogger();
    crearProcesador();
    creardma();
    

    string comando;


    
    
    printf("=== CONSOLA DEL SISTEMA ===\n");

    int mostrarMenu = 1;

    while (1) { 
        if (mostrarMenu) {
            printf("\nComandos:\napagar\nreiniciar\nejecutar\nmemstat\nps\n");
            printf("SO-SIM > ");
        }
        
        mostrarMenu = 1;

        if (fgets(comando, sizeof(comando), stdin) == NULL) continue;
        comando[strcspn(comando, "\n")] = 0;

        char * token = strtok(comando, " ");
        if (token == NULL) {
            mostrarMenu = 0; 
            continue;
        }

       

        if (strcmp(token, "apagar") == 0) {
            printf("Apagando simulación...\n");
            break; 
        }
        else if (strcmp(token, "ejecutar") == 0) {
            
            
            token = strtok(NULL, " ");
            
            if (token != NULL) {    
                Ejecutar(token); 
                
            } else {
                printf("Entrada inválida.\n");
            }
        }
        else if (strcmp(token, "reiniciar") == 0){
            reiniciarPlanificador();
        }
        else if (strcmp(token, "memstat") == 0){
            
        }
        else if (strcmp(token, "ps") == 0){
            
        }
        else{
            printf("Comando desconocido: %s\n", token);
        }
    }

    matardma();
    matarProcesador();
    cerrarLogger();
}