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

#include "TouchscreenGesture.h"

namespace vendor {
namespace lineage {
namespace touch {
namespace V1_0 {
namespace oneplus2 {

static const std::string GESTURE_PATHS[] = {
    "/proc/touchpanel/draw_v",
    "/proc/touchpanel/draw_reversed_v",
    "/proc/touchpanel/draw_right_v",
    "/proc/touchpanel/draw_left_v",
    "/proc/touchpanel/draw_circle",
    "/proc/touchpanel/double_swipe",
    "/proc/touchpanel/right_swipe",
    "/proc/touchpanel/left_swipe",
    "/proc/touchpanel/down_swipe",
    "/proc/touchpanel/up_swipe",
};

TouchscreenGesture::TouchscreenGesture() {
}

// Methods from ::vendor::lineage::touch::V1_0::ITouchscreenGesture follow.
Return<void> TouchscreenGesture::getSupportedGestures(getSupportedGestures_cb _hidl_cb) {
    std::vector<Gesture> gestures;
    gestures.push_back(Gesture{0, "down arrow", 250});
    gestures.push_back(Gesture{1, "up arrow", 251});
    gestures.push_back(Gesture{2, "right arrow", 252});
    gestures.push_back(Gesture{3, "left arrow", 253});
    gestures.push_back(Gesture{4, "letter o", 254});
    gestures.push_back(Gesture{5, "two finger down swipe", 255});
    gestures.push_back(Gesture{6, "one finger right swipe", 256});
    gestures.push_back(Gesture{7, "one finger left swipe", 257});
    gestures.push_back(Gesture{8, "one finger down swipe", 258});
    gestures.push_back(Gesture{9, "one finger up swipe", 259});

    _hidl_cb(gestures);
    return Void();
}

Return<void> TouchscreenGesture::setGestureEnabled(const Gesture& gesture, bool enabled) {
    std::ofstream file(GESTURE_PATHS[gesture.id]);
    file << (enabled ? "1" : "0");
    return Void();
}

}  // namespace oneplus2
}  // namespace V1_0
}  // namespace touch
}  // namespace lineage
}  // namespace vendor
