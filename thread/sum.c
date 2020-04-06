#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define N 10

int A[N];
int B[N];



int Res[N];

void *OK = (void *)123456789L;
void *summer(void *index);
int main()
{

    pthread_t tabThread[N];
    void *status;

    for (int i = 0; i < N; i++)
    {
        A[i] = i;
        B[i] = i;
    }

    for (int i = 0; i < N; i++)
    {
        int * j = malloc(sizeof(int ));
        *j=i;
        if (pthread_create(&tabThread[i], NULL, summer, (void *)j) != 0)
        {
            printf("erreur de creation");
            exit(1);
        }
    }

    for (int i = 0; i < N; i++)
    {
        pthread_join(tabThread[i], &status);
        if (status == OK)
            printf(" Thread %lx completed ok .\n", tabThread[i]);
    }

    for (int i = 0; i < N; i++)
    {
        printf("%d + %d  =  %d\n", A[i], B[i], Res[i]);
    }

    return EXIT_SUCCESS;
}
void *summer(void *index)
{
    int *i = (int *)index;
    Res[*i] = A[*i] + B[*i];
    return OK;
}