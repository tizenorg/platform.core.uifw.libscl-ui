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

#ifndef __SCL_CONTEXT_H__
#define __SCL_CONTEXT_H__

//SCL_BEGIN_DECLS
#include <sys/time.h>
#include "scltypes.h"
#include "sclconfig.h"
#include "sclstructs.h"
#include "scleventcallback.h"
#include <map>
#include <list>
#include <string>

namespace scl
{

typedef struct { // A context information dependant on each multitouch events
    sclboolean used;

    scl8 cur_pressed_key;
    sclwindow cur_pressed_window;
    SclPoint cur_pressed_point;
    struct timeval cur_pressed_time;

    sclwindow cur_move_window;
    SclPoint cur_move_point;

    SCLDragState cur_drag_state;
    SCLKeyModifier cur_key_modifier;

    SclPoint farthest_move_point;
    sclint farthest_move_dist;

    scl8 prev_pressed_key;
    sclwindow prev_pressed_window;

    SclPoint prev_move_point;
    SCLDragState prev_drag_state;

    scl16 event_id;

    sclboolean is_sub_event;
} MultiTouchContext;

typedef struct { // A struct for identifying Magnifier custom label information
    scltouchdevice touch_id;
    sclint index;
} MagnifierCustomLabelIdx;
class MagnifierCustomLabelIdxCompare { // Comparison class for MagnifierCustomLabelIdx
public:
    bool operator()(const MagnifierCustomLabelIdx x ,const MagnifierCustomLabelIdx y) {
        if (x.touch_id != y.touch_id) {
            return x.touch_id > y.touch_id;
        } else {
            return x.index > y.index;
        }
    }
};
typedef std::map<MagnifierCustomLabelIdx, std::string, MagnifierCustomLabelIdxCompare> MagnifierCusomLabelIdxMap;


/**
 * @brief The base class to work as a soft-based keyboard
 *
 * This class implements all functions for working as a soft-based keyboard
 * In side of ISE developer, they can modify it by their requirements.
 */
class CSCLContext
{
private:
    CSCLContext();
public:
    ~CSCLContext();

    /* reset to the default context status */
    void reset();

    /* for singleton */
    static CSCLContext* get_instance();

    SCLDisplayMode get_display_mode() const {
        return m_display_mode;
    }
    void set_display_mode(SCLDisplayMode val) {
        m_prev_display_mode = m_display_mode;
        m_display_mode = val;
    }

    scl8 get_input_mode() const {
        return m_input_mode;
    }
    void set_input_mode(scl8 val) {
        m_prev_input_mode = m_input_mode;
        m_input_mode = val;
    }

    sclshort get_base_layout() const;
    void set_base_layout(sclshort val);

    sclshort get_popup_layout(sclwindow window) const;
    void set_popup_layout(sclwindow window, sclshort val);

    SCLShiftState get_shift_state() const;
    void set_shift_state(SCLShiftState val);

    SCLShiftMultitouchState get_shift_multi_touch_state() const;
    void set_shift_multi_touch_state(SCLShiftMultitouchState val);

    SCLDisplayMode get_prev_display_mode() const {
        return m_prev_display_mode;
    }
    scl8 get_prev_inputmode() const {
        return m_prev_input_mode;
    }

    sclboolean get_hidden_state() const {
        return m_hidden;
    }
    void set_hidden_state(sclboolean val) {
        m_hidden = val;
    }

    SCLRotation get_rotation() const {
        return m_rotation;
    }
    void set_rotation(SCLRotation val) {
        m_rotation = val;
    }

    void set_magnifier_enabled(sclboolean enabled) {
        m_magnifier_enabled = enabled;
    }
    sclboolean get_magnifier_enabled() {
        return m_magnifier_enabled;
    }
    void set_sound_enabled(sclboolean enabled) {
        m_sounce_enabled = enabled;
    }
    sclboolean get_sound_enabled() {
        return m_sounce_enabled;
    }
    void set_vibration_enabled(sclboolean enabled) {
        m_vibration_enabled = enabled;
    }
    sclboolean get_vibration_enabled() {
        return m_vibration_enabled;
    }
    void set_shift_multi_touch_enabled(sclboolean enabled) {
        m_shift_multi_touch_enabled = enabled;
    }
    sclboolean get_shift_multi_touch_enabled() {
        return m_shift_multi_touch_enabled;
    }

    scl8 get_last_pressed_key() {
        return m_last_pressed_key;
    }
    void set_last_pressed_key(scl8 val) {
        m_last_pressed_key = val;
    }
    sclwindow get_last_pressed_window() {
        return m_last_pressed_window;
    }
    void set_last_pressed_window(sclwindow window) {
        m_last_pressed_window = window;
    }

    scltouchdevice get_last_touch_device_id() const {
        return m_last_touch_device_id;
    }
    void set_last_touch_device_id(scltouchdevice touch_id) {
        m_last_touch_device_id = touch_id;
    }

