/*
 * Copyright 2012-2013 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Flora License, Version 1.1 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "sclfeedback.h"
#include "scldebug.h"
#include "sclresourcecache.h"
#include "sclutils.h"
#include "sclcontext.h"

using namespace scl;

CSCLFeedback* CSCLFeedback::get_instance()
{
    static CSCLFeedback instance;
    return &instance;
}

CSCLFeedback::CSCLFeedback()
{
    SCL_DEBUG();
}

CSCLFeedback::~CSCLFeedback()
{
    SCL_DEBUG();
}

sclboolean
CSCLFeedback::show_feedback(SCLFeedbackStyle style)
{
    SCL_DEBUG();
    CSCLUtils *utils = CSCLUtils::get_instance();
    CSCLContext *context = CSCLContext::get_instance();

    if (utils && context) {
        switch (style) {
            case FEEDBACK_STYLE_SOUND:
                if (context->get_sound_enabled()) {
                    utils->play_sound(DEFAULT_SOUND_STYLE);
                }
                break;
            case FEEDBACK_STYLE_VIBRATION:
                if (context->get_vibration_enabled()) {
                    utils->play_vibration(DEFAULT_VIBRATION_STYLE, DEFAULT_VIBRATION_DURATION);
                }
                break;
            default:
                break;
        }
    }
    return TRUE;
}

sclboolean
CSCLFeedback::button_pressed(sclwindow window, sclbyte key_index)
{
    SCL_DEBUG();
    CSCLResourceCache *cache = CSCLResourceCache::get_instance();
    if (cache) {
        const SclLayoutKeyCoordinate *coordinate = cache->get_cur_layout_key_coordinate(window, key_index);
        CSCLUtils *utils = CSCLUtils::get_instance();
        CSCLContext *context = CSCLContext::get_instance();
        if (utils && context && coordinate) {
            if (coordinate->sound_style) {
                if (context->get_sound_enabled()) {
                    utils->play_sound(coordinate->sound_style);
                }
            }
            if (coordinate->vibe_style) {
                if (context->get_vibration_enabled()) {
                    utils->play_vibration(coordinate->vibe_style, DEFAULT_VIBRATION_DURATION);
                }
            }
        }
    }
    return TRUE;
}

sclboolean
CSCLFeedback::button_moved(sclwindow window, sclbyte key_index)
{
    SCL_DEBUG();
    return TRUE;
}

sclboolean
CSCLFeedback::button_released(sclwindow window, sclbyte key_index)
{
    SCL_DEBUG();
    return TRUE;
}

