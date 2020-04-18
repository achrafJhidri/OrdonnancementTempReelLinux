
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define N 20
#define L 1000
int A[N], B[N], RES[N];

sem_t slire;
sem_t scalculer[2];

void erreur(char *message) {
    fprintf(stderr, "Erreur: %s\n", message);
    exit(1);
}

void print_array(int *T) {
    int i;
    for (i=0; i<N; i++)
        printf("%d ", T[i]);
    printf("\n");
}

void *lire(void *arg) {
    int i,j;
    char *filename = (char*) arg;

    FILE *fptr = fopen(filename, "r");

    for (j=0; j<L; j++) {
        sem_wait(&slire);
        sem_wait(&slire);
        for (i=0; i<N; i++) {
            if (j%2 == 0)
                fscanf(fptr, "%d", &A[i]);
            else
                fscanf(fptr, "%d", &B[i]);
        }
        sem_post(&scalculer[0]);
        sem_post(&scalculer[1]);
    }

    fclose(fptr);
    pthread_exit((void*) 0);
}

void *calculer(void *arg) {
    int i,j;
    int *limits = (int*) arg;

    for (i=limits[0]; i<limits[1]; i++)
        RES[i] = 0;

    for (j=0; j<L; j++) {
        sem_wait(&scalculer[limits[2]]);
        for (i=limits[0]; i<limits[1]; i++) {
            if (j%2 == 0)
                RES[i] = RES[i] + A[i];
            else
                RES[i] = RES[i] + B[i];
        }
        sem_post(&slire);
    }

    pthread_exit((void*) 0);
}

int main(int argc, char *argv[]) {
    if (argc != 2) erreur("argc");

    void *status;

    if (sem_init(&slire, 0, 2) != 0) erreur("sem_init");
    if (sem_init(&scalculer[0], 0, 0) != 0) erreur("sem_init");
    if (sem_init(&scalculer[1], 0, 0) != 0) erreur("sem_init");

    pthread_t tid_l, tid_c1, tid_c2;
    if (pthread_create(&tid_l, NULL, lire, (void*) argv[1]) != 0) erreur("pthread_create");
    int limits1[2] = {0, N/2};
    if (pthread_create(&tid_c1, NULL, calculer, (void*) limits1) != 0) erreur("pthread_create");
    int limits2[2] = {N/2, N};
    if (pthread_create(&tid_c2, NULL, calculer, (void*) limits2) != 0) erreur("pthread_create");

    pthread_join(tid_l, &status);
    if (status != 0) erreur("pthread_join");
    pthread_join(tid_c1, &status);
    if (status != 0) erreur("pthread_join");
    pthread_join(tid_c2, &status);
    if (status != 0) erreur("pthread_join");

    print_array(RES);

    if (sem_destroy(&slire) != 0) erreur("sem_destroy");
    if (sem_destroy(&scalculer[0]) != 0) erreur("sem_destroy");
    if (sem_destroy(&scalculer[1]) != 0) erreur("sem_destroy");

    return 0;
}
