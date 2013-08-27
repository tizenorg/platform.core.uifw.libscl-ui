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

#include "sclui.h"
#include "scluiimpl.h"

using namespace scl;

CSCLUI::CSCLUI()
{
    m_impl = CSCLUIImpl::get_instance();
}

CSCLUI::~CSCLUI()
{
    delete m_impl;
}

sclboolean CSCLUI::init(sclwindow parent, const SCLParserType parser_type, const char *entry_filepath)
{
    sclboolean ret = FALSE;
    if (m_impl) {
        ret = m_impl->init(parent, parser_type, entry_filepath);
    }
    return ret;
}

/**
 * Shows the SCL main window
 * For displaying the SCL UI, you should explicitly call this function after CSCLUI class is created
 */
void
//CSCLUI::show(sclboolean auto_relocate /* = TRUE */ )
CSCLUI::show()
{
    if (m_impl) {
        m_impl->show();
    }
}

/**
 * Hides the SCL main window
 * The real hide action does not work about base window because that is child of the active window
 */
void
CSCLUI::hide()
{
    if (m_impl) {
        m_impl->hide();
    }
}


/**
 * Regists an event callback function
 * so that the user which uses SCL can recevies all events occuring in running
 */
void
CSCLUI::set_ui_event_callback(ISCLUIEventCallback *callback, const sclchar *input_mode)
{
    if (m_impl) {
        m_impl->set_ui_event_callback(callback, input_mode);
    }
}

/**
 * Sets the current input mode to the given mode
 * @Usage
 * gCore->set_input_mode("INPUT_MODE_SYMBOL");
 */
sclboolean
CSCLUI::set_input_mode(const sclchar *input_mode)
{
    sclboolean ret = FALSE;

    if (m_impl) {
        ret = m_impl->set_input_mode(input_mode);
    }

    return ret;
}

/**
 * Sets the current rotation
 */
sclboolean
CSCLUI::set_rotation(SCLRotation rotation)
{
    sclboolean ret = FALSE;

    if (m_impl) {
        ret = m_impl->set_rotation(rotation);
    }

    return ret;
}

/**
 * Gets the current rotation
 */
SCLRotation
CSCLUI::get_rotation()
{
    SCLRotation ret = ROTATION_0;

    if (m_impl) {
        ret = m_impl->get_rotation();
    }

    return ret;
}

/**
 * Returns the current display mode
 */
SCLDisplayMode
CSCLUI::get_display_mode()
{
    SCLDisplayMode ret = DISPLAYMODE_PORTRAIT;

    if (m_impl) {
        ret = m_impl->get_display_mode();
    }

    return ret;
}

/**
 * Returns the current input mode
 */
const sclchar*
CSCLUI::get_input_mode()
{
    const sclchar *ret = NULL;

    if (m_impl) {
        ret = m_impl->get_input_mode();
    }

    return ret;
}



/**
 * Sets a private key to the current context
 * The other properties except given parameters will keep to the orginal value.
 * @Usage
 * gCore->set_private_key(INPUT_MODE_NUMBER, LYT_PORTRAIT_NOW_3x4, 0, "private", 999, TRUE);
 *
 * @param fRedraw If true, it will redraw the current key
 */
sclint
CSCLUI::set_private_key(const sclchar* custom_id, sclchar* label, sclchar* imagelabel[SCL_BUTTON_STATE_MAX], sclchar* imagebg[SCL_BUTTON_STATE_MAX], sclulong key_event, sclchar *key_value, sclboolean redraw)
{
    sclint ret = NOT_USED;

    if (m_impl) {
        ret = m_impl->set_private_key(custom_id, label, imagelabel, imagebg, key_event, key_value, redraw);
    }

    return ret;
}


/**
* Unsets a private key to the current context
*/
void
CSCLUI::unset_private_key(const sclchar* custom_id)
{
    if (m_impl) {
        m_impl->unset_private_key(custom_id);
    }
}

/**
* Sets the current theme
*/
/* FIXME : If setting themename is not allowed before initializing,
           the default theme has to be loaded regardless of current theme name
           and the appropriate current theme has to be loaded afterwards, which is very inefficient */
sclboolean
CSCLUI::set_cur_themename(const sclchar *themename)
{
    sclboolean ret = FALSE;

    if (m_impl) {
        ret = m_impl->set_cur_themename(themename);
    }

    return ret;
}

