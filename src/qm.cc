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

static inline bool is_power_of_two_or_zero(const uint16_t x){
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

static inline const uint32_t* pascal(const uint32_t x){
    static const uint32_t table[][11] = {
        {1},
        {1,1},
        {1,2,1},
        {1,3,3,1},
        {1,4,6,4,1},
        {1,5,10,10,5,1},
        {1,6,15,20,15,6,1},
        {1,7,21,35,35,21,7,1},
        {1,8,28,56,70,56,28,8,1},
        {1,9,36,84,126,126,84,36,9,1},
        {1,10,45,120,210,252,210,120,45,10,1}
    };
    return table[x];
}


static inline uint32_t binom(const uint32_t x, const uint32_t y){
    return factorial(x)/(factorial(y)*factorial(x-y));
}

bool is_little_endian(){
    union {
        uint32_t i;
        char c[4];
    } bint = {0x01020304};

    return bint.c[0] != 1;
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

bool qm::valid(){
    uint32_t vars = variables.size();
    if(vars > 0){
        uint32_t max = pow2(vars);
        for(unsigned int i = 0; i < cubes.size(); i++)
            if(cubes[i] >= max)
                return false;
        return true;
    }
    return false;
}

int qm::compute_primes(){
    uint32_t vars = variables.size();
    uint32_t GROUPS = vars+1;
    uint32_t delta_size = (GROUPS*(GROUPS+1))/2;
    uint32_t sigma_size = cubes.size()*vars;
    uint32_t total_size =  sizeof(cube_t)*sigma_size + sizeof(uint32_t)*(pow2(vars)+2*delta_size) + sizeof(char)*sigma_size;

    uint32_t *data = (uint32_t*) alloca(total_size);
    cube_t *prime = (cube_t*) data;
    uint32_t *size = (uint32_t*) (prime + pow2(vars));
    uint32_t *offset = size + delta_size;
    char *check = (char*) (offset + delta_size);
    cube_t *sigma = (cube_t*) (check + sigma_size);

    memset(size, 0, sizeof(uint32_t)*delta_size);
    memset(check, 0, sizeof(char)*sigma_size);

    for(uint32_t i = 0; i < cubes.size(); i++){
        uint16_t c = cubes[i];
        uint32_t group = bitcount(c);
        data[c] = group;
        size[group]++;
    }

    offset[0] = 0;
    for(uint32_t i = 1; i <= GROUPS; i++){
        offset[i] = offset[i-1] + size[i-1];
        size[i-1] = 0;
    }

    for(uint32_t i = 0; i < cubes.size(); i++){
        uint16_t c = cubes[i];
        uint32_t index = offset[data[c]] + size[data[c]]++;
        sigma[index] = {c,0};
    }

    uint32_t groups = GROUPS-1;
    uint32_t o = offset[groups+1]+size[groups+1];

    while(groups){
        printf("\ngroups: %d\n", groups+1);
        uint32_t *noffset = offset + groups+1;
        uint32_t *nsize = size + groups+1;

        bool done = true;


        for(uint32_t group = 0; group <= groups; group++){
            printf("  %d: ", group);
            uint32_t of = offset[group];
            for(uint32_t i = 0; i < size[group]; i++)
                printf("(%d,%d) ", sigma[of+i].s[0], sigma[of+i].s[1]);
            printf("\n");
        }

        for(uint32_t group = 0; group < groups; group++){
            noffset[group] = o;

            for(uint32_t i = 0; i < size[group]; i++){
                uint32_t oi = offset[group]+i;
                for(uint32_t j = 0; j < size[group+1]; j++){
                    uint32_t oj = offset[group+1]+j;

                    cube_t &ci = sigma[oi], &cj = sigma[oj];
                    uint16_t p = ci.s[0] ^ cj.s[0];
                    if(ci.s[1] == cj.s[1] && is_power_of_two_or_zero(p)){
                        // merge
                        cube_t &co = sigma[o];
                        co.s[0] = ci.s[0] & cj.s[0];
                        co.s[1] = ci.s[0] | p;
                        nsize[group]++;

                        check[oi] = 1;
                        check[oj] = 1;
                        o++;
                        done = false;
                    }
                }
            }
        }

        printf("prime %d: ", groups+1);
        for(int i = offset[0]; i < noffset[0]; i++)
            if(!check[i])
                printf("(%d,%d) ", sigma[i].s[0],sigma[i].s[1]);
        printf("\n");

        if(done)
            break;

        // update offsets
        size = nsize;
        offset = noffset;
        groups--;

        //bool done = true;
        //for(uint32_t group = 0; group < groups; group++){
        //    if(nsize[group] != 0 && nsize[group+1] != 0){
        //       done = false;
        //       break;
        //    }
        //}
    }

    uint32_t PRIMES = 0;
    for(uint32_t i = 0; i < o; i++){
        if(!check[i]){
            prime[PRIMES] = sigma[i];
            PRIMES++;
        }
    }

    printf("primes: ");
    for(uint32_t i = 0; i < PRIMES; i++){
        uint16_t *p = (uint16_t*) (prime + i);
        printf("(%d,%d) ", p[0], p[1]);
    }

    return 0;
}





