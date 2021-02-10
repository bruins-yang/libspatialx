//
// Created by shixiong5 on 2021/1/13.
//

#include "index_base.h"
#include <s2/s2point_region.h>
#include <s2/s2latlng.h>
#include "utils.h"

namespace spatialx {

Status IndexBase::PutPoint(const std::string& id, const S2Point& point) {
    S2PointRegion region(point);
    return PutRegion(id, &region);
}

Status IndexBase::PutPolyline(const std::string& id, const S2Polyline& polyline) {
    return PutRegion(id, &polyline);
}

Status IndexBase::PutPolygon(const std::string& id, const S2Polygon& polygon) {
    return PutRegion(id, &polygon);
}

Status IndexBase::PutRegion(const std::string& id, const S2Region* region) {
    std::string val = MakeValue(region);
    std::vector<S2CellId> coverings = MakeCovering(region);
    return PutCoverings(id, coverings, val);
}

Status IndexBase::PutCoverings(const std::string& id, const std::vector<S2CellId>& coverings, const std::string& value) {
    // because most of time the data is point that has only one index.
    if (coverings.size() == 1) {
        std::string key = MakeKey(id, coverings.at(0));
        return storage_->Put(key, value);
    }

    std::vector<std::pair<std::string, std::string>> kvs;
    if (kvs.capacity() < coverings.size()) {
        kvs.reserve(coverings.size());
    }

    for (S2CellId covering : coverings) {
        std::string key = MakeKey(id, covering);
        kvs.emplace_back(std::pair<std::string, std::string>{key, value});
    }

    return storage_->MultiPut(kvs);
}

std::string IndexBase::MakeKey(const std::string& id, S2CellId idx) {
    std::string key;
    key.resize(sizeof(idx) + id.length() + 1);

    utils::Numeric2Bytes(idx.id(), reinterpret_cast<std::byte*>(&key[0]));
    strcpy(&key[0] + sizeof(uint64_t), id.c_str());
    return key;
}

std::string IndexBase::MakeValue(const S2Region* region) {
    S2LatLngRect rect = region->GetRectBound();
    std::string val;

    if (rect.is_point()) {
        // point, lat lng
        val.resize(kPointValueSize);
        double lat = rect.lat_lo().degrees();
        double lng = rect.lng_lo().degrees();
        utils::Numeric2Bytes(lat, reinterpret_cast<std::byte*>(&val[0]));
        utils::Numeric2Bytes(lng, reinterpret_cast<std::byte*>(&val[0] + sizeof(double)));
    } else {
        // out rect bound of region, save low and high point. low lat, low lng, hi lat, hi lng
        val.resize(kOtherValueSize);
        double lo_lat = rect.lat_lo().degrees();
        double lo_lng = rect.lng_lo().degrees();
        double hi_lat = rect.lat_hi().degrees();
        double hi_lng = rect.lng_hi().degrees();
        utils::Numeric2Bytes(lo_lat, reinterpret_cast<std::byte*>(&val[0]));
        utils::Numeric2Bytes(lo_lng, reinterpret_cast<std::byte*>(&val[0] + sizeof(double)));
        utils::Numeric2Bytes(hi_lat, reinterpret_cast<std::byte*>(&val[0] + sizeof(double) * 2));
        utils::Numeric2Bytes(hi_lng, reinterpret_cast<std::byte*>(&val[0] + sizeof(double) * 3));
    }

    return val;
}

Status IndexBase::ParseKey(const std::string& key, std::string &id, S2CellId &idx) {
    if (key.size() <= sizeof(uint64_t)) {
        return Status::kErrBadKeyFormat;
    }

    idx = S2CellId(utils::Bytes2Numeric<uint64_t>(reinterpret_cast<const std::byte*>(key.data())));
    id = key.data() + sizeof(uint64_t);
    return kOK;
}

Status IndexBase::ParseValue(const std::string& value, S2LatLngRect& rect) {
    if (value.size() != kPointValueSize && value.size() != kOtherValueSize) {
        return Status::kErrBadValueFormat;
    }

    double lo_lat = utils::Bytes2Numeric<double>(
            reinterpret_cast<const std::byte*>(value.data()));
    double lo_lng = utils::Bytes2Numeric<double>(
            reinterpret_cast<const std::byte*>(value.data()) + sizeof(double));


    if (value.size() == kOtherValueSize) {
        double hi_lat = utils::Bytes2Numeric<double>(
                reinterpret_cast<const std::byte*>(value.data()) + sizeof(double) * 2);
        double hi_lng = utils::Bytes2Numeric<double>(
                reinterpret_cast<const std::byte*>(value.data()) + sizeof(double) * 3);

        rect = S2LatLngRect(S2LatLng::FromDegrees(lo_lat, lo_lng), S2LatLng::FromDegrees(hi_lat, hi_lng));
    } else {
        S2LatLng ll = S2LatLng::FromDegrees(lo_lat, lo_lng);
        rect = S2LatLngRect(ll, ll);
    }

    return Status::kOK;
}

} // namespace spatialx