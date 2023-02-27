package com.example.ndk.utils

import android.content.Context
import android.content.pm.PackageInfo
import android.content.pm.PackageManager
import android.os.Build
import android.widget.Toast

fun PackageManager.getPackageInfoCompat(packageName: String, flags: Int): PackageInfo =
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
        getPackageInfo(packageName, PackageManager.PackageInfoFlags.of(flags.toLong()))
    } else {
        getPackageInfo(packageName, flags)
    }

fun <T> T.showToast(context: Context, duration: Int = Toast.LENGTH_SHORT) =
    Toast.makeText(context, this.toString(), duration).show()
