package com.lmy.hwvcnative.widget

import android.content.Context
import android.content.res.Resources
import android.graphics.Color
import android.util.AttributeSet
import android.util.TypedValue
import android.view.ViewGroup
import android.widget.TextView
import androidx.annotation.AttrRes
import com.lmy.hwvcnative.entity.AlMediaTrack
import com.lmy.hwvcnative.entity.AlMediaType
import com.lmy.hwvcnative.entity.AlRational
import java.util.*

class AlTrackView : ViewGroup {
    private lateinit var mTimeView: AlTimelineView
    private val scale = AlRational(1, 1)
    private val tMap = TreeMap<Int, AlMediaTrack>()
    private val vMap = TreeMap<Int, TextView>()
    private var originWidth = 0
    private var mVideoColor = Color.LTGRAY
    private var mAudioColor = Color.DKGRAY

        constructor(context: Context) : super(context) {
        onResolveAttribute(context, null, 0, 0)
        onInitialize(context)
    }

    constructor(context: Context, attrs: AttributeSet?)
            : super(context, attrs) {
        onResolveAttribute(context, attrs, 0, 0)
        onInitialize(context)
    }

    constructor(context: Context, attrs: AttributeSet?, @AttrRes defStyleAttr: Int)
            : super(context, attrs, defStyleAttr) {
        onResolveAttribute(context, attrs, defStyleAttr, 0)
        onInitialize(context)
    }

    private fun onResolveAttribute(
        context: Context,
        attrs: AttributeSet?,
        defStyleAttr: Int,
        defStyleRes: Int
    ) {

    }

    private fun onInitialize(context: Context) {
        clipToPadding = false
        mTimeView = AlTimelineView(context)
        mTimeView.setPadding(
            0, applyDimension(TypedValue.COMPLEX_UNIT_DIP, 16f).toInt(),
            0, applyDimension(TypedValue.COMPLEX_UNIT_DIP, 16f).toInt()
        )
        addView(mTimeView, makeLayoutParams())
    }

    override fun setPadding(left: Int, top: Int, right: Int, bottom: Int) {
        super.setPadding(left, top, right, bottom)
        mTimeView.setPadding(left, mTimeView.paddingTop, right, mTimeView.paddingBottom)
    }

    override fun onMeasure(widthMeasureSpec: Int, heightMeasureSpec: Int) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec)
        val width = MeasureSpec.getSize(widthMeasureSpec)
        val height = MeasureSpec.getSize(heightMeasureSpec)
        measureChildren(widthMeasureSpec, heightMeasureSpec)
        if (originWidth <= 0) {
            originWidth = width
        }
        setMeasuredDimension(
            originWidth * scale.num / scale.den + paddingLeft + paddingRight,
            height
        )
    }

    fun setDuration(us: Long) {
        mTimeView.setDuration(us)
    }

    fun addTrack(track: AlMediaTrack) {
        if (tMap.containsKey(track.id)) {
            return
        }
        tMap[track.id] = track
        vMap[track.id] = TextView(context)
        vMap[track.id]?.textSize = 14f
        vMap[track.id]?.text = when (track.type) {
            AlMediaType.TYPE_VIDEO -> "Track ${track.id}"
            AlMediaType.TYPE_AUDIO -> "Track ${track.id}"
            else -> "Unknown Track"
        }
        vMap[track.id]?.setBackgroundColor(
            when (track.type) {
                AlMediaType.TYPE_VIDEO -> mVideoColor
                AlMediaType.TYPE_AUDIO -> mAudioColor
                else -> Color.RED
            }
        )
        val padding = applyDimension(TypedValue.COMPLEX_UNIT_DIP, 8f).toInt()
        vMap[track.id]?.setPadding(padding, padding, padding, padding)
        addView(vMap[track.id], makeLayoutParams())
        requestLayout()
    }

    fun updateTrack(track: AlMediaTrack) {
        if (!tMap.containsKey(track.id)) {
            return
        }
        tMap[track.id] = track
        requestLayout()
    }

    fun setScale(scale: AlRational) {
        this.scale.num = scale.num
        this.scale.den = scale.den
        requestLayout()
    }

    private fun makeLayoutParams(): LayoutParams {
        return LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT)
    }

    private fun applyDimension(unit: Int, value: Float): Float {
        val r = if (context == null) {
            Resources.getSystem()
        } else {
            context.resources
        }
        return TypedValue.applyDimension(unit, value, r.displayMetrics)
    }

    override fun onLayout(changed: Boolean, l: Int, t: Int, r: Int, b: Int) {
        var height = 0

        var w = measuredWidth
        var h = mTimeView.measuredHeight
        mTimeView.measure(MeasureSpec.makeMeasureSpec(w, MeasureSpec.EXACTLY), h)
        mTimeView.layout(l, height, l + w, height + h)
        height += h

        vMap.forEach {
            val track = tMap[it.key]
            val view = it.value

            w = measuredWidth - paddingLeft - paddingRight
            h = view.measuredHeight
            var offset = 0
            if (null != track && mTimeView.getDuration() > 0 && track.duration > 0) {
                offset = (track.seqIn * w / mTimeView.getDuration()).toInt()
                w = (track.duration * w / mTimeView.getDuration()).toInt()
            }
            view.layout(paddingLeft + l + offset, height, paddingLeft + l + w + offset, height + h)
            view.measure(MeasureSpec.makeMeasureSpec(w, MeasureSpec.EXACTLY), h)

            height += h
        }
    }

    companion object {
        const val TAG = "AlTrackView"
    }
}