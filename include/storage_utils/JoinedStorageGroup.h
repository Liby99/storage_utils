#ifndef JOINED_STORAGE_GROUP_H
#define JOINED_STORAGE_GROUP_H

template <std::size_t Index, typename DenseStorage>
class JoinedStorage {
public:
  DenseStorage &storage;
  JoinedStorage(DenseStorage &storage) : storage(storage) {}
};

template <std::size_t Index, typename... DenseStorages>
class JoinedStorageGroupBase {
public:
  bool contains(Entity i) { return true; }

  std::tuple<> get_unchecked(Entity i) { return std::make_tuple(); }
};

template <std::size_t Index, typename DS, typename... DenseStorages>
class JoinedStorageGroupBase<Index, DS, DenseStorages...>
    : public JoinedStorage<Index, DS>,
      public JoinedStorageGroupBase<Index + 1, DenseStorages...> {
public:
  JoinedStorageGroupBase(DS &ds, DenseStorages &... dss)
      : JoinedStorage<Index, DS>(ds),
        JoinedStorageGroupBase<Index + 1, DenseStorages...>(dss...) {}

  bool contains(Entity i) {
    return JoinedStorage<Index, DS>::storage.contains(i) &&
      JoinedStorageGroupBase<Index + 1, DenseStorages...>::contains(i);
  }

  auto get_unchecked(Entity i) {
    return std::tuple_cat(
      JoinedStorage<Index, DS>::storage.get_unchecked(i),
      JoinedStorageGroupBase<Index + 1, DenseStorages...>::get_unchecked(i)
    );
  }
};

template <class VS, class... DSS>
class JoinedStorageGroupIterator;

template <class VS, class... DSS>
class JoinedStorageGroup {
public:
  JoinedStorageGroup(VS &vs, DSS &... dss) : vs(vs), dss(dss...) {}

  bool contains(Entity i) {
    return this->vs.contains(i) && this->dss.contains(i);
  }

  auto get_unchecked(Entity i) {
    return std::tuple_cat(
      std::tie(i),
      this->vs.get_unchecked(i),
      this->dss.get_unchecked(i)
    );
  }

  std::size_t size() {
    return this->vs.size();
  }

  JoinedStorageGroupIterator<VS, DSS...> begin();

  JoinedStorageGroupIterator<VS, DSS...> end();

private:
  VS &vs;
  JoinedStorageGroupBase<0, DSS...> dss;
};

template <class VS, class... DSS>
class JoinedStorageGroupIterator {
public:
  JoinedStorageGroupIterator(JoinedStorageGroup<VS, DSS...> &s) : s(s) {
    this->index = 0;
    while (!s.contains(this->index) && this->index < s.size()) {
      this->index += 1;
    }
  }

  JoinedStorageGroupIterator(JoinedStorageGroup<VS, DSS...> &s, bool is_end) : s(s), index(s.size()) {}

  auto operator*() {
    return this->s.get_unchecked(this->index);
  }

  void operator++() {
    this->index++;
    while (!this->s.contains(this->index) && this->index < this->s.size()) {
      this->index += 1;
    }
  }

  bool operator!=(JoinedStorageGroupIterator<VS, DSS...> other) {
    return this->index != other.index;
  }

private:
  std::size_t index;
  JoinedStorageGroup<VS, DSS...> &s;
};

template <class VS, class... DSS>
JoinedStorageGroupIterator<VS, DSS...> JoinedStorageGroup<VS, DSS...>::begin() {
  return JoinedStorageGroupIterator(*this);
}

template <class VS, class... DSS>
JoinedStorageGroupIterator<VS, DSS...> JoinedStorageGroup<VS, DSS...>::end() {
  return JoinedStorageGroupIterator(*this, true);
}

#endif