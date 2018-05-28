# Copyright (C) 2009 The Android Open Source Project
# Copyright (c) 2011, The Linux Foundation. All rights reserved.
# Copyright (C) 2017-2018 The LineageOS Project
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

import hashlib
import common
import re

def FullOTA_Assertions(info):
  AddBasebandAssertion(info, info.input_zip)
  return

def IncrementalOTA_Assertions(info):
  AddBasebandAssertion(info, info.target_zip)
  return

def AddBasebandAssertion(info, input_zip):
  android_info = input_zip.read("OTA/android-info.txt")
  m = re.search(r'require\s+version-baseband\s*=\s*(.+)', android_info)
  if m:
    timestamp, firmware_version = m.group(1).rstrip().split(',')
    if ((len(timestamp) and '*' not in timestamp) and \
        (len(firmware_version) and '*' not in firmware_version)):
      cmd = 'assert(oneplus2.verify_baseband("{}") == "1" || abort("ERROR: This package requires firmware from OOS {} or newer. Please upgrade firmware and retry!"););'
      info.script.AppendExtra(cmd.format(timestamp, firmware_version))
  return
