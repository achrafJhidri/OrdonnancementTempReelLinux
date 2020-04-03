//************************************************************************************//
//************************************************************************************//
// quand on vx faire de la synchronisation on commence par v  et on initialise a 0 et on fait un p de l'autre***//
// quand on vx faire de l'exclusion mutuelle on commence par p et on initalise a 1 ou N**//
//************************************************************************************//
//************************************************************************************//
Question 8

sem smutex = 1;
...
smutex.P();
<Section Critique>
smutex.V(); //unlock
...


Question 9

sem s = 10;

s.P();
connecter();
// Travail
déconnecter();
s.V();

Question 10
sem sa=0;
sem sb =0 // le premier thread qui arrive doit attendre

void rdv_A(){
    sa.v();
    sb.p();
}
void rdv_B(){
    sb.v();
    sa.p();
}


exemple :
A1 A2 A3 B1 A4 B2
A=1 B=-1 : A1 est bloqué
A=2 B=-2 : A2 est bloqué
A=3 B=-3 : A3 est bloqué
B = -2 signal (il y a A1 qui se reveille) A=2 psk on aappel a.p(); donc => A1 et B1 continue
A=3 B=-3 : A4 est bloqué
B = -2 : signal() il ya A2 qui se reveille) A = 2 psk on appel a.p() donc => A2 et B2 continue

sem sa=0;
sem sb =0 

sem sc=0;
sem sab=0;
sem sac=0;
sem sba=0;

void rdv_A(){
    sa.v();
    sb.p();
}
void rdv_B(){
    sb.v();
    sa.p();
}



sem AforB = 0; sem AforC = 0;
sem BforA = 0; sem BforC = 0;
sem CforA = 0; sem CforB = 0;


RDV_A() {
    AforB.v(); // je suis arrivé 
    AforC.v();  // je suis arrivé
    BforA.p();  // je suis blocké dans le wait de BforA()
    CforA.p();  // je suis blocké dans le wait de CforA()
}
RDV_B() {
    BforA.v(); // je débloque le wait BforA()
    BforC.v();
    AforB.p();
    CforB.p();
}

RDV_C() {
    CforA.v();
    CforB.v();
    BforC.p();
    AforC.p();
}
Question 12

// On a besoin d'un compteur pour stocker combien de threads sont arrivés
int compteur = 0
// pour proteger le compteur
sem smutex = 1
// pour la synchronisation
sem barriere = 0

void Barriere () {
// La difficulté est de compter les arrivées en exclusion mutuelle sans se bloquer.
   smutex.P(); // lock
   compteur++;
   if (compteur == N)
       barriere.V();  // signal
   smutex.V(); // unlock
   barriere.P();  // wait  + désincremente
   barriere.V();  // signal +incrémente
}

exemple
N = 3
A B C

A :
    compteur =1
    bloqué sur barriere
B :
    compteur=2
    bloqué sur barriere
C :
    compteur = 3
    3 == 3 -> V()  (le compteur s.c du semaphore est incrementé) + signal donc A est reveillé
    A continue après le wait est fait un V() -> signal B est reveillé
    B continue après le wait est fait un V() -> signal C est reveillé

Question 13

const int N = 100;
char *tampon[N];

sem smutexP = 1;
sem smutexC = 1;

sem pleines = 0;
sem vides = N;

int ilec = 0;
int iecr = 0;

Producteur(Message m) {
        vides.P(); // prendre une case vide
        smutexP.P();
        tampon[iecr] = m;
        iecr = (iecr + 1) % N;
        smutexP.V();
        pleines.V(); // donner une case pleine
}

Consommateur(Message *m) {
        pleines.P(); // prendre une case pleine
        smutexC.P();
        *m = tampon[ilec];
        ilec = (ilec + 1) % N;
        smutexC.V();
        vides.V(); // donner une case vide
}
