#include "qm.h"
#include <algorithm>
#include <alloca.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits>

using namespace std;

inline void set_bit(uint16_t &x, uint16_t i){
    x |= (1<<i);
}

inline void clear_bit(uint16_t &x, uint16_t i){
     x &= ~(1<<i);
}

inline void toggle_bit(uint16_t &x, uint16_t i){
    x ^= (1 << i);
}

inline bool is_set_bit(uint16_t x, uint16_t i){
    return (x >> i) & 1;
}

inline void set_min_value(unsigned int &x, unsigned int y){
    x = y ^ ((x ^ y) & -(x < y));
}

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

int qm::quine_mccluskey(void *data){
    if(!data)
        return 0;
    unsigned int VARIABLES = variables.size();
    unsigned int MODELS = pow2(VARIABLES);
    unsigned int GROUPS = VARIABLES+1;
    unsigned int PRIMES = 0;
    unsigned int meta_size = GROUPS;
    unsigned int cubes_size = 2*factorial(VARIABLES);

    cube_t *prime = (cube_t*) data;
    uint32_t *size = (uint32_t*) (prime + MODELS);
    uint32_t *offset = size + 2*meta_size;
    char *check = (char*) (offset + 2*meta_size);
    cube_t *cubes = (cube_t*) (check + cubes_size);

    memset(size, 0, sizeof(uint32_t)*meta_size);
    memset(check, 0, sizeof(char)*cubes_size);

    // prepare cubes
    uint32_t *model_to_group = (uint32_t*) data;
    unsigned int ccubes_size = models.size();
    for(unsigned int i = 0; i < ccubes_size; i++){
        uint16_t c = models[i];
        uint32_t group = bitcount(c);
        model_to_group[c] = group;
        size[group]++;
    }

    offset[0] = 0;
    for(unsigned int i = 1; i < GROUPS; i++){
        offset[i] = offset[i-1] + size[i-1];
        size[i-1] = 0;
    }   size[GROUPS-1] = 0;

    for(unsigned int i = 0; i < ccubes_size; i++){
        uint16_t c = models[i];
        uint32_t index = offset[model_to_group[c]] + size[model_to_group[c]]++;
        cubes[index] = {{c,0}};
    }

    // quine-mccluskey
    unsigned int groups = GROUPS-1;
    cube_t *ccubes = cubes, *ncubes = cubes + cubes_size;
    uint32_t *csize = size, *nsize = size + meta_size;
    uint32_t *coffset = offset, *noffset = offset + meta_size;
    while(groups){
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
        for(unsigned int i = 0; i < ccubes_size; i++){
            if(!check[i]){
                bool insert = true;
                for(unsigned int p = 0; p < PRIMES; p++)
                    if(prime[p] == ccubes[i])
                        insert = false;

                if(insert)
                    prime[PRIMES++] = ccubes[i];
            } else check[i] = 0;
        }

        if(ncubes_size == 0)
            break;

        // update offsets
        ccubes_size = ncubes_size;
        swap(csize, nsize);
        swap(coffset, noffset);
        swap(ccubes, ncubes);
        groups--;
    }

    return PRIMES;
}

size_t qm::required_size(){
    unsigned int VARIABLES = variables.size();
    unsigned int MODELS = pow2(VARIABLES);
    unsigned int GROUPS = VARIABLES+1;
    unsigned int meta_size = GROUPS;
    unsigned int cubes_size = 2*factorial(VARIABLES);

    unsigned int total_size = sizeof(cube_t)*2*cubes_size + sizeof(uint32_t)*(MODELS+2*2*meta_size) + sizeof(char)*cubes_size;
    if(total_size >= 800000){
        fprintf(stderr, "cannot fit > 8Mb onto stack!\n");
        exit(1);
    }
    return total_size;
}

int qm::compute_primes(){
    void *data = alloca(required_size());
    unsigned int PRIMES, CANONICAL;

    PRIMES = quine_mccluskey((uint32_t*) data);
    CANONICAL = reduce((uint32_t*) data,PRIMES);

    return CANONICAL;
}

inline unsigned int qm::get_weight(cube_t &c, const uint16_t &MASK){
    uint16_t cover = (~c[1]) & MASK;
    unsigned int weight = bitcount(cover);
    weight = (weight==1?0:weight);
    weight += bitcount((~c[0]) & cover);
    return weight;
}


const char *byte_to_binary(uint16_t x)
{
    static char b[9];
    b[0] = '\0';

    int z;
    for (unsigned int i = 0; i < 8; i++)
    {
        strcat(b, ((x >> i) & 1==1?"1":"0"));
    }

    return b;
}



