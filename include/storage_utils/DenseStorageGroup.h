#include "StorageGroup.h"
#include <algorithm>
#include <unordered_set>

#ifndef DENSE_STORAGE_GROUP_H
#define DENSE_STORAGE_GROUP_H

template <typename... Types>
class DenseStorageGroupIterator {
public:
  DenseStorageGroupIterator(
    const std::size_t &storage_size,
    const std::vector<Entity> &global_index_map,
    StorageGroup<Types...> &storage_group,
    std::size_t index)
    : storage_size(storage_size), global_index_map(global_index_map),
      storage_group(storage_group), index(index) {}

  std::tuple<Entity, Types &...> operator*() {
    return std::tuple_cat(std::tie(this->global_index_map[this->index]),
                          this->storage_group.get_bulk(this->index));
  }

  void operator++() {
    this->index++;
  }

  bool operator!=(DenseStorageGroupIterator<Types...> other) {
    return this->index != other.index;
  }

private:
  std::size_t index;

  const std::size_t &storage_size;
  const std::vector<Entity> &global_index_map;
  StorageGroup<Types...> &storage_group;
};

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
  DenseStorageGroup() : storage_size(0) {}

  std::optional<BulkRef> get(Entity i) {
    if (i < this->data_index_map.size()) {
      auto data_index = this->data_index_map[i];
      if (data_index.has_value()) {
        return this->storage_group.get_bulk(data_index.value());
      }
    }
    return {};
  }

  BulkRef get_unchecked(Entity i) {
    auto data_index = this->data_index_map[i];
    return this->storage_group.get_bulk(data_index.value());
  }

  void insert(Entity i, Types... args) {
    auto data = std::make_tuple(args...);
    return this->insert_bulk(i, data);
  }

  void insert_bulk(Entity i, Bulk data) {
    if (i < this->data_index_map.size()) {
      auto data_index = this->data_index_map[i];
      if (data_index.has_value()) {
        this->storage_group.set_bulk(data_index.value(), data);
        return;
      } // Otherwise, append to the storage.
    } else {
      this->data_index_map.resize(i + 1, {});
    }

    // Append to the storage
    Entity local_index = this->storage_size++;
    this->data_index_map[i] = local_index;
    if (local_index < this->global_index_map.size()) {
      this->storage_group.set_bulk(local_index, data);
      this->global_index_map[local_index] = i;
    } else {
      this->storage_group.push_bulk(data);
      this->global_index_map.push_back(i);
    }
  }

  bool update(Entity i, Types... args) {
    auto data = std::make_tuple(args...);
    return this->update_bulk(i, data);
  }

  bool update_bulk(Entity i, Bulk data) {
    if (i < this->data_index_map.size()) {
      auto data_index = this->data_index_map[i];
      if (data_index.has_value()) {
        this->storage_group.set_bulk(data_index.value(), data);
        return true;
      }
    }
    return false;
  }

  bool remove(Entity i) {
    if (i < this->data_index_map.size()) {
      auto data_index = this->data_index_map[i];
      if (data_index.has_value()) {
        Entity last_index = --this->storage_size;

        // Copy the data_index and invalidate the `i`th index
        this->data_index_map[this->global_index_map[last_index]] = data_index.value();
        this->data_index_map[i] = {};

        // Swap the element on data_size & last_index;
        std::swap(this->global_index_map[data_index.value()],
                  this->global_index_map[last_index]);

        // Swap the components in the storage
        this->storage_group.swap(data_index.value(), last_index);

        // Removal success
        return true;
      }
    }
    return false;
  }

  template <std::size_t Index>
  std::optional<TypeAt<Index>> get_component(Entity i) {
    using S = Storage<Index, TypeAt<Index>>;
    if (i < this->data_index_map.size()) {
      auto data_index = this->data_index_map[i];
      if (data_index.has_value()) {
        return (static_cast<S &>(this->storage_group)).get(data_index.value());
      }
    }
    return {};
  }

  template <std::size_t Index>
  TypeAt<Index> &get_component_unchecked(Entity i) {
    using S = Storage<Index, TypeAt<Index>>;
    auto data_index = this->data_index_map[i];
    return (static_cast<S &>(this->storage_group)).get(data_index.value());
  }

  template <std::size_t Index>
  bool update_component(Entity i, TypeAt<Index> elem) {
    using S = Storage<Index, TypeAt<Index>>;
    if (i < this->data_index_map.size()) {
      auto data_index = this->data_index_map[i];
      if (data_index.has_value()) {
        (static_cast<S &>(this->storage_group)).set(data_index.value(), elem);
        return true;
      }
    }
    return false;
  }

  void print_data_index_map() {
    printf("DataIndexMap: [");
    for (int i = 0; i < this->data_index_map.size(); i++) {
      if (this->data_index_map[i].has_value()) {
        printf("%lu, ", this->data_index_map[i].value());
      } else {
        printf("None, ");
      }
    }
    printf("]\n");
  }

  std::size_t size() { return this->storage_size; }

  bool is_empty() { return this->size() == 0; }

  DenseStorageGroupIterator<Types...> begin() {
    return DenseStorageGroupIterator(this->storage_size, this->global_index_map, this->storage_group, 0);
  }

  DenseStorageGroupIterator<Types...> end() {
    return DenseStorageGroupIterator(this->storage_size, this->global_index_map, this->storage_group, this->storage_size);
  }

private:
  std::size_t storage_size;

  // From global index to local index. The local index is optional since the
  // data might not be contained in this storage. This map should have the same
  // size as the maximal `Entity` appeared in this storage group.
  std::vector<std::optional<Entity>> data_index_map;

  // From local index to global index. Has the size the same as `storage_size`
  // and `storage_group`.
  std::vector<Entity> global_index_map;

  // The dense storage group.
  StorageGroup<Types...> storage_group;
};

#endif