#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define N 10
int A[N], B[N], RES[N], C[N], FINAL[N];
pthread_mutex_t mutex;
int compteurArrives = 0;

void erreur(char *message)
{
    fprintf(stderr, "Erreur: %s\n", message);
    exit(1);
}
pthread_cond_t cond;

void print_array(int *T)
{
    int i;
    for (i = 0; i < N; i++)
        printf("%d ", T[i]);
    printf("\n");
}
void rdv()
{ 
    if (pthread_mutex_lock(&mutex) != 0) erreur("mutex_lock");
    compteurArrives++;
    while (compteurArrives < N)
        if (pthread_cond_wait(&cond, &mutex) != 0)
            erreur(" wait ");

    if (pthread_cond_signal(&cond) != 0) erreur("cond_signal");
    if (pthread_mutex_unlock(&mutex) != 0) erreur("mutex_unlock");
}


void *start_thread(void *arg)
{
    

    int i = *((int *)arg);
    RES[i] = A[i] + B[i];

    rdv();

    FINAL[N - 1 - i] = RES[N - 1 - i] + C[N - 1 - i];
    pthread_exit((void *)0);
}

int main(int argc, char *argv[])
{
    void *status;
    int i;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    for (i = 0; i < N; i++) 
    {
         A[i] = i;
        B[i] = N-i;
        C[i] = i;
    }
    print_array(A);
    print_array(B);
    print_array(C);

    pthread_t tid[N];
    for (i = 0; i < N; i++)
    {
        int *j = (int *)malloc(sizeof(int));
        *j = i;
        if (pthread_create(&tid[i], NULL, start_thread, (void *)j) != 0)
            erreur("pthread_create");
    }

    for (i = 0; i < N; i++)
    {
        pthread_join(tid[i], &status);
        if (status != 0)
            erreur("pthread_join");
    }

    print_array(FINAL);

     if (pthread_mutex_destroy(&mutex) != 0) erreur("mutex_destroy");
    if (pthread_cond_destroy(&cond) != 0) erreur("cond_destroy");

    return 0;
}