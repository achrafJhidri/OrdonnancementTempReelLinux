//************************************************************************************//
//************************************************************************************//
//On peut avoir une famine sur les rédacteurs si les lecteurs arrivent en continue.***//
//************************************************************************************//
//************************************************************************************//
mutex m;
int nblec = 0;  // nombre de lecteurs dans le système
int nbredac = 0;  // nombre de rédacteurs en train d’écrire

cond flect;
cond fredac;

debut_lire() {
    m.lock();
    nblect++;
    while (nbredac != 0)
        flect.wait(m);
    /* reveil en cascade */
    flect.signal();
    m.unlock();
}

debut_redac() {
    m.lock();
    while (nblect > 0 || nbredac != 0)
        fredac.wait(m);
    nbredac = 1;
    m.unlock();
}

fin_lire() {
    m.lock();
    nblect--;
    if (nblect == 0)
        fredac.signal();
    m.unlock();
}

fin_redac()
{
    m.lock();
    nbredac=0;
    if(nbLecteur > 0)
        flecteur.signal(); // ou bien broadcast et on enleve signal du debut lire
   else 
        fredac.signal();
    m.unlock();
}
