////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2013 Jonathan De Wachter (dewachter.jonathan@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

#include <string>
#include <android/native_activity.h>
#include <android/log.h>
#include <dlfcn.h>
#include <errno.h>
#include <stdlib.h>
#include <jni.h>

#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_INFO, "sfml-activity", __VA_ARGS__))

std::string getLibraryName(JNIEnv* lJNIEnv, jobject& objectActivityInfo)
{
    // This function reads the value of meta-data "sfml.app.lib_name" 
    // found in the Android Manifest file and returns it. It performs the 
    // following java code using the JNI interface:
    //
    // ai.metaData.getString("sfml.app.lib_name");
    
    // Get metaData instance from the ActivityInfo object
    jclass classActivityInfo = lJNIEnv->FindClass("android/content/pm/ActivityInfo");
    jfieldID fieldMetaData = lJNIEnv->GetFieldID(classActivityInfo, "metaData", "Landroid/os/Bundle;");
    jobject objectMetaData = lJNIEnv->GetObjectField(objectActivityInfo, fieldMetaData);

    // Create a java string object containing "sfml.app.lib_name"
    jobject objectName = lJNIEnv->NewStringUTF("sfml.app.lib_name");
    
    // Get the value of meta-data named "sfml.app.lib_name"
    jclass classBundle = lJNIEnv->FindClass("android/os/Bundle");
    jmethodID methodGetString = lJNIEnv->GetMethodID(classBundle, "getString", "(Ljava/lang/String;)Ljava/lang/String;");
    jobject objectValue = lJNIEnv->CallObjectMethod(objectMetaData, methodGetString, objectName);
    
    // No meta-data "sfml.app.lib_name" was found so we abord and inform the user
    if (objectValue == NULL)
    {
        LOGE("No meta-data 'sfml.app.lib_name' found in AndroidManifest.xml file");
        exit(1);
    }
        
    // Convert the application name to a C++ string and return it
    const char* applicationName = lJNIEnv->GetStringUTFChars(objectValue, NULL);
    std::string ret(applicationName);
    lJNIEnv->ReleaseStringUTFChars(objectValue, applicationName);
    
    return ret;
}

void* loadLibrary(const char* libraryName, JNIEnv* lJNIEnv, jobject& ObjectActivityInfo)
{
    // Find out the absolute path of the library
    jclass ClassActivityInfo = lJNIEnv->FindClass("android/content/pm/ActivityInfo");
    jfieldID FieldApplicationInfo = lJNIEnv->GetFieldID(ClassActivityInfo, "applicationInfo", "Landroid/content/pm/ApplicationInfo;");
    jobject ObjectApplicationInfo = lJNIEnv->GetObjectField(ObjectActivityInfo, FieldApplicationInfo);

    jclass ClassApplicationInfo = lJNIEnv->FindClass("android/content/pm/ApplicationInfo");
    jfieldID FieldNativeLibraryDir = lJNIEnv->GetFieldID(ClassApplicationInfo, "nativeLibraryDir", "Ljava/lang/String;");

    jobject ObjectDirPath = lJNIEnv->GetObjectField(ObjectApplicationInfo, FieldNativeLibraryDir);

    jclass ClassSystem = lJNIEnv->FindClass("java/lang/System");
    jmethodID StaticMethodMapLibraryName = lJNIEnv->GetStaticMethodID(ClassSystem, "mapLibraryName", "(Ljava/lang/String;)Ljava/lang/String;");

    jstring LibNameObject = lJNIEnv->NewStringUTF(libraryName);
    jobject ObjectName = lJNIEnv->CallStaticObjectMethod(ClassSystem, StaticMethodMapLibraryName, LibNameObject);

    jclass ClassFile = lJNIEnv->FindClass("java/io/File");
    jmethodID FileConstructor = lJNIEnv->GetMethodID(ClassFile, "<init>", "(Ljava/lang/String;Ljava/lang/String;)V");
    jobject ObjectFile = lJNIEnv->NewObject(ClassFile, FileConstructor, ObjectDirPath, ObjectName);

    // Get the library absolute path and convert it
    jmethodID MethodGetPath = lJNIEnv->GetMethodID(ClassFile, "getPath", "()Ljava/lang/String;");
    jstring javaLibraryPath = static_cast<jstring>(lJNIEnv->CallObjectMethod(ObjectFile, MethodGetPath));
    const char* libraryPath = lJNIEnv->GetStringUTFChars(javaLibraryPath, NULL);

    // Manually load the library
    void * handle = dlopen(libraryPath, RTLD_NOW | RTLD_GLOBAL);
    if (!handle)
    {
        LOGE("dlopen(\"%s\"): %s", libraryPath, dlerror());
        exit(1);
    }

    // Release the Java string
    lJNIEnv->ReleaseStringUTFChars(javaLibraryPath, libraryPath);

    return handle;
}

