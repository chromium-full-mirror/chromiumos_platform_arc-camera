/*
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
#ifndef AAA_RKAFSTATEMACHINE_H_
#define AAA_RKAFSTATEMACHINE_H_

#include <utils/Errors.h>
#include "Rk3aPlus.h"
#include <camera/camera_metadata.h>
#include <rk_aiq.h>
#include "LogHelper.h"
#include "PerformanceTraces.h"

NAMESPACE_DECLARATION {
typedef nsecs_t usecs_t;

/**
 * \class RkAfModeBase
 *
 * Base class for all the AutoFocus modes as defined by the Android
 * camera device V3.x API.
 * Each mode will follow certain state transitions. See documentation for
 * android.control.afState
 *
 */
class RkAfModeBase {
public:
    RkAfModeBase();
    virtual ~RkAfModeBase() {};

    virtual status_t processTriggers(const uint8_t &afTrigger,
                                     const uint8_t &afMode,
                                     int preCaptureId,
                                     rk_aiq_af_input_params& afInputParams);
    virtual status_t processResult(rk_aiq_af_results& afResults,
                                   CameraMetadata& result) = 0;
    void resetState(void);
    void resetTrigger(usecs_t triggerTime);
    int getState() { return mCurrentAfState; }
    void updateResult(CameraMetadata& results);
protected:
    void checkIfFocusTimeout();
protected:
    AfControls mLastAfControls;
    uint8_t mCurrentAfState;
    uint8_t  mLensState;
    usecs_t  mLastActiveTriggerTime;   /**< in useconds */
    uint32_t mFramesSinceTrigger;
};

/**
 * \class RkAFModeAuto
 * Derived class from RkAFModeBase for Auto mode
 *
 */
class RkAFModeAuto: public RkAfModeBase {
public:
    RkAFModeAuto();
    virtual status_t processTriggers(const uint8_t &afTrigger,
                                     const uint8_t &afMode,
                                     int preCaptureId,
                                     rk_aiq_af_input_params& afInputParams);
    virtual status_t processResult(rk_aiq_af_results& afResults,
                                  CameraMetadata& result);
};

/**
 * \class RkAFModeContinuousPicture
 * Derived class from RkAFModeBase for Continuous AF mode
 *
 */
class RkAFModeContinuousPicture: public RkAfModeBase {
public:
    RkAFModeContinuousPicture();
    virtual status_t processTriggers(const uint8_t &afTrigger,
                                     const uint8_t &afMode,
                                     int preCaptureId,
                                     rk_aiq_af_input_params& afInputParams);
    virtual status_t processResult(rk_aiq_af_results& afResults,
                                  CameraMetadata& result);
};

/**
 * \class RkAFModeOff
 * Derived class from RkAFModeBase for OFF mode
 *
 */
class RkAFModeOff: public RkAfModeBase {
public:
    RkAFModeOff();
    virtual status_t processTriggers(const uint8_t &afTrigger,
                                     const uint8_t &afMode,
                                     int preCaptureId,
                                     rk_aiq_af_input_params& afInputParams);
    virtual status_t processResult(rk_aiq_af_results& afResults,
                                  CameraMetadata& result);
};

/**
 * \class RkAFStateMachine
 *
 * This class adapts the Android V3 AF triggers and state transitions to
 * the ones implemented by the Rockchip AIQ algorithm
 * This class is platform independent. Platform specific behaviors should be
 * implemented in derived classes from this one or from the RkAFModeBase
 *
 */
class RkAFStateMachine {
public:
    RkAFStateMachine(int CameraId, const Rk3aPlus &aaa);
    virtual ~RkAFStateMachine();

    virtual status_t processTriggers(const uint8_t &afTrigger,
                                     const uint8_t &afMode,
                                     int preCaptureId,
                                     rk_aiq_af_input_params &afInputParams);

    virtual status_t processResult(rk_aiq_af_results &afResults,
                                   rk_aiq_af_input_params &afInputParams,
                                   CameraMetadata& result);

    virtual status_t updateDefaults(const rk_aiq_af_results &afResults,
                                    const rk_aiq_af_input_params &afInputParams,
                                    CameraMetadata &result,
                                    bool fixedFocus = false) const;

private:
    // prevent copy constructor and assignment operator
    RkAFStateMachine(const RkAFStateMachine& other);
    RkAFStateMachine& operator=(const RkAFStateMachine& other);

    void focusDistanceResult(const rk_aiq_af_results &afResults,
                             const rk_aiq_af_input_params &afInputParams,
                             CameraMetadata &result) const;

private: /* members*/
    int mCameraId;
    AfControls      mLastAfControls;
    RkAfModeBase *mCurrentAfMode;
    uint8_t         mCurrentAfState;

    std::vector<uint8_t> mAvailableModes;

    RkAFModeOff mOffMode;
    RkAFModeAuto mAutoMode;

    RkAFModeContinuousPicture mContinuousPictureMode;
    const Rk3aPlus &m3A;
};
} NAMESPACE_DECLARATION_END

#endif // AAA_RKAFSTATEMACHINE_H_
