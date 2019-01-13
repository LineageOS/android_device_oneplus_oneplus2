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

#define LOG_TAG "TouchscreenGestureService"

#include <android-base/file.h>
#include <android-base/logging.h>

#include "TouchscreenGesture.h"

namespace vendor {
namespace lineage {
namespace touch {
namespace V1_0 {
namespace implementation {

const std::map<int32_t, TouchscreenGesture::GestureInfo> TouchscreenGesture::kGestureInfoMap = {
    {0, {250, "down arrow", "/proc/touchpanel/draw_v"}},
    {1, {251, "up arrow", "/proc/touchpanel/draw_reversed_v"}},
    {2, {252, "right arrow", "/proc/touchpanel/draw_right_v"}},
    {3, {253, "left arrow", "/proc/touchpanel/draw_left_v"}},
    {4, {254, "letter o", "/proc/touchpanel/draw_circle"}},
    {5, {255, "two finger down swipe", "/proc/touchpanel/double_swipe"}},
    {6, {256, "one finger right swipe", "/proc/touchpanel/right_swipe"}},
    {7, {257, "one finger left swipe", "/proc/touchpanel/left_swipe"}},
    {8, {258, "one finger down swipe", "/proc/touchpanel/down_swipe"}},
    {9, {259, "one finger up swipe", "/proc/touchpanel/up_swipe"}},
};

// Methods from ::vendor::lineage::touch::V1_0::ITouchscreenGesture follow.
Return<void> TouchscreenGesture::getSupportedGestures(getSupportedGestures_cb resultCb) {
    std::vector<Gesture> gestures;

    for (const auto& entry : kGestureInfoMap) {
        gestures.push_back({entry.first, entry.second.name, entry.second.keycode});
    }
    resultCb(gestures);

    return Void();
}

Return<bool> TouchscreenGesture::setGestureEnabled(const Gesture& gesture, bool enabled) {
    const auto entry = kGestureInfoMap.find(gesture.id);
    if (entry == kGestureInfoMap.end()) {
        return false;
    }

    if (!android::base::WriteStringToFile((enabled ? "1" : "0"), entry->second.path)) {
        LOG(ERROR) << "Failed to write " << entry->second.path;
        return false;
    }

    return true;
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace touch
}  // namespace lineage
}  // namespace vendor
