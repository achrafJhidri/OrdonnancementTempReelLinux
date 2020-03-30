
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>

int *n;
int *tour;

void erreur(char *message) {
    fprintf(stderr, "Erreur: %s\n", message);
    exit(1);
}

void creation_n() {
    int fd;
    if ((fd = open("temp", O_RDWR | O_CREAT | O_TRUNC, 0600)) == -1) erreur("open");
    if (lseek(fd, sizeof(int), 0) == -1) erreur("lseek");
    if (write(fd, "", sizeof(char)) == -1) erreur("write");
    if ((n = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) erreur("mmap");
    if (close(fd) == -1) erreur("close");
    if (unlink("temp") == -1) erreur("unlink");
    *n = 0;
}

void creation_tour() {
    int fd;
    if ((fd = open("temp", O_RDWR | O_CREAT | O_TRUNC, 0600)) == -1) erreur("open");
    if (lseek(fd, sizeof(int), 0) == -1) erreur("lseek");
    if (write(fd, "", sizeof(char)) == -1) erreur("write");
    if ((tour = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) erreur("mmap");
    if (close(fd) == -1) erreur("close");
    if (unlink("temp") == -1) erreur("unlink");
    *tour = 0;
}

void entreeSC(int i) {
    while (*tour != i) ;
}

void sortieSC() {
    *tour = ((*tour)+1) % 2;
}

void fils(int k, int i) {
    int j;
    for (j=0; j<k; j++) {
        entreeSC(i);
        printf("%d : avant n = %d\n", getpid(), *n);
        (*n)++;
        printf("%d : apres n = %d\n", getpid(), *n);
        sortieSC();
    }
    exit(0);
}

void pere() {
    while (wait(NULL) != -1) ;
    printf("n = %d\n", *n);
    if (munmap(n, sizeof(int)) == -1) erreur("munmap");
    if (munmap(tour, sizeof(int)) == -1) erreur("munmap");
    exit(0);
}

int main(int argc, char* argv[]) {
    if (argc != 3) erreur("argc");
    creation_n();
    creation_tour();
    int i;
    for (i=0; i<2; i++) {
        switch (fork()) {
            case -1: erreur("fork");
            case  0: fils(atoi(argv[i+1]), i);
        }
    }
    pere();
}
