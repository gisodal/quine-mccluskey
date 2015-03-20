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

#endif

template <typename T>
struct cube_t {
    T s[2];
    inline bool operator==(const cube_t &c) const {
        return s[0] == c.s[0] && s[1] == c.s[1];
    };
    inline bool operator!=(const cube_t &c) const {
        return s[0] != c.s[0] || s[1] != c.s[1];
    };
    inline bool operator<(const cube_t &c) const {
        if(s[0] < c.s[0] || (s[0] == c.s[0] && s[1] < c.s[1]))
            return true;
        return false;
    };
    inline T& operator[](const int i) {
        return s[i];
    };
};

template <typename M>
class qm {
    public:
        qm();
        ~qm();

        inline void add_variable(uint32_t);
        inline void add_model(M);
        void clear();
        int solve();


        template <typename T> inline unsigned int get_weight(cube_t<T>&, const T&) const;
        int canonical_primes();
        template <typename T> int compute_primes(void*);
        int compute_reduce(void*, unsigned int);
        template <typename T> int quine_mccluskey(void*);
        template <typename P, typename T> int reduce(void*, unsigned int);
        size_t required_size();

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
