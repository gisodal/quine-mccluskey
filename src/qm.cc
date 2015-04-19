#include "qm.h"
#include <algorithm>
#include <alloca.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include "bit.h"
#include <map>
#include "cover.h"

using namespace std;

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
void qm<M>::clear(){
    variables.clear();
    models.clear();
}

template <typename M>
int qm<M>::solve(){
    //printf("     -v%d -o", variables.size());
    //for(unsigned int i = 0; i < models.size() && i < 150; i++){
    //    if(i > 0)
    //        printf(",");
    //    printf("%d", models[i]);
    //}
    //printf("\n");
    primes.resize(0);
    if(models.size() == 0)
        return 0;
    //else if(models.size() == pow2(variables.size()))
        //return 1;
    else if(models.size() == 1){
        primes.resize(1);
        primes[0][0] = *(models.begin());
        primes[0][1] = 0;
        return 2;
    } else if(canonical_primes())
        return 2;

    return -1;
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
        total_size = 800000;
        //fprintf(stderr, "space required > 8Mb onto stack!\n");
        //exit(1);
    }
    return total_size;
}

template <typename M>
template <typename P>
void qm<M>::cpy_primes(cube<P> *primes, unsigned int PRIMES){
    this->primes.resize(PRIMES);
    for(unsigned int i = 0; i < PRIMES; i++){
        this->primes[i][0] = (M) (primes[i][0].value);
        this->primes[i][1] = (M) (primes[i][1].value);
    }
}

template <typename M>
int qm<M>::canonical_primes(){
    //void *data = alloca(required_size());

    //if(!data){
    //    printf("stack allocation failed at size %u (%.2fMb)\n", required_size(), required_size()/(float)1024);
    //    return -2;
    //}
    int PRIMES;
    void *vprimes;

    unsigned int VARIABLES = variables.size();
    if(VARIABLES <= 8){
        cube_size = 8;
        vprimes = alloca(sizeof(cube<uint8_t>)*models.size());
        PRIMES = compute_primes<uint8_t>(cube<uint8_t>::cast(vprimes));
        cpy_primes(cube<uint8_t>::cast(vprimes), PRIMES);
    } else if(VARIABLES <= 16){
        cube_size = 16;
        vprimes = alloca(sizeof(cube<uint16_t>)*models.size());
        PRIMES = compute_primes<uint16_t>(cube<uint16_t>::cast(vprimes));
        cpy_primes(cube<uint16_t>::cast(vprimes), PRIMES);
    } else if(VARIABLES <= 32){
        cube_size = 32;
        vprimes = alloca(sizeof(cube<uint32_t>)*models.size());
        PRIMES = compute_primes<uint32_t>(cube<uint32_t>::cast(vprimes));
        cpy_primes(cube<uint32_t>::cast(vprimes), PRIMES);
    } else if(VARIABLES <= 64){
        cube_size = 64;
        vprimes = alloca(sizeof(cube<uint64_t>)*models.size());
        PRIMES = compute_primes<uint64_t>(cube<uint64_t>::cast(vprimes));
        cpy_primes(cube<uint64_t>::cast(vprimes), PRIMES);
    }
    #if __LP64__
    else if(VARIABLES <= 128){
        cube_size = 128;
        vprimes = alloca(sizeof(cube<uint128_t>)*models.size());
        PRIMES = compute_primes<uint128_t>(cube<uint128_t>::cast(vprimes));
        cpy_primes(cube<uint128_t>::cast(vprimes), PRIMES);
    }
    #endif
    else return -1;

    return PRIMES;
}

template <typename M>
template <typename T>
int qm<M>::compute_primes(cube<T>* primes){
    unsigned int PRIMES = quine_mccluskey<T>(primes);
    if(PRIMES > 0){
        unsigned int MODELS = models.size();
        if(MODELS <= 8){
            return reduce<T,uint8_t>(primes,PRIMES);
        } else if(MODELS <= 16){
            return reduce<T,uint16_t>(primes,PRIMES);
        } else
            return reduce<T,uint32_t>(primes,PRIMES);
    }
    return PRIMES;
}

template <typename M>
unsigned int qm<M>::get_primes_size(){
    return primes.size();
}

template <typename M>
void qm<M>::remove_prime(cube<M> &c){
    primes.erase(std::remove(primes.begin(), primes.end(), c), primes.end());
}


