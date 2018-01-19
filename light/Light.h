/*
 * Copyright (C) 2018 The LineageOS Project
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

#ifndef ANDROID_HARDWARE_LIGHT_V2_0_LIGHT_H
#define ANDROID_HARDWARE_LIGHT_V2_0_LIGHT_H

#include <android/hardware/light/2.0/ILight.h>
#include <hidl/Status.h>

#include <fstream>
#include <mutex>
#include <unordered_map>

namespace android {
namespace hardware {
namespace light {
namespace V2_0 {
namespace implementation {

struct Light : public ILight {
    Light(std::pair<std::ofstream, uint32_t>&& lcd_backlight,
          std::vector<std::ofstream>&& button_backlight,
          std::ofstream&& red_led, std::ofstream&& green_led, std::ofstream&& blue_led,
          std::ofstream&& red_duty_pcts, std::ofstream&& green_duty_pcts, std::ofstream&& blue_duty_pcts,
          std::ofstream&& red_start_idx, std::ofstream&& green_start_idx, std::ofstream&& blue_start_idx,
          std::ofstream&& red_pause_lo, std::ofstream&& green_pause_lo, std::ofstream&& blue_pause_lo,
          std::ofstream&& red_pause_hi, std::ofstream&& green_pause_hi, std::ofstream&& blue_pause_hi,
          std::ofstream&& red_ramp_step_ms, std::ofstream&& green_ramp_step_ms, std::ofstream&& blue_ramp_step_ms,
          std::ofstream&& red_blink, std::ofstream&& green_blink, std::ofstream&& blue_blink,
          std::ofstream&& rgb_blink);

    // Methods from ::android::hardware::light::V2_0::ILight follow.
    Return<Status> setLight(Type type, const LightState& state) override;
    Return<void> getSupportedTypes(getSupportedTypes_cb _hidl_cb) override;

  private:
    void setAttentionLight(const LightState& state);
    void setBatteryLight(const LightState& state);
    void setButtonsBacklight(const LightState& state);
    void setLcdBacklight(const LightState& state);
    void setNotificationLight(const LightState& state);
    void setSpeakerBatteryLightLocked();
    void setSpeakerLightLocked(const LightState& state);

    std::pair<std::ofstream, uint32_t> mLcdBacklight;
    std::vector<std::ofstream> mButtonBacklight;
    std::ofstream mRedLed;
    std::ofstream mGreenLed;
    std::ofstream mBlueLed;
    std::ofstream mRedDutyPcts;
    std::ofstream mGreenDutyPcts;
    std::ofstream mBlueDutyPcts;
    std::ofstream mRedStartIdx;
    std::ofstream mGreenStartIdx;
    std::ofstream mBlueStartIdx;
    std::ofstream mRedPauseLo;
    std::ofstream mGreenPauseLo;
    std::ofstream mBluePauseLo;
    std::ofstream mRedPauseHi;
    std::ofstream mGreenPauseHi;
    std::ofstream mBluePauseHi;
    std::ofstream mRedRampStepMs;
    std::ofstream mGreenRampStepMs;
    std::ofstream mBlueRampStepMs;
    std::ofstream mRedBlink;
    std::ofstream mGreenBlink;
    std::ofstream mBlueBlink;
    std::ofstream mRgbBlink;

    LightState mAttentionState;
    LightState mBatteryState;
    LightState mNotificationState;

    std::unordered_map<Type, std::function<void(const LightState&)>> mLights;
    std::mutex mLock;
};

}  // namespace implementation
}  // namespace V2_0
}  // namespace light
}  // namespace hardware
}  // namespace android

#endif  // ANDROID_HARDWARE_LIGHT_V2_0_LIGHT_H
