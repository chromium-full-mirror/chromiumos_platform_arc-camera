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

#define LOG_TAG "Rk3aAiq"

#include "LogHelper.h"
#include "Rk3aAiq.h"

NAMESPACE_DECLARATION {
Rk3aAiq::Rk3aAiq():
        mAiq(nullptr)
{
    LOG1("@%s", __FUNCTION__);
}

Rk3aAiq::~Rk3aAiq()
{
    LOG1("@%s", __FUNCTION__);
}

bool Rk3aAiq::init(const ia_binary_data* nvm_data,
            unsigned int stats_max_width,
            unsigned int stats_max_height,
            unsigned int max_num_stats_in)
{
    LOG1("@%s", __FUNCTION__);

    mAiq = rk_aiq_init(nvm_data, stats_max_width,
                        stats_max_height, max_num_stats_in);
    CheckError(mAiq == nullptr, false, "@%s, rk_aiq_init fails", __FUNCTION__);

    return true;
}

void Rk3aAiq::deinit()
{
    LOG1("@%s", __FUNCTION__);
    CheckError(mAiq == nullptr, VOID_VALUE, "@%s, mAiq is nullptr", __FUNCTION__);

    rk_aiq_deinit(mAiq);
    mAiq = nullptr;
}

ia_err Rk3aAiq::aeRun(const rk_aiq_ae_input_params* ae_input_params,
              rk_aiq_ae_results** ae_results)
{
    LOG1("@%s", __FUNCTION__);
    CheckError(mAiq == nullptr, ia_err_general, "@%s, mAiq is nullptr", __FUNCTION__);

    return rk_aiq_ae_run(mAiq, ae_input_params, ae_results);
}

ia_err Rk3aAiq::afRun(const rk_aiq_af_input_params* af_input_params,
              rk_aiq_af_results** af_results)
{
    LOG1("@%s", __FUNCTION__);
    CheckError(mAiq == nullptr, ia_err_general, "@%s, mAiq is nullptr", __FUNCTION__);

    return rk_aiq_af_run(mAiq, af_input_params, af_results);
}

ia_err Rk3aAiq::awbRun(const rk_aiq_awb_input_params* awb_input_params,
               rk_aiq_awb_results** awb_results)
{
    LOG1("@%s", __FUNCTION__);
    CheckError(mAiq == nullptr, ia_err_general, "@%s, mAiq is nullptr", __FUNCTION__);

    return rk_aiq_awb_run(mAiq, awb_input_params, awb_results);
}

ia_err Rk3aAiq::gbceRun(const rk_aiq_gbce_input_params* gbce_input_params,
                rk_aiq_gbce_results** gbce_results)
{
    LOG1("@%s", __FUNCTION__);
    CheckError(mAiq == nullptr, ia_err_general, "@%s, mAiq is nullptr", __FUNCTION__);

    return rk_aiq_gbce_run(mAiq, gbce_input_params, gbce_results);
}

ia_err Rk3aAiq::getAiqdData(ia_binary_data* out_rk_aiq_data)
{
    LOG1("@%s", __FUNCTION__);
    CheckError(mAiq == nullptr, ia_err_general, "@%s, mAiq is nullptr", __FUNCTION__);

    return rk_aiq_get_aiqd_data(mAiq, out_rk_aiq_data);
}

ia_err Rk3aAiq::paRun(const rk_aiq_pa_input_params* pa_input_params,
               rk_aiq_pa_results** pa_results)
{
    LOG1("@%s", __FUNCTION__);
    CheckError(mAiq == nullptr, ia_err_general, "@%s, mAiq is nullptr", __FUNCTION__);

    return rk_aiq_pa_run(mAiq, pa_input_params, pa_results);
}

ia_err Rk3aAiq::saRun(const rk_aiq_sa_input_params* sa_input_params,
               rk_aiq_sa_results** sa_results)
{
    LOG1("@%s", __FUNCTION__);
    CheckError(mAiq == nullptr, ia_err_general, "@%s, mAiq is nullptr", __FUNCTION__);

    return rk_aiq_sa_run(mAiq, sa_input_params, sa_results);
}

ia_err Rk3aAiq::statisticsSet(const rk_aiq_statistics_input_params* input_params)
{
    LOG1("@%s", __FUNCTION__);
    CheckError(mAiq == nullptr, ia_err_general, "@%s, mAiq is nullptr", __FUNCTION__);

    return rk_aiq_statistics_set(mAiq, input_params);
}

const char* Rk3aAiq::getVersion(void)
{
    LOG1("@%s", __FUNCTION__);

    return rk_aiq_get_version();
}

bool Rk3aAiq::isInitialized() const
{
    LOG1("@%s", __FUNCTION__);

    return mAiq ? true : false;
}
} NAMESPACE_DECLARATION_END
