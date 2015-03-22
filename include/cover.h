#ifndef COVER_H
#define COVER_H

#include <stdlib.h>
#include <stdint.h>

#include "cover_element.h"

template <int N>
class cover {
    public:
        bool test(unsigned int) const;
        unsigned int count() const;
        bool none() const;
        bool all() const;
        size_t size();
        size_t size_of();
        void set(unsigned int);
        void set_lsb(unsigned int i);
        void set_all();
        cover_element_t* get_cover_elements() const;
        element_t* get_elements() const;

        element_t& operator[](unsigned int) const;
        bool operator==(const cover&) const;
        bool operator!=(const cover&) const;
        cover& operator=(unsigned int);
        cover& operator=(const cover&);
        cover& operator|=(const cover&);
        cover& operator&=(const cover&);
        cover& operator^=(const cover&);
        cover& operator|(const cover&) const;
        cover& operator&(const cover&) const;
        cover& operator^(const cover&) const;

        bool equals(const cover&) const;
        cover& assign(unsigned int);
        cover& assign(const cover&);
        cover& or_assign(const cover&);
        cover& and_assign(const cover&);
        cover& xor_assign(const cover&);
        cover& _or(const cover&) const;
        cover& _and(const cover&) const;
        cover& _xor(const cover&) const;
};

template <> class cover<0> {
    public:
        static cover& cast(void*);
        static size_t cover_size(const unsigned int n);
        bool test(unsigned int) const;
        unsigned int count(const unsigned int) const;
        bool none(const unsigned int) const;
        bool all(const unsigned int) const;
        size_t size(const unsigned int);
        void set(unsigned int);
        void set_lsb(unsigned int i);
        void set_all(const unsigned int);
        cover_element_t* get_cover_elements() const;
        element_t* get_elements() const;

        element_t& operator[](unsigned int) const;

        bool equals(const unsigned int,const cover&) const;
        cover& assign(const unsigned int, unsigned int);
        cover& assign(const unsigned int, const cover&);
        cover& or_assign(const unsigned int, const cover&);
        cover& and_assign(const unsigned int, const cover&);
        cover& xor_assign(const unsigned int, const cover&);
};

#include "cover.th"

typedef cover<0> cover_t;

#endif
