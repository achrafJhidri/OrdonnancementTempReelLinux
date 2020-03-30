#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int pfd01[2], pfd10[2], pfd02[2], pfd20[2];

void erreur(char *s) {
    fprintf(stderr, "%s\n", s);
    exit(1);
}

void pere(char *fich) {
    if (close(pfd01[0]) == -1 || close(pfd10[1]) == -1) erreur("close");
    if (close(pfd02[0]) == -1 || close(pfd20[1]) == -1) erreur("close");

    FILE *fp;
    char buffer[BUFSIZ];
    int T[26], somme, i;

    if ((fp = fopen(fich, "r")) == NULL) erreur("fopen");
    while (fgets(buffer, BUFSIZ, fp) != NULL) {
        if (buffer[0]>='a' && buffer[0]<='z') {
            if (write (pfd01[1], &buffer[0], sizeof(char)) == -1) erreur("write");
        }
        else if (buffer[0]>='0' && buffer[0]<='9') {
            if (write(pfd02[1], &buffer[0], sizeof(char)) == -1) erreur("write");
        }
    }

    if (write(pfd01[1], "*", sizeof(char)) == -1) erreur("write");
    if (write(pfd02[1], "*", sizeof(char)) == -1) erreur("close");
    if (close(pfd01[1]) == -1 || close(pfd02[1]) == -1) erreur("close");

    // Ici on met les wait !!!
    if (wait(NULL) == -1) erreur("wait");
    if (wait(NULL) == -1) erreur("wait");

    if (read(pfd10[0], T, 26*sizeof(int)) == -1) erreur("read");
    if (read(pfd20[0], &somme, sizeof(int)) == -1) erreur("read");

    for (i=0; i<26; i++) printf("T[%c] = %d\n", i+'a', T[i]);
    printf("Somme = %d\n", somme);
    exit(0);
}

void fils1() {
    if (close(pfd01[1]) == -1 || close(pfd10[0]) == -1) erreur("close");
    if (close(pfd02[0]) == -1 || close(pfd02[1]) == -1) erreur("close");
    if (close(pfd20[0]) == -1 || close(pfd20[1]) == -1) erreur("close");

    int T[26], i; char car;

    for (i=0; i<26; i++) T[i] = 0;

    if (read(pfd01[0], &car, sizeof(char)) == -1) erreur("read");
    while (car != '*') {
        T[car-'a']++;
        if (read(pfd01[0], &car, sizeof(char)) == -1) erreur("read");
    }
    if (close(pfd01[0]) == -1) erreur("close");

    if (write(pfd10[1], T, 26*sizeof(int)) == -1) erreur("write");
    if (close(pfd10[1]) == -1) erreur("close");

    exit(0);
}

void fils2() {
    if (close(pfd02[1]) == -1 || close(pfd20[0]) == -1) erreur("close");
    if (close(pfd01[0]) == -1 || close(pfd01[1]) == -1) erreur("close");
    if (close(pfd10[0]) == -1 || close(pfd10[1]) == -1) erreur("close");

    int somme=0; char car;

    if (read(pfd02[0], &car, sizeof(char)) == -1) erreur("read");
    while (car != '*') {
        somme = somme + car-'0';
        if (read(pfd02[0], &car, sizeof(car)) == -1) erreur("read");
    }
    if (close(pfd02[0]) == -1) erreur("close");

    if (write(pfd20[1], &somme, sizeof(int)) == -1) erreur("write");
    if (close(pfd20[1]) == -1) erreur("close");

    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 2) erreur("arg");

    if (pipe(pfd01) == -1 || pipe(pfd10) == -1) erreur("pipe");
    if (pipe(pfd02) == -1 || pipe(pfd20) == -1) erreur("pipe");

    switch (fork()) {
        case -1: erreur("fork");
        case  0: fils1();
    }
    switch (fork()) {
        case -1: erreur("fork");
        case  0: fils2();
    }
    pere(argv[1]);
}