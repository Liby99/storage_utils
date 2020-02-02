#include "../include/Prelude.h"
#include <assert.h>

typedef Tuple<float, float, float> Vector3f;

void structured_binding_1() {
  Vector3f v(1.0f, 2.0f, 3.0f);

  // Get the storages (as references)
  auto &[x, y, z] = v;
  assert(x == 1.0f);
  assert(y == 2.0f);
  assert(z == 3.0f);

  // Do mutation on vector v
  x = 100.0f;

  // Check if the mutation is done
  assert(v.get<0>() == 100.0f);
}

void structured_binding_2() {
  Vector3f v(1.0f, 2.0f, 3.0f);

  // Get the storages (as references)
  auto [x, y, z] = v;
  assert(x == 1.0f);
  assert(y == 2.0f);
  assert(z == 3.0f);

  // Do mutation on local variable `x`
  x = 100.0f;

  // `v` should not be mutated
  assert(v.get<0>() == 1.0f);
}

int main() {
  structured_binding_1();
  structured_binding_2();
}