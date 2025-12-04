#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define NUM_PRODUTORES 2
#define NUM_CONSUMIDORES 2
#define NUM_ITENS 20

int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;

sem_t vaga;  
sem_t item;  
pthread_mutex_t mutex;

void* produtor(void* arg) {
    int id = *(int*)arg;
    int item_produzido;

    for (int i = 0; i < NUM_ITENS / NUM_PRODUTORES; i++) {
        item_produzido = rand() % 100;
        printf("Produtor %d: produziu item %d\n", id, item_produzido);

        sem_wait(&vaga);

        pthread_mutex_lock(&mutex);
        buffer[in] = item_produzido;
        printf("Produtor %d: depositou na posição %d\n", id, in);
        in = (in + 1) % BUFFER_SIZE;
        pthread_mutex_unlock(&mutex);

        sem_post(&item);

        usleep(100000);
    }
    return NULL;
}

void* consumidor(void* arg) {
    int id = *(int*)arg;
    int item_consumido;

    for (int i = 0; i < NUM_ITENS / NUM_CONSUMIDORES; i++) {
        sem_wait(&item);

        pthread_mutex_lock(&mutex);
        item_consumido = buffer[out];
        printf("Consumidor %d: retirou item %d da posição %d\n",
               id, item_consumido, out);
        out = (out + 1) % BUFFER_SIZE;
        pthread_mutex_unlock(&mutex);

        sem_post(&vaga);

        printf("Consumidor %d: consumiu item %d\n", id, item_consumido);
        usleep(150000);
    }
    return NULL;
}

int main() {
    pthread_t prod_threads[NUM_PRODUTORES];
    pthread_t cons_threads[NUM_CONSUMIDORES];
    int prod_ids[NUM_PRODUTORES];
    int cons_ids[NUM_CONSUMIDORES];

    sem_init(&vaga, 0, BUFFER_SIZE);
    sem_init(&item, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    printf("=== PROTOCOLO CORRETO ===\n");
    printf("Iniciando sistema com %d produtores e %d consumidores\n\n",
           NUM_PRODUTORES, NUM_CONSUMIDORES);

    for (int i = 0; i < NUM_PRODUTORES; i++) {
        prod_ids[i] = i + 1;
        pthread_create(&prod_threads[i], NULL, produtor, &prod_ids[i]);
    }

    for (int i = 0; i < NUM_CONSUMIDORES; i++) {
        cons_ids[i] = i + 1;
        pthread_create(&cons_threads[i], NULL, consumidor, &cons_ids[i]);
    }

    for (int i = 0; i < NUM_PRODUTORES; i++)
        pthread_join(prod_threads[i], NULL);

    for (int i = 0; i < NUM_CONSUMIDORES; i++)
        pthread_join(cons_threads[i], NULL);

    sem_destroy(&vaga);
    sem_destroy(&item);
    pthread_mutex_destroy(&mutex);

    printf("\nSistema finalizado com sucesso!\n");
    return 0;
}
