# Copyright (c) 2023 The Brave Authors. All rights reserved.
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this file,
# you can obtain one at https://mozilla.org/MPL/2.0/.
"""A inline part of android_browser_backend_settings.py"""

ANDROID_BRAVE = GenericChromeBackendSettings(browser_type='android-brave',
                                             package='co.asil.browser')

ANDROID_BRAVE_BETA = GenericChromeBackendSettings(
    browser_type='android-brave-beta', package='co.asil.browser_beta')

ANDROID_BRAVE_DEV = GenericChromeBackendSettings(
    browser_type='android-brave-dev', package='co.asil.browser_dev')

ANDROID_BRAVE_NIGHTLY = GenericChromeBackendSettings(
    browser_type='android-brave-nightly', package='co.asil.browser_nightly')

BRAVE_ANDROID_BACKEND_SETTINGS = (ANDROID_BRAVE, ANDROID_BRAVE_BETA,
                                  ANDROID_BRAVE_DEV, ANDROID_BRAVE_NIGHTLY)

# Add brave items to chromium ANDROID_BACKEND_SETTINGS:
ANDROID_BACKEND_SETTINGS = BRAVE_ANDROID_BACKEND_SETTINGS + ANDROID_BACKEND_SETTINGS
