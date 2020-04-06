#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

#define N 20
int A[N], B[N], RES[N];


sem_t sEcrire;
sem_t sLire;

FILE * f ;

void erreur(char *message)
{
    fprintf(stderr, "Erreur: %s\n", message);
    exit(1);
}


void print_array(int *T)
{
    int i;
    for (i = 0; i < N; i++)
        printf("%d ", T[i]);
    printf("\n");
}


void *p1_start(void *arg)
{
    
}
void *p2_start(void *arg)
{
    


 





}

int main(int argc, char *argv[])
{
    sem_init(&sLire,0,2);
    sem_init(&sEcrire,0,2);
   
    if( (f = fopen(argv[1],"rw")) == -1) erreur("file");


    return 0;
}