#include "qm.h"
#include <alloca.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
using namespace std;

static inline uint32_t bitcount(uint32_t x){
    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    return (((x + (x >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

static inline bool is_power_of_two_or_zero(const uint32_t x){
    return (x & (x - 1)) == 0;
}

static inline uint32_t log2(const uint32_t x) { // for x86 and x86-64 architecture
    uint32_t y;
    asm ( "\tbsr %1, %0\n"
            : "=r"(y)
            : "r" (x)
        );
    return y;
}

static inline uint32_t pow2(const uint32_t x) {
    return 1 << x;
}

static inline uint32_t logsum(const uint32_t x) {
    return (x << 1) - 1;
}

static inline uint32_t powsum2(const uint32_t x) {
    return (1 << (x+1)) - 1;
}

static inline uint32_t factorial(const uint32_t x) {
    static const uint32_t table[] = {1, 1, 2, 6, 24, 120, 720,
        5040, 40320, 362880, 3628800, 39916800, 479001600};
    return table[x];
}

static inline uint32_t pascal(const uint32_t x){
    static const uint32_t table [][11] = {
        {1,0,0,0,0,0,0,0,0,0,0},
        {1 1,0,0,0,0,0,0,0,0,0},
        {1,2,1,0,0,0,0,0,0,0,0},
        {1,3,3,1,0,0,0,0,0,0,0},
        {1,4,6,4,1,0,0,0,0,0,0},
        {1,5,10,10,5,1,0,0,0,0,0},
        {1,6,15,20,15,6,1,0,0,0,0},
        {1,7,21,35,35,21,7,1,0,0,0},
        {1,8,28,56,70,56,28,8,1,0,0},
        {1,9,36,84,126,126,84,36,9,1,0},
        {1,10,45,120,210,252,210,120,45,10,1}
    }
    return table[x];
}


static inline uint32_t binom(const uint32_t x, const uint32_t y){
    return factorial(x)/(factorial(y)*factorial(x-y));
}

static inline uint32_t roundup2(uint32_t x){
    x--;
    x |= x >> 1;  // handle  2 bit numbers
    x |= x >> 2;  // handle  4 bit numbers
    x |= x >> 4;  // handle  8 bit numbers
    x |= x >> 8;  // handle 16 bit numbers
    x |= x >> 16; // handle 32 bit numbers
    x++;
    return x;
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

    return 0;
}

int qm::compute_primes(){
    uint32_t vars = variables.size();
    uint32_t groups = vars+1;
    uint32_t delta_size = (groups*(groups+1))/2;
    uint32_t sigma_size = cubes.size()*2;
    uint32_t total_size = 2*cubes.size() + 2*delta_size + 3*sigma_size;

    uint32_t *data = (uint32_t*) alloca(sizeof(uint32_t)*total_size);
    uint32_t *prime1 = data;
    uint32_t *prime2 = prime1 + cubes.size();

    uint32_t *group_size = prime2 + cubes.size();
    uint32_t *offset = group_size + delta_size;
    uint32_t *check = offset + delta_size;
    uint32_t *sigma1 = check + delta_size;
    uint32_t *sigma2 = sigma1 + sigma_size;

    uint32_t primes = 0;

    memset(group_size, 0, sizeof(uint32_t)*delta_size);

    offset[0] = 0;
    offset[1] = 1;
    for(uint32_t i = 1; i <= groups; i++){
        offset[i] = offset[i-1] + pascal(vars,i-1);
        printf("%d: (%d,%d) = %d\n", i, vars, i-1, pascal(vars,i-1));
    }

    for(int i = 0; i < groups; i++)
        printf("%2d: %d\n", offset[i]);

//    for(uint32_t i = 0; i < cubes.size(); i++){
//        sigma1[bitcount(i)] = cubes[i];
//        sigma2[i] = 0;
//    }
//
    //sigma[bitcount(*it)].push_back(prime(*it,0));

    for(int G = groups; G > 1; G--){
        uint32_t *noffset = offset + G;
        uint32_t *nsize = size + G;

        for(int group = 0; group < G-1; group++){
            noffset[group] = noffset[group-1]+nsize[group-1];
            nsize[group] = 0;

            for(int i = 0; i < size[group]; i++){
                uint32_t oi = offset[group]+i,
                for(int j = 0; j < size[group+1]; j++){
                    uint32_t oj = offset[group+1]+j;

                    uint32_t p = s0[oi] ^ s0[oj];
                    if(s1[oi] == s1[oj] && is_power_of_two_or_zero(p)){
                        // merge
                        uint32_t os = noffset[group] + nsize[group];
                        s0[os] = s0[oi] & s0[oj];
                        s1[os] = s1[oi] | p;
                        nsize[group]++;

                        check[oi] = 2;
                        check[oj] = 2;
                    }
                }
            }
        }

        // update offsets
        group_size += G;
        offset = noffset;
    }

    uint32_t SIZE = offset[-1] + size[-1] + size[0];
    uint32_t PRIMES = 0;
    for(int i = 0; i < SIZE; i++){
        if(check[i] == 1){
            prime1[PRIMES] = sigma0[i];
            prime2[PRIMES] = sigma1[i];
            PRIMES++;
        }
    }

    printf("primes: ");
    for(int i = 0; i < PRIMES; i++)
        printf("(%d,%d) ", prime1[i], prime2[i]);

    return 0;
}