template <typename M>
void qm<M>::get_clause(vector<uint32_t> &literals, vector<uint8_t> &negated, unsigned int e){
    cover_element<M> p0 = primes[e][0];
    cover_element<M> p1 = primes[e][1];

    for(unsigned i = 0; i < variables.size(); i++){
        if(p0.test(i) || !p1.test(i)){
            literals.push_back(variables[i]);

            if(!p0.test(i) && !p1.test(i))
                negated.push_back(true);
            else
                negated.push_back(false);
        }
    }
}

template <typename M>
void qm<M>::print(){
    printf("(");
    for(unsigned int s = 0; s < primes.size(); s++){
        cover_element<M> p0 = primes[s][0];
        cover_element<M> p1 = primes[s][1];

        if(s > 0)
           printf("  \u2228  "); // or

        printf("(");
        unsigned int size = 0;
        for(unsigned i = 0; i < variables.size(); i++){
            if(p0.test(i) || !p1.test(i)){
                if(size++ > 0)
                    printf(" \u2227 "); // and
                if(!p0.test(i) && !p1.test(i))
                    printf("\u00AC"); // not

                if(variables[i] < 26)
                    printf("%c", 'a' + variables[i]);
                else
                    printf("%d",variables[i]);
            }
        }
        printf(")");
    }
    printf(")\n");
}

