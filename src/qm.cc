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
    static const uint32_t table[][20] = {
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
        {1,10,45,120,210,252,210,120,45,10,1},
        {1,11,55,165,330,462,462,330,165,55,11,1},
        {1,12,66,220,495,792,924,792,495,220,66,12,1},
        {1,13,78,286,715,1287,1716,1716,1287,715,286,78,13,1},
        {1,14,91,364,1001,2002,3003,3432,3003,2002,1001,364,91,14,1},
        {1,15,105,455,1365,3003,5005,6435,6435,5005,3003,1365,455,105,15,1},
        {1,16,120,560,1820,4368,8008,11440,12870,11440,8008,4368,1820,560,120,16,1},
        {1,17,136,680,2380,6188,12376,19448,24310,24310,19448,12376,6188,2380,680,136,17,1},
        {1,18,153,816,3060,8568,18564,31824,43758,48620,43758,31824,18564,8568,3060,816,153,18,1},
        {1,19,171,969,3876,11628,27132,50388,75582,92378,92378,75582,50388,27132,11628,3876,969,171,19,1}
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
    unsigned int meta_size = GROUPS;
    unsigned int cubes_size = 2*factorial(VARIABLES);

    unsigned int total_size = sizeof(cube_t)*2*cubes_size + sizeof(uint32_t)*(MODELS+2*2*meta_size) + sizeof(char)*cubes_size;
    if(total_size >= 800000){
        fprintf(stderr, "cannot fit > 8Mb onto stack!\n");
        exit(1);
    }

    uint32_t *data = (uint32_t*) alloca(total_size);
    cube_t *prime = (cube_t*) data;
    uint32_t *size = (uint32_t*) (prime + MODELS);
    uint32_t *offset = size + 2*meta_size;
    char *check = (char*) (offset + 2*meta_size);
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
    uint32_t *csize = size, *nsize = size + meta_size;
    uint32_t *coffset = offset, *noffset = offset + meta_size;
    while(groups){
        //printf("\ngroups: %d\n", groups+1);
        //uint32_t *noffset = offset + groups+1;
        //uint32_t *nsize = size + groups+1;

        //for(uint32_t group = 0; group <= groups; group++){
        //    printf("  %d (%d) : ", group, csize[group]);
        //    uint32_t of = coffset[group];
        //    for(uint32_t i = 0; i < csize[group]; i++)
        //        printf("(%d,%d) ", ccubes[of+i].s[0], ccubes[of+i].s[1]);
        //    printf("\n");
        //}

        unsigned int ncubes_size = 0;
        for(unsigned int group = 0; group < groups; group++){
            noffset[group] = ncubes_size;
            nsize[group] = 0;

            for(unsigned int i = 0; i < csize[group]; i++){
                unsigned int oi = coffset[group]+i;
                for(unsigned int j = 0; j < csize[group+1]; j++){
                    unsigned int oj = coffset[group+1]+j;

                    cube_t &ci = ccubes[oi], &cj = ccubes[oj];
                    uint16_t p = ci[0] ^ cj[0];
                    if(ci[1] == cj[1] && is_power_of_two_or_zero(p)){
                        // merge

                        cube_t &co = ncubes[ncubes_size];
                        co[0] = ci[0] & cj[0];
                        co[1] = ci[1] | p;
                        check[oi] = 1;
                        check[oj] = 1;

                        // check for duplicates
                        bool insert = true;
                        for(int c = ncubes_size-1; c >= 0; c--){
                            if(co == ncubes[c]){
                                insert = false;
                                break;
                            }
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
        //printf("prime %d: ", groups+1);
        for(unsigned int i = 0; i < ccubes_size; i++){
            if(!check[i]){
                bool insert = true;
                for(unsigned int p = 0; p < PRIMES; p++)
                    if(prime[p] == ccubes[i])
                        insert = false;

                if(insert){
          //          printf("(%d,%d) ", ccubes[i][1],ccubes[i][1]);
                    prime[PRIMES++] = ccubes[i];
                }
            } else check[i] = 0;
        }
        //printf("\n");

        if(ncubes_size == 0)
            break;

        // update offsets
        ccubes_size = ncubes_size;
        swap(csize, nsize);
        swap(coffset, noffset);
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


    // make prime chart
    uint32_t *chart_size = (uint32_t*) (prime+PRIMES);
    uint32_t *chart_offset = chart_size+models.size();
    uint32_t *chart = chart_offset+models.size();

    unsigned int CHART_SIZE = 0;
    for(auto i = 0; i < models.size(); i++){
        chart_offset[i] = CHART_SIZE;
        chart_size[i] = 0;
        for(auto p = 0; p < PRIMES; p++){
            if((models[i] & (~prime[p][1])) == prime[p][0]){
                chart[CHART_SIZE++] = p;
                chart_size[i]++;
            }
        }
    }

    //printf("[");
    //for(int i = 0; i < models.size(); i++){
    //    if(i > 0)
    //        printf(", ");
    //    printf("[");
    //    for(int j = 0; j < chart_size[i]; j++){
    //        if(j > 0)
    //            printf(", ");
    //        printf("%d", chart[chart_offset[i]+j]);
    //    }
    //    printf("]");
    //}
    //printf("]\n");


    return 0;
}

void qm::uniq(cube_t*, unsigned int){

}



