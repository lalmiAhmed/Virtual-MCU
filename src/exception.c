#include "exception.h"


void load_vector_table(uint32_t *memory) {
    for (int i = 0; i < 48; i++) {
        vector_table[i] = memory[i];
    }
}
