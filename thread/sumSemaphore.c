#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define N 20   // nb de colonnes
#define L 1000 // nb de lignes
int A[N], B[N], RES[N];

sem_t slire;     // pour bloquer lire
sem_t scalculer; // pour bloquer calculer

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
        if (sem_wait(&slire) != 0) erreur("sem_wait");
        for (i=0; i<N; i++) {
            if (j%2 == 0)
                fscanf(fptr, "%d", &A[i]);
            else
                fscanf(fptr, "%d", &B[i]);
        }
        if (sem_post(&scalculer) != 0) erreur("sem_post");
    }

    fclose(fptr);
    pthread_exit((void*) 0);
}

void *calculer(void *arg) {
    int i,j;

    for (i=0; i<N; i++)
        RES[i] = 0;

    for (j=0; j<L; j++) {
        if (sem_wait(&scalculer) != 0) erreur("sem_wait");
        for (i=0; i<N; i++) {
            if (j%2 == 0)
                RES[i] = RES[i] + A[i];
            else
                RES[i] = RES[i] + B[i];
        }
        if (sem_post(&slire) != 0) erreur("sem_post");
    }

    pthread_exit((void*) 0);
}

int main(int argc, char *argv[]) {
    if (argc != 2) erreur("argc");  // argv[1] = nom du fichier donné par l'utilisateur

    void *status;

    if (sem_init(&slire, 0, 1) != 0) erreur("sem_init");
    if (sem_init(&scalculer, 0, 0) != 0) erreur("sem_init");

    pthread_t tid_l, tid_c;
    if (pthread_create(&tid_l, NULL, lire, (void*) argv[1]) != 0) erreur("pthread_create");
    if (pthread_create(&tid_c, NULL, calculer, NULL) != 0) erreur("pthread_create");

    pthread_join(tid_l, &status);
    if (status != 0) erreur("pthread_join");
    pthread_join(tid_c, &status);
    if (status != 0) erreur("pthread_join");

    print_array(RES);

    if (sem_destroy(&slire) != 0) erreur("sem_destroy");
    if (sem_destroy(&scalculer) != 0) erreur("sem_destroy");

    return 0;
}
