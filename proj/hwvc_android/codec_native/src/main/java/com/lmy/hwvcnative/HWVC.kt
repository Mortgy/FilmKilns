package com.lmy.hwvcnative

import android.content.Context

class HWVC {
    companion object {
        @Synchronized
        fun init(context: Context) {
            System.loadLibrary("hwffmpeg")
            System.loadLibrary("yuv")
            System.loadLibrary("hwvcom")
            System.loadLibrary("albitmap")
            System.loadLibrary("hwvc_render")
            System.loadLibrary("hwvc_codec")
            System.loadLibrary("alimage")
            System.loadLibrary("hwvc_native")
        }
    }
}