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

#include "sclcontext.h"
#include "scldebug.h"
#include "sclwindows.h"
#include "sclutils.h"
#include "sclresourcecache.h"
#include "sclres.h"
#include <algorithm>

//#include "sclresource.h"

using namespace scl;

CSCLContext* CSCLContext::m_instance = NULL; /* For singleton */

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
    if (!m_instance) {
        m_instance = new CSCLContext();
    }
    return (CSCLContext*)m_instance;
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
    //SclWindowContext *ctx = windows->get_window_context(window, FALSE);
    SclWindowContext *ctx = windows->get_window_context(window);

    if (ctx) {
        ret = ctx->layout;
    }

    return ret;
}

void
CSCLContext::set_popup_layout( sclwindow window, sclshort val )
{
    SCL_DEBUG();

    CSCLWindows *windows = CSCLWindows::get_instance();
    //SclWindowContext *ctx = windows->get_window_context(window, FALSE);
    SclWindowContext *ctx = windows->get_window_context(window);

    if (ctx) {
        ctx->layout = val;
    }
}

void
CSCLContext::set_base_layout( sclshort val )
{
    SCL_DEBUG();
    CSCLWindows *windows = CSCLWindows::get_instance();
    //SclWindowContext *ctx = windows->get_window_context(windows->get_base_window(), FALSE);
    SclWindowContext *ctx = windows->get_window_context(windows->get_base_window());
    if (ctx) {
        ctx->layout = val;
    }
}

sclshort
CSCLContext::get_base_layout() const
{
    SCL_DEBUG();

    sclshort ret = NOT_USED;
    CSCLWindows *windows = CSCLWindows::get_instance();
    //SclWindowContext *ctx = windows->get_window_context(windows->get_base_window(), FALSE);
    SclWindowContext *ctx = windows->get_window_context(windows->get_base_window());
    if (ctx) {
        ret = ctx->layout;
    }

    return ret;
}

scl16
CSCLContext::get_cur_pressed_event_id(scltouchdevice touch_id)
{
    scl16 ret = NOT_USED;
    MultiTouchContext *ctx = find_multi_touch_context(touch_id);
    if (ctx) {
        ret = ctx->event_id;
    }
    return ret;
}

void
CSCLContext::set_cur_pressed_event_id(scltouchdevice touch_id, scl16 id)
{
    MultiTouchContext *ctx = find_multi_touch_context(touch_id);
    if (ctx) {
        ctx->event_id = id;
    }
}

scl8
CSCLContext::get_cur_pressed_key(scltouchdevice touch_id)
{
    scl8 ret = NOT_USED;
    MultiTouchContext *ctx = find_multi_touch_context(touch_id);
    if (ctx) {
        ret = ctx->cur_pressed_key;
    }
    return ret;
}

void
CSCLContext::set_cur_pressed_key(scltouchdevice touch_id, scl8 val)
{
    MultiTouchContext *ctx = find_multi_touch_context(touch_id);
    if (ctx) {
        ctx->cur_pressed_key = val;
    }
}

sclwindow
CSCLContext::get_cur_pressed_window(scltouchdevice touch_id)
{
    sclwindow ret = SCLWINDOW_INVALID;
    MultiTouchContext *ctx = find_multi_touch_context(touch_id);
    if (ctx) {
        ret = ctx->cur_pressed_window;
    }
    return ret;
}

void
CSCLContext::set_cur_pressed_window(scltouchdevice touch_id, sclwindow val)
{
    MultiTouchContext *ctx = find_multi_touch_context(touch_id);
    if (ctx) {
        ctx->cur_pressed_window = val;
    }
}

SclPoint
CSCLContext::get_cur_pressed_point(scltouchdevice touch_id)
{
    SclPoint ret = {0, 0};
    MultiTouchContext *ctx = find_multi_touch_context(touch_id);
    if (ctx) {
        ret = ctx->cur_pressed_point;
    }
    return ret;
}

void
CSCLContext::set_cur_pressed_point(scltouchdevice touch_id, sclint x, sclint y)
{
    MultiTouchContext *ctx = find_multi_touch_context(touch_id);
    if (ctx) {
        ctx->cur_pressed_point.x = x;
        ctx->cur_pressed_point.y = y;
        set_farthest_move_point(touch_id, x, y); // reset farthest move point
    }
}

SclPoint
CSCLContext::get_cur_move_point(scltouchdevice touch_id)
{
    SclPoint ret = {0, 0};
    MultiTouchContext *ctx = find_multi_touch_context(touch_id);
    if (ctx) {
        ret = ctx->cur_move_point;
    }
    return ret;
}

