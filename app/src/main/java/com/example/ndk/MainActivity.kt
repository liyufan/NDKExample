package com.example.ndk

import android.content.pm.PackageManager
import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import com.example.ndk.databinding.ActivityMainBinding
import com.example.ndk.utils.SignatureUtils
import com.example.ndk.utils.getPackageInfoCompat

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    private val tag = javaClass.simpleName

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = ActivityMainBinding.inflate(layoutInflater)
        val view = binding.root
        setContentView(view)

        val packageInfo = packageManager.getPackageInfoCompat(
            packageName, PackageManager.GET_SIGNING_CERTIFICATES
        )
        val signatures = packageInfo.signingInfo.signingCertificateHistory
        Log.d(tag, "signatures: ${signatures[0].toCharsString()}")

        binding.tv.text = SignatureUtils.signatureParams("userName=lyf")
    }

}
