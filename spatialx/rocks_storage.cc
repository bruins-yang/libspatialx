//
// Created by shixiong5 on 2021/2/1.
//

#include "rocks_storage.h"

#include <utility>
#include <glog/logging.h>

namespace spatialx {

RocksStorage::RocksStorage(std::shared_ptr<rocksdb::DB> db,
                           std::shared_ptr<rocksdb::ColumnFamilyHandle> column_family)
                           : db_(std::move(db))
                           , column_family_(std::move(column_family)) {
    default_read_opt_ = rocksdb::ReadOptions();
    default_write_opt_ = rocksdb::WriteOptions();
}

Status RocksStorage::Get(const std::string &key, std::string *value) {
    auto status = db_->Get(default_read_opt_, column_family_.get(), key, value);
    if (status.IsNotFound()) {
        return Status::kErrNotFound;
    }

    LOG_IF(ERROR, !status.ok()) << "get " << key << " from rocksdb failed, error: " << status.ToString();
    return status.ok() ? Status::kOK : Status::kErrRocksReadFailed;
}

std::vector<Status> RocksStorage::MultiGet(const std::vector<std::string> &keys, std::vector<std::string> *values) {
    std::vector<rocksdb::ColumnFamilyHandle*> column_families(keys.size(), column_family_.get());
    std::vector<rocksdb::Slice> slices;
    slices.reserve(keys.size());
    for (auto& key : keys) {
        slices.emplace_back(rocksdb::Slice{key.data(), key.size()});
    }

    auto status_vec = db_->MultiGet(default_read_opt_, column_families, slices, values);
    std::vector<Status> ret;
    ret.reserve(status_vec.size());
    for (int i = 0; i < status_vec.size(); ++ i) {
        auto status = status_vec[i];
        if (status.ok()) {
            ret.emplace_back(Status::kOK);
        } else if (status.IsNotFound()) {
            ret.emplace_back(Status::kErrNotFound);
        } else {
            LOG(ERROR) << "get " << keys[i] << " from rocksdb failed, error: " << status.ToString();
            ret.emplace_back(Status::kErrRocksReadFailed);
        }
    }

    return ret;
}

Status RocksStorage::Put(const std::string &key, const std::string &value) {
    auto status = db_->Put(default_write_opt_, column_family_.get(), key, value);
    if (!status.ok()) {
        LOG(ERROR) << "put " << key << ":" << value << " failed, error: " << status.ToString();
        return Status::kErrRocksWriteFailed;
    }

    return Status::kOK;
}

Status RocksStorage::MultiPut(const std::vector<KV> &kvs) {
    rocksdb::WriteBatch wb;
    for (auto& kv : kvs) {
        wb.Put(column_family_.get(), kv.first, kv.second);
    }

    auto status = db_->Write(default_write_opt_, &wb);
    if (!status.ok()) {
        LOG(ERROR) << "muti put failed, error: " << status.ToString();
        return Status::kErrRocksWriteFailed;
    }

    return Status::kOK;
}

Status RocksStorage::EqualRange(const std::string &begin, const std::string &end, std::vector<KV> *values) {
    auto iterator = db_->NewIterator(default_read_opt_, column_family_.get());
    for (iterator->Seek(begin); iterator->Valid(); iterator->Next()) {
        if (!iterator->status().ok()) {
            LOG(ERROR) << "equal range iterator is not ok, error: " << iterator->status().ToString();
            return Status::kErrRocksReadFailed;
        }

        values->emplace_back(KV{iterator->key().ToString(), iterator->value().ToString()});
    }

    return Status::kOK;
}

Status RocksStorage::Delete(const std::string &key) {
    auto status = db_->Delete(default_write_opt_, column_family_.get(), key);
    if (!status.ok()) {
        LOG(ERROR) << "delete rocksdb key " << key << " failed, error: " << status.ToString();
        return Status::kErrRocksWriteFailed;
    }

    return Status::kOK;
}

bool RocksStorage::KeyMayExist(const std::string& key) {
    std::string timestamp;
    bool value_found;
    return db_->KeyMayExist(default_read_opt_, column_family_.get(), key, &timestamp, &value_found);
}

} // namespace spatialx