template <typename M>
template <typename T>
int qm<M>::quine_mccluskey(cube<T>* primes){
    unsigned int VARIABLES = variables.size();
    unsigned int GROUPS = VARIABLES+1;
    unsigned int PRIMES = 0;

    //cube<T> *primes = (cube<T>*) data;

    unsigned int *size = (unsigned int*) alloca(sizeof(unsigned int)*2*GROUPS);
    memset(size, 0, sizeof(unsigned int)*2*GROUPS);

    unsigned int *offset = (unsigned int*) alloca(sizeof(unsigned int)*2*GROUPS);
    memset(offset, 0, sizeof(unsigned int)*2*GROUPS);

    vector<uint8_t> check(2*models.size(),0);
    vector< cube<T> > ncubes(2*models.size());
    vector< cube<T> > ccubes(2*models.size());

    // prepare cubes
    T *model_to_group = (T*) malloc(sizeof(T)*models.size());
    unsigned int i = 0;
    for(auto it = models.begin(); it != models.end(); it++, i++){
        M c = *it;
        T group = bitcount(c);
        model_to_group[i] = group;
        size[group]++;
    }

    offset[0] = 0;
    for(unsigned int i = 1; i < GROUPS; i++){
        offset[i] = offset[i-1] + size[i-1];
        size[i-1] = 0;
    }   size[GROUPS-1] = 0;

    i = 0;
    for(auto it = models.begin(); it != models.end(); it++, i++){
        M c = *it;
        uint32_t index = offset[model_to_group[i]] + size[model_to_group[i]]++;
        ccubes[index][0] = c;
        ccubes[index][1] = 0;
    }
    free(model_to_group);

    // quine-mccluskey
    unsigned int groups = GROUPS-1;
    unsigned int *csize = size, *nsize = size + GROUPS;
    unsigned int *coffset = offset, *noffset = offset + GROUPS;
    unsigned int ccubes_size = models.size();
    unsigned int SIZE = check.size();
    if(SIZE <= csize[GROUPS-1] + coffset[GROUPS-1]){
        unsigned int s = csize[GROUPS-1] + coffset[GROUPS-1];
        ncubes.resize(s);
        ccubes.resize(s);
        check.resize(s);
        fill(check.begin()+SIZE, check.end(),0);
        SIZE = s;
    }

    while(groups){
        unsigned int ncubes_size = 0;
        for(unsigned int group = 0; group < groups; group++){
            noffset[group] = ncubes_size;
            nsize[group] = 0;

            for(unsigned int i = 0; i < csize[group]; i++){
                unsigned int oi = coffset[group]+i;
                for(unsigned int j = 0; j < csize[group+1]; j++){
                    unsigned int oj = coffset[group+1]+j;

                    T p = ccubes[oi][0].value ^ ccubes[oj][0].value;
                    if(ccubes[oi][1] == ccubes[oj][1] && is_power_of_two_or_zero(p)){
                        // merge
                        //SIZE = check.size(); // if(SIZE < (unsigned int) 2 * (csize[GROUPS-1] + coffset[GROUPS-1])){
                        if(ncubes_size >= SIZE){
                            ncubes.resize(2*SIZE);
                            ccubes.resize(2*SIZE);
                            check.resize(2*SIZE);
                            fill(check.begin()+SIZE, check.end(),0);
                            SIZE = check.size();
                        }
                        cube<T> &co = ncubes[ncubes_size];
                        co[0] = ccubes[oi][0] & ccubes[oj][0];
                        co[1] = ccubes[oi][1] | p;
                        check[oi] = 1;
                        check[oj] = 1;

                        // check for duplicates
                        bool insert = true;
                        for(unsigned int c = noffset[group]; c < ncubes_size; c++){
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

    return PRIMES;
}

template <typename M>
template <typename T>
inline unsigned int qm<M>::get_weight(cube<T> &c, const T &MASK) const {
    T cover = (~(c[1].value)) & MASK;
    unsigned int weight = bitcount(cover);
    weight = (weight==1?0:weight);
    weight += bitcount((~(c[0].value)) & cover);
    return weight;
}

template <typename M>
template <typename P, typename T>
int qm<M>::reduce(cube<P> *primes, unsigned int PRIMES){
    const unsigned int MODELS = models.size();

    uint16_t *prime_weight = (uint16_t*) alloca(sizeof(uint16_t*)*PRIMES);                   // int prime_weight[PRIMES]
    unsigned int N = cover<T,0>::cover_size(MODELS);
    cover_list<T> *prime_cover_ptr = (cover_list<T>*) malloc(sizeof(unsigned int)*2+sizeof(T)*N*PRIMES);
    cover_list<T> &prime_cover = *prime_cover_ptr;//cover_list<T>::cast((malloc(sizeof(T)*N*MODELS)));  // cover prime_cover[PRIMES]
    prime_cover.set_size(PRIMES);
    prime_cover.set_cover_size(N);
    prime_cover.init(1);

    uint16_t *chart_size = (uint16_t*) alloca(sizeof(uint16_t)*MODELS);   // int chart_size[MODELS]
    uint32_t *chart_offset = (uint32_t*) alloca(sizeof(uint32_t)*MODELS); // int chart_offset[MODELS]
    vector<T> chart;// = (T*) alloca(sizeof(T)*MODELS*PRIMES);                      // int chart[MODELS*PRIMES]

    // determine weight of primes
    const P MASK = ((P)1 << variables.size()) -1;
    for(unsigned int i = 0; i < PRIMES; i++)
        prime_weight[i] = get_weight<P>(primes[i], MASK);

    // make prime chart
    unsigned int i = 0;
    for(auto it = models.begin(); it != models.end(); it++, i++){
        chart_offset[i] = chart.size();
        chart_size[i] = 0;
        for(unsigned int p = 0; p < PRIMES; p++){
            if(((*it) & (~primes[p][1].value)) == primes[p][0].value){
                prime_cover[p].clear(i);
                chart.push_back(p);
                chart_size[i]++;
            }
        }
    }

    // identify essential implicates and remove the models they cover
    cover<T,0> &cvr = cover<T,0>::cast(alloca((cover<T,0>::bytes(N))));
    cvr.init(0,N);
    cvr.set_lsb(MODELS);
    //unsigned int *essentials = (unsigned int*) alloca(sizeof(unsigned int)*PRIMES);
    unsigned int *essentials = (unsigned int*) alloca(sizeof(unsigned int)*PRIMES);
    unsigned int essential_size = 0;
    unsigned weight = 0;
    for(i = 0; i < MODELS && !cvr.none(N); i++){
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
                cvr.and_assign(prime_cover[p],N);
                essentials[essential_size++] = p;
                weight += 1 + prime_weight[p];
            }
        }
    }

    // find minimal prime implicate representation
    unsigned int non_essential_size = 0;
    unsigned int min_weight = ~0u;
    //unsigned int max_depth = 0;
    if(cvr.any(N)){
        unsigned int *non_essentials = essentials+essential_size;
        //void *data = alloca((cover<T,0>::bytes(N))*PRIMES+sizeof(uint16_t)*PRIMES);
        size_t covers_size_t = cover_list<T>::bytes(MODELS, N);
        cover_list<T> &covers = cover_list<T>::cast(alloca(covers_size_t*PRIMES+sizeof(uint16_t)*PRIMES));
        if(!&covers){
            printf("failed to allocate covers\n");
            return -1;
        }
        covers.set_cover_size(N);
        covers.set_size(PRIMES);

        uint16_t *weights = (uint16_t*) covers.end();

        covers[0].assign(cvr,N);
        weights[0] = weight;

        array<unsigned int,2> *stack = (array<unsigned int,2>*) alloca(sizeof(array<unsigned int,2>)*PRIMES); // cube(prime index (< PRIMES), i (< max depth))
        int depth = 0;

        i = 0;
        stack[depth][0] = 0;
        while(depth >= 0){
            if(covers[depth].test(i)){
                while(true){
                    if(stack[depth][0] < chart_size[i]){
                        unsigned int p = chart[chart_offset[i]+stack[depth][0]];

                        // compute weight
                        weights[depth+1] = weights[depth] + prime_weight[p] + 1;

                        // prune
                        if(weights[depth+1] >= min_weight)
                            stack[depth][0]++;
                        else {
                            stack[depth][1] = i;

                            // update cover
                            covers[depth+1].assign(covers[depth],N);
                            covers[depth+1].and_assign(prime_cover[p],N);

                            // determine covering
                            if(covers[depth+1].none(N)){
                                // go through more primes on current depth
                                //unsigned int weight = depth+essentials+1;
                                //set_min_value(min_weight, weights[depth+1] + (weight==1?-1:0));a

                                non_essential_size = depth+1;
                                //max_depth = (max_depth<non_essential_size?non_essential_size:max_depth);
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

                                stack[depth][0]++;
                            } else { // acquire more primes
                                stack[depth][1] = i;
                                depth++;
                                stack[depth][0] = 0;
                                break;
                            }
                        }
                    } else {
                        depth--;
                        if(depth >= 0){
                            stack[depth][0]++;
                            i = stack[depth][1];
                        } else break;
                    }
                }
            }
            i++;
        }
    }

    if(min_weight == (unsigned int) ~0){
        min_weight = (essential_size+non_essential_size==1?weight-1:weight);
    }

    //printf("%u\n", max_depth);
    //printf("%u:", min_weight);
    //for(unsigned int i = 0; i < essential_size+non_essential_size; i++){
    //    printf(" %u:(%lu, %lu)", get_weight<P>(primes[essentials[i]],MASK), primes[essentials[i]][0], primes[essentials[i]][1]);
    //}
    //printf("\n");
    //print_cubes<P>(primes, essentials,essential_size+non_essential_size);

    free(prime_cover_ptr);
    return essential_size+non_essential_size;
}

template <class M>
bool qm<M>::reduced(){
    if(primes.size() < models.size())
        return true;
    else {
        for(unsigned int i = 0; i < primes.size(); i++)
            if(primes[i][1].any())
                return true;
    }

    return false;
}

template <class M>
template <class P>
void qm<M>::print_cubes(cube<P> *primes, unsigned int *essentials, unsigned int SIZE){
    printf("(");
    for(unsigned int s = 0; s < SIZE; s++){
        unsigned int e = essentials[s];
        cover_element<P> p0 = *((cover_element<P>*) &primes[e][0]);
        cover_element<P> p1 = *((cover_element<P>*) &primes[e][1]);

        if(s > 0)
           printf("  \u2228  "); // or

        printf("(");
        unsigned int size = 0;
        for(unsigned i = 0; i < variables.size(); i++){
            if(p0.test(i) || !p1.test(i)){
                if(size++ > 0)
                    printf(" \u2227 "); // and
                if(!p0.test(i) && !p1.test(i))
                    printf("\u00AC"); // not

                if(variables[i] < 26)
                    printf("%c", 'a' + variables[i]);
                else
                    printf("%d",variables[i]);
            }
        }
        printf(")");
    }
    printf(")\n");
}

template class qm<uint8_t>;
template class qm<uint16_t>;
template class qm<uint32_t>;
template class qm<uint64_t>;

template class cover_element<uint8_t>;
template class cover_element<uint16_t>;
template class cover_element<uint32_t>;
template class cover_element<uint64_t>;

#if __LP64__
template class qm<uint128_t>;
template class cover_element<uint128_t>;
#endif

