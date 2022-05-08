#include <Storages/MergeTree/IMergeTreeIndexReturnIdCondition.h>
#include "Storages/MergeTree/MergeTreeIndices.h"

#include <algorithm>


namespace DB
{

std::vector<size_t> IMergeTreeIndexReturnIdCondition::returnIdRecords(MergeTreeIndexGranulePtr granule, size_t from, size_t to) const {
    std::vector<size_t> ans = returnIdRecordsImpl(granule);
    std::vector<size_t> result;
    for (const auto& x : ans) {
        if (x >= from && x < to) {
            result.push_back(x - from);
        }
    }
    std::sort(result.begin(), result.end());
    return result;
}

}
