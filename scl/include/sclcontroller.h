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

#include "sclgwes.h"
#include "sclutils.h"
#include "sclcontext.h"

#ifndef __SCL_CONTROLLER_H__
#define __SCL_CONTROLLER_H__

//SCL_BEGIN_DECLS

namespace scl
{
enum SclInternalSignal {
    SCL_SIG_START,
    SCL_SIG_SHOW,
    SCL_SIG_HIDE,
    SCL_SIG_INPMODE_CHANGE,
    SCL_SIG_DISP_CHANGE,
    SCL_SIG_POPUP_SHOW,
    SCL_SIG_POPUP_HIDE,
    SCL_SIG_MOUSE_PRESS,
    SCL_SIG_MOUSE_LONG_PRESS,
    SCL_SIG_MOUSE_RELEASE,
    SCL_SIG_KEYEVENT,
    SCL_SIG_FOCUS_CHANGE,

    SCL_SIG_MAXNUM
};

/**
 * @brief The base class to work as a soft-based keyboard
 *
 * This class implements all functions for working as a soft-based keyboard
 * In side of ISE developer, they can modify it by their requirements.
 */
class CSCLController
{
private:
    CSCLController();
public:
    ~CSCLController();

    static CSCLController* get_instance();
    void init();

private:
    sclboolean process_button_pressed_event(sclwindow window, sclint x, sclint y, sclbyte key_index,
        scltouchdevice touch_id, sclboolean actual_event = TRUE);
    sclboolean process_button_long_pressed_event(sclwindow window, sclbyte key_index,
        scltouchdevice touch_id, sclboolean actual_event = TRUE);
    sclboolean process_button_repeat_pressed_event(sclwindow window, sclbyte key_index,
        scltouchdevice touch_id, sclboolean actual_event = TRUE);
    sclboolean process_button_move_event(sclwindow window, sclint x, sclint y, sclbyte key_index,
        scltouchdevice touch_id, sclboolean actual_event = TRUE);
    sclboolean process_button_over_event(sclwindow window, sclint x, sclint y, sclbyte keyindex);
    sclboolean process_button_release_event(sclwindow window, sclint x, sclint y, sclbyte key_index,
        scltouchdevice touch_id, sclboolean actual_event = TRUE);

    sclboolean configure_autopopup_window(sclwindow window, sclbyte key_index, SclRectangle* rect);

    sclboolean check_event_transition_enabled(const SclLayoutKeyCoordinate *btnFrom, const SclLayoutKeyCoordinate *btnTo);
    sclboolean check_magnifier_available(sclwindow window, sclbyte key_index, scltouchdevice touch_id);

    SCLKeyModifier get_drag_key_modifier(sclint deltax, sclint deltay, sclfloat dist,
        sclboolean check_farthest,scltouchdevice touch_id, sclbyte extra_option);

public:
    void handle_engine_signal(SclInternalSignal signal, sclwindow skipwindow = SCLWINDOW_INVALID);
    sclboolean process_input_mode_change(const sclbyte mode);
    sclboolean process_rotation_change(const SCLRotation rotation);

    sclboolean mouse_press(sclwindow window, sclint x, sclint y, scltouchdevice touch_id = 0, sclboolean actual_event = TRUE);
    sclboolean mouse_release(sclwindow window, sclint x, sclint y, scltouchdevice touch_id = 0, sclboolean actual_event = TRUE);
    sclboolean mouse_move(sclwindow window, sclint x, sclint y, scltouchdevice touch_id = 0, sclboolean actual_event = TRUE);
    sclboolean mouse_over(sclwindow window, sclint x, sclint y);
    sclboolean timer_event(const scl32 id);

    sclboolean set_longkey_duration(scllong msc);
    sclboolean set_longkey_cancel_dist(sclshort dist);
    sclboolean set_repeatkey_duration(scllong msc);
    sclboolean set_autopopup_key_duration(scllong msc);

    sclboolean set_button_delay_duration(scllong msc);

    SCLDebugMode get_debug_mode();
    void set_debug_mode(SCLDebugMode mode);

    void disable_input_events(sclboolean disabled);

protected:
    /* Interval time(msc) for checking long key event */
    scllong m_long_key_duration;
    sclshort m_long_key_cancel_distance;
    scllong m_repeat_key_duration;
    scllong m_autopopup_key_duration;

    scllong m_button_delay_duration;

    SCLDebugMode m_debug_mode;
    sclint m_debug_variable;

    /* Better to move this variable to CSCLContext */
    sclint m_key_repeated_num;

    sclboolean m_input_events_disabled;
};

}

//SCL_END_DECLS

#endif //__SCL_CONTROLLER_H__
