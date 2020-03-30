
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void erreur(char *message) {
    fprintf(stderr, "Erreur: %s\n", message);
    exit(1);
}

int main(int argc, char* argv[]) {
    while (1) {
        int pid;
        printf("pid du victim : ");
        scanf("%d", &pid);
        if (kill(pid, SIGUSR1) == -1) erreur("kill");
    }
}
