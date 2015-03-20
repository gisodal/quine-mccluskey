#include "qm.h"
#include <algorithm>
#include <alloca.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

using namespace std;

template <typename T>
inline void set_bit(T &x, unsigned i){
    x |= (1<<i);
}

template <typename T>
inline void clear_bit(T &x, unsigned i){
    x &= ~(1<<i);
}

template <typename T>
inline void toggle_bit(T &x, unsigned int i){
    x ^= (1 << i);
}

template <typename T>
inline bool is_set_bit(T x, unsigned int i){
    return (x >> i) & 1;
}

template <typename T>
inline void set_min_value(T &x, T y){
    x = y ^ ((x ^ y) & -(x < y));
}

template <typename T>
inline unsigned int bitcount(T x){
    uint16_t count = 0;
    while(x){
        count += x & 1;
        x >>= 1;
    }
    return count;
}

//template <>
//inline uint16_t bitcount(uint32_t x){
//    x = x - ((x >> 1) & 0x55555555);
//    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
//    return (((x + (x >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
//}


template <>
inline unsigned int bitcount(uint32_t x){ // assume uint32_t == unsigned int
    return __builtin_popcount(x);
}

template <>
inline unsigned int bitcount(uint64_t x){
    return __builtin_popcountl(x);;
}

template <>
inline unsigned int bitcount(uint128_t x){
    return bitcount((uint64_t) x) + bitcount((uint64_t) (x>>64));
}

