#include "qm.h"

int main(){
    qm q;
    q.add_variable(0);
    q.add_variable(1);
    q.add_variable(2);
    q.add_model(0);
    q.add_model(1);
    q.add_model(3);
    q.add_model(4);
    q.add_model(6);
    q.add_model(7);
    q.compute_primes();
}
