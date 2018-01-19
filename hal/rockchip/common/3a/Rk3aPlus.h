/*
 * Copyright (c) 2017, Fuzhou Rockchip Electronics Co., Ltd
 *
 * Based on Intel IPU3 HAL by Intel Corporation.
 *
 * Copyright (C) 2014-2017 Intel Corporation
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

#ifndef AAA_RK3APLUS_H_
#define AAA_RK3APLUS_H_

#include <camera/camera_metadata.h>
#include <vector>
#include <map>

#include "CameraWindow.h"
#include "UtilityMacros.h"
#include "Rk3aCore.h"

NAMESPACE_DECLARATION {
/**
 * \class Rk3aPlus
 *
 * Rk3aPlus is an interface to Rockchip 3A Library.
 * The purpose of this class is to
 * - convert Google specific metadata to rk_aiq input parameters
 * - convert rk_aiq output parameters to Google specific metadata
 * - call 3a functions through Rk3aCore
 *
 */
class Rk3aPlus : public Rk3aCore {
public:
    explicit Rk3aPlus(int camId);
    status_t initAIQ(int maxGridW = MAX_STATISTICS_WIDTH,
                     int maxGridH = MAX_STATISTICS_HEIGHT,
                     ia_binary_data nvmData = {nullptr,0},
                     const char* sensorName = nullptr);

    rk_aiq_frame_use getFrameUseFromIntent(const CameraMetadata * settings);

    status_t fillAeInputParams(const CameraMetadata *settings,
                               AeInputParams &aeInputParams);

    status_t fillAwbInputParams(const CameraMetadata *settings,
                               AwbInputParams &awbInputParams);

    status_t fillAfInputParams(const CameraMetadata *settings,
                               AfInputParams &afInputParams);

    status_t fillPAInputParams(const CameraMetadata &settings,
                               PAInputParams &paInputParams) const;

    status_t fillSAInputParams(const CameraMetadata &settings,
                               SAInputParams &saInputParams) const;

    /*
     * static common operation
     */
    static status_t reFormatLensShadingMap(const LSCGrid &inputLscGrid,
                                           float *dstLscGridRGGB);
    static status_t storeLensShadingMap(const LSCGrid &inputLscGrid,
                                        LSCGrid &resizeLscGrid,
                                        float *dstLscGridRGGB);

    static status_t deepCopyAiqResults(AiqResults &dst,
                                       const AiqResults &src,
                                       bool onlyCopyUpdatedSAResults = false);
    static status_t deepCopyAEResults(rk_aiq_ae_results *dst, const rk_aiq_ae_results *src);
    static status_t deepCopyGBCEResults(rk_aiq_gbce_results *dst,
                                 const rk_aiq_gbce_results *src);
    static status_t deepCopyPAResults(rk_aiq_pa_results *dst, const rk_aiq_pa_results *src);
    static status_t deepCopySAResults(rk_aiq_sa_results *dst, const rk_aiq_sa_results *src);

    float getMinFocusDistance() const { return mMinFocusDistance; }

    char mapUiImageEnhancement2Aiq(int uiValue);

private:
    // prevent copy constructor and assignment operator
    Rk3aPlus(const Rk3aPlus& other);
    Rk3aPlus& operator=(const Rk3aPlus& other);

    void parseMeteringRegion(const CameraMetadata *settings,
                             int tagId, CameraWindow &meteringWindow);

    void parseAfTrigger(const CameraMetadata &settings,
                            rk_aiq_af_input_params &afInputParams,
                            uint8_t &trigger) const;
    status_t parseAFMode(const CameraMetadata *settings,
                            rk_aiq_af_input_params &afInputParams,
                            uint8_t &afMode) const;
    void setAfMode(rk_aiq_af_input_params &afInputParams,
                   uint8_t afMode) const;
    bool afModeIsAvailable(uint8_t afMode ) const;

    status_t parseFocusDistance(const CameraMetadata &settings,
                                rk_aiq_af_input_params &afCfg) const;

// private members
private:
    int mCameraId;

    /**
     * Cached values from static metadata
     */
    std::vector<uint8_t> mAvailableAFModes;
    float mMinFocusDistance;
    int32_t mMinAeCompensation;
    int32_t mMaxAeCompensation;

    int mMinSensitivity;
    int mMaxSensitivity;
    int64_t mMinExposureTime;
    int64_t mMaxExposureTime;
    int64_t mMaxFrameDuration;
}; //  class Rk3aPlus
} NAMESPACE_DECLARATION_END

#endif  //  AAA_RK3APLUS_H_