int qm::reduce(void *data, unsigned int PRIMES){
    cube_t* prime = (cube_t*) data;
    uint32_t *chart_size = (uint32_t*) (prime+PRIMES);
    const uint16_t MODELS = models.size();
    uint32_t *chart_offset = chart_size+MODELS;
    uint16_t *prime_mask = (uint16_t*) (chart_offset+MODELS);
    unsigned int *prime_weight = (unsigned int*) (prime_mask+MODELS*3);
    uint32_t *chart = (uint32_t*) (prime_weight+MODELS);
    memset((void*)prime_mask,0,sizeof(uint16_t)*MODELS+sizeof(unsigned int)*MODELS);

    sort(prime, prime+PRIMES);

    // make prime chart
    unsigned int CHART_SIZE = 0;
    for(unsigned int i = 0; i < MODELS; i++){
        chart_offset[i] = CHART_SIZE;
        chart_size[i] = 0;
        for(unsigned int p = 0; p < PRIMES; p++){
            if((models[i] & (~prime[p][1])) == prime[p][0]){
                prime_mask[p] |= 1<<i;
                chart[CHART_SIZE++] = p;
                chart_size[i]++;
            }
        }
    }

    // identify essential implicates and remove the models they cover
    const uint16_t COVER = (uint16_t)((1 << MODELS)-1);
    const uint16_t MASK = (uint16_t)((1 << variables.size())-1);
    uint16_t cover = COVER;
    uint16_t *essentials = (uint16_t*) alloca(sizeof(uint16_t)*PRIMES);
    unsigned int essential_size = 0;
    unsigned weight = 0;
    for(unsigned int i = 0; i < MODELS && cover; i++){
        if(chart_size[i] == 1){
            unsigned int p = chart[chart_offset[i]];

            // verify uniqueness
            bool insert = true;
            for(int j = essential_size-1; j >= 0; j--){
                if(essentials[j] == p){
                    insert = false;
                    break;
                }
            }

            if(insert){
                printf("p %d ", p);
                cover &= ~prime_mask[p];
                essentials[essential_size++] = p;
                weight += 1 + get_weight(prime[p],MASK);
            }
        }
    }

    // find minimal prime implicate representation
    unsigned int non_essential_size = 0;
    unsigned int min_weight = ~0;
    if(cover != 0){
        uint16_t *non_essentials = essentials+essential_size;
        uint16_t *covers = (uint16_t*) alloca(sizeof(uint16_t)*(MODELS)+sizeof(unsigned int)*(PRIMES));
        unsigned int *weights = (unsigned int*) covers + PRIMES;

        covers[0] = cover;
        weights[0] = weight;

        cube_t stack[100];
        int depth = 0;
        int i = 0;
        stack[depth][0] = 0;
        while(depth >= 0){
            if(is_set_bit(covers[depth],i)){
                while(true){
                    if(stack[depth][0] < chart_size[i]){
                        unsigned int p = chart[chart_offset[i]+stack[depth][0]];
                        stack[depth][1] = i;

                        // compute weight
                        weights[depth+1] = weights[depth] + get_weight(prime[p], MASK) + 1;

                        // prune
                        if(weights[depth+1] > min_weight){
                            stack[depth][0]++;
                            i--;
                        } else {
                            // update cover
                            cover = covers[depth] & (~prime_mask[p]);

                            // determine covering
                            if(cover == 0){
                                // go through more primes on current depth
                                //unsigned int weight = depth+essentials+1;
                                //set_min_value(min_weight, weights[depth+1] + (weight==1?-1:0));a
                                non_essential_size = depth+1;
                                unsigned int tmp_weight = weights[depth+1] - (non_essential_size+essential_size==1?1:0);

                                if(tmp_weight < min_weight) {
                                    for(int d = 0; d <= depth; d++){
                                        unsigned int p = chart[chart_offset[stack[d][1]]+stack[d][0]];
                                        non_essentials[d] = p;
                                    }
                                    min_weight = tmp_weight;
                                }

                                //printf("%d:%u:", counter, (min_weight==~0?0:min_weight));
                                //for(int d = 0; d <= depth; d++){
                                //    unsigned int p = chart[chart_offset[stack[d][1]]+stack[d][0]];
                                //    if(i == essential_size)
                                //        printf("|");
                                //    printf(" %d:%d:(%d, %d)", p, get_weight(prime[p],MASK), prime[p][0], prime[p][1]);
                                //} printf("\n");

                                stack[depth][0]++;
                                continue;
                            } else { // acquire more primes
                                depth++;
                                covers[depth] = cover;
                                stack[depth][0] = 0;
                            }
                        }
                    } else {
                        // if depth < 0, quit!
                        depth--;
                        stack[depth][0]++;
                        i = stack[depth][1]-1;
                    }
                    break;
                }
            }
            i++;
        }
    }

    if(min_weight == (unsigned int) ~0){
        min_weight = (essential_size+non_essential_size==1?weight-1:weight);
    }

    for(unsigned int i = 0; i < essential_size+non_essential_size; i++){
        printf(" %d", essentials[i]);
    } printf("\n");
    sort(essentials, essentials+essential_size+non_essential_size);
    for(unsigned int i = 0; i < essential_size+non_essential_size; i++){
        printf(" %d", essentials[i]);
    } printf("\n");

    printf("%u:", min_weight);
    for(unsigned int i = 0; i < essential_size+non_essential_size; i++){
        printf(" %d:(%d, %d)", get_weight(prime[essentials[i]],MASK), prime[essentials[i]][0], prime[essentials[i]][1]);
    }
    printf("\n");

    return 0;
}

void qm::uniq(cube_t*, unsigned int){

}

