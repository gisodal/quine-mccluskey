#ifndef QM_H
#define QM_H

#include <vector>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <algorithm>
#include <unordered_set>

#if __LP64__

#ifndef _INT128_T
#define _INT128_T
typedef __int128_t  int128_t;
#endif

#ifndef _UINT128_T
#define _UINT128_T
typedef __uint128_t uint128_t;
#endif

#ifndef INT128_MIN
#define INT128_MIN  ((__int128_t)0 - ((__int128_t)1 << 126) - ((__int128_t)1 << 126))
#endif

#ifndef INT128_MAX
#define INT128_MAX  ((__int128_t)-1 + ((__int128_t)1 << 126) + ((__int128_t)1 << 126))
#endif

#ifndef UINT128_MAX
#define UINT128_MAX (((__uint128_t)1 << 127) - (__uint128_t)1 + ((__uint128_t)1 << 127))
#endif

// #else
// #include <boost/multiprecision/cpp_int.hpp>
// using boost::multiprecision::uint128_t;
//
// #ifndef INT128_MIN
// #define INT128_MIN  ((uint128_t)0 - ((uint128_t)1 << 126) - ((__int128_t)1 << 126))
// #endif
//
// #ifndef INT128_MAX
// #define INT128_MAX  ((__int128_t)-1 + ((__int128_t)1 << 126) + ((__int128_t)1 << 126))
// #endif
//
// #ifndef UINT128_MAX
// #define UINT128_MAX (((uint128_t)1 << 127) - (uint128_t)1 + ((uint128_t)1 << 127))
// #endif

#endif

#include "cube.h"
#include "cover_element.h"
#include <vector>

template <typename M>
class qm {
    public:
        qm();
        ~qm();

        inline void add_variable(uint32_t, int index = -1);
        inline void add_model(cube<M>);
        void clear();
        int solve();

        int canonical_primes();
        size_t required_size();
        void print();

        template <typename T> int compute_primes(cube<T>*);
        template <typename T> int quine_mccluskey(cube<T>*);
        template <typename P, typename T> int reduce(cube<P>*, unsigned int);
        template <typename T> inline unsigned int get_weight(cube<T>&, const T&) const;
        template <typename P> void print_cubes(cube<P>*, unsigned int *, unsigned int);
        template <typename P> void cpy_primes(cube<P>*, unsigned int);
        void get_clause(std::vector<uint32_t>&, std::vector<uint8_t>&, unsigned int);
        unsigned int get_primes_size();

        bool reduced();
        int unate_cover();
    //private:
        size_t cube_size;
        std::vector<unsigned int> variables;         // variables with least significant bit first (variables[0])
        std::unordered_set< M > models;
        std::vector< cube<M> > primes;
};

template <typename M>
inline void qm<M>::add_variable(uint32_t v, int index){
    if(index == -1)
        variables.push_back(v);
    else {
        if(variables.size() <= (unsigned int) index)
            variables.resize(index+1);
        variables[index] = v;
    }
}

template <typename M>
inline void qm<M>::add_model(cube<M> model){
    if(model[1].any()){
        cover_element<M> m;
        m = model[0];

        while(true){
            models.insert(m.value);

            bool changed = false;
            for(unsigned int q = variables.size()-1; q >= 0; q--){
                if(!m.test(q) && model[1].test(q)){
                    m.set(q);

                    cover_element<M> t;
                    t.set_lsb(q);
                    m &= t;
                    t.negate();
                    t &= model[0];
                    m |= t;
                    //for(unsigned int r = q+1; r < variables.size(); r++)
                    //    if(model[0].test(r))
                    //        m.set(r);
                    //    else
                    //        m.clear(r);

                    changed = true;
                    break;
                }
            }
            if(!changed)
                break;
        }

    } else {
        models.insert(model[0].value);
    }
}

#endif

