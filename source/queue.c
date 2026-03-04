#include "queue.h"


void crearCola(Queue *q){
    q->inicio = 0;
    q->final = 0;
    q->cantidad = 0;
}

void colaInsertar(Queue *q, int payload) {
    if (q->cantidad == MAX_PROCESOS) {
        return; // se podria activar una interrupcion, not sure...
    }
    
    q->cola[q->final] = payload;
    q->final = (q->final + 1) % MAX_PROCESOS; 
    q->cantidad++;
}

int colaExtraer(Queue *q) {
    if (q->cantidad == 0) {
        return -1;
    }
    
    int payload = q->cola[q->inicio];
    q->inicio = (q->inicio + 1) % MAX_PROCESOS;
    q->cantidad--;
    return payload;
}

int eliminarDeCola(Queue *q, int indiceFisico) {
    
    if (q->cantidad == 0 || indiceFisico < 0 || indiceFisico >= MAX_PROCESOS) {
        return -1; 
    }

    // Validar que el índice físico pertenezca a los elementos activos.
    // Calculamos la "distancia" desde el inicio. Si es mayor o igual a la cantidad, 
    // significa que ese índice es un hueco vacío actualmente.
    int distancia = (indiceFisico - q->inicio + MAX_PROCESOS) % MAX_PROCESOS;
    if (distancia >= q->cantidad) {
        return -1; 
    }

    // Si el índice a eliminar es el inicio, 
    // nos ahorramos el desplazamiento y usamos tu función de extraer.
    if (indiceFisico == q->inicio) {
        return colaExtraer(q);
    }

    // Guardamos el dato
    int payloadEliminado = q->cola[indiceFisico];

    // Desplazamiento circular hacia atrás para tapar el hueco
    int actual = indiceFisico;
    while (1) {
        int siguiente = (actual + 1) % MAX_PROCESOS;
        
        // Si el siguiente a mover es el hueco del final, paramos
        if (siguiente == q->final) {
            break; 
        }
        
        q->cola[actual] = q->cola[siguiente];
        actual = siguiente;
    }

    // Retrocedemos el apuntador final de forma segura
    q->final = (q->final - 1 + MAX_PROCESOS) % MAX_PROCESOS;
    q->cantidad--;

    return payloadEliminado;
}