SCLShiftState
CSCLUI::get_shift_state()
{
    SCLShiftState ret = SCL_SHIFT_STATE_OFF;
    if (m_impl) {
        ret = m_impl->get_shift_state();
    }
    return ret;
}

void
CSCLUI::set_shift_state(SCLShiftState state)
{
    if (m_impl) {
        m_impl->set_shift_state(state);
    }
}

/**
 * This function will be called by the user which uses SCL when the context of the focus application is changed
 * ISE user should explicitly call this function when the context of application is changed.
 * For instance, focus-changed, application-changed,, and so on.
 * This function will call CSCLController to init the related variables.
 */
void
CSCLUI::notify_app_focus_changed()
{
    if (m_impl) {
        m_impl->notify_app_focus_changed();
    }
}

void
CSCLUI::reset_popup_timeout()
{
    if (m_impl) {
        m_impl->notify_app_focus_changed();
    }
}

void
CSCLUI::close_all_popups()
{
    if (m_impl) {
        m_impl->close_all_popups();
    }
}

/**
 * Returns a scale rate (see default screen resolution in sclconfig.h file)
 */
sclfloat
CSCLUI::get_scale_rate()
{
    sclfloat ret = 0.0f;
    if (m_impl) {
        ret = m_impl->get_scale_rate();
    }
    return ret;
}

/**
 * Returns a calculated x value according to the current screen resolution
 */
scl16
CSCLUI::get_scaled_x(scl16 x)
{
    scl16 ret = 0;
    if (m_impl) {
        ret = m_impl->get_scaled_x(x);
    }
    return ret;
}

/**
 * Returns a calculated y value according to the current screen resolution
 */
scl16
CSCLUI::get_scaled_y(scl16 y)
{
    scl16 ret = 0;
    if (m_impl) {
        ret = m_impl->get_scaled_y(y);
    }
    return ret;
}

/**
 * Returns the current rate x value for resizing the keyboard's content
 */
sclfloat
CSCLUI::get_custom_scale_rate_x()
{
    sclfloat ret = 1.0f;
    if (m_impl) {
        ret = m_impl->get_custom_scale_rate_x();
    }
    return ret;
}

/**
 * Returns the current rate y value for resizing the keyboard's content
 */
sclfloat
CSCLUI::get_custom_scale_rate_y()
{
    sclfloat ret = 1.0f;
    if (m_impl) {
        ret = m_impl->get_custom_scale_rate_y();
    }
    return ret;
}

/**
 * Sets the custom rate value for resizing the keyboard's content
 */
void
CSCLUI::set_custom_scale_rate(sclfloat x, sclfloat y)
{
    if (m_impl) {
        m_impl->set_custom_scale_rate(x, y);
    }
}

/**
 * Returns the scl main window size
 */
SclRectangle
CSCLUI::get_main_window_rect()
{
    SclRectangle ret = {0};

    if (m_impl) {
        ret = m_impl->get_main_window_rect();
    }

    return ret;
}

/**
 * Returns the size of given input mode
 */
SclSize
CSCLUI::get_input_mode_size(const sclchar *input_mode, SCLDisplayMode display_mode)
{
    SclSize ret = {0};

    if (m_impl) {
        ret = m_impl->get_input_mode_size(input_mode, display_mode);
    }

    return ret;
}

/**
* Returns the screen resolution
*/
void
CSCLUI::get_screen_resolution(sclint *width, sclint *height)
{
    if (m_impl) {
        m_impl->get_screen_resolution(width, height);
    }
}



void
CSCLUI::set_debug_mode(SCLDebugMode mode)
{
    if (m_impl) {
        m_impl->set_debug_mode(mode);
    }
}

SCLDebugMode
CSCLUI::get_debug_mode()
{
    SCLDebugMode ret = DEBUGMODE_DISABLED;
    if (m_impl) {
        ret = m_impl->get_debug_mode();
    }
    return ret;
}

void
CSCLUI::set_update_pending(sclboolean pend)
{
    if (m_impl) {
        m_impl->set_update_pending(pend);
    }
}

void
CSCLUI::enable_button(const sclchar* custom_id, sclboolean enabled)
{
    if (m_impl) {
        m_impl->enable_button(custom_id, enabled);
    }
}