void ANativeActivity_onCreate(ANativeActivity* activity, void* savedState, size_t savedStateSize)
{
    // Before we can load a library, we need to find out its location. As
    // we're powerless here in C/C++, we need the JNI interface to communicate
    // with the attached Java virtual machine and perform some Java calls in
    // order to retrieve the absolute path of our libraries.
    //
    // Here's the snippet of Java code it performs:
    // --------------------------------------------
    // ai = getPackageManager().getActivityInfo(getIntent().getComponent(), PackageManager.GET_META_DATA);
    // File libraryFile = new File(ai.applicationInfo.nativeLibraryDir, System.mapLibraryName(libname));
    // String path = libraryFile.getPath();
    //
    // With libname being the library name such as "jpeg".

    // Initialize JNI
    jint lResult;
    jint lFlags = 0;

    JavaVM* lJavaVM = activity->vm;
    JNIEnv* lJNIEnv = activity->env;

    JavaVMAttachArgs lJavaVMAttachArgs;
    lJavaVMAttachArgs.version = JNI_VERSION_1_6;
    lJavaVMAttachArgs.name = "NativeThread";
    lJavaVMAttachArgs.group = NULL;

    // Attach the current thread to the JAva virtual machine
    lResult=lJavaVM->AttachCurrentThread(&lJNIEnv, &lJavaVMAttachArgs);

    if (lResult == JNI_ERR) {
        LOGE("Couldn't attach the current thread to the Java virtual machine");
        exit(1);
    }

    // Retrieve the NativeActivity
    jobject ObjectNativeActivity = activity->clazz;
    jclass ClassNativeActivity = lJNIEnv->GetObjectClass(ObjectNativeActivity);

    // Retrieve the ActivityInfo
    jmethodID MethodGetPackageManager = lJNIEnv->GetMethodID(ClassNativeActivity, "getPackageManager", "()Landroid/content/pm/PackageManager;");
    jobject ObjectPackageManager = lJNIEnv->CallObjectMethod(ObjectNativeActivity, MethodGetPackageManager);

    jmethodID MethodGetIndent = lJNIEnv->GetMethodID(ClassNativeActivity, "getIntent", "()Landroid/content/Intent;");
    jobject ObjectIntent = lJNIEnv->CallObjectMethod(ObjectNativeActivity, MethodGetIndent);

    jclass ClassIntent = lJNIEnv->FindClass("android/content/Intent");
    jmethodID MethodGetComponent = lJNIEnv->GetMethodID(ClassIntent, "getComponent", "()Landroid/content/ComponentName;");

    jobject ObjectComponentName = lJNIEnv->CallObjectMethod(ObjectIntent, MethodGetComponent);

    jclass ClassPackageManager = lJNIEnv->FindClass("android/content/pm/PackageManager");

    //jfieldID FieldGET_META_DATA = lJNIEnv->GetStaticFieldID(ClassPackageManager, "GET_META_DATA", "L");
    //jobject GET_META_DATA = lJNIEnv->GetStaticObjectField(ClassPackageManager, FieldGET_META_DATA);
    // getActivityInfo(getIntent().getComponent(), PackageManager.GET_META_DATA) -> ActivityInfo object
    jmethodID MethodGetActivityInfo = lJNIEnv->GetMethodID(ClassPackageManager, "getActivityInfo", "(Landroid/content/ComponentName;I)Landroid/content/pm/ActivityInfo;");

    // todo: do not hardcode the GET_META_DATA integer value but retrieve it instead
    jobject ObjectActivityInfo = lJNIEnv->CallObjectMethod(ObjectPackageManager, MethodGetActivityInfo, ObjectComponentName, (jint)128);

    // Load our libraries in reverse order
    loadLibrary("c++_shared", lJNIEnv, ObjectActivityInfo);
    loadLibrary("sfml-system", lJNIEnv, ObjectActivityInfo);
    loadLibrary("sfml-window", lJNIEnv, ObjectActivityInfo);
    loadLibrary("sfml-graphics", lJNIEnv, ObjectActivityInfo);
    loadLibrary("sndfile", lJNIEnv, ObjectActivityInfo);
    loadLibrary("openal", lJNIEnv, ObjectActivityInfo);
    loadLibrary("sfml-audio", lJNIEnv, ObjectActivityInfo);
    loadLibrary("sfml-network", lJNIEnv, ObjectActivityInfo);
    
    std::string libName = getLibraryName(lJNIEnv, ObjectActivityInfo);
    void* handle = loadLibrary(libName.c_str(), lJNIEnv, ObjectActivityInfo);

    // todo: should we detach the current thread ? because if we do, it
    // crashes (lJavaVM->DetachCurrentThread();)

    // Call the original ANativeActivity_onCreate function
    void (*ANativeActivity_onCreate)(ANativeActivity*, void*, size_t) = dlsym(handle, "ANativeActivity_onCreate");

    if (!ANativeActivity_onCreate)
    {
        LOGE("sfml-activity: Undefined symbol ANativeActivity_onCreate");
        exit(1);
    }

    ANativeActivity_onCreate(activity, savedState, savedStateSize);
}
