#include "qm.h"

using namespace std;

inline uint32_t bitcount(uint32_t i){
    i = i - ((i >> 1) & 0x55555555);
    i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
    return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

inline bool is_power_of_two_or_zero(x){
    return (x & (x - 1)) == 0;
}

inline prime *merge(i, j){
    if i[1] != j[1]:
        return NULL;
    y = i[0] ^ j[0]
    if(!is_power_of_two_or_zero(y))
        return NULL;

    //return (i[0] & j[0],i[1]|y);
    return NULL;
}

qm::qm(){

}

qm::~qm(){
    clear();
}

void qm::clear(){
    variables.clear();
    cubes.clear();
    primes.clear();
}

int qm::solve(){
    if(cubes.size() == 0)
        return 0;
    if(cubes.size() == (1 << variables.size()))
        return 1;

    if(compute_primes())
        return -1;

    if(unate_cover())
        return -2;

    return 2;
}

int qm::compute_primes(){
    vector< vector<prime_t> > sigma(variables.size()+1);

    for(auto it = cubes.begin(); it != cubes.end(); it++)
        sigma[bitcount(*it)].push_back(prime(*it,0));

    uint32_t sigmas = sigma.size();
    while(sigmas){

    }


    return 0;
}

int qm::unate_cover(){
    return 0;
}




