#ifndef QUEUE
#define QUEUE
#include "cabecera.h"

typedef struct{
    int cola[MAX_PROCESOS];
    int inicio;
    int final;
    int cantidad;
}Queue;

void crearCola(Queue *q);
void colaInsertar(Queue *q, int payload);
int colaExtraer(Queue *q);
int eliminarDeCola(Queue *q, int indiceFisico);

#endif