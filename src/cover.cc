#include "cover.h"
#include "bit.h"

cover<0>& cover<0>::cast(void *v){
    return *((cover<0>*)v);
}

inline unsigned int div_round_up(unsigned int x, unsigned int y){
    //return (x + y - 1) / y; // overflow on x = 0
    return 1 + ((x - 1) / y);
}

size_t cover<0>::cover_size(unsigned int n){
    return div_round_up(n,sizeof(cover_element_t)*8);
}

cover_element_t* cover<0>::get_cover_elements() const{
    return ((cover_element_t*)this);
}

element_t* cover<0>::get_elements() const {
    return ((element_t*)this);
}

bool cover<0>::test(unsigned int i) const {
    const unsigned int m = sizeof(element_t)*8;
    cover_element_t *elements = get_cover_elements();
    return elements[i/m].test(i%m);
}

unsigned int cover<0>::count(const unsigned int N) const {
    unsigned int bc = 0;
    cover_element_t *elements = get_cover_elements();
    for(unsigned int i = 0; i < N; i++)
        bc += bitcount(*((element_t*)&(elements[i])));
    return bc;
}

bool cover<0>::none(const unsigned int N) const {
    cover_element_t *elements = get_cover_elements();
    for(unsigned int i = 0; i < N; i++)
        if(elements[i] != 0)
            return false;
    return true;
}

bool cover<0>::all(const unsigned int N) const {
    cover_element_t *elements = get_cover_elements();
    for(unsigned int i = 0; i < N; i++)
        if(elements[i] != ~(element_t)0)
            return false;
    return true;
}

element_t& cover<0>::operator[](unsigned int i) const{
    return get_elements()[i];
}

size_t cover<0>::size(const unsigned int N){
    return sizeof(element_t)*N*8;
}

void cover<0>::set(unsigned int i) {
    const unsigned int m = sizeof(element_t)*8;
    cover_element_t *elements = get_cover_elements();
    elements[i/m].set(i%m);
}

void cover<0>::set_lsb(unsigned int n) {
    const unsigned int m = sizeof(element_t)*8;
    cover_element_t *elements = get_cover_elements();
    for(unsigned int i = 0; i < n/m; i++)
        elements[i] = ~(element_t)0;

    if(n % m)
        elements[n/m] = ((1 << ((n%m)-1))-1) | (1 << ((n%m)-1));
}

void cover<0>::set_all(const unsigned int N) {
    cover_element_t *elements = get_cover_elements();
    for(unsigned int i = 0; i < N; i++)
        elements[i] = ~0;
}

bool cover<0>::equals(const unsigned int N,const cover &c) const {
    element_t* t = get_elements();
    for(unsigned int i = 0; i < N; i++)
        if(t[i] != c[i])
            return false;
    return true;
}

cover<0>& cover<0>::assign(const unsigned int N, unsigned int v){
    element_t* t = (element_t*) this;
    size_t size = sizeof(element_t)*8;
    for(unsigned int i = 0; i < N && i < (sizeof(unsigned int)/sizeof(element_t)); i++)
        t[i] = (element_t) (v>>(i*size));
    return *this;
}

cover<0>& cover<0>::assign(const unsigned int N, const cover &c) {
    element_t *t = get_elements();
    for(unsigned int i = 0; i < N; i++)
        t[i] = c[i];
    return *this;
}

cover<0>& cover<0>::or_assign(const unsigned int N, const cover &c) {
    element_t *t = get_elements();
    for(unsigned int i = 0; i < N; i++)
        t[i] |= c[i];
    return *this;
}

cover<0>& cover<0>::and_assign(const unsigned int N, const cover &c) {
    element_t *t = get_elements();
    for(unsigned int i = 0; i < N; i++)
        t[i] &= c[i];
    return *this;
}

cover<0>& cover<0>::xor_assign(const unsigned int N, const cover &c) {
    element_t *t = get_elements();
    for(unsigned int i = 0; i < N; i++)
        t[i] ^= c[i];
    return *this;
}

//cover_list::iterator::iterator& operator++(unsigned int){
//    return (element += size);
//}
//
//cover_list::iterator::iterator& operator--(unsigned int){
//    return (element -= size);
//}
//
//cover_list::iterator& cover_list::iterator::operator=(const void *v, unsigned int s){
//    element = (cover_element<T>*) v;
//    size = s;
//}
//
//cover_list::iterator& cover_list::iterator::operator=(const iterator &it){
//    size = it.size;
//    element = it.element;
//    return *this;
//}
//
//bool cover_list::iterator::operator!=(const iterator &it) const {
//    return element != it.element;
//}
//
//bool cover_list::iterator::operator==(const iterator &it) const {
//    return element == it.element;
//}



