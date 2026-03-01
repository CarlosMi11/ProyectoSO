#define N 20

typedef struct{
    int cola[N];
    int inicio;
    int final;
    int cantidad;
}Queue;

void crearCola(Queue *q){
    q->inicio = 0;
    q->final = 0;
    q->cantidad = 0;
}

void colaInsertar(Queue *q, int payload) {
    if (q->cantidad == N) {
        return; // se podria activar una interrupcion, not sure...
    }
    
    q->cola[q->final] = payload;
    q->final = (q->final + 1) % N; 
    q->cantidad++;
}

int colaExtraer(Queue *q) {
    if (q->cantidad == 0) {
        return -1;
    }
    
    int payload = q->cola[q->inicio];
    q->inicio = (q->inicio + 1) % N;
    q->cantidad--;
    return payload;
}