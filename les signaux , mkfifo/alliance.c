#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void erreur(char *message) {
    fprintf(stderr, "Erreur: %s\n", message);
    exit(1);
}

void deroute(int sig) {
    printf("Signal %d (%s) reçu\n", sig, sys_siglist[sig]);
    exit(0);
}

int main(int argc, char* argv[]) {
    if (signal(SIGUSR1, deroute) == SIG_ERR) erreur("signal");
    printf("Hello! Je suis %d : \n", getpid());
    pause();
}

