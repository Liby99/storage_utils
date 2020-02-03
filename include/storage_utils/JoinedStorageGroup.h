#include "DenseStorageGroup.h"
#include "VecStorageGroup.h"

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
  std::tuple<> get_bulk(Entity i) { return std::make_tuple(); }

  bool contains(Entity i) { return true; }
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
};

template <class VS, class... DSS>
class JoinedStorageGroup {
public:
  JoinedStorageGroup(VS &vs, DSS &... dss) : vs(vs), dss(dss...) {}

  bool contains(Entity i) {
    return this->vs.contains(i) && this->dss.contains(i);
  }

private:
  VS &vs;
  JoinedStorageGroupBase<0, DSS...> dss;
};

#endif