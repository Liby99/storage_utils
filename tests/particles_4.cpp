#include "../include/Prelude.h"
#include <assert.h>
#include <random>

typedef std::tuple<float, float> Vector2f;

// Position, Velocity
typedef DenseStorageGroup<Vector2f, Vector2f> Particles;

float random_0_1() {
  return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

Vector2f random_point_on_side() {
  std::default_random_engine generator;
  std::uniform_int_distribution<int> distribution(0, 4);
  int wall = distribution(generator);
  switch (wall) {
    case 0: return Vector2f(0.0, random_0_1());
    case 1: return Vector2f(random_0_1(), 0.0);
    case 2: return Vector2f(1.0, random_0_1());
    default: return Vector2f(random_0_1(), 1.0);
  }
}

Vector2f random_direction() {
  float theta = random_0_1() * 3.14159265354;
  float speed = random_0_1() * 0.01 + 0.01;
  return Vector2f(cosf(theta) * speed, sinf(theta) * speed);
}

void step(Particles &particles) {
  // Remove all the particles that are outside, and count the number
  unsigned int removed_count = 0;
  for (auto [index, position, _] : particles) {
    bool out_x = std::get<0>(position) < 0.0 || std::get<0>(position) > 1.0;
    bool out_y = std::get<1>(position) < 0.0 || std::get<1>(position) > 1.0;
    if (out_x || out_y) {
      particles.remove(index);
      removed_count += 1;
    }
  }

  // Move all the particles
  for (auto [_, position, velocity] : particles) {
    std::get<0>(position) += std::get<0>(velocity);
    std::get<1>(position) += std::get<1>(velocity);
  }

  // Add particles
  for (int i = 0; i < removed_count; i++) {
    particles.insert(random_point_on_side(), random_direction());
  }
}

int main() {
  Particles particles;

  // Initialize 1000 particles
  for (int i = 0; i < 500; i++) {
    particles.insert(random_point_on_side(), random_direction());
  }

  // Run 1000 cycles
  for (int i = 0; i < 1000; i++) {
    step(particles);
  }
}