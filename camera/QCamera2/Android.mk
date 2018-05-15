ifneq (,$(filter $(TARGET_ARCH), arm arm64))

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
        util/QCameraCmdThread.cpp \
        util/QCameraQueue.cpp \
        util/QCameraFlash.cpp \
        QCamera2Hal.cpp \
        QCamera2Factory.cpp

#HAL 3.0 source
LOCAL_SRC_FILES += \
        HAL3/QCamera3HWI.cpp \
        HAL3/QCamera3Mem.cpp \
        HAL3/QCamera3Stream.cpp \
        HAL3/QCamera3Channel.cpp \
        HAL3/QCamera3VendorTags.cpp \
        HAL3/QCamera3PostProc.cpp \
        HAL3/QCamera3CropRegionMapper.cpp

#HAL 1.0 source
LOCAL_SRC_FILES += \
        HAL/QCamera2HWI.cpp \
        HAL/QCameraMem.cpp \
        HAL/QCameraStateMachine.cpp \
        HAL/QCameraChannel.cpp \
        HAL/QCameraStream.cpp \
        HAL/QCameraPostProc.cpp \
        HAL/QCamera2HWICallbacks.cpp \
        HAL/QCameraParameters.cpp \
        HAL/QCameraThermalAdapter.cpp

LOCAL_CFLAGS := -Wall -Wextra -Werror -Wno-format -Wno-gnu-designator -Wno-unused-parameter -Wno-unused-variable -Wno-unused-private-field -Wno-unused-label -Wno-tautological-pointer-compare
LOCAL_CFLAGS += -DHAS_MULTIMEDIA_HINTS

ifeq ($(TARGET_USES_AOSP),true)
LOCAL_CFLAGS += -DVANILLA_HAL
endif

#use media extension
ifeq ($(TARGET_USES_MEDIA_EXTENSIONS), true)
LOCAL_CFLAGS += -DUSE_MEDIA_EXTENSIONS
endif

#HAL 1.0 Flags
LOCAL_CFLAGS += -DDEFAULT_DENOISE_MODE_ON -DHAL3

LOCAL_C_INCLUDES := \
        $(LOCAL_PATH)/stack/common \
        frameworks/native/include/media/hardware \
        frameworks/native/include/media/openmax \
        system/media/camera/include \
        $(LOCAL_PATH)/../mm-image-codec/qexif \
        $(LOCAL_PATH)/../mm-image-codec/qomx_core \
        $(LOCAL_PATH)/util \
        hardware/qcom/media-caf/msm8994/libstagefrighthw \
        device/qcom/common/power \
        $(call include-path-for, android.hidl.token@1.0-utils) \
        $(call include-path-for, android.hardware.graphics.bufferqueue@1.0)

#HAL 1.0 Include paths
LOCAL_C_INCLUDES += \
        frameworks/native/include/media/hardware \
        $(LOCAL_PATH)/HAL

LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr

#LOCAL_STATIC_LIBRARIES := libqcamera2_util
LOCAL_C_INCLUDES += \
        hardware/qcom/display/msm8994/libgralloc
LOCAL_C_INCLUDES += \
        hardware/qcom/display/msm8994/libqdutils

LOCAL_SHARED_LIBRARIES := liblog libcamera_client liblog libhardware libutils libcutils libdl
LOCAL_SHARED_LIBRARIES += libmmcamera_interface libmmjpeg_interface libui libcamera_metadata
LOCAL_SHARED_LIBRARIES += libqdMetaData libqdutils
LOCAL_SHARED_LIBRARIES += android.hidl.token@1.0-utils
LOCAL_SHARED_LIBRARIES += android.hardware.graphics.bufferqueue@1.0

LOCAL_CLANG := true
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_MODULE := camera.msm8994
LOCAL_MODULE_TAGS := optional

LOCAL_32_BIT_ONLY := $(BOARD_QTI_CAMERA_32BIT_ONLY)
include $(BUILD_SHARED_LIBRARY)

include $(call first-makefiles-under,$(LOCAL_PATH))

endif
