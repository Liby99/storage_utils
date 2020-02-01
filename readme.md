# Storage Utilities

The vector storage utilities in C++.

## Design Goal

There are two kinds of storages, one dense vec storage and one sparse vector storage.

### Dense vector storage

The dense vector storage should provide these functionalities:

- Being a template over the element type `T`
- All the elements stored in a compact linear memory space
- When initialized, allocate it with default or provided capacity, resize when
  the capacity is overloaded
- Ability to get the `capacity` as well as the actual number of elements `size`.
- Provide $O(1)$ element removal. The removed slot will not be emptied but instead,
  invalidated. This operation is done by adding the removed index to a `HashSet`.
- Provide the ability to average case $O(1)$ insertion. When the removed index
  `HashSet` is not empty, then randomly pick a index in that `HashSet` to put the
  element into that index. If the removed index set is empty, then push the element
  to the end of the dense vector.
- Provide the ability to update the data at a given index
- Allow outside managed removed slot. This will enable multiple `DenseVecStorage`s
  to be grouped together. Since all the storages are sharing the same indices, only
  one set of removed index set is stored.
- Provide the ability to iterate through all the elements.
- Provide the ability to copy the whole dense vector into another *truly* dense
  vector containing all the *valid* elements inside the storage.
- Allow the ability to serialize the storage. The information that needs to be stored
  are: `capacity` as `usize`, removed indices set as array, and the whole `data`
  as type `T` array (including the invalidated elements).

As an example:

``` c++
// Initialization
auto positions = DenseVecStorage<Vector3f>();

// Asserting is empty
assert(positions.is_empty());

// Add elements
for (int i = 0; i < 10; i++) {
  positions.add(Vector3f(i, i, i));
}

// Asserting is not empty and its size
assert(!positions.is_empty());
assert(positions.size() == 10);

// Remove element returns optional (Some) since there's an element at index 1
assert(positions.remove(1)); // Returns Vector3f(1, 1, 1)

// Remove element in an invalidated index returns None since now there is no element
// at index 1
assert(!positions.remove(1)); // Returns {} (None in std::optional)

// Inserting element
positions.add(Vector3f(100, 100, 100)); // This will be inserted to index 1
positions.get(1); // Vector3f(100, 100, 100)
```

Plus, since we mentioned grouping of the `DenseVecStorage`s, we should introduce a
new class `DenseVecStorageGroup` that templates on arbitrary amount of input types.
Each group will manage a unified `HashSet` storing removed indices. When the user
wants to insert a new element, one needs to provide data for all storages at the
same time. The user can, of course, specify a type parameter index to `update` or
`get` to access the element at that specific group index.

### Sparse vector storage

The sparse vector storage should provide these functionalities:

> TODO

### Interop of Dense and Sparse vector storages

> TODO

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
add_subdirectory(PATH_TO_YOUR_STORAGE_UTILS_DIR)
```