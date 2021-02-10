//
// Created by shixiong5 on 2021/2/1.
//

#ifndef LIBSPATIALX_SPATIAL_ROCKS_STORAGE_H_
#define LIBSPATIALX_SPATIAL_ROCKS_STORAGE_H_

#include <string>
#include <vector>
#include <rocksdb/db.h>
#include "storage.h"

namespace spatialx {

class RocksStorage : public Storage {
public:
    RocksStorage(std::shared_ptr<rocksdb::DB> db, std::shared_ptr<rocksdb::ColumnFamilyHandle> column_family);

    Status Get(const std::string& key, std::string *value) override;

    std::vector<Status> MultiGet(const std::vector<std::string>& keys, std::vector<std::string> *values) override;

    Status Put(const std::string& key, const std::string& value) override;

    // storage should promise MultiPut is a transaction.
    Status MultiPut(const std::vector<KV>& kvs) override;

    Status EqualRange(const std::string& begin, const std::string& end, std::vector<KV> *values) override;

    Status Delete(const std::string& key) override;

    bool KeyMayExist(const std::string& key) override;

private:
    std::shared_ptr<rocksdb::DB> db_;
    std::shared_ptr<rocksdb::ColumnFamilyHandle> column_family_;

    rocksdb::ReadOptions default_read_opt_;
    rocksdb::WriteOptions default_write_opt_;
};

} // namespace spatialx

#endif //LIBSPATIALX_SPATIAL_ROCKS_STORAGE_H_
