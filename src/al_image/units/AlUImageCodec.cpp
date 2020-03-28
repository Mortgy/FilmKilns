/*
* Copyright (c) 2018-present, aliminabc@gmail.com.
*
* This source code is licensed under the MIT license found in the
* LICENSE file in the root directory of this source tree.
*/

#include "AlUImageCodec.h"
#include "AlBitmapFactory.h"
#include "AlTexManager.h"
#include "AlMath.h"
#include "AlRotateFilter.h"
#include "ObjectBox.h"

#define TAG "AlUImageCodec"

AlUImageCodec::AlUImageCodec(const string &alias) : Unit(alias) {
    registerEvent(EVENT_IMAGE_CODEC_DECODE,
                  reinterpret_cast<EventFunc>(&AlUImageCodec::onDecode));
    registerEvent(EVENT_IMAGE_CODEC_ENCODE,
                  reinterpret_cast<EventFunc>(&AlUImageCodec::onEncode));
}

AlUImageCodec::~AlUImageCodec() {

}

bool AlUImageCodec::onCreate(AlMessage *msg) {
    return true;
}

bool AlUImageCodec::onDestroy(AlMessage *msg) {
    return true;
}

bool AlUImageCodec::onDecode(AlMessage *msg) {
    AlBitmap *bmp = AlBitmapFactory::decodeFile(msg->desc);
    if (nullptr == bmp) {
        AlLogE(TAG, "decode %s failed", msg->desc.c_str());
        return true;
    }
    auto rotation = bmp->getRotation();
    AlTexDescription desc;
    desc.size.width = bmp->getWidth();
    desc.size.height = bmp->getHeight();
    desc.wrapMode = AlTexDescription::WrapMode::BORDER;
    desc.fmt = GL_RGBA;
    AlBuffer *buf = AlBuffer::wrap(bmp->getPixels(), bmp->getByteSize());
    auto *tex = AlTexManager::instance()->alloc(desc, buf);
    delete buf;
    delete bmp;
    _correctAngle(&tex, rotation);

    AlMessage *m = AlMessage::obtain(EVENT_IMAGE_CODEC_DECODE_NOTIFY, ObjectBox::wrap(tex));
    m->arg1 = msg->arg1; //Feedback req code.
    m->desc = msg->desc; //Feedback image file path.
    postEvent(m);
    return true;
}

bool AlUImageCodec::onEncode(AlMessage *msg) {

    AlMessage *m = AlMessage::obtain(EVENT_IMAGE_CODEC_ENCODE_NOTIFY);
    m->arg1 = msg->arg1; //Feedback req code.
    postEvent(m);
    return true;
}

void AlUImageCodec::_correctAngle(HwAbsTexture **tex,
                                  AlRational radian) {
    HwAbsTexture *destTex = nullptr;
    auto rFloat = fmod(std::abs(radian.toFloat()), 2.0);
    if (0 != rFloat) {
        AlTexDescription desc;
        desc.fmt = (*tex)->fmt();
        if (0.5 == rFloat || 1.5 == rFloat) {///宽高对换
            desc.size.width = (*tex)->getHeight();
            desc.size.height = (*tex)->getWidth();
        } else {
            desc.size.width = (*tex)->getWidth();
            desc.size.height = (*tex)->getHeight();
        }
        destTex = AlTexManager::instance()->alloc(desc);
        AlRotateFilter filter;
        filter.prepare();
        filter.setRotation(radian);
        glViewport(0, 0, destTex->getWidth(), destTex->getHeight());
        filter.draw(*tex, destTex);
        AlTexManager::instance()->recycle(tex);
        *tex = destTex;
    }
}