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

#include "sclevents.h"
#ifdef  __WIN32__
#include "sclevents-win32.h"
#elif defined(__EFL__)
#include "sclevents-efl.h"
#else
#include "sclevents-gtk.h"
#endif
#include "scldebug.h"
#include "sclwindows.h"

#include "sclcontroller.h"
#include "sclresourcecache.h"
#include "sclkeyfocushandler.h"
#include <dlog.h>

using namespace scl;

CSCLEvents::CSCLEvents()
{
    SCL_DEBUG();
    m_impl = 0;
    m_touch_event_offset.x = m_touch_event_offset.y = 0;
}

CSCLEvents::~CSCLEvents()
{
    SCL_DEBUG();

    if (m_impl) {
        delete m_impl;
        m_impl = NULL;
    }
}

void CSCLEvents::init()
{
    CSCLEventsImpl *impl = get_scl_events_impl();
    if (impl) {
        impl->init();
    }
}

void CSCLEvents::fini()
{
    CSCLEventsImpl *impl = get_scl_events_impl();
    if (impl) {
        impl->fini();
    }
}

CSCLEventsImpl* CSCLEvents::get_scl_events_impl()
{
    SCL_DEBUG();
    if (m_impl == 0) {
#ifdef  __WIN32__
        m_impl = new CSCLEventsImplWin32;
#elif defined(__EFL__)
        m_impl = new CSCLEventsImplEfl;
#else
        m_impl = new CSCLEventsImplGtk;
#endif
    }
    return m_impl;
}

CSCLEvents* CSCLEvents::get_instance()
{
    static CSCLEvents instance;
    return &instance;
}

void
CSCLEvents::connect_window_events( sclwindow wnd, const sclint evt )
{
    CSCLWindows *windows = CSCLWindows::get_instance();
    //SclWindowContext *window_context = windows->get_window_context(wnd, FALSE);
    SclWindowContext *window_context = windows->get_window_context(wnd);
    if (window_context) {
        if (!(window_context->is_virtual)) {
            get_scl_events_impl()->connect_window_events(wnd, evt);
        }
    }
}

void
CSCLEvents::set_touch_event_offset(const SclPoint pos)
{
    m_touch_event_offset = pos;
}

SclPoint*
CSCLEvents::get_touch_event_offset()
{
    return &m_touch_event_offset;
}

sclboolean
CSCLEvents::process_key_event(const char *key)
{
    const char *keyname = key;
    LOGD("=-=-=-=- key_pressed \n");
    CSCLController *controller = CSCLController::get_instance();
    LOGD("=-=-=-=- keyname(char) = %s \n",keyname);

    CSCLResourceCache *cache = CSCLResourceCache::get_instance();
    const SclLayoutKeyCoordinate *prevcoordinate = NULL;
    const SclLayoutKeyCoordinate *coordinate = NULL;

    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLKeyFocusHandler* focus_handler = CSCLKeyFocusHandler::get_instance();

    sclwindow current_focus_window = focus_handler->get_current_focus_window();
    scl8 current_key_index = focus_handler->get_current_focus_key();
    sclwindow focus_window = current_focus_window;
    scl8 key_index = current_key_index;

    if (strcmp(keyname, "Right") == 0) {
        focus_handler->process_navigation(HIGHLIGHT_NAVIGATE_RIGHT);
        focus_window = focus_handler->get_current_focus_window();
        key_index = focus_handler->get_current_focus_key();
    } else if (strcmp(keyname, "Left") == 0) {
        focus_handler->process_navigation(HIGHLIGHT_NAVIGATE_LEFT);
        focus_window = focus_handler->get_current_focus_window();
        key_index = focus_handler->get_current_focus_key();
    } else if (strcmp(keyname, "Up") == 0) {
        focus_handler->process_navigation(HIGHLIGHT_NAVIGATE_UP);
        focus_window = focus_handler->get_current_focus_window();
        key_index = focus_handler->get_current_focus_key();
    } else if (strcmp(keyname, "Down") == 0) {
        focus_handler->process_navigation(HIGHLIGHT_NAVIGATE_DOWN);
        focus_window = focus_handler->get_current_focus_window();
        key_index = focus_handler->get_current_focus_key();
    } else if ((strcmp(keyname, "Return") == 0)||(strcmp(keyname, "Enter") == 0)) {
        coordinate = cache->get_cur_layout_key_coordinate(current_focus_window, current_key_index);
        //button_context->state = BUTTON_STATE_NORMAL;
        if (coordinate) {
            controller->mouse_press(current_focus_window, coordinate->x, coordinate->y, TRUE);
            controller->mouse_release(current_focus_window, coordinate->x, coordinate->y, TRUE);
            if (KEY_TYPE_MODECHANGE != coordinate->key_type) {
                //button_context->state = BUTTON_STATE_PRESSED;
                //windows->update_window(window, coordinate->x, coordinate->y, coordinate->width, coordinate->height);
            } else {
                focus_handler->init_key_index();
            }
            return TRUE;
        } else {
            return FALSE;
        }
    } else {
        return FALSE;
    }

    if (current_key_index != key_index || current_focus_window != focus_window) {
        prevcoordinate = cache->get_cur_layout_key_coordinate(current_focus_window, current_key_index);
        coordinate = cache->get_cur_layout_key_coordinate(focus_window, key_index);
        //prev_button_context->state = BUTTON_STATE_NORMAL;
        //button_context->state = BUTTON_STATE_PRESSED;
        if (coordinate && prevcoordinate) {
            if (current_focus_window == focus_window) {
                sclshort x,y,width,height;
                if (prevcoordinate->x < coordinate->x) {
                    x = prevcoordinate->x;
                } else {
                    x = coordinate->x;
                }

                if (prevcoordinate->y < coordinate->y) {
                    y = prevcoordinate->y;
                } else {
                    y = coordinate->y;
                }

                if (prevcoordinate->x + prevcoordinate->width > coordinate->x + coordinate->width) {
                    width = prevcoordinate->x + prevcoordinate->width - x;
                } else {
                    width = coordinate->x + coordinate->width - x;
                }

                if (prevcoordinate->y + prevcoordinate->height > coordinate->y + coordinate->height) {
                    height = prevcoordinate->y + prevcoordinate->height - y;
                } else {
                    height = coordinate->y + coordinate->height - y;
                }
                windows->update_window(focus_window, x, y, width, height);
            } else {
                windows->update_window(focus_window,
                    coordinate->x, coordinate->y, coordinate->width, coordinate->height);
                windows->update_window(current_focus_window,
                    prevcoordinate->x, prevcoordinate->y, prevcoordinate->width, prevcoordinate->height);
            }
        }
    } else {
    }
    return TRUE;
}

