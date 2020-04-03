#include <mutex>

int balance = 1000;
 mutex m ;
void retirer(int n){

    m.lock();
    balance-=n;
    m.unlock();
}

void ajouter (int n){

    m.lock();
    balance+=n;
    m.unlock();
}