#ifndef COVER_H
#define COVER_H

#include <stdint.h>
typedef uint32_t cover_element_t;

template <const unsigned int N>
class cover {
    public:
        cover_element_t& operator[](unsigned int);
        bool operator==(cover&);
        void operator~();
        void operator|(cover&);
        void operator=(cover&);
        void operator&(cover&);
        void operator^(cover&);
    private:
        cover_element_t elements[N];
        //const unsigned int size = N;
};



#endif
