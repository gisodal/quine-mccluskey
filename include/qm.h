#include <vector>
#include <stdint.h>

struct prime {
    uint32_t p1;
    uint32_t p2;
    prime(uint32_t pp1, uint32_t pp2){
        p1 = pp1;
        p2 = pp2;
    };
};

typedef prime prime_t;

class qm {
    public:
        qm();
        ~qm();

        inline void add_variable(uint32_t);
        inline void add_model(uint32_t);
        void clear();
        int solve();

        int compute_primes();
        int unate_cover();

    private:
        std::vector<uint32_t> variables;         // variables with least significant bit first (variables[0])
        std::vector<uint32_t> cubes;
        std::vector<prime_t> primes;
};

inline void qm::add_variable(uint32_t v){
    variables.push_back(v);
}

inline void qm::add_model(uint32_t m){
    cubes.push_back(m);
}

