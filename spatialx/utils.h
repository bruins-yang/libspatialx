//
// Created by shixiong5 on 2021/1/28.
//

#ifndef LIBSPATIALX_SPATIALX_UTILS_H_
#define LIBSPATIALX_SPATIALX_UTILS_H_

#include <bit>
#include <string>
#include <type_traits>

namespace spatialx {
namespace utils {

template <typename Numeric, std::enable_if_t<std::is_arithmetic<Numeric>::value, bool> = true>
void Numeric2Bytes(Numeric n, std::byte* dst) {
    if constexpr (std::endian::native == std::endian::little) {
        memcpy(static_cast<void*>(dst), static_cast<void*>(&n), sizeof(Numeric));
    } else {
        for (int i = 0; i < sizeof(Numeric); ++ i) {
            *(dst + i) = 0xFF & (n >> (sizeof(Numeric) - i - 1));
        }
    }
}

template <typename Numeric, std::enable_if_t<std::is_arithmetic<Numeric>::value, bool> = true>
Numeric Bytes2Numeric(const std::byte* src) {
    Numeric n;
    if constexpr (std::endian::native == std::endian::little) {
        memcpy(static_cast<void*>(&n), static_cast<const void*>(src), sizeof(Numeric));
    } else {
        for (int i = 0; i < sizeof(Numeric); ++ i) {
            *(static_cast<std::byte*>(&n) + i) = *(src + sizeof(Numeric) - i - 1);
        }
    }

    return n;
}

} // namespace utils
} // namespace spatialx

#endif //LIBSPATIALX_SPATIALX_UTILS_H_
