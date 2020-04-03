
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
                nb--;
        }
        m.unlock();
}