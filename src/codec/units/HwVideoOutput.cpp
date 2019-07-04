/*
* Copyright (c) 2018-present, lmyooyo@gmail.com.
*
* This source code is licensed under the GPL license found in the
* LICENSE file in the root directory of this source tree.
*/

#include "../include/HwVideoOutput.h"
#include "libyuv.h"

HwVideoOutput::HwVideoOutput() : Unit() {
    name = __FUNCTION__;
    registerEvent(EVENT_COMMON_PREPARE, reinterpret_cast<EventFunc>(&HwVideoOutput::eventPrepare));
    registerEvent(EVENT_COMMON_PIXELS_READY,
                  reinterpret_cast<EventFunc>(&HwVideoOutput::eventResponsePixels));
    registerEvent(EVENT_COMMON_PIXELS, reinterpret_cast<EventFunc>(&HwVideoOutput::eventWrite));
    registerEvent(EVENT_VIDEO_OUT_START, reinterpret_cast<EventFunc>(&HwVideoOutput::eventStart));
    registerEvent(EVENT_VIDEO_OUT_PAUSE, reinterpret_cast<EventFunc>(&HwVideoOutput::eventPause));
}

HwVideoOutput::~HwVideoOutput() {

}

bool HwVideoOutput::eventPrepare(Message *msg) {
    recording = false;
    encoder = new HwFFmpegEncoder();
    if (!encoder->prepare("/sdcard/hw_encoder.mp4", 720, 1280)) {
        Logcat::e("HWVC", "HwVideoOutput::eventPrepare encoder open failed.");
    }
    videoFrame = new HwVideoFrame(nullptr, HwFrameFormat::HW_IMAGE_YV12, 720, 1280);
    return true;
}

bool HwVideoOutput::eventRelease(Message *msg) {
    if (encoder) {
        encoder->stop();
        encoder->release();
        delete encoder;
        encoder = nullptr;
    }
    if (videoFrame) {
        delete videoFrame;
        videoFrame = nullptr;
    }
    return true;
}

bool HwVideoOutput::eventResponsePixels(Message *msg) {
    if (recording) {
        postEvent(new Message(EVENT_COMMON_PIXELS_READ, nullptr));
    }
    return true;
}

bool HwVideoOutput::eventWrite(Message *msg) {
    if (!recording) {
        return true;
    }
    HwBuffer *buf = static_cast<HwBuffer *>(msg->obj);
    msg->obj = nullptr;
    write(buf);
    return true;
}

bool HwVideoOutput::eventStart(Message *msg) {
    recording = true;
    return true;
}

bool HwVideoOutput::eventPause(Message *msg) {
    recording = false;
    return true;
}

void HwVideoOutput::write(HwBuffer *buf) {
    if (!buf) {
        Logcat::e("HWVC", "HwVideoOutput::write failed. Buffer is null.");
        return;
    }
    int pixelCount = videoFrame->getWidth() * videoFrame->getHeight();
    libyuv::ConvertToI420(buf->getData(), pixelCount,
                          videoFrame->getBuffer()->getData(), videoFrame->getWidth(),
                          videoFrame->getBuffer()->getData() + pixelCount,
                          videoFrame->getWidth() / 2,
                          videoFrame->getBuffer()->getData() + pixelCount * 5 / 4,
                          videoFrame->getWidth() / 2,
                          0, 0,
                          videoFrame->getWidth(), videoFrame->getHeight(),
                          videoFrame->getWidth(), videoFrame->getHeight(),
                          libyuv::kRotate0, libyuv::FOURCC_ABGR);
    videoFrame->setPts(count * 33000);
    ++count;
    if (encoder) {
        encoder->write(videoFrame);
    } else {
        Logcat::e("HWVC", "HwVideoOutput::write failed. Encoder has release.");
    }
}