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

#include <stdio.h>
#include <stdlib.h>

#include "sclcontext.h"
#include "scldebug.h"
#include "sclwindows.h"
#include "sclutils.h"
#include "sclresourcecache.h"
#include "sclres.h"
#include <algorithm>

//#include "sclresource.h"

using namespace scl;

CSCLContext::CSCLContext()
{
    SCL_DEBUG();

    reset();
}

CSCLContext::~CSCLContext()
{
    SCL_DEBUG();
}

CSCLContext*
CSCLContext::get_instance()
{
    static CSCLContext instance;
    return &instance;
}

void
CSCLContext::reset()
{
    SCL_DEBUG();

    m_display_mode = DISPLAYMODE_PORTRAIT;
    m_input_mode = 0;

    m_shift_state = SCL_SHIFT_STATE_OFF;
    m_shift_multi_touch_state = SCL_SHIFT_MULTITOUCH_OFF;

    m_rotation = ROTATION_0;
    m_prev_display_mode = DISPLAYMODE_PORTRAIT;
    m_prev_input_mode = NOT_USED;

    m_magnifier_enabled = TRUE;
    m_sounce_enabled = TRUE;
    m_vibration_enabled = TRUE;
    m_shift_multi_touch_enabled = TRUE;
#ifdef _TV
    m_highlight_ui_enabled = TRUE;
#else
    m_highlight_ui_enabled = FALSE;
#endif
    m_highlight_ui_animation_enabled = FALSE;

    m_tts_enabled = FALSE;

    m_multi_touch_context.clear();
    m_last_touch_device_id = SCLTOUCHDEVICE_INVALID;

    m_last_pressed_window = SCLWINDOW_INVALID;
    m_last_pressed_key = NOT_USED;

    memset(m_cur_sub_layout, 0x00, sizeof(m_cur_sub_layout));
}

sclshort
CSCLContext::get_popup_layout( sclwindow window ) const
{
    SCL_DEBUG();

    sclshort ret = NOT_USED;
    CSCLWindows *windows = CSCLWindows::get_instance();
    //SclWindowContext *window_context = windows->get_window_context(window, FALSE);
    SclWindowContext *window_context = windows->get_window_context(window);

    if (window_context) {
        ret = window_context->layout;
    }

    return ret;
}

void
CSCLContext::set_popup_layout( sclwindow window, sclshort val )
{
    SCL_DEBUG();

    CSCLWindows *windows = CSCLWindows::get_instance();
    //SclWindowContext *window_context = windows->get_window_context(window, FALSE);
    SclWindowContext *window_context = windows->get_window_context(window);

    if (window_context) {
        window_context->layout = val;
    }
}

void
CSCLContext::set_base_layout( sclshort val )
{
    SCL_DEBUG();
    CSCLWindows *windows = CSCLWindows::get_instance();
    //SclWindowContext *window_context = windows->get_window_context(windows->get_base_window(), FALSE);
    SclWindowContext *window_context = windows->get_window_context(windows->get_base_window());
    if (window_context) {
        window_context->layout = val;
    }
}

sclshort
CSCLContext::get_base_layout() const
{
    SCL_DEBUG();

    sclshort ret = NOT_USED;
    CSCLWindows *windows = CSCLWindows::get_instance();
    //SclWindowContext *window_context = windows->get_window_context(windows->get_base_window(), FALSE);
    SclWindowContext *window_context = windows->get_window_context(windows->get_base_window());
    if (window_context) {
        ret = window_context->layout;
    }

    return ret;
}

scl16
CSCLContext::get_cur_pressed_event_id(scltouchdevice touch_id)
{
    scl16 ret = NOT_USED;
    MultiTouchContext *multi_touch_context = find_multi_touch_context(touch_id);
    if (multi_touch_context) {
        ret = multi_touch_context->event_id;
    }
    return ret;
}

void
CSCLContext::set_cur_pressed_event_id(scltouchdevice touch_id, scl16 id)
{
    MultiTouchContext *multi_touch_context = find_multi_touch_context(touch_id);
    if (multi_touch_context) {
        multi_touch_context->event_id = id;
    }
}

scl8
CSCLContext::get_cur_pressed_key(scltouchdevice touch_id)
{
    scl8 ret = NOT_USED;
    MultiTouchContext *multi_touch_context = find_multi_touch_context(touch_id);
    if (multi_touch_context) {
        ret = multi_touch_context->cur_pressed_key;
    }
    return ret;
}

