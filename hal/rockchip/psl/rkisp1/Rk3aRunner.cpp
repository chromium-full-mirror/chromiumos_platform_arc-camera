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

#define LOG_TAG "Rk3aRunner"

#include "Rk3aRunner.h"

#include <math.h>
#include "LogHelper.h"
#include "CameraMetadataHelper.h"
#include "Rk3aPlus.h"
#include "RkAEStateMachine.h"
#include "RkAFStateMachine.h"
#include "RkAWBStateMachine.h"
#include "LensHw.h"
#include "RKISP1Common.h"
#include "SettingsProcessor.h"


namespace android {
namespace camera2 {


Rk3aRunner::Rk3aRunner(int camerId, Rk3aPlus *aaaWrapper, SettingsProcessor *settingsProcessor, LensHw *lensController) :
        mCameraId(camerId),
        m3aWrapper(aaaWrapper),
        mAeState(nullptr)
{
    HAL_TRACE_CALL(CAMERA_DEBUG_LOG_LEVEL1);
}

status_t Rk3aRunner::init()
{
    HAL_TRACE_CALL(CAMERA_DEBUG_LOG_LEVEL1);

    /*
     * Initialize the AE State Machine
     */
    mAeState = new RkAEStateMachine(mCameraId);

    /*
     * Initialize the AF State Machine
     */
    mAfState = new RkAFStateMachine(mCameraId, *m3aWrapper);

    /*
     * Initialize the AWB State Machine
     */
    mAwbState = new RkAWBStateMachine(mCameraId);

    return OK;
}

Rk3aRunner::~Rk3aRunner()
{
    HAL_TRACE_CALL(CAMERA_DEBUG_LOG_LEVEL1);

    m3aWrapper = nullptr;

    delete mAeState;
    mAeState = nullptr;

    delete mAwbState;
    mAwbState = nullptr;

    delete mAfState;
    mAfState = nullptr;
}

/**
 * run2A
 *
 * Runs AE and AWB for a request and submits the request for capture
 * together with the capture settings obtained after running these 2A algorithms
 *
 *\param [IN] reqState: Pointer to the request control structure to process
 *\return NO_ERROR
 */
status_t Rk3aRunner::run2A(RequestCtrlState &reqState, bool forceUpdated)
{
    HAL_TRACE_CALL(CAMERA_DEBUG_LOG_LEVEL2);

    status_t status = NO_ERROR;

    int reqId = reqState.request->getId();
    /*
     * Auto Exposure Compensation
     * certain settings changes require running the AE algorithm during AE
     * locked state. These at least:
     *  1) ev_shift changes
     *  2) FPS rate changes (TODO)
     */
    bool forceAeRun = mLatestInputParams.aeInputParams.ev_shift !=
                      reqState.aiqInputParams.aeInputParams.ev_shift;

    // process state when the request is actually processed
    mAeState->processState(reqState.aaaControls.controlMode,
                           reqState.aaaControls.ae);

    // copy control mode for capture unit to use
    reqState.captureSettings->controlMode = reqState.aaaControls.controlMode;
    reqState.captureSettings->controlAeMode = reqState.aaaControls.ae.aeMode;

    if (forceAeRun || mAeState->getState() != ANDROID_CONTROL_AE_STATE_LOCKED) {
        status = m3aWrapper->runAe(nullptr,
                                   &reqState.aiqInputParams.aeInputParams,
                                   &reqState.captureSettings->aiqResults.aeResults);

        if (CC_LIKELY(status == OK)) {
            reqState.aeState = ALGORITHM_RUN;
        } else {
            LOGE("Run AE failed for request Id %d", reqId);
            return UNKNOWN_ERROR;
        }
    }

    status = mAwbState->processState(reqState.aaaControls.controlMode,
                                     reqState.aaaControls.awb);

    /*
     * Client may enable AWB lock right from the start, so force AWB
     * to run at least once.
     */
    bool forceAwbRun = (reqId == 0);
    bool awbLocked = (mAwbState->getState() == ANDROID_CONTROL_AWB_STATE_LOCKED);

    /*
     * Auto White Balance
     */
    if (forceAwbRun || !awbLocked) {
        status = m3aWrapper->runAwb(nullptr,
                                    &reqState.aiqInputParams.awbParams,
                                    &reqState.captureSettings->aiqResults.awbResults);
        if (CC_LIKELY(status == OK)) {
            reqState.awbState = ALGORITHM_RUN;
        } else {
            LOGE("Run AWB failed for request Id %d", reqId);
            return UNKNOWN_ERROR;
        }
    } else {
        reqState.captureSettings->aiqResults.awbResults = mLatestResults.awbResults;
    }

    status = m3aWrapper->runMisc(nullptr,
                                 &reqState.aiqInputParams.miscParams,
                                 &reqState.captureSettings->aiqResults.miscIspResults);
    if (!CC_LIKELY(status == OK)) {
        LOGE("Run misc failed for request Id %d", reqId);
        return UNKNOWN_ERROR;
    }

    /*
     * Result processing before we send them to HW
     */

    processAeResults(reqState);

    processAwbResults(reqState);

    return status;
}

/**
 * Runs the auto focus algorithm
 * Runs the state machine
 * Updates the metadata results
 * TODO: send the partial metadata results ahead of time
 *
 * \param[in,out] reqState The AF input parameters are input to this routine
 * the AF results also stored in this struct are output from this routine.
 *
 * The AF algorithm state is used to determine whether we need to run or not.
 */
void Rk3aRunner::runAf(RequestCtrlState &reqState, bool bypass)
{
    HAL_TRACE_CALL(CAMERA_DEBUG_LOG_LEVEL2);

    status_t status = OK;
    bool fixedFocus = (m3aWrapper->getMinFocusDistance() == 0.0f);
    rk_aiq_af_input_params *afInputParams;
    rk_aiq_af_results *afResults;
    afInputParams = &reqState.aiqInputParams.afParams;
    afResults = &reqState.captureSettings->aiqResults.afResults;

    status = processAfTriggers(reqState);
    if (status != OK) {
        LOGE("Af triggers processing failed");
        goto exit;
    }

    if ((reqState.afState != ALGORITHM_READY &&
         reqState.aaaControls.af.afMode != ANDROID_CONTROL_AF_MODE_OFF) ||
         mLensController == nullptr) {
        /*
         * Not ready to run AF for several reasons:
         * - No stats (algo not ready) and not in manual (this should not happen)
         * - No lens controller (because this sensor is fixed focus)
         * update the state and leave
         */
        LOG2("AF state not ready or fixed focus sensor");
        mAfState->updateDefaults(*afResults,
                                 *afInputParams,
                                 *reqState.ctrlUnitResult,
                                 fixedFocus);
        goto exit;
    }

    /*
     * Get the lens position and time now, just before running AF
     */
    if (mLensController != nullptr) {
        status = mLensController->getLatestPosition(
                &afInputParams->lens_position,
                &afInputParams->lens_movement_start_timestamp);

        if (CC_UNLIKELY(status != OK)) {
            LOGW("Failed to get latest Lens Position");
        }
    }

    if (bypass) {
        *afResults = mLatestResults.afResults;
        status = OK;
    } else {
        status = m3aWrapper->runAf(nullptr, afInputParams, afResults);
    }

    if (status == OK) {
        reqState.afState = ALGORITHM_RUN;
        status = processAfResults(reqState);
    } else {
        LOGW("AF Failed, update default");
        mAfState->updateDefaults(*afResults,
                                 *afInputParams,
                                 *reqState.ctrlUnitResult,
                                 fixedFocus);
    }

exit:
    CameraWindow &reportedAfregion = reqState.captureSettings->afRegion;
    if (reqState.captureSettings->afRegion.isValid()) {
        //# ANDROID_METADATA_Dynamic android.control.afRegions done
        reqState.ctrlUnitResult->update(ANDROID_CONTROL_AF_REGIONS,
                        reportedAfregion.meteringRectangle(), METERING_RECT_SIZE);
    }

    return;
}

/**
 * Generic results handler which runs after 3A has run. At this point of time
 * the state transitions for AE, AWB and possibly AF should be handled and
 * those results can be written to request metadata
 *
 * \param[in,out] reqState Request state structure
 */
status_t Rk3aRunner::processAeResults(RequestCtrlState &reqState)
{
    HAL_TRACE_CALL(CAMERA_DEBUG_LOG_LEVEL2);

    if (reqState.request == nullptr) {
        LOGE("Request is nullptr");
        return BAD_VALUE;
    }

    rk_aiq_ae_input_params &inParams = reqState.aiqInputParams.aeInputParams;
    uint8_t sceneFlickerMode = ANDROID_STATISTICS_SCENE_FLICKER_NONE;
    switch (inParams.flicker_reduction_mode) {
    case rk_aiq_ae_flicker_reduction_50hz:
        sceneFlickerMode = ANDROID_STATISTICS_SCENE_FLICKER_50HZ;
        break;
    case rk_aiq_ae_flicker_reduction_60hz:
        sceneFlickerMode = ANDROID_STATISTICS_SCENE_FLICKER_60HZ;
        break;
    default:
        sceneFlickerMode = ANDROID_STATISTICS_SCENE_FLICKER_NONE;
    }
    //# ANDROID_METADATA_Dynamic android.statistics.sceneFlicker done
    reqState.ctrlUnitResult->update(ANDROID_STATISTICS_SCENE_FLICKER,
                                    &sceneFlickerMode, 1);

    ///////////// AE precapture handling starts
    rk_aiq_ae_results &aeResult =
        reqState.captureSettings->aiqResults.aeResults;

    LOG2("%s exp_time=%d gain=%f", __FUNCTION__,
            aeResult.exposure.exposure_time_us,
            aeResult.exposure.analog_gain);

    mAeState->processResult(aeResult, *reqState.ctrlUnitResult,
                            reqState.request->getId());

    //# ANDROID_METADATA_Dynamic android.control.aeRegions done
    reqState.ctrlUnitResult->update(ANDROID_CONTROL_AE_REGIONS,
                        reqState.captureSettings->aeRegion.meteringRectangle(),
                        5);

    //# ANDROID_METADATA_Dynamic android.control.aeExposureCompensation done
    // TODO get step size (currently 1/3) from static metadata
    int32_t exposureCompensation =
            round((reqState.aiqInputParams.aeInputParams.ev_shift) * 3);

    reqState.ctrlUnitResult->update(ANDROID_CONTROL_AE_EXPOSURE_COMPENSATION,
                                    &exposureCompensation,
                                    1);
    return OK;

}

/**
 *
 * Analyze the results of the AF algorithm and runs the state machine to update
 * the capture results
 *
 * It also relays to the lens driver the Optical Image Stabilization control
 *
 * This method is called just after AF algorithm has run
 *
 * \param[in,out] reqState from which input parameters are:
 *        .aiqInputParams: AF input params for processing AF result
 *        .captureSettings: settings for OIS
 *        .CameraMetadata: control values from framework
 *        and output parameters:
 *        .CameraMetadata: dynamic values to framework
 */
status_t Rk3aRunner::processAfResults(RequestCtrlState &reqState)
{
    if (CC_UNLIKELY(reqState.captureSettings.get() == nullptr)) {
        LOGE("Null capture settings when processing AF results - BUG");
        return UNKNOWN_ERROR;
    }

    status_t status = OK;
    AiqResults &aiqResults = reqState.captureSettings->aiqResults;
    int32_t focusDistanceBoundLow = 0;
    int32_t focusDistanceBoundHigh = 0;
    float afDistanceControl = 0;
    float afDistanceDynamic = 0;
    rk_aiq_af_input_params *tempAfInputParams = &reqState.aiqInputParams.afParams;
    rk_aiq_af_results tempAfResults;
    camera_metadata_entry_t entry;
    bool resultParsed = false;

    CLEAR(tempAfResults);
    CLEAR(entry);

    /**
     * Process state machine transitions
     */
    status = mAfState->processResult(aiqResults.afResults,
                                     reqState.aiqInputParams.afParams,
                                     *reqState.ctrlUnitResult);

    /*
     * When setting the focus distance manually, the focus distance value is
     * converted from diopters (float) to millimeters (int), quantized to VCM
     * (Voice Coil Motor) units and eventually converted back to diopters.
     * This can cause a small offset between incoming and outgoing
     * (= control and dynamic) value of ANDROID_LENS_FOCUS_DISTANCE.
     * By calculating allowed bounds (that vary according to the current focus
     * distance), we are able to know when the difference between incoming and
     * outgoing focus distance is caused by this quantization. When the lens is
     * moving, the focus distance is outside the bounds. In that case we don't
     * tweak the outgoing (=dynamic) distance.
     */
    if (tempAfInputParams && tempAfInputParams->manual_focus_parameters != nullptr) {
        const CameraMetadata *settings = reqState.request->getSettings();
        if (CC_UNLIKELY(settings == nullptr)) {
            LOGE("Failed reading metadata settings - BUG");
            return UNKNOWN_ERROR;
        }

        resultParsed = MetadataHelper::getMetadataValue(*settings,
                                            ANDROID_LENS_FOCUS_DISTANCE,
                                            afDistanceControl,
                                            1);
        if (!resultParsed) {
            LOGE("Failed reading ANDROID_LENS_FOCUS_DISTANCE from metadata - BUG");
            return UNKNOWN_ERROR;
        }
        LOG2("ANDROID_LENS_FOCUS_DISTANCE control: %f", afDistanceControl);


        // Read and print out the dynamic focus distance for debugging purposes
        entry = reqState.ctrlUnitResult->find(ANDROID_LENS_FOCUS_DISTANCE);
        if (entry.count == 1) {
            afDistanceDynamic = (float)entry.data.f[0];
            LOG2("ANDROID_LENS_FOCUS_DISTANCE dynamic: %f", afDistanceDynamic);
        }

        tempAfInputParams = &reqState.aiqInputParams.afParams;
        tempAfInputParams->lens_position =
                            aiqResults.afResults.next_lens_position + 1;
        status = m3aWrapper->runAf(nullptr, tempAfInputParams, &tempAfResults);
        focusDistanceBoundLow = tempAfResults.current_focus_distance;

        tempAfInputParams->lens_position =
                            aiqResults.afResults.next_lens_position - 1;
        status = m3aWrapper->runAf(nullptr, tempAfInputParams, &tempAfResults);
        focusDistanceBoundHigh = tempAfResults.current_focus_distance;

        LOG2("current_focus_distance in mm: %d, bounds: [%d, %d]",
                                aiqResults.afResults.current_focus_distance,
                                focusDistanceBoundLow,
                                focusDistanceBoundHigh);

        if (aiqResults.afResults.current_focus_distance >= focusDistanceBoundLow &&
            aiqResults.afResults.current_focus_distance <= focusDistanceBoundHigh) {
            reqState.ctrlUnitResult->update(ANDROID_LENS_FOCUS_DISTANCE,
                                            &afDistanceControl,
                                            1);
        }
    }

    // TODO: remove this once the request flow is fixed
    mLatestResults.afResults = aiqResults.afResults;

    /*
     * Act on the OIS control
     */
    if (mLensController != nullptr)
        mLensController->enableOis(reqState.captureSettings->opticalStabilizationMode);

    return status;
}

status_t Rk3aRunner::processAfTriggers(RequestCtrlState &reqAiqCfg)
{

    /**
     * Update state machine if needed and modify the AF Input params
     * Keep in mind that even in fixedFocus case, we still are in AF AUTO!
     * we still have an AF object for the state machine, but it is not processed
     * only the results are updated. (bypass)
     */
    rk_aiq_af_input_params &afInputParams = reqAiqCfg.aiqInputParams.afParams;
    return mAfState->processTriggers(
        reqAiqCfg.aaaControls.af.afTrigger,
        reqAiqCfg.aaaControls.af.afMode,
        0,
        afInputParams);
}

/**
 * Generic results handler which runs after AWB has run. At this point of time
 * we can modify the results before we send them towards the HW (sensor/ISP)
 *
 * \param reqState[in,out]: Request state structure
 * \return OK
 * \return UNKNOWN_ERROR
 */
status_t Rk3aRunner::processAwbResults(RequestCtrlState &reqState)
{
    if (CC_UNLIKELY(reqState.captureSettings.get() == nullptr)) {
        LOGE("Null capture settings when processing AWB results- BUG");
        return UNKNOWN_ERROR;
    }
    status_t status = OK;

    status = mAwbState->processResult(reqState.captureSettings->aiqResults.awbResults,
                                      *reqState.ctrlUnitResult);

    return status;

}

} /* namespace camera2 */
} /* namespace android */
