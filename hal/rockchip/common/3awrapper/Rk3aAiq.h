/*
 * Copyright (C) 2017 Intel Corporation.
 * Copyright (c) 2017, Fuzhou Rockchip Electronics Co., Ltd
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

#ifndef COMMON_3AWRAPPER_RK3AAIQ_H_
#define COMMON_3AWRAPPER_RK3AAIQ_H_
#include <rk_aiq.h>

NAMESPACE_DECLARATION {
class Rk3aAiq {
public:
    Rk3aAiq();
    virtual ~Rk3aAiq();

    bool init(const ia_binary_data* aiqb_data,
            const ia_binary_data* nvm_data,
            const ia_binary_data* aiqd_data,
            unsigned int stats_max_width,
            unsigned int stats_max_height,
            unsigned int max_num_stats_in,
            uintptr_t cmc_handle,
            uintptr_t mkn_handle);
    void deinit();
    ia_err aeRun(const rk_aiq_ae_input_params* ae_input_params,
                  rk_aiq_ae_results** ae_results);
    ia_err afRun(const rk_aiq_af_input_params* af_input_params,
                  rk_aiq_af_results** af_results);
    ia_err awbRun(const rk_aiq_awb_input_params* awb_input_params,
                   rk_aiq_awb_results** awb_results);
    ia_err gbceRun(const rk_aiq_gbce_input_params* gbce_input_params,
                    rk_aiq_gbce_results** gbce_results);
    ia_err getAiqdData(ia_binary_data* out_rk_aiq_data);
    ia_err paRun(const rk_aiq_pa_input_params* pa_input_params,
                   rk_aiq_pa_results** pa_results);
    ia_err saRun(const rk_aiq_sa_input_params* sa_input_params,
                   rk_aiq_sa_results** sa_results);
    ia_err statisticsSet(const rk_aiq_statistics_input_params* input_params);
    const char* getVersion(void);

    bool isInitialized() const;

private:
    rk_aiq* mAiq;
};
} NAMESPACE_DECLARATION_END
#endif // COMMON_3AWRAPPER_RK3AAIQ_H_