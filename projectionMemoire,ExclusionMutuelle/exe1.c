#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
const int N = 10;
int *T;
void erreur(char *s)
{
    fprintf(stderr, "%s\n", s);
    exit(1);
}
void creation()
{
    int fd = open(" temp ", O_RDWR | O_CREAT | O_TRUNC, 0600);
    if (fd == -1)
        erreur(" open ");
    //  if ( lseek ( fd , N *sizeof(int) , 0) == -1) erreur (" lseek ") ;
    if (write(fd, &N, sizeof(int)) == -1)
        erreur(" write ");
    T = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (T == MAP_FAILED)
        erreur(" mmap ");
    if (close(fd) == -1)
        erreur(" close ");
    if (unlink(" temp ") == -1)
        erreur(" unlink ");
}
void pere()
{

    if (wait(NULL) == -1)
        erreur("wait");
    if (wait(NULL) == -1)
        erreur("wait");
    printf("la nouvelle valeur de N est = %d\n", *T);
    if (munmap(T, sizeof(int)) == -1)
        erreur(" munmap ");
    exit(0);
}

void fils(int a)
{

    for (int i = 0; i < a; i++)
    {
        (*T)++;
    }
    exit(0);
}
int main(int argc, char *argv[])
{
    if (argc != 3)
        erreur("args");

    int a[2];

    a[0] = atoi(argv[1]);
    a[1] = atoi(argv[2]);

    creation();
    for (int i = 0; i < 2; i++)
    {
        switch (fork())
        {
        case -1:
            erreur(" fork ");
        case 0:
            fils(a[i]);
        }
    }
    pere();
}
