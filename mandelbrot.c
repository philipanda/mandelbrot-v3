#include "mandelbrot.h"

int find_mandelbrot_depth(const struct complex_t c, unsigned int max_iter) {
  struct complex_t z = {0,0};
  unsigned int n = 0;

  while (complex_modulus_squared(z) <= 4 && n < max_iter) {
    z = complex_square(z);
    complex_add(&z, c);
    ++n;
  }
  return n + 1;
}