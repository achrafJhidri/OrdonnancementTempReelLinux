#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>  // pour utiliser mmap
#include <sys/wait.h>

int *n;  // variable globale à partager et à incrémenter
int *occupe; // pour faire la condition pour l'exclusion mutuelle. Il faut la partager => projeter en mémoire

void erreur(char *message) {
    fprintf(stderr, "Erreur: %s\n", message);
    exit(1);
}

void creation_n() {  // Vous pouvez trouver cela dans CM1
    int fd;
    if ((fd = open("temp", O_RDWR | O_CREAT | O_TRUNC, 0600)) == -1) erreur("open");
    if (lseek(fd, sizeof(int), 0) == -1) erreur("lseek");
    if (write(fd, "", sizeof(char)) == -1) erreur("write");
    if ((n = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) erreur("mmap");
    if (close(fd) == -1) erreur("close");
    if (unlink("temp") == -1) erreur("unlink");
    *n = 0;  // on profite pour initialiser à 0
}

void creation_occ() {
    int fd;
    if ((fd = open("temp", O_RDWR | O_CREAT | O_TRUNC, 0600)) == -1) erreur("open");
    if (lseek(fd, sizeof(int), 0) == -1) erreur("lseek");
    if (write(fd, "", sizeof(char)) == -1) erreur("write");
    if ((occupe = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) erreur("mmap");
    if (close(fd) == -1) erreur("close");
    if (unlink("temp") == -1) erreur("unlink");
    *occupe = 0;
}

void entreeSC() {
    while (*occupe == 1) ;
    *occupe = 1;
}

void sortieSC() {
    *occupe = 0;
}

void fils(int k) {
    int j;
    for (j=0; j<k; j++) {
        entreeSC();  // Si on met entreeSC juste avant le for et sortieSC juste après la fin du for, on n'a pas de parallélisation.
                     // Le premier processus qui arrive à entrer dans la section critique finit son exécution en bloquant l'autre processus.
        printf("%d, %d : avant n = %d\n", getpid(), j, *n);
        (*n)++;  // n'oubliez pas que n est un pointeur, pour incrementer sa valeur on doit faire *n
        printf("%d, %d : apres n = %d\n", getpid(), j, *n);
        sortieSC();
    }
    exit(0);  // on n'oublie pas exit à la fin du fils !!
}

void pere() {
    while (wait(NULL) != -1);  // si on oublie cela il y aura deux problèmes :
                               // (1) on a des zombies et
                               // (2) le père n'attend pas la fin de ses fils avant afficher le résultat, donc le résultat n'est pas complète
    printf("n = %d\n", *n);
    if (munmap(n, sizeof(int)) == -1) erreur("munmap");  // on n'oublie pas à libérer la mémoire
    exit(0);
}

int main(int argc, char* argv[]) {
    if (argc != 3) erreur("argc");
    creation_n();  // il faut la coder pour initialiser la variable globale n !
    creation_occ();
    int i;
    for (i=0; i<=1; i++) {
        switch (fork()) {
            case -1: erreur("fork");
            case  0: fils(atoi(argv[i+1]));
        }
    }
    pere();
}