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

#define LOG_TAG "android.hardware.light@2.0-service.oneplus2"

#include <android-base/logging.h>
#include <hidl/HidlTransportSupport.h>
#include <utils/Errors.h>

#include "Light.h"

// libhwbinder:
using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;

// Generated HIDL files
using android::hardware::light::V2_0::ILight;
using android::hardware::light::V2_0::implementation::Light;

const static std::string kLcdBacklightPath = "/sys/class/leds/lcd-backlight/brightness";
const static std::string kLcdMaxBacklightPath = "/sys/class/leds/lcd-backlight/max_brightness";
const static std::string kButton1BacklightPath = "/sys/class/leds/button-backlight/brightness";
const static std::string kButton2BacklightPath = "/sys/class/leds/button-backlight1/brightness";
const static std::string kButton3BacklightPath = "/sys/class/leds/button-backlight2/brightness";
const static std::string kRedLedPath = "/sys/class/leds/red/brightness";
const static std::string kGreenLedPath = "/sys/class/leds/green/brightness";
const static std::string kBlueLedPath = "/sys/class/leds/blue/brightness";
const static std::string kRedDutyPctsPath = "/sys/class/leds/red/duty_pcts";
const static std::string kGreenDutyPctsPath = "/sys/class/leds/green/duty_pcts";
const static std::string kBlueDutyPctsPath = "/sys/class/leds/blue/duty_pcts";
const static std::string kRedStartIdxPath = "/sys/class/leds/red/start_idx";
const static std::string kGreenStartIdxPath = "/sys/class/leds/green/start_idx";
const static std::string kBlueStartIdxPath = "/sys/class/leds/blue/start_idx";
const static std::string kRedPauseLoPath = "/sys/class/leds/red/pause_lo";
const static std::string kGreenPauseLoPath = "/sys/class/leds/green/pause_lo";
const static std::string kBluePauseLoPath = "/sys/class/leds/blue/pause_lo";
const static std::string kRedPauseHiPath = "/sys/class/leds/red/pause_hi";
const static std::string kGreenPauseHiPath = "/sys/class/leds/green/pause_hi";
const static std::string kBluePauseHiPath = "/sys/class/leds/blue/pause_hi";
const static std::string kRedRampStepMsPath = "/sys/class/leds/red/ramp_step_ms";
const static std::string kGreenRampStepMsPath = "/sys/class/leds/green/ramp_step_ms";
const static std::string kBlueRampStepMsPath = "/sys/class/leds/blue/ramp_step_ms";
const static std::string kRedBlinkPath = "/sys/class/leds/red/blink";
const static std::string kGreenBlinkPath = "/sys/class/leds/green/blink";
const static std::string kBlueBlinkPath = "/sys/class/leds/blue/blink";
const static std::string kRgbBlinkPath = "/sys/class/leds/rgb/rgb_blink";

