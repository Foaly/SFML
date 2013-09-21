LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := freetype
LOCAL_SRC_FILES := lib/$(TARGET_ARCH_ABI)/libfreetype.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := jpeg
LOCAL_SRC_FILES := lib/$(TARGET_ARCH_ABI)/libjpeg.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := ogg
LOCAL_SRC_FILES := lib/$(TARGET_ARCH_ABI)/libogg.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
TARGET_ARCH_ABI := armeabi armeabi-v7a x86

include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := flac
LOCAL_SRC_FILES := lib/$(TARGET_ARCH_ABI)/libFLAC.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
TARGET_ARCH_ABI := armeabi armeabi-v7a x86

include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := vorbis
LOCAL_SRC_FILES := lib/$(TARGET_ARCH_ABI)/libvorbis.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
TARGET_ARCH_ABI := armeabi armeabi-v7a x86

include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := vorbisenc
LOCAL_SRC_FILES := lib/$(TARGET_ARCH_ABI)/libvorbisenc.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
TARGET_ARCH_ABI := armeabi armeabi-v7a x86

include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := sndfile
LOCAL_SRC_FILES := lib/$(TARGET_ARCH_ABI)/libsndfile.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SHARED_LIBRARIES := ogg flac vorbis vorbisenc
TARGET_ARCH_ABI := armeabi armeabi-v7a x86

include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := openal
LOCAL_SRC_FILES := lib/$(TARGET_ARCH_ABI)/libopenal.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SHARED_LIBRARIES := ogg flac vorbis vorbisenc
TARGET_ARCH_ABI := armeabi armeabi-v7a x86

include $(PREBUILT_SHARED_LIBRARY)
