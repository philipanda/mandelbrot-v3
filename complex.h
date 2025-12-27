struct complex_t {
    double r; // X
    double i; // Y
};

double complex_modulus_squared(struct complex_t c);

struct complex_t complex_square(struct complex_t c);

void complex_add(struct complex_t *a, const struct complex_t b);

struct complex_t complex_sum(const struct complex_t a, const struct complex_t b);

struct complex_t complex_sub(const struct complex_t a, const struct complex_t b);