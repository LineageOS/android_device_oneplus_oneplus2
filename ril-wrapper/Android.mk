LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE           := libril-wrapper
LOCAL_MULTILIB         := 64
LOCAL_VENDOR_MODULE    := true
LOCAL_SRC_FILES        := ril-wrapper.c
LOCAL_SHARED_LIBRARIES := libdl liblog libril libcutils
LOCAL_CFLAGS           := -Wall -Werror
include $(BUILD_SHARED_LIBRARY)