void
CSCLContext::set_cur_pressed_key(scltouchdevice touch_id, scl8 val)
{
    MultiTouchContext *window_context = find_multi_touch_context(touch_id);
    if (window_context) {
        window_context->cur_pressed_key = val;
    }
}

sclwindow
CSCLContext::get_cur_pressed_window(scltouchdevice touch_id)
{
    sclwindow ret = SCLWINDOW_INVALID;
    MultiTouchContext *multi_touch_context = find_multi_touch_context(touch_id);
    if (multi_touch_context) {
        ret = multi_touch_context->cur_pressed_window;
    }
    return ret;
}

void
CSCLContext::set_cur_pressed_window(scltouchdevice touch_id, sclwindow val)
{
    MultiTouchContext *multi_touch_context = find_multi_touch_context(touch_id);
    if (multi_touch_context) {
        multi_touch_context->cur_pressed_window = val;
    }
}

SclPoint
CSCLContext::get_cur_pressed_point(scltouchdevice touch_id)
{
    SclPoint ret = {0, 0};
    MultiTouchContext *multi_touch_context = find_multi_touch_context(touch_id);
    if (multi_touch_context) {
        ret = multi_touch_context->cur_pressed_point;
    }
    return ret;
}

void
CSCLContext::set_cur_pressed_point(scltouchdevice touch_id, sclint x, sclint y)
{
    MultiTouchContext *multi_touch_context = find_multi_touch_context(touch_id);
    if (multi_touch_context) {
        multi_touch_context->cur_pressed_point.x = x;
        multi_touch_context->cur_pressed_point.y = y;
        set_farthest_move_point(touch_id, x, y); // reset farthest move point
    }
}

SclPoint
CSCLContext::get_cur_moving_point(scltouchdevice touch_id)
{
    SclPoint ret = {0, 0};
    MultiTouchContext *multi_touch_context = find_multi_touch_context(touch_id);
    if (multi_touch_context) {
        ret = multi_touch_context->cur_moving_point;
    }
    return ret;
}

void
CSCLContext::set_cur_moving_point(scltouchdevice touch_id, sclint x, sclint y)
{
    MultiTouchContext *multi_touch_context = find_multi_touch_context(touch_id);
    if (multi_touch_context) {
        multi_touch_context->cur_moving_point.x = x;
        multi_touch_context->cur_moving_point.y = y;
    }
}

sclwindow
CSCLContext::get_cur_moving_window(scltouchdevice touch_id)
{
    sclwindow ret = SCLWINDOW_INVALID;
    MultiTouchContext *multi_touch_context = find_multi_touch_context(touch_id);
    if (multi_touch_context) {
        ret = multi_touch_context->cur_moving_window;
    }
    return ret;
}


void
CSCLContext::set_cur_moving_window(scltouchdevice touch_id, sclwindow window)
{
    MultiTouchContext *window_context = find_multi_touch_context(touch_id);
    if (window_context) {
        window_context->cur_moving_window = window;
    }
}

struct timeval
CSCLContext::get_cur_pressed_time(scltouchdevice touch_id)
{
    struct timeval ret = {0,0};
    MultiTouchContext *multi_touch_context = find_multi_touch_context(touch_id);
    if (multi_touch_context) {
        ret = multi_touch_context->cur_pressed_time;
    }
    return ret;
}

void
CSCLContext::set_cur_pressed_time(scltouchdevice touch_id)
{
    MultiTouchContext *multi_touch_context = find_multi_touch_context(touch_id);
    if (multi_touch_context) {
        gettimeofday(&(multi_touch_context->cur_pressed_time), NULL);
    }
}

SclPoint
CSCLContext::get_farthest_move_point(scltouchdevice touch_id)
{
    SclPoint ret = {0, 0};
    MultiTouchContext *multi_touch_context = find_multi_touch_context(touch_id);
    if (multi_touch_context) {
        ret = multi_touch_context->farthest_move_point;
    }
    return ret;
}

void
CSCLContext::set_farthest_move_point(scltouchdevice touch_id, sclint x, sclint y)
{
    MultiTouchContext *multi_touch_context = find_multi_touch_context(touch_id);
    if (multi_touch_context) {
        CSCLUtils *utils = CSCLUtils::get_instance();
        multi_touch_context->farthest_move_dist =
            utils->get_approximate_distance(x, y,
            multi_touch_context->cur_pressed_point.x, multi_touch_context->cur_pressed_point.y);

        multi_touch_context->farthest_move_point.x = x;
        multi_touch_context->farthest_move_point.y = y;
    }
}

