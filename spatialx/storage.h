//
// Created by shixiong5 on 2021/1/12.
//

#ifndef LIBSPATIALX_SPATIAL_STORAGE_H_
#define LIBSPATIALX_SPATIAL_STORAGE_H_

#include "status.h"

namespace spatialx {

class KV {
public:
    KV() = default;
    KV(std::string_view key, std::string_view value) : key_(key), value_(value){}

    std::string_view key() {return key_;}

    std::string_view value() {return value_;}
private:
    std::string_view key_;
    std::string_view value_;
};


class Storage {
public:
    class Iterator : public std::iterator<std::input_iterator_tag, KV> {
    public:
        virtual Status status() {return Status::kOK;}
    };

    virtual Status Get(const std::string& key, std::string *value) = 0;

    virtual std::vector<Status> MultiGet(const std::vector<std::string>& keys, std::vector<std::string> *values) = 0;

    virtual Status Put(const std::string& key, const std::string& value) = 0;

    // storage should promise MultiPut is a transaction.
    virtual Status MultiPut(const std::vector<KV>& kvs) = 0;

    virtual std::pair<Iterator, Iterator> EqualRange(const std::string& begin, const std::string& end) = 0;

    virtual Status Delete(const std::string& key) = 0;

    virtual bool KeyMayExist(const std::string& key) = 0;
};

} // namespace spatialx

#endif //LIBSPATIALX_SPATIAL_STORAGE_H_
