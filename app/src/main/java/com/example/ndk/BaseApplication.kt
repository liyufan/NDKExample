package com.example.ndk

import android.annotation.SuppressLint
import android.app.Application
import android.content.Context
import com.example.ndk.utils.SignatureUtils
import com.example.ndk.utils.getPackageInfoCompat

class BaseApplication : Application() {

    companion object {
        @SuppressLint("StaticFieldLeak")
        lateinit var context: Context
    }

    override fun onCreate() {
        super.onCreate()
        context = applicationContext
        SignatureUtils.signatureVerify(this)
    }

    fun getPackageInfo(packageName: String, flags: Int) =
        packageManager.getPackageInfoCompat(packageName, flags)

}
