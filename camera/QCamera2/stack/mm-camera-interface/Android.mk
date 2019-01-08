LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    src/mm_camera_interface.c \
    src/mm_camera.c \
    src/mm_camera_channel.c \
    src/mm_camera_stream.c \
    src/mm_camera_thread.c \
    src/mm_camera_sock.c

ifneq (,$(filter msm8974 msm8916 msm8226 msm8610 msm8916 apq8084 msm8084 msm8994 msm8992,$(TARGET_BOARD_PLATFORM)))
    LOCAL_CFLAGS += -DVENUS_PRESENT
endif

LOCAL_CFLAGS += -D_ANDROID_

LOCAL_COPY_HEADERS_TO := mm-camera-interface
LOCAL_COPY_HEADERS += ../common/cam_intf.h
LOCAL_COPY_HEADERS += ../common/cam_types.h

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/inc \
    $(LOCAL_PATH)/../common \
    $(call project-path-for,qcom-media)/mm-core/inc \
    system/media/camera/include


LOCAL_CFLAGS += -Wall -Wextra -Werror

LOCAL_MODULE := libmmcamera_interface
LOCAL_HEADER_LIBRARIES := generated_kernel_headers
LOCAL_SHARED_LIBRARIES := libdl libcutils liblog
LOCAL_MODULE_TAGS := optional
LOCAL_VENDOR_MODULE := true

LOCAL_32_BIT_ONLY := true
include $(BUILD_SHARED_LIBRARY)
