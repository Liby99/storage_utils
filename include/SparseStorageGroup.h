#include "StorageGroup.h"
#include <unordered_set>

#ifndef SPARSE_STORAGE_GROUP_H
#define SPARSE_STORAGE_GROUP_H

template <typename... Types>
class SparseStorageGroup {
public:
  // The helper type `TypeAt<Index>`
  template <std::size_t Index>
  using TypeAt = typename extract_type_at<Index, Types...>::Type;

  // The helper type `Bulk` for the tuple of all types
  using Bulk = std::tuple<Types...>;

  // The helper type `BulkRef` for the tuple containing reference to all types
  using BulkRef = std::tuple<Types &...>;

  /**
   * Default constructor
   */
  SparseStorageGroup() : storage_group(), max_size(0) {}

private:
  std::size_t max_size;
  std::vector<std::size_t> mapped_indices;
  StorageGroup<Types...> storage_group;

  bool is_valid(std::size_t i) {
    return i < this->max_size && !this->is_removed(i);
  }
};

#endif