cmake_minimum_required(VERSION 3.14)

project(libspatialx LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(/usr/local/include)

add_library(libspatialx STATIC
        spatialx/fixed_level_index.cc
        spatialx/fixed_level_index.h
        spatialx/storage.h
        spatialx/status.h
        spatialx/index_base.cc
        spatialx/index_base.h
        spatialx/utils.h
        spatialx/rocks_storage.cc
        spatialx/rocks_storage.h
        spatialx/spatial_query.cc
        spatialx/spatial_query.h
        )

target_compile_definitions(libspatialx PRIVATE LIBSPATIALX_LIBRARY)
