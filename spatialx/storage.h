//
// Created by shixiong5 on 2021/1/12.
//

#ifndef LIBSPATIALX_SPATIAL_STORAGE_H_
#define LIBSPATIALX_SPATIAL_STORAGE_H_

#include "status.h"

namespace spatialx {

class Storage {
public:

    virtual Status Get(const std::string& key, std::string *value) = 0;

    virtual std::vector<Status> MultiGet(const std::vector<std::string>& keys, std::vector<std::string> *values) = 0;

    virtual Status Put(const std::string& key, const std::string& value) = 0;

    // storage should promise MultiPut is a transaction.
    virtual Status MultiPut(const std::vector<std::pair<std::string, std::string>>& kvs) = 0;

    virtual Status EqualRange(const std::string& begin, const std::string& end, std::vector<std::string> *values) = 0;
};

} // namespace spatialx

#endif //LIBSPATIALX_SPATIAL_STORAGE_H_
