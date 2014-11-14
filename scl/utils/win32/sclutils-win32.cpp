/*
 * Copyright (c) 2012 - 2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <stdarg.h>

#include "sclutils.h"
#include "scldebug.h"

using namespace scl;

void
CSCLUtilsImplWin32::init() {
    SCL_DEBUG();
    open_devices();
}

void
CSCLUtilsImplWin32::fini() {
    SCL_DEBUG();
    close_devices();
}

sclboolean
CSCLUtilsImplWin32::get_screen_resolution(sclint *x, sclint *y) {
    SCL_DEBUG();
    return TRUE;
}

sclboolean
CSCLUtilsImplWin32::play_tts(const sclchar* str) {
    SCL_DEBUG();
    return TRUE;
}

sclboolean
CSCLUtilsImplWin32::play_sound(const sclchar* sndStyle) {
    SCL_DEBUG();
    return TRUE;
}

sclboolean
CSCLUtilsImplWin32::play_vibration(const sclchar* vibeStyle, const scllong duration) {
    SCL_DEBUG();
    return TRUE;
}

sclboolean
CSCLUtilsImplWin32::open_devices() {
    SCL_DEBUG();
    return TRUE;
}

sclboolean
CSCLUtilsImplWin32::close_devices() {
    SCL_DEBUG();
    return TRUE;
}

sclint CSCLUtilsImplWin32::log(const sclchar *msg)
{
    SCL_DEBUG();
    return 0;
}
