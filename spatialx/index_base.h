//
// Created by shixiong5 on 2021/1/13.
//

#ifndef LIBSPATIALX_SPATIAL_INDEX_BASE_H_
#define LIBSPATIALX_SPATIAL_INDEX_BASE_H_

#include <memory>

#include <s2/s2point.h>
#include <s2/s2polyline.h>
#include <s2/s2polygon.h>

#include "storage.h"

namespace spatialx {

using StoragePtr = std::shared_ptr<Storage>;
using QueryRange = std::pair<S2CellId, S2CellId>;

class IndexBase {
public:
    // put a point
    virtual Status PutPoint(const std::string& id, const S2Point& point);

    // put a polyline
    virtual Status PutPolyline(const std::string& id, const S2Polyline& polyline);

    // put a polygon
    virtual Status PutPolygon(const std::string& id, const S2Polygon& polygon);

protected:
    virtual std::vector<S2CellId> MakeCovering(const S2Region* region) = 0;

    virtual std::vector<QueryRange> MakeQueryRange(const S2Region* region) = 0;

private:
    Status PutRegion(const std::string& id, const S2Region* region);

    Status PutCoverings(const std::string& id, const std::vector<S2CellId>& coverings, const std::string& value);

    // key: idx + id
    std::string MakeKey(const std::string& id, S2CellId idx);

    // value: the minimun rect of region, used for imprecise result filtrate.
    std::string MakeValue(const S2Region* region);

    Status ParseKey(const std::string& key, std::string &id, S2CellId &idx);

    Status ParseValue(const std::string& value, S2LatLngRect& rect);

private:
    const int kPointValueSize = sizeof(double) * 2;
    const int kOtherValueSize = sizeof(double) * 4;

    StoragePtr storage_;
};

} // namespace spatialx

#endif //LIBSPATIALX_SPATIAL_INDEX_BASE_H_
