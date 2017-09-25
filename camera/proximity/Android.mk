# Copyright (C) 2008 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.



LOCAL_PATH := $(call my-dir)

ifneq ($(TARGET_SIMULATOR),true)

# HAL module implemenation, not prelinked, and stored in
# hw/<SENSORS_HARDWARE_MODULE_ID>.<ro.product.board>.so
include $(CLEAR_VARS)

# LOCAL_MODULE := sensors.herring
LOCAL_MODULE := sensors.hal.tof

LOCAL_C_INCLUDES:= \
        kernel/include/linux/input/ \

LOCAL_MODULE_RELATIVE_PATH := hw

LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS := -DLOG_TAG=\"Sensors\"
LOCAL_SRC_FILES :=                        \
                sensors.cpp               \
                SensorBase.cpp            \
                ProximitySensor.cpp    	  \
                InputEventReader.cpp

LOCAL_SHARED_LIBRARIES := liblog liblog libcutils 
LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_SRC_FILES:= \
    laser_init.cpp \

LOCAL_MODULE:= laser_init
LOCAL_C_INCLUDES += \
    system/extras/ext4_utils \
    system/core/mkbootimg

LOCAL_MODULE_PATH := $(TARGET_OUT)/bin
LOCAL_UNSTRIPPED_PATH := $(TARGET_ROOT_OUT_UNSTRIPPED)

LOCAL_STATIC_LIBRARIES := \
    libinit \
    libfs_mgr \
    libsquashfs_utils \
    liblogwrap \
    libcutils \
    libbase \
    libext4_utils_static \
    libutils \
    liblog \
    libselinux \
    libmincrypt \
    libc++_static \
    libdl \
    libsparse_static \
    libz

LOCAL_SHARED_LIBRARIES := liblog libcutils liblog

LOCAL_CLANG := $(init_clang)
include $(BUILD_EXECUTABLE)


endif # !TARGET_SIMULATOR