    void set_tts_enabled(sclboolean enabled) {
        m_tts_enabled = enabled;
    }

    sclboolean get_tts_enabled() {
        return m_tts_enabled;
    }

    void set_cur_highlighted_key(scl8 val) {
        m_cur_highlighted_key = val;
    }

    scl8 get_cur_highlighted_key() {
        return m_cur_highlighted_key;
    }

    void set_cur_highlighted_window(sclwindow window) {
        m_cur_highlighted_window = window;
    }

    sclwindow get_cur_highlighted_window() {
        return m_cur_highlighted_window;
    }

    void create_multi_touch_context(scltouchdevice touch_id, sclboolean isSubEvent = FALSE);
    void destroy_multi_touch_context(scltouchdevice touch_id);
    MultiTouchContext* find_multi_touch_context(scltouchdevice touch_id);

    scl16 get_cur_pressed_event_id(scltouchdevice touch_id);
    void set_cur_pressed_event_id(scltouchdevice touch_id, scl16 id);
    scl8 get_cur_pressed_key(scltouchdevice touch_id);
    void set_cur_pressed_key(scltouchdevice touch_id, scl8 val);
    sclwindow get_cur_pressed_window(scltouchdevice touch_id);
    void set_cur_pressed_window(scltouchdevice touch_id, sclwindow window);
    SclPoint get_cur_pressed_point(scltouchdevice touch_id);
    void set_cur_pressed_point(scltouchdevice touch_id, sclint x, sclint y);
    struct timeval get_cur_pressed_time(scltouchdevice touch_id);
    void set_cur_pressed_time(scltouchdevice touch_id);

    void set_custom_magnifier_label(scltouchdevice touch_id, sclint index, const sclchar* label);
    const sclchar* get_custom_magnifier_label(scltouchdevice touch_id, sclint index);

    SclPoint get_cur_move_point(scltouchdevice touch_id);
    void set_cur_move_point(scltouchdevice touch_id, sclint x, sclint y);
    sclwindow get_cur_move_window(scltouchdevice touch_id);
    void set_cur_move_window(scltouchdevice touch_id, sclwindow window);
    SclPoint get_farthest_move_point(scltouchdevice touch_id);
    void set_farthest_move_point(scltouchdevice touch_id, sclint x, sclint y);
    sclint get_farthest_move_dist(scltouchdevice touch_id);

    SCLDragState get_cur_drag_state(scltouchdevice touch_id);
    void set_cur_drag_state(scltouchdevice touch_id, SCLDragState state);
    SCLKeyModifier get_cur_key_modifier(scltouchdevice touch_id);
    void set_cur_key_modifier(scltouchdevice touch_id, SCLKeyModifier modifier);

    scl8 get_prev_pressed_key(scltouchdevice touch_id);
    void set_prev_pressed_key(scltouchdevice touch_id, scl8 val);
    sclwindow get_prev_pressed_window(scltouchdevice touch_id);
    void set_prev_pressed_window(scltouchdevice touch_id, sclwindow window);

    SclPoint get_prev_move_point(scltouchdevice touch_id);
    void set_prev_move_point(scltouchdevice touch_id, sclint x, sclint y);
    SCLDragState get_prev_drag_state(scltouchdevice touch_id);
    void set_prev_drag_state(scltouchdevice touch_id, SCLDragState state);

    sclint get_multi_touch_context_num();
    sclboolean get_multi_touch_event(sclint seqorder, SclUIEventDesc *desc);
    sclint get_multi_touch_event_order(scltouchdevice touch_id);

    sclchar* get_cur_sublayout();
    sclboolean set_cur_sublayout(const sclchar* sublayout);

protected:
    /* stores the current context */
    SCLDisplayMode m_display_mode;
    scl8 m_input_mode;

    /* stores the last context */
    SCLDisplayMode m_prev_display_mode;
    scl8 m_prev_input_mode;

    sclboolean m_hidden;
    SCLShiftState m_shift_state;
    SCLShiftMultitouchState m_shift_multi_touch_state;

    sclchar m_cur_sub_layout[MAX_SIZE_OF_SUBLAYOUT_STRING];

    SCLRotation m_rotation;

    sclboolean m_magnifier_enabled;
    sclboolean m_sounce_enabled;
    sclboolean m_vibration_enabled;
    sclboolean m_shift_multi_touch_enabled;

    sclwindow m_last_pressed_window;
    scl8 m_last_pressed_key;

    sclboolean m_tts_enabled;
    scl8 m_cur_highlighted_key;
    sclwindow m_cur_highlighted_window;

    scltouchdevice m_last_touch_device_id;
    std::map<scltouchdevice, MultiTouchContext> m_multi_touch_context;
    std::list<scltouchdevice> m_multi_touch_seq;

    MagnifierCusomLabelIdxMap m_custom_magnifier_label;
};

}

//SCL_END_DECLS

#endif //__SCL_CONTEXT_H__
