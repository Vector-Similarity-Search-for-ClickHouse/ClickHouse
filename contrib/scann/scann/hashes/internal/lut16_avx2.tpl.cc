// Copyright 2022 The Google Research Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifdef __x86_64__
#include "scann/hashes/internal/lut16_avx2.inc"

namespace research_scann {
namespace asymmetric_hashing_internal {

template class LUT16Avx2<1, PrefetchStrategy::kOff>;
template class LUT16Avx2<1, PrefetchStrategy::kSeq>;
template class LUT16Avx2<1, PrefetchStrategy::kSmart>;
template class LUT16Avx2<2, PrefetchStrategy::kOff>;
template class LUT16Avx2<2, PrefetchStrategy::kSeq>;
template class LUT16Avx2<2, PrefetchStrategy::kSmart>;
template class LUT16Avx2<3, PrefetchStrategy::kOff>;
template class LUT16Avx2<3, PrefetchStrategy::kSeq>;
template class LUT16Avx2<3, PrefetchStrategy::kSmart>;
template class LUT16Avx2<4, PrefetchStrategy::kOff>;
template class LUT16Avx2<4, PrefetchStrategy::kSeq>;
template class LUT16Avx2<4, PrefetchStrategy::kSmart>;
template class LUT16Avx2<5, PrefetchStrategy::kOff>;
template class LUT16Avx2<5, PrefetchStrategy::kSeq>;
template class LUT16Avx2<5, PrefetchStrategy::kSmart>;
template class LUT16Avx2<6, PrefetchStrategy::kOff>;
template class LUT16Avx2<6, PrefetchStrategy::kSeq>;
template class LUT16Avx2<6, PrefetchStrategy::kSmart>;
template class LUT16Avx2<7, PrefetchStrategy::kOff>;
template class LUT16Avx2<7, PrefetchStrategy::kSeq>;
template class LUT16Avx2<7, PrefetchStrategy::kSmart>;
template class LUT16Avx2<8, PrefetchStrategy::kOff>;
template class LUT16Avx2<8, PrefetchStrategy::kSeq>;
template class LUT16Avx2<8, PrefetchStrategy::kSmart>;
template class LUT16Avx2<9, PrefetchStrategy::kOff>;
template class LUT16Avx2<9, PrefetchStrategy::kSeq>;
template class LUT16Avx2<9, PrefetchStrategy::kSmart>;

}  // namespace asymmetric_hashing_internal
}  // namespace research_scann

#endif
