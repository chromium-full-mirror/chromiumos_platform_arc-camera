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

#define LOG_TAG "ParameterWorker"

#include "ParameterWorker.h"

namespace android {
namespace camera2 {

const unsigned int PARA_WORK_BUFFERS = 1;

ParameterWorker::ParameterWorker(std::shared_ptr<V4L2VideoNode> node,
                                 const StreamConfig& activeStreams,
                                 int cameraId) :
        FrameWorker(node, cameraId, PARA_WORK_BUFFERS, "ParameterWorker")
{
    HAL_TRACE_CALL(CAMERA_DEBUG_LOG_LEVEL1);
}

ParameterWorker::~ParameterWorker()
{
    HAL_TRACE_CALL(CAMERA_DEBUG_LOG_LEVEL1);
}

status_t ParameterWorker::configure(std::shared_ptr<GraphConfig> &config)
{
    HAL_TRACE_CALL(CAMERA_DEBUG_LOG_LEVEL1);
    status_t ret = OK;

    FrameInfo frame;
    int page_size = getpagesize();
    frame.width = page_size;
    frame.height = 1;
    frame.stride = frame.width;
    //frame.format = V4L2_META_FMT_IPU3_PARAMS;
    ret = setWorkerDeviceFormat(frame);
    if (ret != OK)
        return ret;

    ret = setWorkerDeviceBuffers(getDefaultMemoryType(IMGU_NODE_PARAM));
    if (ret != OK)
        return ret;

    ret = allocateWorkerBuffers();
    if (ret != OK)
        return ret;

    mIndex = 0;

    return OK;
}

status_t ParameterWorker::prepareRun(std::shared_ptr<DeviceMessage> msg)
{
    HAL_TRACE_CALL(CAMERA_DEBUG_LOG_LEVEL1);
    std::lock_guard<std::mutex> l(mParamsMutex);

    mMsg = msg;

    // Don't queue ISP parameter buffer if test pattern mode is used.
    if (mMsg->pMsg.processingSettings->captureSettings->testPatternMode
            != ANDROID_SENSOR_TEST_PATTERN_MODE_OFF) {
        return OK;
    }

    status_t status = mNode->putFrame(mBuffers[mIndex]);
    if (status != OK) {
        LOGE("putFrame failed");
        return UNKNOWN_ERROR;
    }

    mIndex = (mIndex + 1) % mPipelineDepth;

    return OK;
}

status_t ParameterWorker::run()
{
    HAL_TRACE_CALL(CAMERA_DEBUG_LOG_LEVEL1);

    // Don't dequeue ISP parameter buffer if test pattern mode is used.
    if (mMsg->pMsg.processingSettings->captureSettings->testPatternMode
            != ANDROID_SENSOR_TEST_PATTERN_MODE_OFF) {
        return OK;
    }

    V4L2BufferInfo outBuf;

    status_t status = mNode->grabFrame(&outBuf);
    if (status < 0) {
        LOGE("grabFrame failed");
        return UNKNOWN_ERROR;
    }

    return OK;
}

status_t ParameterWorker::postRun()
{
    HAL_TRACE_CALL(CAMERA_DEBUG_LOG_LEVEL1);
    mMsg = nullptr;
    return OK;
}

} /* namespace camera2 */
} /* namespace android */
