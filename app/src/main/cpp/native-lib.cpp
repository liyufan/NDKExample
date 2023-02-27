#include <jni.h>
#include <string>
#include <android/log.h>

#ifndef PROTOTYPES
#define PROTOTYPES 1
#endif

extern "C" {
#include "md5/global.h"
#include "md5/md5.h"
}

using namespace std;

static const char *TAG = "ndk.cpp";
static const char *EXTRA_SIGNATURE = "Lee";
static int isVerify = 0;
static const char *PACKAGE_NAME = "com.example.ndk";
static const char *APP_SIGNATURE = "308202e4308201cc020101300d06092a864886f70d010105050030373116301406035504030c0d416e64726f69642044656275673110300e060355040a0c07416e64726f6964310b30090603550406130255533020170d3232303932373131333031335a180f32303532303931393131333031335a30373116301406035504030c0d416e64726f69642044656275673110300e060355040a0c07416e64726f6964310b300906035504061302555330820122300d06092a864886f70d01010105000382010f003082010a0282010100a54db0f77fc5f7985460ee9232e21615e331894ecde3e5e09004614f635a2e1bb5b1a0a557a8f595f6bd63956fbd57b3f6d0d21c64274f504de84ef405f9d12ab46bc87fe7bcb19079067661dc49401e3140b1dd9f4f56cadb60daea983080d0eb296853e54cd93ac75a742e4b520ae19943fef85baadc760d2bf3f84a23c9e99f4e9b8133f8228a3860462aee7c91079d72c72c8f444facc81c74461444357c7a5ba5759a5261eb44e700c06f61bf24af057a53a620b886a68f5418109e0c0b169b3b06d46eab38c46f0f5220668a38b103137ddcf7249dcfe59ed8d4f5813dd9e1c8bda3cd00cbfb44c8e8aa735ece72cf65f3723f6b1b5f96782fd858466f0203010001300d06092a864886f70d01010505000382010100535276307ed274e4ff254b1b9fbcfc6a2a64b92eb16ea6c8175f900ae52c26c1049ac3b93273e092a985eff8812592f2e41c38aa2e942febc9b54211699e76dd935d8458942f541a4fb265298cda6eaf1537d2c378e6bf77d170519faf8e63fca0e52d8f2c7d98ec13cca3a0e8764b0a00dfc962e4c25387e59d954221aeb103c2d94d2a6e0b84b8250693fd623d64d41f380e453a4dcf568b4b13cd764d1ea428cdd16e71f4fdd30075482cb72eefb8ed7e3115e5084ea484fb08613cfeadb479f803c9a848e17dc793097915c800ef846466379756f5a1ebcca9b28e26dcd42279c164480bb9fe5c51f6456323dfae586c6561f0d32f86730ff0c332381307";

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_ndk_utils_SignatureUtils_signatureParams(JNIEnv *env, jobject /* this */,
                                                          jstring params_) {
    if (!isVerify) {
        return env->NewStringUTF("error signature");
    }

    const char *params = env->GetStringUTFChars(params_, nullptr);

    string signature_str(params);
    signature_str.insert(0, EXTRA_SIGNATURE);
    signature_str = signature_str.substr(0, signature_str.length() - 2);

    auto *ctx = new MD5_CTX();
    MD5Init(ctx);
    MD5Update(ctx, (unsigned char *) signature_str.c_str(), signature_str.length());
    unsigned char digest[16];
    MD5Final(digest, ctx);

    char md5_str[33];
    for (unsigned char i: digest) {
        sprintf(md5_str, "%s%02x", md5_str, i);
    }
    md5_str[32] = '\0';

    env->ReleaseStringUTFChars(params_, params);

    delete ctx;

    return env->NewStringUTF(md5_str);
}

/**
PackageInfo packageInfo = context.getPackageManager().getPackageInfoCompat(context.getPackageName(), PackageManager.GET_SIGNING_CERTIFICATES);
Signature[] signs = packageInfo.signingInfo.getSigningCertificateHistory();
return signs[0].toCharsString();
 */
extern "C" JNIEXPORT void JNICALL
Java_com_example_ndk_utils_SignatureUtils_signatureVerify(JNIEnv *env, jobject /* this */,
                                                          jobject application) {
    // 1. 获取包名
    auto j_clz = env->GetObjectClass(application);
    auto j_mid = env->GetMethodID(j_clz, "getPackageName", "()Ljava/lang/String;");
    auto j_package_name = (jstring) env->CallObjectMethod(application, j_mid);
    auto c_package_name = env->GetStringUTFChars(j_package_name, nullptr);
    // 2. 比对包名
    if (strcmp(c_package_name, PACKAGE_NAME) != 0) { return; }
    __android_log_print(ANDROID_LOG_DEBUG, TAG, "包名一致：%s", c_package_name);
    // 3. 获取签名
    // 3.1 获取PackageManager
    j_mid = env->GetMethodID(j_clz, "getPackageManager", "()Landroid/content/pm/PackageManager;");
    auto j_package_manager = env->CallObjectMethod(application, j_mid);
    // 3.2 获取PackageInfo
    j_mid = env->GetMethodID(j_clz, "getPackageInfo",
                             "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
    j_clz = env->FindClass("android/content/pm/PackageManager");
    auto j_fid = env->GetStaticFieldID(j_clz, "GET_SIGNING_CERTIFICATES", "I");
    auto j_flags = env->GetStaticIntField(j_clz, j_fid);
    auto j_package_info = env->CallObjectMethod(application, j_mid, j_package_name, j_flags);
    // 3.3 获取 signatures 数组
    j_clz = env->GetObjectClass(j_package_info);
    j_fid = env->GetFieldID(j_clz, "signingInfo", "Landroid/content/pm/SigningInfo;");
    auto j_signingInfo = env->GetObjectField(j_package_info, j_fid);
    j_clz = env->GetObjectClass(j_signingInfo);
    j_mid = env->GetMethodID(j_clz, "getSigningCertificateHistory",
                             "()[Landroid/content/pm/Signature;");
    auto j_signatures = (jobjectArray) env->CallObjectMethod(j_signingInfo, j_mid);
    // 3.4 获取 signatures[0]
    auto j_signatures_first = env->GetObjectArrayElement(j_signatures, 0);
    // 3.5 调用 Signature 的 toCharsString 方法
    j_clz = env->GetObjectClass(j_signatures_first);
    j_mid = env->GetMethodID(j_clz, "toCharsString", "()Ljava/lang/String;");
    auto j_signature = (jstring) env->CallObjectMethod(j_signatures_first, j_mid);
    auto c_signature = env->GetStringUTFChars(j_signature, nullptr);
    // 4. 比对签名是否一致
    if (strcmp(c_signature, APP_SIGNATURE) != 0) { return; }
    // 5. 标记签名验证通过
    isVerify = 1;
    __android_log_print(ANDROID_LOG_DEBUG, TAG, "签名一致：%s", c_signature);
}
