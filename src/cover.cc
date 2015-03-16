#include "cover.h"

template <const unsigned int N>
bool cover<N>::operator==(cover<N> &c){
    for(unsigned int i = 0; i < N; i++)
        if(elements[i] != c[i])
            return false;
    return true;
}

template <const unsigned int N>
cover_element_t& cover<N>::operator[](unsigned int i){
    return &elements[i];
}

template <const unsigned int N>
void cover<N>::operator~(){
    for(unsigned int i = 0; i < N; i++)
        elements[i] = ~elements[i];
}

template <const unsigned int N>
void cover<N>::operator|(cover &c){
    for(unsigned int i = 0; i < N; i++)
        elements[i] |= c[i];
}

template <const unsigned int N>
void cover<N>::operator=(cover &c){
    for(unsigned int i = 0; i < N; i++)
        elements[i] = c[i];
}

template <const unsigned int N>
void cover<N>::operator&(cover &c){
    for(unsigned int i = 0; i < N; i++)
        elements[i] &= c[i];
}

template <const unsigned int N>
void cover<N>::operator^(cover &c){
    for(unsigned int i = 0; i < N; i++)
        elements[i] ^= c[i];
}



