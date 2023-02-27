package com.example.ndk.utils

import com.example.ndk.BaseApplication

object SignatureUtils {

    init {
        System.loadLibrary("signature")
    }

    external fun signatureParams(params: String): String

    external fun signatureVerify(application: BaseApplication)

}
