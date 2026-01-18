#include "stdio.h"
#include "stdlib.h"
#include "pthread.h"
#include "semaphore.h"
#include "time.h"


sem_t mutex;                    /* controla o acesso a 'rc' */
sem_t db;                       /* controla o acesso a base de dados */
int rc = 0;                     /* número de processos lendo ou querendo ler */
int dbv;                        /* valor na base dados */

void* reader(void *arg) {
    int i = *((int *) arg);

    while(1) {               /* repete para sempre */
        sem_wait(&mutex);       /* obtém acesso exclusivo à 'rc' */
        rc = rc + 1;            /* um leitor a mais agora */

        if (rc == 1) {          /* se este for o primeiro leitor... */
            sem_wait(&db);
        }

        sem_post(&mutex);       /* libera o acesso exclusivo a 'rc' */
        read_data_base(i);       /* acesso aos dados */
        sem_wait(&mutex);       /* obtém acesso exclusivo a 'rc' */
        rc = rc - 1;            /* um leitor a menos agora */

        if (rc == 0) {          /* se este for o último leitor */
            sem_post(&db);
        }

        sem_post(&mutex);       /* libera o acesso exclusivo a 'rc' */
        use_data_read(i);        /* região não crítica */
    }
}

void* writer(void *arg) {
    int i = *((int *) arg);
    int value;

    while(1) {               /* repete para sempre */
        value = think_up_data(i);        /* região não crítica */
        sem_wait(&db);          /* obtém acesso exclusivo */
        write_data_base(i, value);      /* atualiza os dados */
        sem_post(&db);          /* libera o acesso exclusivo */
    }
}

void read_data_base(int i) {
    srand(time(0));
    printf("Reader %d leu o valor %d na base de dados!\n", i+1, dbv);
    delay( rand() % 5);
}

void use_data_read(int i) {
    srand(time(0));
    printf("Reader %d estah usando os dados lidos!\n", i+1);
    delay(rand() % 5);
}

int think_up_data(int i) {
    srand(time(0));
    printf("Writer %d estah pensando no que escrever!\n", i+1);
    delay(rand() % 5);
    return rand()%100;
}

void write_data_base(int i, int value) {
    srand(time(0));
    dbv = value;
    printf("Writer %d escreveu o valor %d nos dados!\n", i+1, value);
    delay( rand() % 5);
}



void delay(int number_of_seconds) {
    // Converting time into milli_seconds
    int milli_seconds = 1000 * number_of_seconds;

    // Storing start time
    clock_t start_time = clock();

    // looping till required time is not achieved
    while (clock() < start_time + milli_seconds);
}

int main() {
    sem_init(&mutex, 0, 1);
    sem_init(&db, 0, 1);

    pthread_t r[3], w[2];

    int i;
    srand(time(0));
    dbv=rand()%100; /*inicia dbv*/

    /* criando leitores */
    for (i = 0; i < 3 ; i++) {
        pthread_create(&r[i], NULL, reader, (void*) &i);
        delay(1);
    }

    /* criando escritores */
    for (i = 0; i< 2; i++) {
        pthread_create(&w[i], NULL, writer, (void*) &i);
        delay(1);
    }

    while(1);

    return 0;
}
