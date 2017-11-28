/*
 * Copyright (c) 2017, Fuzhou Rockchip Electronics Co., Ltd
 *
 * Based on Intel IPU3 HAL by Intel Corporation.
 *
 * Copyright (C) 2017 Intel Corporation
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

#ifndef AAA_RK3AHELPER_H_
#define AAA_RK3AHELPER_H_

#include <rk_aiq.h>
#include <ia_coordinate.h>

NAMESPACE_DECLARATION {

#define MAX_COLOR_CONVERSION_MATRIX 3
class Rk3aHelper {
public:
    // Static debug dumpers
    static void dumpStatInputParams(const rk_aiq_statistics_input_params* sip);
    static void dumpAfInputParams(const rk_aiq_af_input_params* afCfg);
    static void dumpAeInputParams(const rk_aiq_ae_input_params &aeInput);
    static void dumpAeResult(const rk_aiq_ae_results* aeResult);
    static void dumpAfResult(const rk_aiq_af_results* afResult);
    static void dumpAwbResult(const rk_aiq_awb_results* awbResult);
    static void dumpRGBSGrids(const rk_aiq_rgbs_grid **rgbs_grids, int gridCount);
    static void dumpPaResult(const rk_aiq_pa_results* paResult);
    static void dumpSaResult(const rk_aiq_sa_results* saResult);
};

} NAMESPACE_DECLARATION_END
#endif  // AAA_RK3AHELPER_H_
