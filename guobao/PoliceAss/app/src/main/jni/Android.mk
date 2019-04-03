# Android.mk for FFmpeg
#
# Lei Xiaohua ������
# leixiaohua1020@126.com
# http://blog.csdn.net/leixiaohua1020
# 

LOCAL_PATH := $(call my-dir)

# FFmpeg library
include $(CLEAR_VARS)
LOCAL_MODULE := avcodec
LOCAL_SRC_FILES := libavcodec-56.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := avdevice
LOCAL_SRC_FILES := libavdevice-56.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := avfilter
LOCAL_SRC_FILES := libavfilter-5.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := avformat
LOCAL_SRC_FILES := libavformat-56.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := avutil
LOCAL_SRC_FILES := libavutil-54.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := postproc
LOCAL_SRC_FILES := libpostproc-53.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := swresample
LOCAL_SRC_FILES := libswresample-1.so
include $(PREBUILT_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := swscale
LOCAL_SRC_FILES := libswscale-3.so
include $(PREBUILT_SHARED_LIBRARY)

# Program
include $(CLEAR_VARS)
LOCAL_MODULE := sfftranscoder
LOCAL_SRC_FILES :=ffmpeg_jni.c ffmpeg_mod.c ffmpeg_opt.c ffmpeg_filter.c cmdutils.c
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include
LOCAL_LDLIBS := -llog -lz
LOCAL_SHARED_LIBRARIES := avcodec avdevice avfilter avformat avutil postproc swresample swscale
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libnmUVC
LOCAL_SRC_FILES := ImageProc.c utils.cpp color.cpp
LOCAL_LDLIBS    := -llog -ljnigraphics
LOCAL_SHARED_LIBRARIES := liblog libcutils libjnigraphics
include $(BUILD_SHARED_LIBRARY)

#include $(CLEAR_VARS)
#LOCAL_MODULE := rgbInterpolation
#LOCAL_SRC_FILES := gbpe_policeass_utils_RgbInterpolation.c
#LOCAL_C_INCLUDES += $(LOCAL_PATH)/include
#LOCAL_LDLIBS := -llog -lz
#LOCAL_SHARED_LIBRARIES := avcodec avdevice avfilter avformat avutil postproc swresample swscale
#include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
OPENCV_CAMERA_MODULES:=on
OPENCV_INSTALL_MODULES:=on
include $(LOCAL_PATH)/OpencvNative/jni/OpenCV.mk
FILE_LIST := $(wildcard $(LOCAL_PATH)/easyPR/src/*/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_PATH)/EasyPR.cpp
LOCAL_SRC_FILES += $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES += $(LOCAL_PATH)/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/easyPR/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/OpencvNative/jni/include
LOCAL_MODULE     := EasyPR
LOCAL_LDLIBS += -llog

include $(BUILD_SHARED_LIBRARY)
