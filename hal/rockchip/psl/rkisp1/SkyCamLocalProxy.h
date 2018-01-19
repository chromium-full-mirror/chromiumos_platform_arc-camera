/*
 * Copyright (c) 2017, Fuzhou Rockchip Electronics Co., Ltd
 *
 * Based on Intel IPU3 HAL by Intel Corporation.
 *
 * Copyright (C) 2016-2017 Intel Corporation
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

#ifndef PSL_RKISP1_SKYCAMLOCALPROXY_H_
#define PSL_RKISP1_SKYCAMLOCALPROXY_H_

#include "SkyCamProxy.h"

class KBL_AIC;

const int NUM_ISP_PIPES = 1;

namespace android {
namespace camera2 {

class SkyCamLocalProxy: public SkyCamProxy
{
public:
    SkyCamLocalProxy();
    virtual ~SkyCamLocalProxy();

    status_t init(int cameraId, RKISP1ISPPipe **pipe, unsigned int numPipes,
            const ia_cmc_t* cmcParsed,
            const ia_binary_data* aiqb,
            RKISP1AICRuntimeParams *runtimeParams,
            unsigned int dumpAicParameters = 0,
            int test_frameworkDump = 0);
    virtual void Run(RKISP1AICRuntimeParams &runtimeParams);
    virtual void Reset(RKISP1AICRuntimeParams &runtimeParams);
    virtual std::string GetAICVersion();
    virtual aic_config* GetAicConfig();

private:

    KBL_AIC *mSkyCam;
    RKISP1ISPPipe *mPipe; /* SkyCamLocalProxy doesn't own mPipe */
};

} /* namespace camera2 */
} /* namespace android */

#endif /* PSL_RKISP1_SKYCAMLOCALPROXY_H_ */
