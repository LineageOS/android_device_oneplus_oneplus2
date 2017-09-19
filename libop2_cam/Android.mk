LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_SRC_FILES := op2_cam.c
LOCAL_MODULE := libop2_cam
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)
