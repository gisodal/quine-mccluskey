#ifndef QM_H
#define QM_H

#include <vector>
#include <stdint.h>
#include <stddef.h>

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

template <typename M>
class qm {
    public:
        qm();
        ~qm();

        inline void add_variable(uint32_t);
        inline void add_model(M);
        void clear();
        int solve();

        int canonical_primes();
        template <typename T> int compute_primes(void*);
        template <typename T> int quine_mccluskey(void*);
        template <typename P, typename T> int reduce(void*, unsigned int);
        template <typename T> inline unsigned int get_weight(cube<T>&, const T&) const;
        size_t required_size();
        template <typename P> void print_cubes(void *, unsigned int *, unsigned int);

        int unate_cover();
        bool valid();
    private:
        size_t cube_size;
        std::vector<unsigned int> variables;         // variables with least significant bit first (variables[0])
        std::vector<M> models;
};

template <typename M>
inline void qm<M>::add_variable(uint32_t v){
    variables.push_back(v);
}

template <typename M>
inline void qm<M>::add_model(M m){
    models.push_back(m);
}

#endif