void
CSCLContext::set_cur_move_point(scltouchdevice touch_id, sclint x, sclint y)
{
    MultiTouchContext *ctx = find_multi_touch_context(touch_id);
    if (ctx) {
        ctx->cur_move_point.x = x;
        ctx->cur_move_point.y = y;
    }
}

sclwindow
CSCLContext::get_cur_move_window(scltouchdevice touch_id)
{
    sclwindow ret = SCLWINDOW_INVALID;
    MultiTouchContext *ctx = find_multi_touch_context(touch_id);
    if (ctx) {
        ret = ctx->cur_move_window;
    }
    return ret;
}


void
CSCLContext::set_cur_move_window(scltouchdevice touch_id, sclwindow window)
{
    MultiTouchContext *ctx = find_multi_touch_context(touch_id);
    if (ctx) {
        ctx->cur_move_window = window;
    }
}

struct timeval
CSCLContext::get_cur_pressed_time(scltouchdevice touch_id)
{
    struct timeval ret = {0};
    MultiTouchContext *ctx = find_multi_touch_context(touch_id);
    if (ctx) {
        ret = ctx->cur_pressed_time;
    }
    return ret;
}

void
CSCLContext::set_cur_pressed_time(scltouchdevice touch_id)
{
    MultiTouchContext *ctx = find_multi_touch_context(touch_id);
    if (ctx) {
        gettimeofday(&(ctx->cur_pressed_time), NULL);
    }
}

SclPoint
CSCLContext::get_farthest_move_point(scltouchdevice touch_id)
{
    SclPoint ret = {0, 0};
    MultiTouchContext *ctx = find_multi_touch_context(touch_id);
    if (ctx) {
        ret = ctx->farthest_move_point;
    }
    return ret;
}

void
CSCLContext::set_farthest_move_point(scltouchdevice touch_id, sclint x, sclint y)
{
    MultiTouchContext *ctx = find_multi_touch_context(touch_id);
    if (ctx) {
        CSCLUtils *utils = CSCLUtils::get_instance();
        sclint deltax = ctx->cur_pressed_point.x - x;
        sclint deltay = ctx->cur_pressed_point.y - y;
        ctx->farthest_move_dist = utils->get_approximate_distance(x, y, ctx->cur_pressed_point.x, ctx->cur_pressed_point.y);

        ctx->farthest_move_point.x = x;
        ctx->farthest_move_point.y = y;
    }
}

sclint
CSCLContext::get_farthest_move_dist(scltouchdevice touch_id)
{
    sclint ret = 0;
    MultiTouchContext *ctx = find_multi_touch_context(touch_id);
    if (ctx) {
        ret = ctx->farthest_move_dist;
    }
    return ret;
}

SCLDragState
CSCLContext::get_cur_drag_state(scltouchdevice touch_id)
{
    SCLDragState ret = SCL_DRAG_STATE_NONE;
    MultiTouchContext *ctx = find_multi_touch_context(touch_id);
    if (ctx) {
        ret = ctx->cur_drag_state;
    }
    return ret;
}

void
CSCLContext::set_cur_drag_state(scltouchdevice touch_id, SCLDragState state)
{
    MultiTouchContext *ctx = find_multi_touch_context(touch_id);
    if (ctx) {
        ctx->cur_drag_state = state;
    }
}

SCLKeyModifier
CSCLContext::get_cur_key_modifier(scltouchdevice touch_id)
{
    SCLKeyModifier ret = KEY_MODIFIER_NONE;
    MultiTouchContext *ctx = find_multi_touch_context(touch_id);
    if (ctx) {
        ret = ctx->cur_key_modifier;
    }
    return ret;
}

void
CSCLContext::set_cur_key_modifier(scltouchdevice touch_id, SCLKeyModifier modifier)
{
    MultiTouchContext *ctx = find_multi_touch_context(touch_id);
    if (ctx) {
        ctx->cur_key_modifier = modifier;
    }
}

scl8
CSCLContext::get_prev_pressed_key(scltouchdevice touch_id)
{
    scl8 ret = NOT_USED;
    MultiTouchContext *ctx = find_multi_touch_context(touch_id);
    if (ctx) {
        ret = ctx->prev_pressed_key;
    }
    return ret;
}

void
CSCLContext::set_prev_pressed_key(scltouchdevice touch_id, scl8 val)
{
    MultiTouchContext *ctx = find_multi_touch_context(touch_id);
    if (ctx) {
        ctx->prev_pressed_key = val;
    }
}

sclwindow
CSCLContext::get_prev_pressed_window(scltouchdevice touch_id)
{
    sclwindow ret = SCLWINDOW_INVALID;
    MultiTouchContext *ctx = find_multi_touch_context(touch_id);
    if (ctx) {
        ret = ctx->prev_pressed_window;
    }
    return ret;
}

