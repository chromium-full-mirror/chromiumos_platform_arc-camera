/*
 * Copyright (c) 2017, Fuzhou Rockchip Electronics Co., Ltd
 *
 * Based on Intel IPU3 HAL by Intel Corporation.
 *
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

#ifndef PSL_RKISP1_IPC_IPCAIC_H_
#define PSL_RKISP1_IPC_IPCAIC_H_
#include "RKISP1AICCommon.h"
#include "IPCCommon.h"
#include <KBL_AIC.h>
#include "RKISP1ISPPipe.h"
#include "IPCAiq.h"

NAMESPACE_DECLARATION {
typedef struct {
    RKISP1AICRuntimeParams base;

    aic_input_frame_parameters_t input_frame_params;
    aic_resolution_config_parameters_t frame_resolution_parameters;
    rk_aiq_output_frame_parameters_t output_frame_params;
    rk_aiq_exposure_parameters exposure_results;
    rk_aiq_hist_weight_grid weight_grid;
    rk_aiq_awb_results awb_results;
    rk_aiq_gbce_results gbce_results;
    pa_run_params_results pa_results;
    sa_run_params_results sa_results;
    ia_rectangle focus_rect;

    // rk_aiq_hist_weight_grid weight_grid;
    unsigned char weights[MAX_SIZE_WEIGHT_GRID];

    // rk_aiq_gbce_results gbce_results;
    float r_gamma_lut[MAX_NUM_GAMMA_LUTS];
    float b_gamma_lut[MAX_NUM_GAMMA_LUTS];
    float g_gamma_lut[MAX_NUM_GAMMA_LUTS];
    float tone_map_lut[MAX_NUM_TOME_MAP_LUTS];
} RKISP1AICRuntimeParams_data;

struct Transport {
    unsigned int numPipes;
    ia_binary_data_mod aiqb;
    uintptr_t cmcRemoteHandle;
    RKISP1AICRuntimeParams_data input;
    unsigned int dumpAicParameters;
    int test_frameworkDump;
};

#define MAX_IA_AIC_VERSION_PARAMS_DATA_SIZE 100
struct ia_aic_version_params {
    char data[MAX_IA_AIC_VERSION_PARAMS_DATA_SIZE];
    unsigned int size;
};

class IPCAic {
public:
    IPCAic();
    virtual ~IPCAic();

    bool clientFlattenInit(const RKISP1AICRuntimeParams& runtimeParams,
                                unsigned int numPipes,
                                const ia_binary_data* aiqb,
                                uintptr_t cmc,
                                unsigned int dumpAicParameters,
                                int testFrameworkDump,
                                Transport* transport);
    bool serverUnflattenInit(const Transport& transport,
                                RKISP1AICRuntimeParams** params,
                                ia_binary_data* aiqb,
                                ia_cmc_t** cmc,
                                unsigned int* numPipes,
                                unsigned int* dumpAicParameters,
                                int* test_frameworkDump);

    bool clientFlattenRun(const RKISP1AICRuntimeParams& runtimeParams, Transport* transport);
    bool serverUnflattenRun(const Transport& transport, RKISP1AICRuntimeParams** params);

    bool clientFlattenReset(const RKISP1AICRuntimeParams& runtimeParams, Transport* transport);
    bool serverUnflattenReset(const Transport& transport, RKISP1AICRuntimeParams** params);

private:
    bool flattenRKISP1AICRuntimeParams(const RKISP1AICRuntimeParams& params, RKISP1AICRuntimeParams_data* input);
    bool unflattenRKISP1AICRuntimeParams(RKISP1AICRuntimeParams_data* input);
};
} NAMESPACE_DECLARATION_END
#endif // PSL_RKISP1_IPC_IPCAIC_H_
