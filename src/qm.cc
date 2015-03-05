#include "qm.h"
#include <algorithm>
#include <alloca.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits>

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

unsigned int space(const uint32_t n){
    uint32_t base[11];
    memcpy((void*) base,(void*) pascal(n), sizeof(uint32_t)*n+1);

    unsigned int size = pow2(n);
    for(int G = n+1; G > 0; G--)
        for(int i = 0; i < G; i++)
            size += (base[i] *= G-i);

    return size;
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
    models.clear();
    primes.clear();
}

int qm::solve(){
    if(models.size() == 0){
        printf("'0'\n");
        return 0;
    } else if(models.size() == pow2(variables.size())){
        printf("'1'\n");
        return 1;
    } else if(models.size() == 1){
        printf("(%d,0)\n",models[0]);
        return 2;
    } else if(compute_primes())
        return 2;

    return 0;
}

bool qm::valid(){
    uint32_t vars = variables.size();
    if(vars > 0){
        uint32_t max = pow2(vars);
        for(unsigned int i = 0; i < models.size(); i++)
            if(models[i] >= max)
                return false;
        return true;
    }
    return false;
}

int qm::compute_primes(){
    unsigned int VARIABLES = variables.size();
    unsigned int MODELS = pow2(VARIABLES);
    unsigned int GROUPS = VARIABLES+1;
    unsigned int PRIMES = 0;
    unsigned int meta_size = (GROUPS*(GROUPS+1))/2;
    unsigned int cubes_size = 2*factorial(VARIABLES);

    unsigned int total_size = sizeof(cube_t)*2*cubes_size + sizeof(uint32_t)*(MODELS+2*meta_size) + sizeof(char)*cubes_size;
    if(total_size >= 800000){
        fprintf(stderr, "cannot fit > 8Mb onto stack!\n");
        exit(1);
    }

    uint32_t *data = (uint32_t*) alloca(total_size);
    cube_t *prime = (cube_t*) data;
    uint32_t *size = (uint32_t*) (prime + MODELS);
    uint32_t *offset = size + meta_size;
    char *check = (char*) (offset + meta_size);
    cube_t *cubes = (cube_t*) (check + cubes_size);

    memset(size, 0, sizeof(uint32_t)*meta_size);
    memset(check, 0, sizeof(char)*cubes_size);


    // prepare cubes
    unsigned int ccubes_size = models.size();
    for(unsigned int i = 0; i < ccubes_size; i++){
        uint16_t c = models[i];
        uint32_t group = bitcount(c);
        data[c] = group;
        size[group]++;
    }

    offset[0] = 0;
    for(unsigned int i = 1; i < GROUPS; i++){
        offset[i] = offset[i-1] + size[i-1];
        size[i-1] = 0;
    }   size[GROUPS-1] = 0;

    for(unsigned int i = 0; i < ccubes_size; i++){
        uint16_t c = models[i];
        uint32_t index = offset[data[c]] + size[data[c]]++;
        cubes[index] = {{c,0}};
    }

    // quine-mccluskey
    unsigned int groups = GROUPS-1;
    cube_t *ccubes = cubes, *ncubes = cubes + cubes_size;
    while(groups){
        printf("\ngroups: %d\n", groups+1);
        uint32_t *noffset = offset + groups+1;
        uint32_t *nsize = size + groups+1;


        for(uint32_t group = 0; group <= groups; group++){
            printf("  %d (%d) : ", group, size[group]);
            uint32_t of = offset[group];
            for(uint32_t i = 0; i < size[group]; i++)
                printf("(%d,%d) ", ccubes[of+i].s[0], ccubes[of+i].s[1]);
            printf("\n");
        }


        unsigned int ncubes_size = 0;
        for(unsigned int group = 0; group < groups; group++){
            noffset[group] = ncubes_size;
            nsize[group] = 0;

            for(unsigned int i = 0; i < size[group]; i++){
                unsigned int oi = offset[group]+i;
                for(unsigned int j = 0; j < size[group+1]; j++){
                    unsigned int oj = offset[group+1]+j;

                    cube_t &ci = ccubes[oi], &cj = ccubes[oj];
                    uint16_t p = ci.s[0] ^ cj.s[0];
                    if(ci.s[1] == cj.s[1] && is_power_of_two_or_zero(p)){
                        // merge

                        cube_t &co = ncubes[ncubes_size];
                        co.s[0] = ci.s[0] & cj.s[0];
                        co.s[1] = ci.s[1] | p;
                        check[oi] = 1;
                        check[oj] = 1;

                        bool insert = true;
                        for(int c = ncubes_size-1; c >= 0; c--)
                            if(co == ncubes[c]){
                                insert = false;
                                break;
                            }

                        if(insert){
                            nsize[group]++;
                            ncubes_size++;
                        }
                    }
                }
            }
        }

        // get primes
        printf("prime %d: ", groups+1);
        for(unsigned int i = 0; i < ccubes_size; i++){
            if(!check[i]){
                bool insert = true;
                for(unsigned int p = 0; p < PRIMES; p++)
                    if(prime[p] == ccubes[i])
                        insert = false;

                if(insert){
                    printf("(%d,%d) ", ccubes[i].s[0],ccubes[i].s[1]);
                    prime[PRIMES++] = ccubes[i];
                }
            } else check[i] = 0;
        }
        printf("\n");

        if(ncubes_size == 0)
            break;

        // update offsets
        ccubes_size = ncubes_size;
        size = nsize;
        offset = noffset;
        swap(ccubes, ncubes);
        groups--;
    }

    sort(prime, prime+PRIMES);

    for(uint32_t i = 0; i < PRIMES; i++){
        if( i > 0)
            printf(",");
        uint16_t *p = (uint16_t*) (prime + i);
        printf("(%d,%d)", p[0], p[1]);
    }
    printf("\n");

    return 0;
}

void qm::uniq(cube_t*, unsigned int){

}



