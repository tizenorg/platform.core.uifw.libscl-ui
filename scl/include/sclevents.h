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
#include <string.h>

#include "scltypes.h"
#include "sclconfig.h"

#ifndef __SCL_EVENTS_H__
#define __SCL_EVENTS_H__

#ifdef __cplusplus
//SCL_BEGIN_DECLS
#endif

namespace scl
{

/**@brief window event definition */
typedef enum _SCLCbEvent {
    SCL_EVENT_MOUSE = 1 << 1, /**< for mouse event */
    SCL_EVENT_EXPOSE = 1 << 2 /**< for expose */
}SCLCbEvent;

/**@brief window event definition */
typedef enum _SCLMouseEvent {
    SCL_MOUSE_EVENT_PRESS,
    SCL_MOUSE_EVENT_MOVE,
    SCL_MOUSE_EVENT_RELEASE,
}SCLMouseEvent;

/**
 * @brief The base class to work as a soft-based keyboard
 *
 * This class implements all functions for working as a soft-based keyboard
 * In side of ISE developer, they can modify it by their requirements.
 */
class CSCLEventsImpl
{
public :
    virtual void init() = 0;
    virtual void fini() = 0;

    virtual void connect_window_events(const sclwindow wnd, const sclint evt) = 0;
    virtual void create_timer(const scl16 id, const scl32 interval, scl16 value, sclboolean addToMap) = 0;
    virtual void destroy_timer(const scl32 id) = 0;
    virtual void destroy_all_timer() = 0;

    virtual void generate_mouse_event(SCLMouseEvent type, scl16 x, scl16 y) = 0;

};

class CSCLEvents
{
public :
    CSCLEvents();
    ~CSCLEvents();

    static CSCLEvents* get_instance();

    void init();
    void fini();

    void connect_window_events(sclwindow wnd, const sclint evt);

    void create_timer(const scl16 id, const scl32 interval, scl16 value, sclboolean addToMap = TRUE) {
        get_scl_events_impl()->destroy_timer(id);
        get_scl_events_impl()->create_timer(id, interval, value, addToMap);
    }

    void destroy_timer(const scl32 id) {
        get_scl_events_impl()->destroy_timer(id);
    }

    void destroy_all_timer() {
        get_scl_events_impl()->destroy_all_timer();
    }

    void set_touch_event_offset(const SclPoint pos);
    SclPoint* get_touch_event_offset();

    virtual void generate_mouse_event(SCLMouseEvent type, scl16 x, scl16 y) {
        get_scl_events_impl()->generate_mouse_event(type, x, y);
    }

    sclboolean process_key_event(const char *key);

protected :
    CSCLEventsImpl* get_scl_events_impl();

private :
    CSCLEventsImpl* m_impl;
    SclPoint m_touch_event_offset;
};


} /* End of scl namespace */

#ifdef __cplusplus
//SCL_END_DECLS
#endif

#endif //__SCL_EVENTS_H__