template <typename T>
inline bool is_power_of_two_or_zero(const T x){
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

template <typename M>
qm<M>::qm(){

}

template <typename M>
qm<M>::~qm(){
    clear();
}

template <typename M>
void qm::clear(){
    variables.clear();
    models.clear();
}

template <typename M>
int qm<M>::solve(){
    if(models.size() == 0){
        printf("'0'\n");
        return 0;
    } else if(models.size() == pow2(variables.size())){
        printf("'1'\n");
        return 1;
    } else if(models.size() == 1){
        printf("(%lu,0)\n",models[0]);
        return 2;
    } else if(canonical_primes())
        return 2;

    return 0;
}

template <typename M>
bool qm<M>::valid(){
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

template <typename M>
size_t qm<M>::required_size(){
    unsigned int VARIABLES = variables.size();
    unsigned int MODELS = pow2(VARIABLES);
    unsigned int GROUPS = VARIABLES+1;
    unsigned int meta_size = GROUPS;
    unsigned int cubes_size = 2*factorial(VARIABLES);

    unsigned int total_size = sizeof(uint64_t)*2*cubes_size + sizeof(uint32_t)*(MODELS+2*2*meta_size) + sizeof(char)*cubes_size;
    if(total_size >= 800000){
        fprintf(stderr, "compute_primesot fit > 8Mb onto stack!\n");
        exit(1);
    }
    return total_size;
}

template <typename M>
int qm<M>::canonical_primes(){
    //void *data = alloca(required_size());
    void *data = malloc(required_size());
    if(!data){
        printf("stack allocation failed at size %u (%.2fMb)\n", required_size(), required_size()/(float)1024);
        return -2;
    }
    int PRIMES;

    unsigned int VARIABLES = variables.size();
    if(VARIABLES <= 8){
        cube_size = 8;
        PRIMES = compute_primes<uint8_t>(data);
    } else if(VARIABLES <= 16){
        cube_size = 16;
        PRIMES = compute_primes<uint16_t>(data);
    } else if(VARIABLES <= 32){
        cube_size = 32;
        PRIMES = compute_primes<uint32_t>(data);
    } else if(VARIABLES <= 64){
        cube_size = 64;
        PRIMES = compute_primes<uint64_t>(data);
    } else if(VARIABLES <= 128){
        cube_size = 128;
        PRIMES = compute_primes<uint128_t>(data);
    } else return -1;

    // TODO: store primes
    free(data);
    return PRIMES;
}

template <typename M>
template <typename T>
int qm<M>::compute_primes(void *data){
    unsigned int PRIMES = quine_mccluskey<T>(data);
    if(PRIMES > 0){
        unsigned int MODELS = models.size();
        if(MODELS <= 8){
        //    return reduce<T,uint8_t>(data,PRIMES);
        //} else if(MODELS <= UINT16_MAX){
            return reduce<T,uint16_t>(data,PRIMES);
        } else if(MODELS <= 32){
            return reduce<T,uint32_t>(data,PRIMES);
        } else if(MODELS <= 64){
            return reduce<T,uint64_t>(data,PRIMES);
        } else if(MODELS <= 128){
            return reduce<T,uint128_t>(data,PRIMES);
        } else return -1;
    }
    return PRIMES;
}

template <typename M>
template <typename T>
int qm<M>::quine_mccluskey(void *data){
    unsigned int VARIABLES = variables.size();
    unsigned int MODELS = pow2(VARIABLES);
    unsigned int GROUPS = VARIABLES+1;
    unsigned int PRIMES = 0;
    unsigned int meta_size = GROUPS;
    unsigned int cubes_size = 2*factorial(VARIABLES);

    cube_t<T> *primes = (cube_t<T>*) data;
    uint32_t *size = (uint32_t*) (primes + MODELS);
    uint32_t *offset = size + 2*meta_size;
    char *check = (char*) (offset + 2*meta_size);
    cube_t<T> *cubes = (cube_t<T>*) (check + cubes_size);

    memset(size, 0, sizeof(uint32_t)*meta_size);
    memset(check, 0, sizeof(uint8_t)*cubes_size);

    // prepare cubes
    T *model_to_group = (T*) data;
    unsigned int ccubes_size = models.size();
    for(unsigned int i = 0; i < ccubes_size; i++){
        M c = models[i];
        T group = bitcount(c);
        model_to_group[c] = group;
        size[group]++;
    }

    offset[0] = 0;
    for(unsigned int i = 1; i < GROUPS; i++){
        offset[i] = offset[i-1] + size[i-1];
        size[i-1] = 0;
    }   size[GROUPS-1] = 0;

    for(unsigned int i = 0; i < ccubes_size; i++){
        T c = models[i];
        uint32_t index = offset[model_to_group[c]] + size[model_to_group[c]]++;
        cubes[index] = {{c,0}};
    }

    // quine-mccluskey
    unsigned int groups = GROUPS-1;
    cube_t<T> *ccubes = cubes, *ncubes = cubes + cubes_size;
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

                    cube_t<T> &ci = ccubes[oi], &cj = ccubes[oj];
                    uint16_t p = ci[0] ^ cj[0];
                    if(ci[1] == cj[1] && is_power_of_two_or_zero(p)){
                        // merge

                        cube_t<T> &co = ncubes[ncubes_size];
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
                    if(primes[p] == ccubes[i])
                        insert = false;

                if(insert)
                    primes[PRIMES++] = ccubes[i];
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

    sort(primes, primes+PRIMES);
    for(unsigned int i = 0; i < PRIMES; i++){
        printf(" (%d, %d)", primes[i][0], primes[i][1]);
    }
    printf("\n");
    return PRIMES;
}

template <typename M>
template <typename T>
inline unsigned int qm<M>::get_weight(cube_t<T> &c, const T &MASK) const{
    T cover = (~(c[1])) & MASK;
    unsigned int weight = bitcount(cover);
    weight = (weight==1?0:weight);
    weight += bitcount((~(c[0])) & cover);
    return weight;
}

template <typename M>
template <typename P, typename T>
int qm<M>::reduce(void *data, unsigned int PRIMES){
    const unsigned int MODELS = models.size();
    cube_t<P>* primes = (cube_t<P>*) data;                        // cube_t primes[PRIMES]
    uint16_t *prime_weight = (uint16_t*) (primes+PRIMES);          // int prime_weight[PRIMES]
    T *prime_cover = (T*) (prime_weight+PRIMES);                  // cover prime_cover[PRIMES]
    uint16_t *chart_size = (uint16_t*) (prime_cover+PRIMES);      // int chart_size[MODELS]
    uint32_t *chart_offset = (uint32_t*) (chart_size+MODELS);     // int chart_offset[MODELS]
    T *chart = (T*) (chart_offset+MODELS);                         // int chart[MODELS]

    memset((void*)prime_cover,0,sizeof(T)*MODELS+sizeof(unsigned int)*MODELS);
    sort(primes, primes+PRIMES);

    // make prime chart
    unsigned int CHART_SIZE = 0;
    for(unsigned int i = 0; i < MODELS; i++){
        chart_offset[i] = CHART_SIZE;
        chart_size[i] = 0;
        for(unsigned int p = 0; p < PRIMES; p++){
            if((models[i] & (~primes[p][1])) == primes[p][0]){
                prime_cover[p] |= 1<<i;
                chart[CHART_SIZE++] = p;
                chart_size[i]++;
            }
        }
    }

    // identify essential implicates and remove the models they cover
    const T COVER = (T) ((1 << (MODELS-1))-1) | (1 << (MODELS-1));   // (1 << 32)-1 = 0 !!
    const P MASK = (T)((1 << (variables.size()-1))-1) | (1 << (variables.size()-1));
    T cover = COVER;
    //unsigned int *essentials = (unsigned int*) alloca(sizeof(unsigned int)*PRIMES);
    unsigned int *essentials = (unsigned int*) malloc(sizeof(unsigned int)*PRIMES);
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
                cover &= ~prime_cover[p];
                essentials[essential_size++] = p;
                weight += 1 + get_weight<P>(primes[p],MASK);
            }
        }
    }

    // find minimal prime implicate representation
    unsigned int non_essential_size = 0;
    unsigned int min_weight = ~0;
    unsigned int max_depth = 0;
    if(cover != 0){
        unsigned int *non_essentials = essentials+essential_size;
        T *covers = (T*) alloca(sizeof(T)*(MODELS)+sizeof(cube_t<P>)*(PRIMES));
        if(!covers){
            printf("failed to allocate covers\n");
            return -1;
        }
        unsigned int *weights = (unsigned int*) covers + PRIMES;

        covers[0] = cover;
        weights[0] = weight;

        cube_t<T> stack[100]; // cube_t(prime index (< PRIMES), i (< max depth))
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
                        weights[depth+1] = weights[depth] + get_weight<P>(primes[p], MASK) + 1;

                        // prune
                        if(weights[depth+1] > min_weight){
                            stack[depth][0]++;
                            i--;
                        } else {
                            // update cover
                            cover = covers[depth] & (~prime_cover[p]);

                            // determine covering
                            if(cover == 0){
                                // go through more primes on current depth
                                //unsigned int weight = depth+essentials+1;
                                //set_min_value(min_weight, weights[depth+1] + (weight==1?-1:0));a

                                non_essential_size = depth+1;
                                max_depth = (max_depth<non_essential_size?non_essential_size:max_depth);
                                unsigned int tmp_weight = weights[depth+1];
                                if(non_essential_size+essential_size==1)
                                    tmp_weight--;

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
                                //    printf(" %d:%d:(%d, %d)", p, get_weight(primes[p],MASK), primes[p][0], primes[p][1]);
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
                        if(depth >= 0){
                            stack[depth][0]++;
                            i = stack[depth][1]-1;
                        }
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
    sort(essentials, essentials+essential_size+non_essential_size);

    printf("%u\n", max_depth);
    printf("%u:", min_weight);
    for(unsigned int i = 0; i < essential_size+non_essential_size; i++){
        printf(" %u:(%lu, %lu)", get_weight<P>(primes[essentials[i]],MASK), primes[essentials[i]][0], primes[essentials[i]][1]);
    }
    printf("\n");

    return 0;
}

template class qm<uint8_t>;
template class qm<uint16_t>;
template class qm<uint32_t>;
template class qm<uint64_t>;
template class qm<uint128_t>;

