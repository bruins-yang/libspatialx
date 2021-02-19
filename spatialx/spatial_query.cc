//
// Created by shixiong5 on 2021/2/18.
//

#include "spatial_query.h"
#include <utility>
#include "utils.h"

namespace spatialx {

SpatialQuery::SpatialQuery(IndexPtr index)
    : index_(std::move(index)) {
    assert(index != nullptr);
}

Status SpatialQuery::Intersect(const S2Region *region, std::vector<std::string>* result) {
    auto query_range = index_->MakeQueryRange(region);
    auto storage = index_->storage();
    std::vector<KV> kvs;

    for (auto& range : query_range) {
        auto beg = MakeKey(range.first);
        auto end = MakeKey(range.second);
        auto status = storage->EqualRange(beg, end, &kvs);
        if (status != Status::kOK) {
            return status;
        }
        S2CrossingEdgeQuery
        // TODO
    }

    // TODO
}

Status SpatialQuery::Nearby(double lat, double lng, double radius, std::vector<std::string>* result) {
    // TODO
}

Status SpatialQuery::In(double lat, double lng, std::vector<std::string>* result) {
    // TODO
}

std::string SpatialQuery::MakeKey(S2CellId idx) {
    std::string key;
    key.resize(sizeof(S2CellId));
    utils::Numeric2Bytes(idx.id(), reinterpret_cast<std::byte*>(&key[0]));
    return key;
}

} // namespace spatialx