
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define N 10

int pidother;
char *FIFO_PATH[2];

void erreur(char *message) {
    fprintf(stderr, "Erreur: %s\n", message);
    exit(1);
}

/******************************************************************************
 * immunisation du signal SIGUSR1                                             *
 * utilisé pour la synchronisation entre attack et defend                     *
 ******************************************************************************/
// Question 6
void deroute_synch(int sig) {
    printf("Checking... ");
}

/******************************************************************************
 * immunisation du signal SIGUSR2                                             *
 * reçu si l'autre joueur a quitté le jeu                                     *
 ******************************************************************************/
// Question 6
void deroute_connexion_lost(int sig) {
    printf("Connexion lost!\n");
    exit(1);
}

/******************************************************************************
 * immunisation du signal SIGINT                                              *
 * envoyer le signal SIGUSR2 à l'autre joueur pour l'informer                 *
 * les fifo sont detruits                                                     *
 ******************************************************************************/
// Question 6
void deroute_ctrlc(int sig) {
    if (kill(pidother, SIGUSR2) == -1) erreur("kill");
    if (remove(FIFO_PATH[0]) == -1) erreur("remove");
    if (remove(FIFO_PATH[1]) == -1) erreur("remove");
    exit(1);
}

void print_tab(char tab[N][N]) {
    int i, j;
    printf("\n");
    printf("  | ");
    for (i=0; i<N; i++) {
        printf("%d ", i);
    }
    printf("\n");
    printf("--+-");
    for (i=0; i<N; i++) {
        printf("--");
    }
    printf("\n");
    for (i=0; i<N; i++) {
        printf("%d | ", i);
        for (j=0; j<N; j++) {
            printf("%c ", tab[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Propose à l'utilisateur de donner la position d'attaque
void get_attack_position(int pos[2]) {
    printf("I attack!\n");
    do {
        printf("x=");
        scanf("%d", &pos[0]);
        printf("y=");
        scanf("%d", &pos[1]);
        printf("\n");
        if ((pos[0] >= 0) && (pos[0] < N) && (pos[1] >= 0) && (pos[1] < N)) {
            printf("Attack at position (%d,%d)\n", pos[0], pos[1]);
            return;
        }
        printf("Try again!\n");
    } while (1);
}

void attack(char tab[N][N], int fifo[2]) {
    // Question 8
    // L'attaquant demande à l'utilisateur la position pour attaquer.
    int pos[2];
    get_attack_position(pos);
    // Ensuite, il envoie cette position au défendeur
    if (write(fifo[1], pos, 2*sizeof(int)) == -1) erreur("write");
    // et il le réveille.
    if (kill(pidother, SIGUSR1) == -1) erreur("kill");
    // Après, il attend la réponse
    char rep;
    if (read(fifo[0], &rep, sizeof(char)) == -1) erreur("read");
    // et il met à jour le tableau tab (le tableau de l'autre joueur)
    tab[pos[0]][pos[1]] = rep;

    if (rep == 'X')
        printf("Hit!!!\n");
    else
        printf("Missed!\n");
    print_tab(tab);
}

void defend(char tab[N][N], int fifo[2]) {
    // Question 8
    // Le défendeur était en pause (voir dans la fonction main la partie "jouer")
    // Le signal SIGUSR1 (déjà immunisé) le réveille. (rien à faire alors)
    // Il lit la position d'attaque.
    int pos[2];
    if (read(fifo[0], &pos, 2*sizeof(int)) == -1) erreur("read");
    // Il met à jour le tableau tab (son propre tableau)
    char rep = tab[pos[0]][pos[1]];
    // et il informe l'attaquant.
    if (write(fifo[1], &rep, sizeof(char)) == -1) erreur("write");

    if (rep == 'X')
        printf("hitted at position (%d,%d)\n\n", pos[0], pos[1]);
    else
        printf("safe\n\n");
}

int main(int argc, char* argv[]) {
    int player, other;
    int fifo[2];

    /**************************************************************************
     * gestion de arguments                                                   *
     **************************************************************************/
    // Question 4
    if (argc != 4) erreur("argc: ./bataille-navale player fifo-in fifo-out");
    player = atoi(argv[1]); // 0 or 1
    if (player != 0 && player != 1) erreur ("wrong player");
    other = (player+1) % 2;
    FIFO_PATH[0] = argv[2]; // read   ->>> variable globale - on l'utilise pour la destruction des fifos si l'utilisateur fait Ctr-C
    FIFO_PATH[1] = argv[3]; // write  ->>> variable globale

    /**************************************************************************
     * création des tubes                                                     *
     **************************************************************************/
    // Question 5
    if (player == 0) {
        if (mkfifo(FIFO_PATH[0], 0777) == -1) erreur("mkfifo");
        if (mkfifo(FIFO_PATH[1], 0777) == -1) erreur("mkfifo");
    }

    /**************************************************************************
     * immunisation des signaux                                               *
     **************************************************************************/
    // Question 6
    if (signal(SIGUSR1, deroute_synch) == SIG_ERR) erreur("signal");
    if (signal(SIGUSR2, deroute_connexion_lost) == SIG_ERR) erreur("signal");
    if (signal(SIGINT, deroute_ctrlc) == SIG_ERR) erreur("signal");
    // il faut coder les trois fonctions deroute_synch, deroute_connexion_lost, deroute_ctrlc (voir plus haut)

    /**************************************************************************
     * établir la connexion                                                   *
     **************************************************************************
     * Ouverture dans le bon ordre pour chaque joueur                         *
     * de fifo[0] en lecture et fifo[1] en écriture,                          *
     * et récupération du pid de l'autre joueur                               *
     **************************************************************************/
    // Question 7
    // L'ordre ici est très important !! Si on ne fait pas attention il y aura un interblocage
    int pidplayer = getpid();
    // Les deux joueurs doivent ouvrir d'abord le même pipe et après l'autre
    // N'oubliez pas que chaque joueur utilise fifo[0] pour la lecture et fifo[1] pour l’écriture.
    // Donc, par exemple, la variable fifo[0] ne contient pas le même fifo pour les deux joueurs.
    printf("Hello! I am %d\n", pidplayer);
    if (player == 0) {
        if ((fifo[1] = open(FIFO_PATH[1], O_WRONLY)) == -1) erreur("open");
        if (write(fifo[1], &pidplayer, sizeof(int)) == -1) erreur("write");
        if ((fifo[0] = open(FIFO_PATH[0], O_RDONLY)) == -1) erreur("open");
        if (read(fifo[0], &pidother, sizeof(int)) == -1) erreur("read");
    }
    else { // player 1
        if ((fifo[0] = open(FIFO_PATH[0], O_RDONLY)) == -1) erreur("open");
        if (read(fifo[0], &pidother, sizeof(int)) == -1) erreur("read");
        if ((fifo[1] = open(FIFO_PATH[1], O_WRONLY)) == -1) erreur("open");
        if (write(fifo[1], &pidplayer, sizeof(int)) == -1) erreur("write");
    }
    // Joueur 0 ouvre d'abord fifo[1] et joueur 1 fifo[0] mais les deux variables correspondent au même pipe.

    /**************************************************************************
     * initialisation des tableaux du jeux                                    *
     **************************************************************************/
    char montableau[N][N] = {
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', 'X', ' ', ' ', 'X', 'X', 'X', ' '},
        {' ', ' ', ' ', 'X', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', 'X', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', 'X', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'X', 'X'}
    };
    print_tab(montableau);
    char sontableau[N][N] = {[0 ... N-1] = {[0 ... N-1] = '?'}};
    print_tab(sontableau);

    /**************************************************************************
     * jouer                                                                  *
     **************************************************************************/
    while (1) {
        if (player == 0)
            attack(sontableau, fifo);
        printf("Waiting...\n");
        pause();
        defend(montableau, fifo);
        if (player == 1)
            attack(sontableau, fifo);
    }

    /**************************************************************************
     * fermature des tubes                                                    *
     **************************************************************************/
    // Question 9
    if (close(fifo[0]) == -1) erreur("close");
    if (close(fifo[1]) == -1) erreur("close");

    /**************************************************************************
     * destruction des tubes                                                  *
     **************************************************************************/
    // Question 9
    if (player == 0) {
        if (remove(FIFO_PATH[0]) == -1) erreur("remove");
        if (remove(FIFO_PATH[1]) == -1) erreur("remove");
    }
}