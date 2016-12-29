LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

OPENCV_INSTALL_MODULES:=on
OPENCV_LIB_TYPE:=STATIC

include ../../sdk/native/jni/OpenCV.mk

LOCAL_MODULE    := mixed_sample
LOCAL_SRC_FILES := svm.h global.h jni_part.cpp assignmeant.cpp color_clustering.cpp colorfeat.cpp function.cpp histogram_intersection.cpp readmat.cpp svm_predict.cpp svm.cpp texturefeat.cpp
LOCAL_LDLIBS +=  -llog -ldl

include $(BUILD_SHARED_LIBRARY)
