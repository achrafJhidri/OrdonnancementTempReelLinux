
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void erreur(char * s) {
    fprintf(stderr, "%s\n", s);
    exit(1);
}

int pfd12[2], pfd23[2];

void petit_fils() {
    if (close(1) == -1) erreur("close");
    if (dup(pfd12[1]) != 1) erreur("dup");
    if (close(pfd12[0]) == -1 || close(pfd12[1]) == -1) erreur("close");
    if (close(pfd23[0]) == -1 || close(pfd23[1]) == -1) erreur("close");
    execlp("ps", "ps", "-e", NULL);
    erreur("execlp");
}

void fils(char *mot) {
    switch (fork()) {
        case -1: erreur("fork");
        case  0: petit_fils();
    }

    if (close(0) == -1) erreur("close");
    if (dup(pfd12[0]) != 0) erreur("dup");
    if (close(1) == -1) erreur("close");
    if (dup(pfd23[1]) != 1) erreur("dup");
    if (close(pfd12[0]) == -1 || close(pfd12[1]) == -1) erreur("close");
    if (close(pfd23[0]) == -1 || close(pfd23[1]) == -1) erreur("close");
    execlp("grep", "grep", mot, NULL);
    erreur("execlp");
}

void pere() {
    if (close(0) == -1) erreur("close");
    if (dup(pfd23[0]) != 0) erreur("dup");
    if (close(pfd12[0]) == -1 || close(pfd12[1]) == -1) erreur("close");
    if (close(pfd23[0]) == -1 || close(pfd23[1]) == -1) erreur("close");
    execlp("sort", "sort", "-rn", NULL);
    erreur("execlp");
}

int main(int argc, char *argv[]) {
    if (argc != 2) erreur("arg");
    if (pipe(pfd12) == -1 || pipe(pfd23) == -1) erreur("pipe");

    switch (fork()) {
        case -1: erreur("fork");
        case  0: fils(argv[1]);
    }
    pere();
    exit(0);
}