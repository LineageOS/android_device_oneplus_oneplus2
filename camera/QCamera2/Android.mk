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

LOCAL_CFLAGS := -Wall -Wextra -Werror
LOCAL_CFLAGS += -DHAS_MULTIMEDIA_HINTS

ifeq ($(TARGET_USES_AOSP),true)
LOCAL_CFLAGS += -DVANILLA_HAL
endif

LOCAL_CFLAGS += -DVENDOR_EDIT

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
    $(call project-path-for,qcom-media)/libstagefrighthw \
    system/media/camera/include \
    $(LOCAL_PATH)/../mm-image-codec/qexif \
    $(LOCAL_PATH)/../mm-image-codec/qomx_core \
    $(LOCAL_PATH)/util \
    $(TARGET_OUT_HEADERS)/qcom/display

#HAL 1.0 Include paths
LOCAL_C_INCLUDES += \
    frameworks/native/include/media/hardware \
    $(call project-path-for,qcom-camera)/QCamera2/HAL

LOCAL_HEADER_LIBRARIES := generated_kernel_headers


LOCAL_SHARED_LIBRARIES := libcamera_client liblog libhardware libutils libcutils libdl
LOCAL_SHARED_LIBRARIES += libmmcamera_interface libmmjpeg_interface libui libcamera_metadata
LOCAL_SHARED_LIBRARIES += libqdMetaData libstagefrighthw

LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_MODULE := camera.$(TARGET_BOARD_PLATFORM)
LOCAL_MODULE_TAGS := optional
LOCAL_VENDOR_MODULE := true

LOCAL_32_BIT_ONLY := true
include $(BUILD_SHARED_LIBRARY)

include $(call first-makefiles-under,$(LOCAL_PATH))
