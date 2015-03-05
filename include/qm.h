#include <vector>
#include <stdint.h>

struct cube {
    uint16_t s[2];
    inline bool operator==(const cube &c) const {
        return *((uint32_t*)s) == *((uint32_t*)c.s);
    };
    inline bool operator!=(const cube &c) const {
        return *((uint32_t*)s) != *((uint32_t*)c.s);
    };
    inline bool operator<(const cube &c) const {
        if(s[0] < c.s[0] || (s[0] == c.s[0] && s[1] < c.s[1]))
            return true;
        return false;
    };
};

template <unsigned int s>
class array {
    public:
        array(){ size = 0;};
        void push_back(uint16_t v){};

    private:
        unsigned int size;


};

typedef cube cube_t;

class qm {
    public:
        qm();
        ~qm();

        inline void add_variable(uint32_t);
        inline void add_model(uint16_t);
        void clear();
        int solve();

        int compute_primes();
        int unate_cover();
        bool valid();
    private:
        void uniq(cube_t*, unsigned int);
        std::vector<uint32_t> variables;         // variables with least significant bit first (variables[0])
        std::vector<uint16_t> models;
        std::vector<cube_t> primes;
};

inline void qm::add_variable(uint32_t v){
    variables.push_back(v);
}

inline void qm::add_model(uint16_t m){
    models.push_back(m);
}

