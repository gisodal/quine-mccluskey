#ifndef COVER_LIST_H
#define COVER_LIST_H

#include "cover.h"

template <typename T>
class cover_list {
    public:
//        class iterator {
//            iterator& operator++();
//            iterator& operator--();
//            iterator& operator=(const iterator &it);
//            bool operator!=(const iterator &it) const;
//            bool operator==(const iterator &it) const;
//
//            unsigned int size;
//            cover_t *element;
//        };

        void init();
        static size_t bytes(unsigned int,unsigned int);
        static cover_list& cast(void*);
        void set_size(unsigned int);
        void set_cover_size(unsigned int);
        unsigned int cover_size();
        unsigned int size();
        cover<T,0>& operator[](unsigned int);
    private:
        unsigned int SIZE;
        unsigned int COVER_SIZE;
        cover<T,0> covers[];
};

#include "cover_list.th"

typedef cover_list<uint32_t> cover_list_t;

#endif

