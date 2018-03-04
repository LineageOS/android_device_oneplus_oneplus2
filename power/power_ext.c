/*
 * Copyright (c) 2016 The CyanogenMod Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "PowerHAL_MSM8994_Ext"

#include <utils/Log.h>
#include "utils.h"

#define BIG_MIN_CPU_PATH "/sys/devices/system/cpu/cpu4/core_ctl/min_cpus"
#define BIG_MAX_CPU_PATH "/sys/devices/system/cpu/cpu4/core_ctl/max_cpus"

void power_set_interactive_ext(int on)
{
    ALOGD("%sabling big CPU cluster", on ? "En" : "Dis");
    sysfs_write(BIG_MAX_CPU_PATH, on ? "4" : "0");
    sysfs_write(BIG_MIN_CPU_PATH, on ? "0" : "0");
}
