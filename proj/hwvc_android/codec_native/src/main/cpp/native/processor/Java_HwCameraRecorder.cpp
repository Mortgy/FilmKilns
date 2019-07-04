/*
* Copyright (c) 2018-present, lmyooyo@gmail.com.
*
* This source code is licensed under the GPL license found in the
* LICENSE file in the root directory of this source tree.
*/
#include "../include/HwJavaNativeHelper.h"
#include "HwCameraRecorder.h"
#include "../include/HwAndroidWindow.h"

#ifdef __cplusplus
extern "C" {
#endif

static JMethodDescription cOnHandleMessage = {"Java_com_lmy_hwvcnative_processor_HwCameraRecorder",
                                              "onHandleMessage", "(I)V"};
static int HwCameraRecorderWhat = 0;

static HwCameraRecorder *getHandler(jlong handler) {
    return reinterpret_cast<HwCameraRecorder *>(handler);
}

JNIEXPORT jlong JNICALL Java_com_lmy_hwvcnative_processor_HwCameraRecorder_create
        (JNIEnv *env, jobject thiz) {
    HwCameraRecorder *p = new HwCameraRecorder();
    p->post([] {
        HwJavaNativeHelper::getInstance()->attachThread();
    });
    jlong handler = reinterpret_cast<jlong>(p);
    HwJavaNativeHelper::getInstance()->registerAnObject(env, handler, thiz);
    return handler;
}

JNIEXPORT void JNICALL Java_com_lmy_hwvcnative_processor_HwCameraRecorder_postEvent
        (JNIEnv *env, jobject thiz, jlong handler, jint what) {
    if (handler) {
        HwCameraRecorderWhat = what;
        getHandler(handler)->post([handler]() {
            jobject jObject = nullptr;
            JNIEnv *pEnv = nullptr;
            jmethodID methodID = nullptr;
            if (HwJavaNativeHelper::getInstance()->findEnv(&pEnv) &&
                HwJavaNativeHelper::getInstance()->findJObject(handler, &jObject) &&
                HwJavaNativeHelper::getInstance()->findMethod(handler,
                                                              cOnHandleMessage,
                                                              &methodID)) {
                pEnv->CallVoidMethod(jObject, methodID, HwCameraRecorderWhat);
            }
        });
    }
}

JNIEXPORT void JNICALL Java_com_lmy_hwvcnative_processor_HwCameraRecorder_prepare
        (JNIEnv *env, jobject thiz, jlong handler, jobject surface) {
    if (handler) {
        getHandler(handler)->prepare(new HwAndroidWindow(env, surface));
    }
}

JNIEXPORT void JNICALL Java_com_lmy_hwvcnative_processor_HwCameraRecorder_updateWindow
        (JNIEnv *env, jobject thiz, jlong handler, jobject surface) {
    if (handler) {
        getHandler(handler)->updateWindow(new HwAndroidWindow(env, surface));
    }
}

JNIEXPORT void JNICALL Java_com_lmy_hwvcnative_processor_HwCameraRecorder_start
        (JNIEnv *env, jobject thiz, jlong handler) {
    if (handler) {
        getHandler(handler)->start();
    }
}

JNIEXPORT void JNICALL Java_com_lmy_hwvcnative_processor_HwCameraRecorder_pause
        (JNIEnv *env, jobject thiz, jlong handler) {
    if (handler) {
        getHandler(handler)->pause();
    }
}

JNIEXPORT void JNICALL Java_com_lmy_hwvcnative_processor_HwCameraRecorder_release
        (JNIEnv *env, jobject thiz, jlong handler) {
    if (handler) {
        HwCameraRecorder *p = getHandler(handler);
        p->post([] {
            HwJavaNativeHelper::getInstance()->detachThread();
        });
        delete p;
    }
    HwJavaNativeHelper::getInstance()->unregisterAnObject(env, handler);
}

JNIEXPORT void JNICALL Java_com_lmy_hwvcnative_processor_HwCameraRecorder_invalidate
        (JNIEnv *env, jobject thiz, jlong handler, jint textureId, jint w, jint h) {
    if (handler) {
        getHandler(handler)->invalidate(textureId, w, h);
    }
}

#ifdef __cplusplus
}
#endif