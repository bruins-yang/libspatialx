//
// Created by shixiong5 on 2021/1/12.
//

#include "fixed_level_index.h"
#include <sstream>
#include <utility>
#include <s2/s2region_coverer.h>

namespace spatialx {

FixedLevelIndex::FixedLevelIndex(StoragePtr storage, Options opt)
    : storage_(std::move(storage))
    , opt_(opt) {

    // if option is store in storage, it will replace current options.
    LoadOptionsIfExist();
}

FixedLevelIndex::~FixedLevelIndex() = default;

std::vector<S2CellId> FixedLevelIndex::MakeCovering(const S2Region* region) {
    S2RegionCoverer::Options opt;
    opt.set_fixed_level(opt_.level());
    opt.set_max_cells(std::numeric_limits<int>::max());

    std::vector<S2CellId> coverings;
    S2RegionCoverer coverer(opt);
    coverer.GetCovering(*region, &coverings);
    return coverings;
}

std::vector<QueryRange> FixedLevelIndex::MakeQueryRange(const S2Region* region) {
    auto coverings = MakeCovering(region);
    assert(!coverings.empty());

    if (coverings.size() == 1) {
        return std::vector<QueryRange>(1, {coverings[0], coverings[0]});
    }

    return MergeQueryCovering(std::move(coverings));
}

/*
 * 空间检索目前使用比较广泛的大概有两种方案，一种是空间填充曲线，一种是R-Tree,
 * 例如google s2，geohash，uber h3都是这种，那么在以空间填充曲线的实现方案中，
 * 是以一条线填充二维的面，这样平面上的点可以对应到曲线上的一个点，如果曲线可以用
 * 一个整数或者字符串表示的话，那空间上的一个点也可以用一个整数或者字符串表示。
 * 由空间填充曲线原理可知，在空间上相邻的两个点在曲线上不一定相邻，在曲线上相邻的两个点
 * 在空间上一定相邻，而绝大多数的空间检索都有一定的空间局限性，也就是说大多数的查询范围
 * 是呈区域性质的，那么所计算出来的查询索引在空间上是有可能相邻的，进而有可能在曲线上也是
 * 相邻的。如果在曲线上是相邻的那就说明具体到数据存储上也是相邻的(假如存储是使用的有序存储)。
 * 这样，由查询图形所计算出来的查询索引在一定条件下是可以合并的，即将多个相邻的查询索引合并
 * 为一个索引范围，多个索引的随机查找也就变为单个查找和多个遍历操作，在有序存储的条件下，
 * 可大大减少随机查找的次数。
 * 在圆形的查找条件下，经过测试可减少95%~99%查找操作。
 */
std::vector<QueryRange> FixedLevelIndex::MergeQueryCovering(std::vector<S2CellId>&& coverings) {
    assert(coverings.size() > 1);
    std::sort(coverings.begin(), coverings.end());
    std::vector<QueryRange> query_range_vec;

    S2CellId beg, end;
    for (int i = 0; i < coverings.size(); ++ i) {
        auto curr = coverings[i];
        if (i == 0) {
            beg = curr;
            end = curr;
        } else if (i == coverings.size() - 1) {
            if (end.next() == curr) {
                query_range_vec.emplace_back(std::pair<S2CellId, S2CellId>{beg, curr});
            } else {
                query_range_vec.emplace_back(std::pair<S2CellId, S2CellId>{beg, end});
                query_range_vec.emplace_back(std::pair<S2CellId, S2CellId>{curr, curr});
            }
        } else {
            if (end.next() == curr) {
                end = curr;
            } else {
                query_range_vec.emplace_back(std::pair<S2CellId, S2CellId>{beg, end});
                beg = curr;
                end = curr;
            }
        }
    }

    return query_range_vec;
}

void FixedLevelIndex::LoadOptionsIfExist() {
    std::string val;
    auto status = GetOption("level", &val);
    if (status == Status::kOK) {
        char* end = nullptr;
        int level = std::strtol(val.c_str(), &end, 10);
        opt_.set_level(level);
    }
}

Status FixedLevelIndex::GetOption(const char *name, std::string *value) {
    std::stringstream ss;
    ss << FixedLevelIndex::Name() << "::";
    ss << name;

    return storage_->Get(ss.str(), value);
}

} // namespace spatialx