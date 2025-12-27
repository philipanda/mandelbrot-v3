#include "complex.h"

double complex_modulus_squared(struct complex_t c) {
    return c.r * c.r + c.i * c.i;
}

struct complex_t complex_square(struct complex_t c) {
    return (struct complex_t){
        c.r * c.r - c.i * c.i,
        2 * c.r * c.i
    };
}

void complex_add(struct complex_t *a, const struct complex_t b) {
    a->r += b.r;
    a->i += b.i;
}

struct complex_t complex_sum(struct complex_t a, const struct complex_t b) {
    return (struct complex_t){
        a.r + b.r,
        a.i + b.i 
    };
}

struct complex_t complex_sub(const struct complex_t a, const struct complex_t b) {
    return (struct complex_t){
        a.r - b.r,
        a.i - b.i
    };
}