sclint
CSCLContext::get_farthest_move_dist(scltouchdevice touch_id)
{
    sclint ret = 0;
    MultiTouchContext *multi_touch_context = find_multi_touch_context(touch_id);
    if (multi_touch_context) {
        ret = multi_touch_context->farthest_move_dist;
    }
    return ret;
}

SCLDragState
CSCLContext::get_cur_drag_state(scltouchdevice touch_id)
{
    SCLDragState ret = SCL_DRAG_STATE_NONE;
    MultiTouchContext *multi_touch_context = find_multi_touch_context(touch_id);
    if (multi_touch_context) {
        ret = multi_touch_context->cur_drag_state;
    }
    return ret;
}

void
CSCLContext::set_cur_drag_state(scltouchdevice touch_id, SCLDragState state)
{
    MultiTouchContext *multi_touch_context = find_multi_touch_context(touch_id);
    if (multi_touch_context) {
        multi_touch_context->cur_drag_state = state;
    }
}

SCLKeyModifier
CSCLContext::get_cur_key_modifier(scltouchdevice touch_id)
{
    SCLKeyModifier ret = KEY_MODIFIER_NONE;
    MultiTouchContext *multi_touch_context = find_multi_touch_context(touch_id);
    if (multi_touch_context) {
        ret = multi_touch_context->cur_key_modifier;
    }
    return ret;
}

void
CSCLContext::set_cur_key_modifier(scltouchdevice touch_id, SCLKeyModifier modifier)
{
    MultiTouchContext *multi_touch_context = find_multi_touch_context(touch_id);
    if (multi_touch_context) {
        multi_touch_context->cur_key_modifier = modifier;
    }
}

scl8
CSCLContext::get_prev_pressed_key(scltouchdevice touch_id)
{
    scl8 ret = NOT_USED;
    MultiTouchContext *multi_touch_context = find_multi_touch_context(touch_id);
    if (multi_touch_context) {
        ret = multi_touch_context->prev_pressed_key;
    }
    return ret;
}

void
CSCLContext::set_prev_pressed_key(scltouchdevice touch_id, scl8 val)
{
    MultiTouchContext *multi_touch_context = find_multi_touch_context(touch_id);
    if (multi_touch_context) {
        multi_touch_context->prev_pressed_key = val;
    }
}

sclwindow
CSCLContext::get_prev_pressed_window(scltouchdevice touch_id)
{
    sclwindow ret = SCLWINDOW_INVALID;
    MultiTouchContext *multi_touch_context = find_multi_touch_context(touch_id);
    if (multi_touch_context) {
        ret = multi_touch_context->prev_pressed_window;
    }
    return ret;
}

void
CSCLContext::set_prev_pressed_window(scltouchdevice touch_id, sclwindow val)
{
    MultiTouchContext *multi_touch_context = find_multi_touch_context(touch_id);
    if (multi_touch_context) {
        multi_touch_context->prev_pressed_window = val;
    }
}

SclPoint
CSCLContext::get_prev_moving_point(scltouchdevice touch_id)
{
    SclPoint ret = {0, 0};
    MultiTouchContext *multi_touch_context = find_multi_touch_context(touch_id);
    if (multi_touch_context) {
        ret = multi_touch_context->prev_moving_point;
    }
    return ret;
}

void
CSCLContext::set_prev_moving_point(scltouchdevice touch_id, sclint x, sclint y)
{
    MultiTouchContext *multi_touch_context = find_multi_touch_context(touch_id);
    if (multi_touch_context) {
        multi_touch_context->prev_moving_point.x = x;
        multi_touch_context->prev_moving_point.y = y;
    }
}

SCLDragState
CSCLContext::get_prev_drag_state(scltouchdevice touch_id)
{
    SCLDragState ret = SCL_DRAG_STATE_NONE;
    MultiTouchContext *multi_touch_context = find_multi_touch_context(touch_id);
    if (multi_touch_context) {
        ret = multi_touch_context->prev_drag_state;
    }
    return ret;
}

void
CSCLContext::set_prev_drag_state(scltouchdevice touch_id, SCLDragState state)
{
    MultiTouchContext *multi_touch_context = find_multi_touch_context(touch_id);
    if (multi_touch_context) {
        multi_touch_context->prev_drag_state = state;
    }
}

