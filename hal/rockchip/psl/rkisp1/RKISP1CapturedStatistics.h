/*
 * Copyright (c) 2017, Fuzhou Rockchip Electronics Co., Ltd
 *
 * Based on Intel IPU3 HAL by Intel Corporation.
 *
 * Copyright (C) 2015-2017 Intel Corporation
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

#ifndef CAMERA3_HAL_RKISP1CAPTUREDSTATISTICS_H_
#define CAMERA3_HAL_RKISP1CAPTUREDSTATISTICS_H_

#include <memory>
#include "Rk3aPlus.h"

namespace android {
namespace camera2 {

/**
 * \struct RKISP1CapturedStatistics
 *
 * It can store one or more types of statistics (AF,AWB,AE).
 *
 * Firstly, the statistics will be gotten
 * from the node "3a stat" of IMGU in StatisticsWorker.
 * Then the statistics will be sent to the ControlUnit.
 * In the ControlUnit before running AE, AF and AWB,
 * the statistics is set to the algo via rk_aiq_statistics_set().
 */
struct RKISP1CapturedStatistics {
    rk_aiq_statistics_input_params aiqStatsInputParams;
};

} //namespace android
} //namespace camera2

#endif /* CAMERA3_HAL_RKISP1CAPTUREDSTATISTICS_H_ */
