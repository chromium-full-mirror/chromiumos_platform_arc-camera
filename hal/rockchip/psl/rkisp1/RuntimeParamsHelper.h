/*
 * Copyright (C) 2017 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PSL_RKISP1_RUNTIMEPARAMSHELPER_H_
#define PSL_RKISP1_RUNTIMEPARAMSHELPER_H_

#include <RKISP1AICCommon.h>
#include "utils/Errors.h"

namespace android {
namespace camera2 {

class RuntimeParamsHelper {
public:
    static status_t allocateAiStructs(RKISP1AICRuntimeParams &runtimeParams);
    static void deleteAiStructs(RKISP1AICRuntimeParams &runtimeParams);
    static void copyPaResults(RKISP1AICRuntimeParams &to, rk_aiq_pa_results &from);
    static void copySaResults(RKISP1AICRuntimeParams &to, rk_aiq_sa_results &from);
    static void copyWeightGrid(RKISP1AICRuntimeParams &to, rk_aiq_hist_weight_grid *from);

private:
    RuntimeParamsHelper();
    virtual ~RuntimeParamsHelper();
};

} /* namespace camera2 */
} /* namespace android */

#endif /* PSL_RKISP1_RUNTIMEPARAMSHELPER_H_ */
