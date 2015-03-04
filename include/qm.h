#include <vector>
#include <stdint.h>

struct cube {
    uint16_t s[2];
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
        std::vector<uint32_t> variables;         // variables with least significant bit first (variables[0])
        std::vector<uint16_t> cubes;
        std::vector<cube_t> primes;
};

inline void qm::add_variable(uint32_t v){
    variables.push_back(v);
}

inline void qm::add_model(uint16_t m){
    cubes.push_back(m);
}

