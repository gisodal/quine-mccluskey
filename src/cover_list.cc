#include "cover_list.h"

cover_list& cover_list::cast(void *v){
    return *((cover_list*)v);
}

void cover_list::set_size(unsigned int size){
    SIZE = size;
}

void cover_list::set_cover_size(unsigned int size){
    COVER_SIZE = size;
}

unsigned int cover_list::size(){
    return SIZE;
}

size_t cover_list::bytes(unsigned int models, unsigned int n){
    return sizeof(cover_list) + cover_t::cover_size(models)*sizeof(cover_element_t)*n;
}

unsigned int cover_list::cover_size(){
    return COVER_SIZE;
}

cover_t& cover_list::operator[](unsigned int i){
    return *(covers + sizeof(cover_element_t)*COVER_SIZE*i);
}
