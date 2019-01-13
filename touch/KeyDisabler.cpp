/*
 * Copyright (C) 2019 The LineageOS Project
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

#include <fstream>

#include "KeyDisabler.h"

namespace vendor {
namespace lineage {
namespace touch {
namespace V1_0 {
namespace oneplus2 {

KeyDisabler::KeyDisabler() {
}

// Methods from ::vendor::lineage::touch::V1_0::IKeyDisabler follow.
Return<void> KeyDisabler::setEnabled(bool enabled) {
    std::ofstream file("/proc/s1302/virtual_key");
    file << (enabled ? "1" : "0");
    return Void();
}

}  // namespace oneplus2
}  // namespace V1_0
}  // namespace touch
}  // namespace lineage
}  // namespace vendor