void
CSCLContext::create_multi_touch_context(scltouchdevice touch_id, sclboolean is_sub_event)
{
    MultiTouchContext new_multi_touch_context;
    memset(&new_multi_touch_context, 0x00, sizeof(MultiTouchContext));
    new_multi_touch_context.used = TRUE;
    new_multi_touch_context.cur_pressed_key = NOT_USED;
    new_multi_touch_context.cur_pressed_window = SCLWINDOW_INVALID;
    new_multi_touch_context.event_id = 0;
    new_multi_touch_context.prev_pressed_key = NOT_USED;
    new_multi_touch_context.prev_pressed_window = SCLWINDOW_INVALID;
    new_multi_touch_context.cur_pressed_point.x = new_multi_touch_context.cur_pressed_point.y = 0;
    new_multi_touch_context.cur_pressed_time.tv_sec = new_multi_touch_context.cur_pressed_time.tv_usec = 0;
    new_multi_touch_context.cur_drag_state = SCL_DRAG_STATE_NONE;
    new_multi_touch_context.is_sub_event = is_sub_event;
    m_multi_touch_context[touch_id] = new_multi_touch_context;

    m_multi_touch_seq.insert(m_multi_touch_seq.end(), touch_id);

    CSCLUtils *utils = CSCLUtils::get_instance();
    if (utils) {
        for(std::list<scltouchdevice>::iterator iter = m_multi_touch_seq.begin();iter != m_multi_touch_seq.end();std::advance(iter, 1)) {
            utils->log("LIST : %d\n", *iter);
        }
        utils->log("\n");
    }
}

void
CSCLContext::destroy_multi_touch_context(scltouchdevice touch_id)
{
    MultiTouchContext *multi_touch_context = find_multi_touch_context(touch_id);
    if (multi_touch_context) {
        memset(multi_touch_context, 0x00, sizeof(MultiTouchContext));
        m_multi_touch_context.erase(touch_id);
    }

    sclboolean bFound = TRUE;
    std::list<scltouchdevice>::iterator iter;
    do {
        iter = std::find(m_multi_touch_seq.begin(), m_multi_touch_seq.end(), touch_id);
        if (iter != m_multi_touch_seq.end()) {
            m_multi_touch_seq.erase(iter);
        } else {
            bFound = FALSE;
        }
    } while (bFound);

    /*
    CSCLUtils *utils = CSCLUtils::get_instance();
    if (utils) {
        for(iter = m_multi_touch_seq.begin();iter != m_multi_touch_seq.end();std::advance(iter, 1)) {
            utils->log("LIST : %d\n", *iter);
        }
        utils->log("\n");
    }
    */
}

MultiTouchContext*
CSCLContext::find_multi_touch_context(scltouchdevice touch_id)
{
    MultiTouchContext* ret = NULL;
    std::map<scltouchdevice, MultiTouchContext>::iterator iter = m_multi_touch_context.find(touch_id);
    if (iter != m_multi_touch_context.end()) {
        ret = &(iter->second);
    }

    return ret;
}

sclint
CSCLContext::get_multi_touch_context_num()
{
    return m_multi_touch_seq.size();
}

sclboolean
CSCLContext::get_multi_touch_event(sclint order, SclUIEventDesc *desc)
{
    sclboolean ret = FALSE;
    sclint index = 0;

    CSCLResourceCache *cache = CSCLResourceCache::get_instance();

    for(std::list<scltouchdevice>::iterator list_iter = m_multi_touch_seq.begin();
        !ret && list_iter != m_multi_touch_seq.end();std::advance(list_iter, 1)) {
            if (index == order) {
                MultiTouchContext *multi_touch_context = find_multi_touch_context(*list_iter);
                if (multi_touch_context) {
                    const SclLayoutKeyCoordinate *coordinate =
                        cache->get_cur_layout_key_coordinate(multi_touch_context->cur_pressed_window,
                            multi_touch_context->cur_pressed_key);

                    if (coordinate) {
                        SCLShiftState shift_index = get_shift_state();
                        if (shift_index < 0 || shift_index >= SCL_SHIFT_STATE_MAX) shift_index = SCL_SHIFT_STATE_OFF;
                        desc->key_event = coordinate->key_event[shift_index][0];
                        desc->key_value = coordinate->key_value[shift_index][0];
                        desc->key_type = coordinate->key_type;
                    } else {
                        desc->key_event = 0;
                        desc->key_value = NULL;
                        desc->key_type = KEY_TYPE_NONE;
                    }
                    desc->key_modifier = multi_touch_context->cur_key_modifier;

                    desc->touch_id = (*list_iter);
                    desc->mouse_pressed_point = multi_touch_context->cur_pressed_point;
                    desc->mouse_current_point = multi_touch_context->cur_moving_point;
                    desc->mouse_farthest_point = multi_touch_context->farthest_move_point;

                    desc->touch_event_order = index;

                    desc->event_type = EVENT_TYPE_NONE;

                    ret = TRUE;
                }
            }
            index++;
    }

    return ret;
}

