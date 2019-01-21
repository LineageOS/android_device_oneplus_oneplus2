LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

ifneq ($(TARGET_PLATFORM_DEVICE_BASE),)
LOCAL_CFLAGS += -DUSES_BOOTDEVICE_PATH
endif

LOCAL_C_INCLUDES := bootable/recovery
LOCAL_SRC_FILES := recovery_updater.c
LOCAL_MODULE := librecovery_updater_oneplus2
LOCAL_MODULE_TAGS := eng
include $(BUILD_STATIC_LIBRARY)
