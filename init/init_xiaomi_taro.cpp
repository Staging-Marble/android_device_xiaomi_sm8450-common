/*
   Copyright (c) 2015, The Linux Foundation. All rights reserved.
   Copyright (C) 2016 The CyanogenMod Project.
   Copyright (C) 2019-2020 The LineageOS Project.
   Copyright (C) 2021 The Android Open Source Project.
   Copyright (C) 2022-2023 Paranoid Android.

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

#include <cstdlib>
#include <string>
#include <vector>

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>
#include <android-base/properties.h>

#include "property_service.h"
#include "vendor_init.h"

#include <libinit_variant.h>

using android::base::GetProperty;
using std::string;

// List of partitions to override props
static const string source_partitions[] = {
    "", "bootimage.", "odm.", "product.", "system.",
    "system_ext.", "vendor.", "vendor_dlkm."
};

void property_override(char const prop[], char const value[]) {
    auto pi = (prop_info*) __system_property_find(prop);

    if (pi != nullptr)
        __system_property_update(pi, value, strlen(value));
    else
        __system_property_add(prop, strlen(prop), value, strlen(value));
}

void set_ro_build_prop(const string &prop, const string &value) {
    string prop_name;

    for (const string &source : source_partitions) {
        prop_name = "ro.product." + source + prop;
        property_override(prop_name.c_str(), value.c_str());
    }
}

void set_variant_props(const variant_info_t variant) {
    set_ro_build_prop("brand", variant.brand);
    set_ro_build_prop("device", variant.device);
    set_ro_build_prop("model", variant.model);
    set_ro_build_prop("name", variant.name);
    set_ro_build_prop("marketname", variant.marketname);
    property_override("ro.product.board", variant.device.c_str());
    property_override("bluetooth.device.default_name", variant.marketname.c_str());
}

void search_set_variant_props(const std::vector<variant_info_t> variants) {
    string hwc = GetProperty("ro.boot.hwc", "");

    for (const auto& variant : variants) {
        if (variant.hwc == hwc) {
            set_variant_props(variant);
            break;
        }
    }
}
