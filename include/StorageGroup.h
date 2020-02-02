#include <cstddef>
#include <utility>
#include <vector>

#ifndef STORAGE_GROUP_H
#define STORAGE_GROUP_H

template <std::size_t Index, typename T>
class Storage {
public:
  Storage() {}

  std::vector<T> &get_data() { return data; }

  T get(std::size_t i) const { return this->data[i]; }

  void set(std::size_t i, T elem) { this->data[i] = elem; }

  void push(T elem) { this->data.push_back(elem); }

private:
  std::vector<T> data;
};

template <std::size_t Index, typename... Types>
class StorageGroupBase {
public:
  std::tuple<> get_bulk(std::size_t i) const { return std::make_tuple(); }

  template <typename... AllTypes>
  void set_bulk(std::size_t i, const std::tuple<AllTypes...> &args) {}

  template <typename... AllTypes>
  void push_bulk(const std::tuple<AllTypes...> &args) {}
};

template <std::size_t Index, typename T, typename... Types>
class StorageGroupBase<Index, T, Types...>
    : public Storage<Index, T>, public StorageGroupBase<Index + 1, Types...> {
public:
  static constexpr std::size_t SIZE = sizeof...(Types) + 1;

  StorageGroupBase()
      : Storage<Index, T>(), StorageGroupBase<Index + 1, Types...>() {}

  std::tuple<T, Types...> get_bulk(std::size_t i) const {
    std::tuple<Types...> rs =
        StorageGroupBase<Index + 1, Types...>::get_bulk(i);
    T hd = Storage<Index, T>::get(i);
    return std::tuple_cat(std::tie(hd), rs);
  }

  template <typename... AllTypes>
  void set_bulk(std::size_t i, const std::tuple<AllTypes...> &args) {
    Storage<Index, T>::set(i, std::get<Index>(args));
    StorageGroupBase<Index + 1, Types...>::set_bulk(i, args);
  }

  template <typename... AllTypes>
  void push_bulk(const std::tuple<AllTypes...> &args) {
    Storage<Index, T>::push(std::get<Index>(args));
    StorageGroupBase<Index + 1, Types...>::push_bulk(args);
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