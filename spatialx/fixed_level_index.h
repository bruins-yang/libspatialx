//
// Created by shixiong5 on 2021/1/12.
//

#ifndef LIBSPATIALX_SPATIAL_FIXED_LEVEL_INDEX_H_
#define LIBSPATIALX_SPATIAL_FIXED_LEVEL_INDEX_H_

#include <memory>
#include "index_base.h"

namespace spatialx {

class FixedLevelIndex : public IndexBase{
public:
    class Options {
        int level_ = 15;

    public:
        // level of index, 0 ~ 30, ref: https://github.com/google/s2geometry
        // if it's first time to use this index, the level will be written into the storage, else it will not effect.
        void set_level(int level) {
            assert(level >= 0 && level <= 30);
            level_ = level;
        }

        int level() const {
            return level_;
        }
    };

    FixedLevelIndex(StoragePtr storage, Options opt);

    virtual ~FixedLevelIndex();

    const Options* options() const {
        return &opt_;
    }

    std::vector<S2CellId> MakeCovering(const S2Region* region) override;

    std::vector<QueryRange> MakeQueryRange(const S2Region* region) override;

    // name of index, it will be used to store index meta data.
    static const char* Name() {
        return "fixed_level_index";
    }

private:
    void LoadOptionsIfExist();

    Status GetOption(const char* name, std::string *value);

    std::vector<QueryRange> MergeQueryCovering(std::vector<S2CellId>&& coverings);

private:
    // options
    Options opt_;

    // storage interface
    StoragePtr storage_;
};

} // namespace spatialx

#endif //LIBSPATIALX_SPATIAL_FIXED_LEVEL_INDEX_H_