void
CSCLContext::set_prev_pressed_window(scltouchdevice touch_id, sclwindow val)
{
    MultiTouchContext *ctx = find_multi_touch_context(touch_id);
    if (ctx) {
        ctx->prev_pressed_window = val;
    }
}

SclPoint
CSCLContext::get_prev_move_point(scltouchdevice touch_id)
{
    SclPoint ret = {0, 0};
    MultiTouchContext *ctx = find_multi_touch_context(touch_id);
    if (ctx) {
        ret = ctx->prev_move_point;
    }
    return ret;
}

void
CSCLContext::set_prev_move_point(scltouchdevice touch_id, sclint x, sclint y)
{
    MultiTouchContext *ctx = find_multi_touch_context(touch_id);
    if (ctx) {
        ctx->prev_move_point.x = x;
        ctx->prev_move_point.y = y;
    }
}

SCLDragState
CSCLContext::get_prev_drag_state(scltouchdevice touch_id)
{
    SCLDragState ret = SCL_DRAG_STATE_NONE;
    MultiTouchContext *ctx = find_multi_touch_context(touch_id);
    if (ctx) {
        ret = ctx->prev_drag_state;
    }
    return ret;
}

void
CSCLContext::set_prev_drag_state(scltouchdevice touch_id, SCLDragState state)
{
    MultiTouchContext *ctx = find_multi_touch_context(touch_id);
    if (ctx) {
        ctx->prev_drag_state = state;
    }
}

void
CSCLContext::create_multi_touch_context(scltouchdevice touch_id, sclboolean isSubEvent)
{
    MultiTouchContext newctx;
    memset(&newctx, 0x00, sizeof(MultiTouchContext));
    newctx.used = TRUE;
    newctx.cur_pressed_key = NOT_USED;
    newctx.cur_pressed_window = SCLWINDOW_INVALID;
    newctx.event_id = 0;
    newctx.prev_pressed_key = NOT_USED;
    newctx.prev_pressed_window = SCLWINDOW_INVALID;
    newctx.cur_pressed_point.x = newctx.cur_pressed_point.y = 0;
    newctx.cur_pressed_time.tv_sec = newctx.cur_pressed_time.tv_usec = 0;
    newctx.cur_drag_state = SCL_DRAG_STATE_NONE;
    newctx.is_sub_event = isSubEvent;
    m_multi_touch_context[touch_id] = newctx;

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
    MultiTouchContext *ctx = find_multi_touch_context(touch_id);
    if (ctx) {
        memset(ctx, 0x00, sizeof(MultiTouchContext));
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

    CSCLUtils *utils = CSCLUtils::get_instance();
    if (utils) {
        for(std::list<scltouchdevice>::iterator iter = m_multi_touch_seq.begin();iter != m_multi_touch_seq.end();std::advance(iter, 1)) {
            utils->log("LIST : %d\n", *iter);
        }
        utils->log("\n");
    }
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
CSCLContext::get_multi_touch_event(sclint seqorder, SclUIEventDesc *desc)
{
    sclboolean ret = FALSE;
    sclint iSeqIndex = 0;

    CSCLResourceCache *cache = CSCLResourceCache::get_instance();

    for(std::list<scltouchdevice>::iterator list_iter = m_multi_touch_seq.begin();
        !ret && list_iter != m_multi_touch_seq.end();std::advance(list_iter, 1)) {
            if (iSeqIndex == seqorder) {
                MultiTouchContext *pContext = find_multi_touch_context(*list_iter);
                if (pContext) {
                    const SclLayoutKeyCoordinate *coordination =
                        cache->get_cur_layout_key_coordinate(pContext->cur_pressed_window, pContext->cur_pressed_key);

                    if (coordination) {
                        SCLShiftState shiftidx = get_shift_state();
                        if (shiftidx < 0 || shiftidx >= SCL_SHIFT_STATE_MAX) shiftidx = SCL_SHIFT_STATE_OFF;
                        desc->key_event = coordination->key_event[shiftidx][0];
                        desc->key_value = coordination->key_value[shiftidx][0];
                        desc->key_type = coordination->key_type;
                    } else {
                        desc->key_event = 0;
                        desc->key_value = NULL;
                        desc->key_type = KEY_TYPE_NONE;
                    }
                    desc->key_modifier = pContext->cur_key_modifier;

                    desc->touch_id = (*list_iter);
                    desc->mouse_pressed_point = pContext->cur_pressed_point;
                    desc->mouse_current_point = pContext->cur_move_point;
                    desc->mouse_farthest_point = pContext->farthest_move_point;

                    desc->touch_event_order = iSeqIndex;

                    desc->event_type = EVENT_TYPE_NONE;

                    ret = TRUE;
                }
            }
            iSeqIndex++;
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
