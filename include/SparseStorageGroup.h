#include "StorageGroup.h"
#include <unordered_set>

#ifndef SPARSE_STORAGE_GROUP_H
#define SPARSE_STORAGE_GROUP_H

template <typename... Types>
class SparseStorageGroup {
public:
  SparseStorageGroup() : storage_group(), max_size(0) {}

private:
  std::size_t max_size;
  std::vector<std::size_t> mapped_indices;
  std::unordered_set<std::size_t> removed_indices;
  StorageGroup<Types...> storage_group;
};

#endif