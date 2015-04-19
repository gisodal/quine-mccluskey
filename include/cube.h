#ifndef CUBE_H
#define CUBE_H

#include "cover_element.h"

//template<typename T>
//using cube = std::tuple< cover_element<T>, cover_element<T> >;

template <typename T>
struct cube {
    cover_element<T> s[2];
    inline bool operator==(const cube &c) const {
        return s[0] == c.s[0] && s[1] == c.s[1];
    };
    inline bool operator!=(const cube &c) const {
        return s[0] != c.s[0] || s[1] != c.s[1];
    };
    inline bool operator<(const cube &c) const {
        if(s[0] < c.s[0] || (s[0] == c.s[0] && s[1] < c.s[1]))
            return true;
        return false;
    };
    inline T& operator[](const int i) {
        return s[i];
    };
    static cube<T>* cast(void *v){
        return (cube<T>*) v;
    };
};

#endif

