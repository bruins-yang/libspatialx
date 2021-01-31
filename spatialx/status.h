//
// Created by shixiong5 on 2021/1/12.
//

#ifndef LIBSPATIALX_SPATIAL_STATUS_H_
#define LIBSPATIALX_SPATIAL_STATUS_H_

namespace spatialx {

enum Status {
    kOK       = 0,

    kWrongKeyFormat,
    kWrongValueFormat,
};

} // namespace spatialx

#endif //LIBSPATIALX_SPATIAL_STATUS_H_
