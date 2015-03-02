#include "qm.h"

int main(){
    qm q;
    q.add_variable(0);
    q.add_variable(1);
    q.add_variable(3);
    q.add_variable(4);
    q.add_model(0);
    q.add_model(1);
    q.add_model(4);
    q.add_model(5);
    q.compute_primes();
}
