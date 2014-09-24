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

#ifndef __SCL_UIIMPL_H__
#define __SCL_UIIMPL_H__

#include "scltypes.h"
#include "sclstructs.h"
#include "scleventcallback.h"

//SCL_BEGIN_DECLS

namespace scl
{
/**
 * @brief The base class to work as a soft-based keyboard
 *
 * This class implements all functions for working as a soft-based keyboard
 * In side of ISE developer, they can modify it by their requirements.
 */
class CSCLUIImpl
{
private:
    CSCLUIImpl();

public:
    ~CSCLUIImpl();
    static CSCLUIImpl* get_instance();

    sclboolean init(sclwindow parent, SCLParserType parser_type, const char *entry_filepath);
    void fini();

    void show();
    void hide();

    void set_ui_event_callback(ISCLUIEventCallback *callback, const sclchar *input_mode);

    sclboolean set_rotation(SCLRotation degree);
    SCLRotation get_rotation();

    SCLDisplayMode get_display_mode();

    sclboolean set_input_mode(const sclchar *input_mode);
    const sclchar* get_input_mode();
    sclboolean set_popup_input_mode(sclwindow window, const sclchar *input_mode);
    const sclchar* get_popup_input_mode(sclwindow window);

    void set_update_pending(sclboolean pend);

    sclint set_private_key(const sclchar* custom_id, sclchar* label, sclchar* imagelabel[SCL_BUTTON_STATE_MAX], sclchar* imagebg[SCL_BUTTON_STATE_MAX], sclulong key_event, sclchar *key_value, sclboolean redraw);
    void unset_private_key(const sclchar* custom_id);

    void enable_button(const sclchar* custom_id, sclboolean enabled);

    sclboolean set_cur_themename(const sclchar *themename);

    sclboolean set_cur_sublayout(const sclchar *sub_layout_name);
    const sclchar* get_cur_sublayout();

    void set_shift_state(SCLShiftState state);
    SCLShiftState get_shift_state();

    void set_caps_lock_mode(sclboolean mode);
    sclboolean get_caps_lock_mode();

    void notify_app_focus_changed();
    void reset_popup_timeout();
    void close_all_popups();

    SclRectangle get_main_window_rect();
    SclSize get_input_mode_size(const sclchar *input_mode, SCLDisplayMode display_mode);
    void get_screen_resolution(sclint *width, sclint *height);

    scl16 get_scaled_x(scl16 x);
    scl16 get_scaled_y(scl16 y);
    sclfloat get_scale_rate();

    sclfloat get_custom_scale_rate_x();
    sclfloat get_custom_scale_rate_y();
    void set_custom_scale_rate(sclfloat x, sclfloat y);

    void set_custom_starting_coordinates(sclint x, sclint y);
    void set_custom_starting_coordinates_option(SCLStartingCoordinatesOption option);

    sclint get_multi_touch_context_num();
    sclboolean get_multi_touch_event(sclint seqorder, SclUIEventDesc *desc);

    void set_debug_mode(SCLDebugMode mode);
    SCLDebugMode get_debug_mode();

    sclboolean set_longkey_duration(scllong msc);
    sclboolean set_longkey_cancel_dist(sclshort dist);
    sclboolean set_repeatkey_duration(scllong msc);
    sclboolean set_autopoup_key_duration(scllong msc);
    sclboolean set_button_delay_duration(scllong msc);

    void enable_magnifier(sclboolean enabled);
    void enable_sound(sclboolean enabled);
    void enable_vibration(sclboolean enabled);
    void enable_tts(sclboolean enabled);
    void enable_shift_multi_touch(sclboolean enabled);
    void enable_highlight_ui(sclboolean enabled);
    void enable_highlight_ui_animation(sclboolean enabled);

    void enable_touch_offset(sclboolean enabled);
    void disable_input_events(sclboolean disabled);

    void set_custom_magnifier_label(scltouchdevice touch_id, sclint index, const sclchar* label);

    void set_string_substitution(const sclchar *original, const sclchar *substitute);
    void unset_string_substitution(const sclchar *original);
    sclboolean get_autocapital_shift_state();
    void set_autocapital_shift_state(sclboolean flag);

    sclboolean process_key_event(const char *key);

private:
    sclboolean m_initialized;
    sclboolean m_autocapital_shift_state;
};

}

//SCL_END_DECLS

#endif //__SCL_UIIMPL_H__
