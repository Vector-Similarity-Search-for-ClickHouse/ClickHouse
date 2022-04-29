#include <Storages/MergeTree/IMergeTreeIndexReturnIdCondition.h>
#include "Storages/MergeTree/MergeTreeIndices.h"


namespace DB
{

std::vector<size_t> IMergeTreeIndexReturnIdCondition::returnIdRecords(MergeTreeIndexGranulePtr granule, size_t before, size_t after) const {
    std::vector<size_t> ans = returnIdRecordsImpl(granule);
    std::vector<size_t> result;
    for (const auto& x : ans) {
        if (x >= before && x < after) {
            result.push_back(x - before);
        }
    }
    return result;
}

}