sclint
CSCLContext::get_multi_touch_event_order(scltouchdevice touch_id)
{
    sclint ret = -1;
    sclint iSeqIndex = 0;

    for(std::list<scltouchdevice>::iterator list_iter = m_multi_touch_seq.begin();
        (ret == -1) && list_iter != m_multi_touch_seq.end();std::advance(list_iter, 1)) {
            if (touch_id == (*list_iter)) {
                ret = iSeqIndex;
            } else {
                iSeqIndex++;
            }
    }

    return ret;
}

sclchar* CSCLContext::get_cur_sublayout()
{
    return m_cur_sub_layout;
}

sclboolean CSCLContext::set_cur_sublayout(const sclchar* sublayout)
{
    sclboolean ret = FALSE;
    if (sublayout) {
        strncpy(m_cur_sub_layout, sublayout, MAX_SIZE_OF_SUBLAYOUT_STRING);
        m_cur_sub_layout[MAX_SIZE_OF_SUBLAYOUT_STRING - 1] = '\0';
    } else {
        m_cur_sub_layout[0] = '\0';
    }
    return ret;
}

void
CSCLContext::set_custom_magnifier_label(scltouchdevice touch_id, sclint index, const sclchar* label)
{
    if (scl_check_arrindex(index, MAX_SIZE_OF_LABEL_FOR_ONE)) {
        MagnifierCustomLabelIdx label_index;
        label_index.touch_id = touch_id;
        label_index.index = index;
        if (label) {
            m_custom_magnifier_label[label_index] = std::string(label);
        } else {
            MagnifierCusomLabelIdxMap::iterator iter = m_custom_magnifier_label.find(label_index);
            if (iter != m_custom_magnifier_label.end()) {
                m_custom_magnifier_label.erase(iter);
            }
        }
    }
}

const sclchar*
CSCLContext::get_custom_magnifier_label(scltouchdevice touch_id, sclint index)
{
    MagnifierCustomLabelIdx label_index;
    label_index.touch_id = touch_id;
    label_index.index = index;

    const sclchar* ret = NULL;

    if (scl_check_arrindex(index, MAX_SIZE_OF_LABEL_FOR_ONE)) {
        MagnifierCusomLabelIdxMap::iterator iter = m_custom_magnifier_label.find(label_index);
        if (iter != m_custom_magnifier_label.end()) {
            ret = (iter->second).c_str();
        }
    }

    return ret;
}

SCLShiftState CSCLContext::get_shift_state() const
{
    SCLShiftState ret = SCL_SHIFT_STATE_OFF;
    if (m_shift_state >= 0 && m_shift_state < SCL_SHIFT_STATE_MAX) {
        ret = m_shift_state;
    }
    return ret;
}

void CSCLContext::set_shift_state(SCLShiftState val)
{
    if (val >= 0 && val < SCL_SHIFT_STATE_MAX) {
        m_shift_state = val;
        if (val == SCL_SHIFT_STATE_OFF) {
            m_shift_multi_touch_state = SCL_SHIFT_MULTITOUCH_OFF;
        } else if (val == SCL_SHIFT_STATE_ON) {
            m_shift_multi_touch_state = SCL_SHIFT_MULTITOUCH_ON_RELEASED;
        } else if (val == SCL_SHIFT_STATE_LOCK) {
            m_shift_multi_touch_state = SCL_SHIFT_MULTITOUCH_LOCK;
        }
    }
}

sclboolean CSCLContext::get_caps_lock_mode() const
{
    return m_caps_lock_mode;
}

void CSCLContext::set_caps_lock_mode(sclboolean val)
{
    m_caps_lock_mode = val;
}

SCLShiftMultitouchState CSCLContext::get_shift_multi_touch_state() const
{
    SCLShiftMultitouchState ret = SCL_SHIFT_MULTITOUCH_OFF;
    if (m_shift_multi_touch_state >= 0 && m_shift_multi_touch_state < SCL_SHIFT_MULTITOUCH_MAX) {
        ret = m_shift_multi_touch_state;
    }
    return ret;
}

void CSCLContext::set_shift_multi_touch_state(SCLShiftMultitouchState val)
{
    if (val >= 0 && val < SCL_SHIFT_MULTITOUCH_MAX) {
        m_shift_multi_touch_state = val;
    }
}
