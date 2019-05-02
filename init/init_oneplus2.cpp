/*
   Copyright (c) 2016, The CyanogenMod Project
   Copyright (C) 2017-2019, The LineageOS Project

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
   ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <android-base/logging.h>
#include <android-base/properties.h>
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>
#include <sys/sysinfo.h>

#include "property_service.h"
#include "vendor_init.h"

using android::init::property_set;

void property_override(char const prop[], char const value[])
{
    prop_info *pi;

    pi = (prop_info*) __system_property_find(prop);
    if (pi)
        __system_property_update(pi, value, strlen(value));
    else
        __system_property_add(prop, strlen(prop), value, strlen(value));
}

void property_override_dual(char const system_prop[], char const vendor_prop[], char const value[])
{
    property_override(system_prop, value);
    property_override(vendor_prop, value);
}

void vendor_load_properties() {
    struct sysinfo sys;
    int rf_version = stoi(android::base::GetProperty("ro.boot.rf_v1", ""));

    switch (rf_version) {
    case 14:
        /* China model */
        property_override_dual("ro.product.model", "ro.product.vendor.model", "ONE A2001");
        property_set("ro.rf_version", "TDD_FDD_Ch_All");
        property_set("telephony.lteOnCdmaDevice", "1");
        property_set("ro.telephony.default_network", "20,20");
        break;
    case 24:
        /* Europe / Asia model */
        property_override_dual("ro.product.model", "ro.product.vendor.model", "ONE A2003");
        property_set("ro.rf_version", "TDD_FDD_Eu");
        property_set("ro.telephony.default_network", "9,9");
        break;
    case 34:
        /* America model */
        property_override_dual("ro.product.model", "ro.product.vendor.model", "ONE A2005");
        property_set("ro.rf_version", "TDD_FDD_Am");
        property_set("telephony.lteOnCdmaDevice", "1");
        property_set("ro.telephony.default_network", "9,9");
        break;
    default:
        LOG(ERROR) << __func__ << ": unexcepted rf version!";
    }

    property_override("ro.build.product", "OnePlus2");
    property_override_dual("ro.product.device", "ro.product.vendor.device", "OnePlus2");

    /* Dalvik props */
    sysinfo(&sys);
    if (sys.totalram > 3072ull * 1024 * 1024) {
        /* Values for 4GB RAM vatiants */
        property_set("dalvik.vm.heapgrowthlimit", "288m");
        property_set("dalvik.vm.heapsize", "768m");
    } else {
        /* Values for 3GB RAM vatiants */
        property_set("dalvik.vm.heapgrowthlimit", "192m");
        property_set("dalvik.vm.heapsize", "512m");
    }
}
