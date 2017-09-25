OMX_CORE_PATH := $(call my-dir)

# ------------------------------------------------------------------------------
#                Make the shared library (libqomx_core)
# ------------------------------------------------------------------------------

include $(CLEAR_VARS)
LOCAL_PATH := $(OMX_CORE_PATH)
LOCAL_MODULE_TAGS := optional

omx_core_defines:= -Werror \
                   -g -O3

LOCAL_CFLAGS := -Wno-format -Wno-gnu-designator -Wno-unused-parameter -Wno-unused-variable -Wno-unused-private-field -Wno-unused-label -Wno-tautological-pointer-compare $(omx_core_defines)

OMX_HEADER_DIR := frameworks/native/include/media/openmax

LOCAL_C_INCLUDES := $(OMX_HEADER_DIR)
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../qexif
LOCAL_C_INCLUDES += system/core/liblog/include/

LOCAL_SRC_FILES := qomx_core.c

LOCAL_MODULE           := libqomx_core
LOCAL_PRELINK_MODULE   := false
LOCAL_SHARED_LIBRARIES := libcutils libdl liblog

LOCAL_32_BIT_ONLY := true
include $(BUILD_SHARED_LIBRARY)
