//
// Created by shixiong5 on 2021/2/18.
//

#ifndef LIBSPATIALX_SAPTIAL_INTERSECT_QUERY_H_
#define LIBSPATIALX_SAPTIAL_INTERSECT_QUERY_H_

#include "index_base.h"

namespace spatialx {

class SpatialQuery {
public:
    SpatialQuery(IndexPtr index);

    Status Intersect(const S2Region* region, std::vector<std::string>* result);

    Status Nearby(double lat, double lng, double radius, std::vector<std::string>* result);

    Status In(double lat, double lng, std::vector<std::string>* result);

private:
    std::string MakeKey(S2CellId idx);

private:
    IndexPtr index_;
};

} // namespace spatialx

#endif //LIBSPATIALX_SAPTIAL_INTERSECT_QUERY_H_
