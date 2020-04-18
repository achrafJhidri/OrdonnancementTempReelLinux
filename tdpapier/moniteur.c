
int nb = 0;  // variable d'état -> nombre de threads arrivés
mutex m;
cond f;  // condition

barriere() {
        m.lock();
        nb++;  // je suis arrivé !
        while (nb < N)  // si ils ne sont pas tous là
                f.wait(m);  // j'attends
        f.signal();  // une fois réveillé ou si je suis le dernier, je réveille un autre
                     // le dernier thread réveille le 1er
                     // le 1er thread réveille le 2eme
                     // etc
        m.unlock();
}

Solution avec broadcast

barriere() {
        m.lock();
        nb++;  // je suis arrivé !
        if (nb < N)  // si ils ne sont pas tous là
                f.wait(m);  // j'attends

        if (nb == N) {
                f.broadcast();
                nb--;//empeche que chacun fais un broadcast,
                     //on peut faire nb=0 si on compte réutiliser la barriere
        }
        m.unlock();
}



Question 16
Il a 3 états logiques : MANGE, PENSE, AFAIM.

Question 17


mutex m;
cond file_prive[N];
EtatPhilo[N] = {PENSE, PENSE, ... , PENSE}; // variables d’état

prendre_fourchettes(i) {
        m.lock()

        while (EtatPhilo[(i+1)%N] == MANGE ||
                   EtatPhilo[(i-1+N)%N] == MANGE)
                file_prive[i].wait(m);

        EtatPhilo[i] = MANGE;
        m.unlock()
}

poser_fourchettes(i) {
        m.lock()
        EtatPhilo[i] = PENSE;
        file_prive[(i+1)%N].signal();
        file_prive[(i-1+N)%N].signal();
        m.unlock()
}

Question 18

mutex m;
cond file_publique;
EtatPhilo[N] = {PENSE, PENSE, ... , PENSE};

prendre_fourchettes(i) {
        m.lock();
        
        while (EtatPhilo[(i+1)%N] == MANGE ||
                   EtatPhilo[(i-1+N)%N] == MANGE)
                file_publique.wait(m);

        EtatPhilo[i] = MANGE;


        m.unlock();
}

poser_fourchettes(i) {
        m.lock();
        EtatPhilo[i] = PENSE;
        file_publique.broadcast();
        m.unlock();
}

question 19 
il se px que quelqun reste en famine 1 2 3 4 5 , 1 4 2 5 , le 3 reste affamé


Question 20

sem fourch[N] = {1, 1, 1, ..., 1};

prendre_fourchette(i) {
        fourch[i].P();
        fourch[(i+1)%N].P()
}

poser_fourchette(i) {
        fourch[i].V();
        fourch[(i+1)%N].V()
}


Question 21

Il y a un interblocage si tous les philosophes ont pris la fourchette de gauche en même temps !


Question 22
sem fourch[N] = {1, 1, 1, ..., 1};

prendre_fourchette(i) {
        if(i != N-1) {
                fourch[i].P();
                fourch[(i+1)%N].P()
        }else{
                fourch[(i+1)%N].P();
                foruch[i].P();
        }
}

poser_fourchette(i) {
        fourch[i].V();
        fourch[(i+1)%N].V()
}

Question 23
sem fourch[N] = {1, 1, 1, ..., 1};
sem nbP = N-1
prendre_fourchette(i) {
                nbP.P();
                fourch[i].P();
                fourch[(i+1)%N].P()

}

poser_fourchette(i) {
        fourch[i].V();
        fourch[(i+1)%N].V()
        nbP.V();

}

