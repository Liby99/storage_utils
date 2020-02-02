#include "../include/Prelude.h"
#include <assert.h>

typedef std::tuple<float, float, float> Vector3f;

typedef std::tuple<float, Vector3f, Vector3f> Particle;

// Mass, Position, Velocity
typedef DenseStorageGroup<float, Vector3f, Vector3f> Particles;

int main() {
  Particles particles;

  // Insert 100 consecutive
  for (int i = 0; i < 100; i++) {
    particles.insert(i, std::make_tuple(i, i, i), std::make_tuple(0.0, 0.0, 0.0));
  }

  // Remove every first of two. Only 50 particles left now, scattered apart
  for (int i = 0; i < 100; i += 2) {
    particles.remove(i);
  }

  // Add back 50 particles, these particles should fill the gap of previous removal
  for (int i = 0; i < 50; i++) {
    particles.insert(i + 100, std::make_tuple(i, i, i), std::make_tuple(0.0, 0.0, 0.0));
  }

  // Now the size should be 100, and `max_size` still 100, meaning that
  // all the gaps are filled without introducing new indices
  assert(particles.size() == 100);
  assert(particles._max_size() == 100);

  // Extract the mass storage, the extracted storage should also contain 100 elements
  std::vector<float> mass_storage = particles.extract<0>();
  assert(mass_storage.size() == 100);

  // Remove every first of the four. Only 75 particles should left now
  for (int i = 0; i < 100; i += 4) {
    particles.remove(i);
  }

  // Extract the mass storage again, the extracted storage should only contain 75 elements
  std::vector<float> mass_storage_2 = particles.extract<0>();
  assert(mass_storage_2.size() == 75);
}