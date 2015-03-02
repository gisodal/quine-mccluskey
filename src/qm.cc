#include "qm.h"

using namespace std;

int qm::compute_primes(){
    uint32_t delta_size, sigma_size, primes, groups;

    uint32_t *prime1, *prime2, *offset, *size, *check, *sigma1, *sigma2;


    offset[0] = 0;
    offset[1] = 1;
    for(int i = 1; i <= groups; i++){
        offset[i] = offset[i-1] + pascal(groups,i-1);
        size[i] = 0;
    }


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





