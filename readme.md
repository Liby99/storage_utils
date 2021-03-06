# Storage Utilities

A header only vector storage utilities library in C++.

## Usage

``` c++
// mass, position, velocity
using Particles = VecStorageGroup<float, Vector2f, Vector2f>;

// theta_c, theta_s, deformation_gradient
using Deformations = DenseStorageGroup<float, float, Matrix2f>;

// hardening
using Hardenings = DenseStorageGroup<float>;

int main() {
  Particles particles;
  Deformations deformations;
  Hardenings hardenings;

  for (int i = 0; i < 100; i++) {
    auto id = particles.insert(1.0, Vector2f(1.0, 1.0), Vector2f(1.0, 1.0));
    if (i < 25) { hardenings.insert(id, 1.0); }
    if (i < 50) { deformations.insert(id, 0.0, 1.0, Matrix2f(1.0, 0.0, 0.0, 1.0)); }
  }

  for (auto [id, m, x, v, tc, ts, F] : particles.join(deformations)) {
    // Do things with particle & deformations
  }

  for (auto [id, m, x, v, hardening] : particles.join(hardenings)) {
    // Do things with particle & hardenings
  }

  for (auto [id, m, x, v, tc, ts, F, h] : particles.join(deformations, hardenings)) {
    // Do things with particle & deformation & hardenings
  }
}
```

## Compile & Run Test

This repo follows standard CMake build process:

```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

After that, if you want to test, you can type:

```
$ make test
```

This will run each of the executables inside the `tests` folder.

## Include this library in your CMake Project

Since this project is a header only library, you can simply use the following code in your `CMakeLists.txt` file:

``` cmake
add_subdirectory(${PATH_TO_YOUR_STORAGE_UTILS_DIR})
target_include_directories(${YOUR_TARGET} PUBLIC ${PATH_TO_YOUR_STORAGE_UTILS_DIR}/include/)
```

And then you can use, in your `C++` code,

```
#include <storage_utils/Prelude.h>
```