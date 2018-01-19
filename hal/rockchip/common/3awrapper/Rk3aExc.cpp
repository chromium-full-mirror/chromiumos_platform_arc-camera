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

#define LOG_TAG "Rk3aExc"

#include "LogHelper.h"
#include "Rk3aExc.h"

NAMESPACE_DECLARATION {
Rk3aExc::Rk3aExc()
{
    LOG1("@%s", __FUNCTION__);
}

Rk3aExc::~Rk3aExc()
{
    LOG1("@%s", __FUNCTION__);
}

ia_err Rk3aExc::AnalogGainToSensorUnits(
                      const cmc_parsed_analog_gain_conversion_t* gain_conversion,
                      float analog_gain,
                      unsigned short* analog_gain_code)
{
    LOG1("@%s", __FUNCTION__);
    return ia_exc_analog_gain_to_sensor_units(gain_conversion, analog_gain, analog_gain_code);
}

ia_err Rk3aExc::SensorUnitsToAnalogGain(
                      const cmc_parsed_analog_gain_conversion_t* gain_conversion,
                      unsigned short gain_code,
                      float* analog_gain)
{
    LOG1("@%s", __FUNCTION__);
    return ia_exc_sensor_units_to_analog_gain(gain_conversion, gain_code, analog_gain);
}
} NAMESPACE_DECLARATION_END
