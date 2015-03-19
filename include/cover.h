#ifndef COVER_H
#define COVER_H

#include <stdlib.h>

static unsigned int cover_size(const size_t base, unsigned int n){
    unsigned int c = 1;
    while(base <= n){
        n >>= 1;
        c <<= 1;
    }
    return (c > 1?c>>1:c);
}

template <typename T, const unsigned int N>
class cover {
    public:
        T& operator[](unsigned int);
        bool operator==(cover&);
        bool is_set(unsigned int);
        cover& operator|=(cover&);
        cover& operator|(cover&);
        cover& operator=(cover&);
        cover& operator=(unsigned int);
        cover& operator&=(cover&);
        cover& operator&(cover&);
        cover& operator^(cover&);
        cover& operator^=(cover&);
        T* front();
        T* back();
        size_t size();
        size_t bit_size();
    private:
        T* data[N];
        //const unsigned int size = N;
};

template <typename T, const unsigned int N>
bool cover<T,N>::is_set(unsigned int i){
    unsigned int size = (sizeof(T)*8);
    return (((T*)this)[i/(sizeof(T)*8)] >> (i%( sizeof(T)*8))) & 1;
}

template <typename T, const unsigned int N>
size_t cover<T,N>::size(){
    return sizeof(T)*N;
}

template <typename T, const unsigned int N>
size_t cover<T,N>::bit_size(){
    return sizeof(T)*N*8;
}

template <typename T, const unsigned int N>
T* cover<T,N>::front(){
    return (T*) this;
}

template <typename T, const unsigned int N>
T* cover<T,N>::back(){
    return front() + N;
}

template <typename T, const unsigned int N>
bool cover<T,N>::operator==(cover<T,N> &c){
    for(unsigned int i = 0; i < N; i++)
        if(((T*)this)[i] != c[i])
            return false;
    return true;
}

template <typename T, const unsigned int N>
T& cover<T,N>::operator[](unsigned int i){
    return ((T*)this)[i];
}

template <typename T, const unsigned int N>
cover<T,N>& cover<T,N>::operator|=(cover &c){
    for(unsigned int i = 0; i < N; i++)
        ((T*)this)[i] |= c[i];
    return *this;
}

template <typename T, const unsigned int N>
cover<T,N>& cover<T,N>::operator|(cover &c){
    static cover<T,N> tmp;
    for(unsigned int i = 0; i < N; i++)
        tmp[i] |= ((T*) this)[i] | c[i];
    return tmp;
}

template <typename T, const unsigned int N>
cover<T,N>& cover<T,N>::operator=(cover &c){
    for(unsigned int i = 0; i < N; i++)
        ((T*)this)[i] = c[i];
    return *this;
}

template <typename T, const unsigned int N>
cover<T,N>& cover<T,N>::operator=(unsigned int v){
    unsigned int size = sizeof(T)*8;
    T* p = (T*) this;
    for(unsigned int i = 0; i < N && i < (sizeof(unsigned int)/sizeof(T)); i++)
        ((T*) this)[i] = (T) (v>>(i*size));
    return *this;
}

template <typename T, const unsigned int N>
cover<T,N>& cover<T,N>::operator&=(cover &c){
    for(unsigned int i = 0; i < N; i++)
        ((T*)this)[i] &= c[i];
    return *this;
}

template <typename T, const unsigned int N>
cover<T,N>& cover<T,N>::operator^=(cover &c){
    for(unsigned int i = 0; i < N; i++)
        ((T*)this)[i] ^= c[i];
    return *this;
}

template <typename T, const unsigned int N>
cover<T,N>& cover<T,N>::operator&(cover &c){
    static cover<T,N> tmp;
    for(unsigned int i = 0; i < N; i++)
        tmp[i] = ((T*)this)[i] & c[i];
    return tmp;
}

template <typename T, const unsigned int N>
cover<T,N>& cover<T,N>::operator^(cover &c){
    static cover<T,N> tmp;
    for(unsigned int i = 0; i < N; i++)
        tmp[i] = ((T*)this)[i] ^ c[i];
    return tmp;
}

#endif