int main() {
    uint32_t lcdMaxBrightness = 255;
    std::vector<std::ofstream> buttonBacklight;

    std::ofstream lcdBacklight(kLcdBacklightPath);
    if (!lcdBacklight) {
        LOG(ERROR) << "Failed to open " << kLcdBacklightPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ifstream lcdMaxBacklight(kLcdMaxBacklightPath);
    if (!lcdMaxBacklight) {
        LOG(ERROR) << "Failed to open " << kLcdMaxBacklightPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    } else {
        lcdMaxBacklight >> lcdMaxBrightness;
    }

    std::ofstream button1Backlight(kButton1BacklightPath);
    if (button1Backlight) {
        buttonBacklight.emplace_back(std::move(button1Backlight));
    } else {
        LOG(WARNING) << "Failed to open " << kButton1BacklightPath << ", error=" << errno
                     << " (" << strerror(errno) << ")";
    }

    std::ofstream button2Backlight(kButton2BacklightPath);
    if (button2Backlight) {
        buttonBacklight.emplace_back(std::move(button2Backlight));
    } else {
        LOG(WARNING) << "Failed to open " << kButton2BacklightPath << ", error=" << errno
                     << " (" << strerror(errno) << ")";
    }

    std::ofstream button3Backlight(kButton3BacklightPath);
    if (button3Backlight) {
        buttonBacklight.emplace_back(std::move(button3Backlight));
    } else {
        LOG(WARNING) << "Failed to open " << kButton3BacklightPath << ", error=" << errno
                     << " (" << strerror(errno) << ")";
    }

    std::ofstream redLed(kRedLedPath);
    if (!redLed) {
        LOG(ERROR) << "Failed to open " << kRedLedPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream greenLed(kGreenLedPath);
    if (!greenLed) {
        LOG(ERROR) << "Failed to open " << kGreenLedPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream blueLed(kBlueLedPath);
    if (!blueLed) {
        LOG(ERROR) << "Failed to open " << kBlueLedPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream redDutyPcts(kRedDutyPctsPath);
    if (!redDutyPcts) {
        LOG(ERROR) << "Failed to open " << kRedDutyPctsPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream greenDutyPcts(kGreenDutyPctsPath);
    if (!greenDutyPcts) {
        LOG(ERROR) << "Failed to open " << kGreenDutyPctsPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream blueDutyPcts(kBlueDutyPctsPath);
    if (!blueDutyPcts) {
        LOG(ERROR) << "Failed to open " << kBlueDutyPctsPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream redStartIdx(kRedStartIdxPath);
    if (!redStartIdx) {
        LOG(ERROR) << "Failed to open " << kRedStartIdxPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream greenStartIdx(kGreenStartIdxPath);
    if (!greenStartIdx) {
        LOG(ERROR) << "Failed to open " << kGreenStartIdxPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream blueStartIdx(kBlueStartIdxPath);
    if (!blueStartIdx) {
        LOG(ERROR) << "Failed to open " << kBlueStartIdxPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream redPauseLo(kRedPauseLoPath);
    if (!redPauseLo) {
        LOG(ERROR) << "Failed to open " << kRedPauseLoPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream greenPauseLo(kGreenPauseLoPath);
    if (!greenPauseLo) {
        LOG(ERROR) << "Failed to open " << kGreenPauseLoPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream bluePauseLo(kBluePauseLoPath);
    if (!bluePauseLo) {
        LOG(ERROR) << "Failed to open " << kBluePauseLoPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream redPauseHi(kRedPauseHiPath);
    if (!redPauseHi) {
        LOG(ERROR) << "Failed to open " << kRedPauseHiPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream greenPauseHi(kGreenPauseHiPath);
    if (!greenPauseHi) {
        LOG(ERROR) << "Failed to open " << kGreenPauseHiPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream bluePauseHi(kBluePauseHiPath);
    if (!bluePauseHi) {
        LOG(ERROR) << "Failed to open " << kBluePauseHiPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream redRampStepMs(kRedRampStepMsPath);
    if (!redRampStepMs) {
        LOG(ERROR) << "Failed to open " << kRedRampStepMsPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream greenRampStepMs(kGreenRampStepMsPath);
    if (!greenRampStepMs) {
        LOG(ERROR) << "Failed to open " << kGreenRampStepMsPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream blueRampStepMs(kBlueRampStepMsPath);
    if (!blueRampStepMs) {
        LOG(ERROR) << "Failed to open " << kBlueRampStepMsPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream redBlink(kRedBlinkPath);
    if (!redBlink) {
        LOG(ERROR) << "Failed to open " << kRedBlinkPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream greenBlink(kGreenBlinkPath);
    if (!greenBlink) {
        LOG(ERROR) << "Failed to open " << kGreenBlinkPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream blueBlink(kBlueBlinkPath);
    if (!blueBlink) {
        LOG(ERROR) << "Failed to open " << kBlueBlinkPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    std::ofstream rgbBlink(kRgbBlinkPath);
    if (!rgbBlink) {
        LOG(ERROR) << "Failed to open " << kRgbBlinkPath << ", error=" << errno
                   << " (" << strerror(errno) << ")";
        return -errno;
    }

    android::sp<ILight> service = new Light(
            {std::move(lcdBacklight), lcdMaxBrightness}, std::move(buttonBacklight),
            std::move(redLed), std::move(greenLed), std::move(blueLed),
            std::move(redDutyPcts), std::move(greenDutyPcts), std::move(blueDutyPcts),
            std::move(redStartIdx), std::move(greenStartIdx), std::move(blueStartIdx),
            std::move(redPauseLo), std::move(greenPauseLo), std::move(bluePauseLo),
            std::move(redPauseHi), std::move(greenPauseHi), std::move(bluePauseHi),
            std::move(redRampStepMs), std::move(greenRampStepMs), std::move(blueRampStepMs),
            std::move(redBlink), std::move(greenBlink), std::move(blueBlink),
            std::move(rgbBlink));

    configureRpcThreadpool(1, true);

    android::status_t status = service->registerAsService();

    if (status != android::OK) {
        LOG(ERROR) << "Cannot register Light HAL service";
        return 1;
    }

    LOG(INFO) << "Light HAL Ready.";
    joinRpcThreadpool();
    // Under normal cases, execution will not reach this line.
    LOG(ERROR) << "Light HAL failed to join thread pool.";
    return 1;
}
