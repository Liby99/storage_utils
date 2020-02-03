#include "../include/Prelude.h"
#include <assert.h>

typedef std::tuple<float, float, float> Vector3f;

typedef std::tuple<float, Vector3f, Vector3f> Particle;

// Mass, Position, Velocity
typedef VecStorageGroup<float, Vector3f, Vector3f> Particles;

int main() {
  Particles particles;

  // Insert 100 points
  for (int i = 0; i < 100; i++) {
    particles.insert(i, Vector3f(10, 10, 10), Vector3f(10, 10, 10));
  }

  // Iterating through all the particles should give
  std::size_t i = 0;
  for (auto [index, mass, position, velocity] : particles) {
    assert(index == i);
    assert(mass == i);
    i += 1;
  }
  assert(i == 100);

  // Remove every first of two
  for (int i = 0; i < 100; i += 2) {
    particles.remove(i);
  }

  // Then iterating through all the particles should give
  std::size_t j = 0;
  for (auto [index, mass, position, velocity] : particles) {
    assert(index == 2 * j + 1);
    j += 1;
  }
  assert(j == 50);
}