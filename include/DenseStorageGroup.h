#include "StorageGroup.h"
#include <unordered_set>

#ifndef DENSE_STORAGE_GROUP_H
#define DENSE_STORAGE_GROUP_H

template <typename... Types>
class DenseStorageGroup {
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
  DenseStorageGroup() : data_size(0) {}

private:
  std::size_t data_size;
  std::vector<std::size_t> data_map;
  std::vector<std::size_t> index_map;
  StorageGroup<Types...> storage_group;
};

#endif