#ifndef COVER_H
#define COVER_H

#include <stdlib.h>
#include <stdint.h>

#include "cover_element.h"

template <typename T, unsigned int N>
class cover {
    public:
        cover();
        void init(const unsigned int SIZE = N);
        static cover& cast(void*);
        bool test(unsigned int) const;
        unsigned int count(const unsigned int SIZE = N) const;
        bool none(const unsigned int SIZE = N) const;
        bool all(const unsigned int SIZE = N) const;
        size_t size(const unsigned int SIZE = N);
        void set(unsigned int);
        void set_lsb(unsigned int i);
        void set_all(const unsigned int SIZE = N);
        cover_element<T>* get_cover_elements() const;
        T* get_elements() const;

        T& operator[](unsigned int) const;

        bool equals(const cover&,const unsigned int SIZE = N) const;
        cover& assign(unsigned int,const unsigned int SIZE = N);
        cover& assign(const cover&,const unsigned int SIZE = N);
        cover& or_assign(const cover&,const unsigned int SIZE = N);
        cover& and_assign(const cover&,const unsigned int SIZE = N);
        cover& xor_assign(const cover&,const unsigned int SIZE = N);

        //bool operator==(const cover&) const;
        //bool operator!=(const cover&) const;
        //cover& operator=(unsigned int);
        //cover& operator=(const cover&);
        //cover& operator|=(const cover&);
        //cover& operator&=(const cover&);
        //cover& operator^=(const cover&);
        //cover& operator|(const cover&) const;
        //cover& operator&(const cover&) const;
        //cover& operator^(const cover&) const;

    private:
        T values[N];
};

template <typename T>
class cover<T,0> {
    public:
        void init(const unsigned int);
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
        cover_element<T>* get_cover_elements() const;
        T* get_elements() const;

        T& operator[](unsigned int) const;

        bool equals(const cover&, const unsigned int) const;
        cover& assign(unsigned int, const unsigned int);
        cover& assign(const cover&, const unsigned int);
        cover& or_assign(const cover&, const unsigned int);
        cover& and_assign(const cover&, const unsigned int);
        cover& xor_assign(const cover&, const unsigned int);
};

#include "cover.th"

typedef cover<uint32_t,0> cover_t;

#endif
