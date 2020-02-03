#include <cstddef>
#include <utility>
#include <vector>
#include <algorithm>

#ifndef STORAGE_GROUP_H
#define STORAGE_GROUP_H

using Entity = std::size_t;

template <std::size_t Index, typename T>
class Storage {
public:
  Storage() {}

  T &get(Entity i) { return this->data[i]; }

  void set(Entity i, T elem) { this->data[i] = elem; }

  void push(T elem) { this->data.push_back(elem); }

  void swap(Entity i, Entity j) { std::swap(this->data[i], this->data[j]); }

private:
  std::vector<T> data;
};

template <std::size_t Index, typename... Types>
class StorageGroupBase {
public:
  std::tuple<> get_bulk(Entity i) { return std::make_tuple(); }

  template <typename... AllTypes>
  void set_bulk(Entity i, const std::tuple<AllTypes...> &args) {}

  template <typename... AllTypes>
  void push_bulk(const std::tuple<AllTypes...> &args) {}

  void swap(Entity i, Entity j) {}
};

template <std::size_t Index, typename T, typename... Types>
class StorageGroupBase<Index, T, Types...>
    : public Storage<Index, T>, public StorageGroupBase<Index + 1, Types...> {
public:
  static constexpr std::size_t SIZE = sizeof...(Types) + 1;

  StorageGroupBase()
      : Storage<Index, T>(), StorageGroupBase<Index + 1, Types...>() {}

  std::tuple<T &, Types &...> get_bulk(Entity i) {
    std::tuple<Types &...> rs =
        StorageGroupBase<Index + 1, Types...>::get_bulk(i);
    T &hd = Storage<Index, T>::get(i);
    return std::tuple_cat(std::tie(hd), rs);
  }

  template <typename... AllTypes>
  void set_bulk(Entity i, const std::tuple<AllTypes...> &args) {
    Storage<Index, T>::set(i, std::get<Index>(args));
    StorageGroupBase<Index + 1, Types...>::set_bulk(i, args);
  }

  template <typename... AllTypes>
  void push_bulk(const std::tuple<AllTypes...> &args) {
    Storage<Index, T>::push(std::get<Index>(args));
    StorageGroupBase<Index + 1, Types...>::push_bulk(args);
  }

  void swap(Entity i, Entity j) {
    Storage<Index, T>::swap(i, j);
    StorageGroupBase<Index + 1, Types...>::swap(i, j);
  }
};

template <typename T, typename... Types>
struct StorageGroup : StorageGroupBase<0, T, Types...> {};

template <std::size_t Index, typename T, typename... Args>
struct extract_type_at {
  using Type = typename extract_type_at<Index - 1, Args...>::Type;
};

template <typename T, typename... Args>
struct extract_type_at<0, T, Args...> {
  using Type = T;
};

#endif