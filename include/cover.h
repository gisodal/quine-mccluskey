#ifndef COVER_H
#define COVER_H

#include <stdlib.h>

template <typename T>
class cover_element {
    public:
        size_t size() const;
        unsigned int count() const;
        bool test(unsigned int) const;
        bool none() const;
        bool all() const;
        void set_all();
        void set_lsb(unsigned int i);
        void set(unsigned int, bool val = true);
        bool operator==(const cover_element&) const;
        cover_element& operator=(unsigned int);
        cover_element& operator=(const cover_element&);
        cover_element& operator|=(const cover_element&);
        cover_element& operator&=(const cover_element&);
        cover_element& operator^=(const cover_element&);
        cover_element& operator|(const cover_element&) const;
        cover_element& operator&(const cover_element&) const;
        cover_element& operator^(const cover_element&) const;
    private:
        T value;
};

#include "cover.th"

class cover {
    public:
        static unsigned int size(const size_t base, unsigned int n);
        bool operator==(const unsigned int,const cover&) const;
        bool test(const unsigned int,unsigned int) const;
        size_t int size(const unsigned int) const;
        unsigned int count(const unsigned int) const;
        bool none(const unsigned int) const;
        bool all(const unsigned int) const;
        void set(const unsigned int, unsigned int, bool val = true);
        void set_lsb(const unsigned int, unsigned int i);
        void set_all(const unsigned int);
        cover& operator=(const unsigned int, unsigned int);
        cover& operator=(const unsigned int, const cover&);
        cover& operator|=(const unsigned int, const cover&);
        cover& operator&=(const unsigned int, const cover&);
        cover& operator^=(const unsigned int, const cover&);
        cover& operator|(const unsigned int, const cover&) const;
        cover& operator&(const unsigned int, const cover&) const;
        cover& operator^(const unsigned int, const cover&) const;
};

class cover_list {
    public:
        class iterator {
            iterator& operator++(unsigned int);
            iterator& operator--(unsigned int)
            iterator& operator=(const void *v, unsigned int s);
            iterator& operator=(const iterator &it);
            bool operator!=(const iterator &it) const;
            bool operator==(const iterator &it) const;
            cover *element;
            unsigned int size;
        };

        void init(void *, unsigned int);
        void set_list_size(unsigned int);
        cover& operator[](unsigned int);
    private:
        unsigned int size;
        unsigned int cover_size;
        cover *covers;
};

#endif
