package com.lmy.samplenative

import android.content.Intent
import android.net.Uri
import android.view.View
import android.widget.Toast
import androidx.recyclerview.widget.DividerItemDecoration
import androidx.recyclerview.widget.RecyclerView
import com.lmy.samplenative.adapter.OnRecyclerItemClickListener
import com.lmy.samplenative.adapter.SimpleTextAdapter
import com.lmy.samplenative.ui.*
import com.microsoft.officeuifabric.listitem.ListItemDivider
import kotlinx.android.synthetic.main.activity_main.*
import java.io.File

class MainActivity : BaseActivity(), OnRecyclerItemClickListener.OnItemClickListener {

    private val ITEMS = arrayListOf(
        Item("New Image Editor") { startActivity(Intent(this, AlImageActivity::class.java)) },
        Item("Audio Player") { startActivity(Intent(this, AudioActivity::class.java)) },
        Item("Video V2") { actionVideoV2() },
        Item("Video Player") { startActivity(Intent(this, VideoActivity::class.java)) },
        Item("Audio Recorder & Mux") { startActivity(Intent(this, AudioRecordAndMuxActivity::class.java)) },
        Item("Video Recorder") { startActivity(Intent(this, CameraActivity::class.java)) },
        Item("Display Recorder") { startActivity(Intent(this, AlDisplayActivity::class.java)) },
        Item("FFmpeg cmd") { startActivity(Intent(this, FFCMDActivity::class.java)) },
        Item("Echo Player") { startActivity(Intent(this, EchoActivity::class.java)) },
        Item("Image Editor") { startActivity(Intent(this, ImageActivity::class.java)) },
        Item("Voice Player") { startActivity(Intent(this, AudioPlayerActivity::class.java)) },
        Item("Voice Recorder") { startActivity(Intent(this, TestAudioRecorderActivity::class.java)) },
        Item("TestMemFIFOActivity") { startActivity(Intent(this, TestMemFIFOActivity::class.java)) },
        Item("Show Bitmap") { startActivity(Intent(this, BitmapActivity::class.java)) }
    )

    override fun getLayoutResource(): Int = R.layout.activity_main
    override fun initView() {
        setSupportActionBar(toolbar)
        toolbar.title = resources.getString(R.string.app_name)
        toolbar.subtitle = BuildConfig.VERSION_NAME
        SourcesManager(this).initialize()
        val adapter = SimpleTextAdapter()
        val list = ArrayList<String>()
        ITEMS.forEach { list.add(it.name) }
        adapter.bindData(list)
        listView.adapter = adapter
        listView.addItemDecoration(ListItemDivider(this, DividerItemDecoration.VERTICAL))
        listView.addOnItemTouchListener(OnRecyclerItemClickListener(this, this))
        if(isNightMode()) {
            listView.setBackgroundColor(resources.getColor(R.color.black_232))
        }
    }

    class Item(val name: String, val action: () -> Unit)

    override fun onItemClick(parent: RecyclerView?, view: View?, position: Int) {
        ITEMS[position].action()
    }

    private fun actionVideoV2() {
        val testFile = File(externalCacheDir, "/video/hw_small.mp4")
        if (!testFile.exists()) {
            Toast.makeText(this, "File NOT exist.", Toast.LENGTH_SHORT).show()
            return
        }
        val intent = Intent(this, AlVideoV2Activity::class.java)
        intent.data = Uri.fromFile(testFile)
        startActivity(intent)
    }
}
