/*
 * Copyright (c) 2017, Fuzhou Rockchip Electronics Co., Ltd
 *
 * Based on Intel IPU3 HAL by Intel Corporation.
 *
 * Copyright (C) 2016-2017 Intel Corporation.
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

#define LOG_TAG "StatisticsWorker"

#include "StatisticsWorker.h"
#include "LogHelper.h"
#include "NodeTypes.h"

namespace android {
namespace camera2 {

const unsigned int STAT_WORK_BUFFERS = 1;

StatisticsWorker::StatisticsWorker(std::shared_ptr<V4L2VideoNode> node, int cameraId):
        FrameWorker(node, cameraId, STAT_WORK_BUFFERS, "StatisticsWorker")
{
    HAL_TRACE_CALL(CAMERA_DEBUG_LOG_LEVEL1);
    mPollMe = true;
}

StatisticsWorker::~StatisticsWorker()
{
    HAL_TRACE_CALL(CAMERA_DEBUG_LOG_LEVEL1);
}

status_t StatisticsWorker::configure(std::shared_ptr<GraphConfig> &/*config*/)
{
    HAL_TRACE_CALL(CAMERA_DEBUG_LOG_LEVEL1);
    FrameInfo frame;
    int page_size = getpagesize();
    frame.width = page_size;
    frame.height = 1;
    frame.stride = frame.width;
    frame.format = V4L2_PIX_FMT_YUYV;
    status_t ret = setWorkerDeviceFormat(frame);
    if (ret != OK)
        return ret;

    ret = setWorkerDeviceBuffers(getDefaultMemoryType(IMGU_NODE_STAT));
    if (ret != OK)
        return ret;

    ret = allocateWorkerBuffers();
    if (ret != OK)
        return ret;

    if (mCameraBuffers[0]->size() < mFormat.sizeimage()) {
        LOGE("Stats buffer is not big enough");
        return UNKNOWN_ERROR;
    }

    mIndex = 0;

    return ret;
}

status_t StatisticsWorker::prepareRun(std::shared_ptr<DeviceMessage> msg)
{
    HAL_TRACE_CALL(CAMERA_DEBUG_LOG_LEVEL2);
    mMsg = msg;

    status_t status = OK;

    status |= mNode->putFrame(mBuffers[mIndex]);

    if (status != OK) {
        LOGE("Failed to queue buffer to statistics device");
        return status;
    }

    mIndex = (mIndex + 1) % mPipelineDepth;

    return OK;
}

status_t StatisticsWorker::run()
{
    HAL_TRACE_CALL(CAMERA_DEBUG_LOG_LEVEL2);
    if (!mMsg) {
        LOGE("Message is not set - Fix the bug");
        return UNKNOWN_ERROR;
    }

    #define DUMP_INTERVAL 10
    status_t status = OK;

    V4L2BufferInfo buf;

    status = mNode->grabFrame(&buf);

    if (status < 0) {
        LOGE("Failed to dequeue buffer from statistics device");
        return status;
    }

    ICaptureEventListener::CaptureMessage outMsg;
    outMsg.id = ICaptureEventListener::CAPTURE_MESSAGE_ID_EVENT;
    outMsg.data.event.type = ICaptureEventListener::CAPTURE_EVENT_2A_STATISTICS;
    std::shared_ptr<RKISP1CapturedStatistics> stats = std::make_shared<RKISP1CapturedStatistics>();
    if (stats.get() == nullptr) {
        LOGE("Not enough memory to allocate stats");
        return NO_MEMORY;
    }

    notifyListeners(&outMsg);

    return OK;
}

status_t StatisticsWorker::postRun()
{
    mMsg = nullptr;
    return OK;
}

} /* namespace camera2 */
} /* namespace android */
