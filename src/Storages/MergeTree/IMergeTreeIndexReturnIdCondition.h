#pragma once

#include "Storages/MergeTree/MergeTreeIndices.h"

namespace DB
{

class IMergeTreeIndexReturnIdCondition : public IMergeTreeIndexCondition {
public:
    virtual ~IMergeTreeIndexReturnIdCondition() override = default;
    
    virtual bool alwaysUnknownOrTrue() const override = 0;

    virtual bool mayBeTrueOnGranule(MergeTreeIndexGranulePtr granule) const override = 0;

    std::vector<size_t> returnIdRecords(MergeTreeIndexGranulePtr granule, size_t before, size_t after) const;

protected:
    virtual std::vector<size_t> returnIdRecordsImpl(MergeTreeIndexGranulePtr granule) const = 0;
};


using MergeTreeIndexReturnIdConditionPtr = std::shared_ptr<IMergeTreeIndexReturnIdCondition>;
using MergeTreeIndexReturnIdConditions = std::vector<MergeTreeIndexReturnIdConditionPtr>;

}