sclint
CSCLUI::get_multi_touch_context_num()
{
    sclint ret = 0;
    if (m_impl) {
        ret = m_impl->get_multi_touch_context_num();
    }
    return ret;
}

sclboolean
CSCLUI::get_multi_touch_event(sclint seqorder, SclUIEventDesc *desc)
{
    sclboolean ret = FALSE;
    if (m_impl) {
        ret = m_impl->get_multi_touch_event(seqorder, desc);
    }
    return ret;
}

sclboolean
CSCLUI::set_longkey_duration(scllong msc)
{
    sclboolean ret = FALSE;
    if (m_impl) {
        ret = m_impl->set_longkey_duration(msc);
    }
    return ret;
}

sclboolean
CSCLUI::set_longkey_cancel_dist(sclshort dist)
{
    sclboolean ret = FALSE;
    if (m_impl) {
        ret = m_impl->set_longkey_cancel_dist(dist);
    }
    return ret;
}

sclboolean
CSCLUI::set_repeatkey_duration(scllong msc)
{
    sclboolean ret = FALSE;
    if (m_impl) {
        ret = m_impl->set_repeatkey_duration(msc);
    }
    return ret;
}

sclboolean
CSCLUI::set_autopoup_key_duration(scllong msc)
{
    sclboolean ret = FALSE;
    if (m_impl) {
        ret = m_impl->set_autopoup_key_duration(msc);
    }
    return ret;
}

sclboolean
CSCLUI::set_button_delay_duration(scllong msc)
{
    sclboolean ret = FALSE;
    if (m_impl) {
        ret = m_impl->set_button_delay_duration(msc);
    }
    return ret;
}

void
CSCLUI::enable_magnifier(sclboolean enabled)
{
    if (m_impl) {
        m_impl->enable_magnifier(enabled);
    }
}

void
CSCLUI::enable_sound(sclboolean enabled)
{
    if (m_impl) {
        m_impl->enable_sound(enabled);
    }
}

void
CSCLUI::enable_vibration(sclboolean enabled)
{
    if (m_impl) {
        m_impl->enable_vibration(enabled);
    }
}

void
CSCLUI::enable_tts(sclboolean enabled)
{
    if (m_impl) {
        m_impl->enable_tts(enabled);
    }
}

void
CSCLUI::enable_shift_multi_touch(sclboolean enabled)
{
    if (m_impl) {
        m_impl->enable_shift_multi_touch(enabled);
    }
}

void
CSCLUI::enable_touch_offset(sclboolean enabled)
{
    if (m_impl) {
        m_impl->enable_touch_offset(enabled);
    }
}

void
CSCLUI::disable_input_events(sclboolean disabled)
{
    if (m_impl) {
        m_impl->disable_input_events(disabled);
    }
}

sclboolean
CSCLUI::set_cur_sublayout(const sclchar *sub_layout_name)
{
    sclboolean ret = FALSE;
    if (m_impl) {
        ret = m_impl->set_cur_sublayout(sub_layout_name);
    }
    return ret;
}

const sclchar*
CSCLUI::get_cur_sublayout()
{
    const sclchar* ret = NULL;
    if (m_impl) {
        ret = m_impl->get_cur_sublayout();
    }
    return ret;
}

void
CSCLUI::set_custom_magnifier_label(scltouchdevice touch_id, sclint index, const sclchar* label)
{
    if (m_impl) {
        m_impl->set_custom_magnifier_label(touch_id, index, label);
    }
}

void
CSCLUI::set_string_substitution(const sclchar *original, const sclchar *substitute)
{
    if (m_impl) {
        m_impl->set_string_substitution(original, substitute);
    }
}

void
CSCLUI::unset_string_substitution(const sclchar *original)
{
    if (m_impl) {
        m_impl->unset_string_substitution(original);
    }
}

void
CSCLUI::set_autocapital_shift_state(sclboolean flag) {
    if (m_impl) {
        m_impl->set_autocapital_shift_state(flag);
    }
}

sclint
CSCLUI::get_autocapital_shift_state() {
    sclint flag = 0;

    if (m_impl) {
        flag = m_impl->get_autocapital_shift_state();
    }

    return flag;
}
