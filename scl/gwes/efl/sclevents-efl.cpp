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

#include "sclevents-efl.h"
#include "scldebug.h"
#include "sclcontroller.h"
#include "sclgraphics.h"
#include "scluibuilder.h"
#include "sclerroradjustment.h"
#include "sclresource.h"
#include "sclresourcecache.h"
#include "sclres_manager.h"

#include <Elementary.h>
#include <Ecore_X.h>
#include <dlog.h>
#include <utilX.h>

#include "sclkeyfocushandler.h"

using namespace scl;

#ifdef USING_KEY_GRAB
#define HANDLE_KEY_EVENTS
#endif

#define E_PROP_TOUCH_INPUT "X_TouchInput"

sclboolean mouse_pressed = FALSE; /* Checks whether mouse is pressed or not */
sclwindow pressed_window = SCLWINDOW_INVALID;

#define MIN_XY_DIFF 14

Eina_Bool mouse_press(void *data, int type, void *event_info);
Eina_Bool mouse_move (void *data, int type, void *event_info);
Eina_Bool mouse_release (void *data, int type, void *event_info);

Eina_Bool client_message_cb(void *data, int type, void *event);

#ifdef HANDLE_KEY_EVENTS
Eina_Bool key_pressed(void *data, int type, void *event_info);
#endif

/**
 * Constructor
 */
CSCLEventsImplEfl::CSCLEventsImplEfl()
{
    SCL_DEBUG();

    m_mouse_down_handler = NULL;
    m_mouse_move_handler = NULL;
    m_mouse_up_handler = NULL;

    m_xclient_msg_handler = NULL;
    m_key_pressed_handler = NULL;
}

/**
 * De-constructor
 */
CSCLEventsImplEfl::~CSCLEventsImplEfl()
{
    SCL_DEBUG();

    fini();
}

void CSCLEventsImplEfl::init()
{
    /* Initializes all window resources */
    m_mouse_down_handler = ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_DOWN, mouse_press, NULL);
    m_mouse_move_handler = ecore_event_handler_add(ECORE_EVENT_MOUSE_MOVE, mouse_move, NULL);
    m_mouse_up_handler = ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_UP, mouse_release, NULL);

    m_xclient_msg_handler = ecore_event_handler_add(ECORE_X_EVENT_CLIENT_MESSAGE, client_message_cb, NULL);
#ifdef HANDLE_KEY_EVENTS
    m_key_pressed_handler = ecore_event_handler_add(ECORE_EVENT_KEY_DOWN, key_pressed, NULL);
#endif
}

void CSCLEventsImplEfl::fini()
{
    if (m_mouse_down_handler) ecore_event_handler_del(m_mouse_down_handler);
    m_mouse_down_handler = NULL;
    if (m_mouse_move_handler) ecore_event_handler_del(m_mouse_move_handler);
    m_mouse_move_handler = NULL;
    if (m_mouse_up_handler) ecore_event_handler_del(m_mouse_up_handler);
    m_mouse_up_handler = NULL;
    if (m_xclient_msg_handler) ecore_event_handler_del(m_xclient_msg_handler);
    m_xclient_msg_handler = NULL;
#ifdef HANDLE_KEY_EVENTS
    if (m_key_pressed_handler) ecore_event_handler_del(m_key_pressed_handler);
#endif
    m_key_pressed_handler = NULL;

}

sclboolean get_window_rect(const sclwindow window, SclRectangle *rect)
{
    SCL_DEBUG();
    sclboolean ret = FALSE;
    CSCLUtils *utils = CSCLUtils::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLContext *context = CSCLContext::get_instance();
    if (windows && context && utils && rect) {
        SclWindowContext *window_context = windows->get_window_context(window);
        sclint scr_w, scr_h;
        /* get window size */
        utils->get_screen_resolution(&scr_w, &scr_h);
        if (window_context) {
            switch (context->get_rotation()) {
                case ROTATION_90_CW:
                    {
                        rect->height = window_context->geometry.width;
                        rect->width = window_context->geometry.height;
                        rect->y = scr_w - rect->height - window_context->geometry.x;
                        rect->x = window_context->geometry.y;
                    }
                    break;
                case ROTATION_180:
                    {
                        rect->width = window_context->geometry.width;
                        rect->height = window_context->geometry.height;
                        rect->x = scr_w - window_context->geometry.x - rect->width;
                        rect->y = scr_h - window_context->geometry.y - rect->height;
                    }
                    break;
                case ROTATION_90_CCW:
                    {
                        rect->height = window_context->geometry.width;
                        rect->width = window_context->geometry.height;
                        rect->y = window_context->geometry.x;
                        rect->x= scr_h - window_context->geometry.y - rect->width;
                    }
                    break;
                default:
                    {
                        rect->x = window_context->geometry.x;
                        rect->y = window_context->geometry.y;
                        rect->width = window_context->geometry.width;
                        rect->height = window_context->geometry.height;
                    }
                    break;
            }
            ret = TRUE;
        } else {
            rect->x = rect->y = rect->width = rect->height = 0;
        }
    }
    return ret;
}

/**  Here x and y contains "actual" x and y position relative to portrait root window,
     and window_context->width,height contains the window's orientation dependant width and height */
SclPoint get_rotated_local_coords(sclint x, sclint y, SCLRotation rotation, SclRectangle *rect) {
    SclPoint ret = {0, 0};

    if (rect) {
        switch (rotation) {
            case ROTATION_90_CW:
                {
                    ret.x = (rect->y + rect->width) - y;
                    ret.y = x - rect->x;
                }
                break;
            case ROTATION_180:
                {
                    ret.x = (rect->x + rect->width) - x;
                    ret.y = (rect->y + rect->height) - y;
                }
                break;
            case ROTATION_90_CCW:
                {
                    ret.x = y - rect->y;
                    ret.y = (rect->x + rect->height) - x;
                }
                break;
            default:
                {
                    ret.x = x - rect->x;
                    ret.y = y - rect->y;
                }
                break;
        }
    }
    return ret;
}

//void mouse_press (void *data, Evas *e, Evas_Object *object, void *event_info)
Eina_Bool mouse_press(void *data, int type, void *event_info)
{
    SCL_DEBUG();

    Evas_Event_Mouse_Down *ev1 = (Evas_Event_Mouse_Down*)event_info;
    LOGD("mouse_press : %d %d\n", ev1->output.x, ev1->output.y);

    CSCLController *controller = CSCLController::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLContext *context = CSCLContext::get_instance();
    CSCLUtils *utils = CSCLUtils::get_instance();
    CSCLErrorAdjustment *adjustment = CSCLErrorAdjustment::get_instance();

    Ecore_Event_Mouse_Button *ev = (Ecore_Event_Mouse_Button*)event_info;

    if (controller && windows && context && utils && adjustment && ev) {
        sclbyte index = 0;
        sclboolean processed = FALSE;
        sclwindow window = SCLWINDOW_INVALID;

        Ecore_X_Window inputWindow = 0;
        Ecore_X_Atom inputAtom = ecore_x_atom_get ("DeviceMgr Input Window");
        ecore_x_window_prop_xid_get (ecore_x_window_root_first_get(),
            inputAtom, ECORE_X_ATOM_WINDOW, &inputWindow, 1);
        if (inputWindow == 0) {
            utils->log("Error : input window NULL!");
        }

        unsigned int touch_input = 0;
        int res = ecore_x_window_prop_card32_get(inputWindow,
            ecore_x_atom_get(E_PROP_TOUCH_INPUT), &touch_input, 1);

        utils->log("E_PROP_TOUCH_INPUT : %d %d\n", res, touch_input);

        if (1 == res) {
            if (1 == touch_input) {
                adjustment->enable_touch_offset(TRUE);
            } else if (0 == touch_input) {
                adjustment->enable_touch_offset(FALSE);
            }
        }

        sclboolean is_scl_window = FALSE;
        if (elm_win_xwindow_get(static_cast<Evas_Object*>(windows->get_base_window())) == ev->window) {
            is_scl_window = TRUE;
        } else if (elm_win_xwindow_get(static_cast<Evas_Object*>(windows->get_magnifier_window())) == ev->window) {
            is_scl_window = TRUE;
        } else {
            do {
                window = windows->get_nth_window_in_Z_order_list(index);
                SclWindowContext *window_context = windows->get_window_context(window);
                if (window_context) {
                    if (window_context->is_virtual) {
                        is_scl_window  = TRUE;
                    } else if (elm_win_xwindow_get(static_cast<Evas_Object*>(window)) == ev->window) {
                        is_scl_window = TRUE;
                    }
                }
                index++;
            } while (index < MAX_ZORDER_NUM && window != SCLWINDOW_INVALID);
            index = 0;
        }
        if (!is_scl_window) return TRUE;

        SclRectangle rect = {0,0,0,0};
        do {
            window = windows->get_nth_window_in_Z_order_list(index);
            if (window) {
                // Update the position of the target window
                //windows->get_window_context(window, TRUE);
                SclWindowContext *window_context = windows->get_window_context(window);
                if (window_context) {
                    windows->get_window_rect(window, &(window_context->geometry));
                    if (get_window_rect(window, &rect)) {
                        int adjustx = ev->root.x;
                        int adjusty = ev->root.y;

                        SclResParserManager *sclres_manager = SclResParserManager::get_instance();
                        PSclDefaultConfigure default_configure = NULL;
                        if (sclres_manager) {
                            default_configure = sclres_manager->get_default_configure();
                        }
                        if (default_configure) {
                            SCLDisplayMode display_mode = context->get_display_mode();
                            if (scl_check_arrindex(display_mode, DISPLAYMODE_MAX)) {
                                adjustment->apply_touch_offset(default_configure->touch_offset_level[display_mode], &adjustx, &adjusty);
                            }
                        }

                        sclint winwidth = rect.width;
                        sclint winheight = rect.height;
                        if (context->get_display_mode() != DISPLAYMODE_PORTRAIT) {
                            rect.height = winwidth;
                            rect.width = winheight;
                        }

                        sclboolean process_event = FALSE;
                        if ((adjustx >= rect.x && adjustx <= (rect.x + winwidth)) &&
                            (adjusty >= rect.y && adjusty <= (rect.y + winheight))) {
                                process_event = TRUE;
                        }
                        if (process_event)
                        {
                            // Now convert the global coordinate to appropriate local coordinate
                            SclPoint coords = get_rotated_local_coords(ev->root.x, ev->root.y, context->get_rotation(), &rect);
                            controller->mouse_press(window, coords.x, coords.y, ev->multi.device);
                            mouse_pressed = TRUE;
                            processed = TRUE;
                            pressed_window = window;
                        }
                    }
                }
            }
            index++;
        } while (index < MAX_ZORDER_NUM && window != SCLWINDOW_INVALID && !processed);

        if (!processed) {
            window = pressed_window;
            if (get_window_rect(window, &rect)) {
                if (context->get_rotation() == ROTATION_90_CW || context->get_rotation() == ROTATION_90_CCW) {
                    sclint temp = rect.width;
                    rect.width = rect.height;
                    rect.height = temp;
                }

                // Now convert the global coordinate to appropriate local coordinate
                SclPoint coords = get_rotated_local_coords(ev->root.x, ev->root.y, context->get_rotation(), &rect);
                controller->mouse_press(window, coords.x, coords.y, ev->multi.device);
                mouse_pressed = TRUE;
                processed = TRUE;
            }
        }
    }

    return TRUE;

    /*CSCLContext *context = CSCLContext::get_instance();
    controller->mouse_press((sclwindow)data, ev->output.x, ev->output.y);
    mouse_pressed = TRUE;*/

    //LOGD("=-=-=-=- mouse_press : %p %d %d\n", data, ev->output.x, ev->output.y);
}

//void mouse_release (void *data, Evas *e, Evas_Object *object, void *event_info)
Eina_Bool mouse_release (void *data, int type, void *event_info)
{
    SCL_DEBUG();

    CSCLController *controller = CSCLController::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLContext *context = CSCLContext::get_instance();

    //Evas_Event_Mouse_Up *ev = (Evas_Event_Mouse_Up*)event_info;
    Ecore_Event_Mouse_Button *ev = (Ecore_Event_Mouse_Button*)event_info;

    //if (!mouse_pressed) return FALSE;

    if (controller && windows && context && ev) {
        LOGD("mouse_release : %d %d, %d %d\n", ev->root.x, ev->root.y, ev->x, ev->y);

        sclbyte index = 0;
        sclboolean processed = FALSE;
        sclwindow window = SCLWINDOW_INVALID;
        SclRectangle rect;
        sclboolean dimwinevent = FALSE;
        SclWindowContext *dim_window_context = windows->get_window_context(windows->get_dim_window());
        if (dim_window_context) {
            if (!(dim_window_context->is_virtual)) {
                if (elm_win_xwindow_get(static_cast<Evas_Object*>(windows->get_dim_window())) == ev->window) {
                    dimwinevent = TRUE;
                }
            }
        }
        if (dimwinevent) {
            controller->mouse_press(windows->get_dim_window(), ev->root.x, ev->root.y, ev->multi.device);
        } else {
            do {
                window = windows->get_nth_window_in_Z_order_list(index);
                if (window) {
                    SclWindowContext *window_context = windows->get_window_context(window);
                    if (window_context) {
                        windows->get_window_rect(window, &(window_context->geometry));
                        if (get_window_rect(window, &rect)) {
                            int adjustx = ev->root.x;
                            int adjusty = ev->root.y;

                            SclResParserManager *sclres_manager = SclResParserManager::get_instance();
                            PSclDefaultConfigure default_configure = NULL;
                            if (sclres_manager) {
                                default_configure = sclres_manager->get_default_configure();
                            }
                            if (default_configure) {
                                SCLDisplayMode display_mode = context->get_display_mode();
                                CSCLErrorAdjustment *adjustment = CSCLErrorAdjustment::get_instance();
                                if (adjustment && scl_check_arrindex(display_mode, DISPLAYMODE_MAX)) {
                                    adjustment->apply_touch_offset(default_configure->touch_offset_level[display_mode], &adjustx, &adjusty);
                                }
                            }

                            sclint winwidth = rect.width;
                            sclint winheight = rect.height;
                            if (context->get_display_mode() != DISPLAYMODE_PORTRAIT) {
                                rect.height = winwidth;
                                rect.width = winheight;
                            }

                            sclboolean process_event = FALSE;
                            if ((adjustx >= rect.x && adjustx <= (rect.x + winwidth)) &&
                                (adjusty >= rect.y && adjusty <= (rect.y + winheight))) {
                                    process_event = TRUE;
                            }
                            if (process_event)
                            {
                                /* Now convert the global coordinate to appropriate local coordinate */
                                SclPoint coords = get_rotated_local_coords(ev->root.x, ev->root.y, context->get_rotation(), &rect);
                                controller->mouse_release(window, coords.x, coords.y, ev->multi.device);
                                processed = TRUE;
                            }
                        }
                    }
                }
                index++;
            } while (index < MAX_ZORDER_NUM && window != SCLWINDOW_INVALID && !processed);
        }

        if (!processed) {
            window = pressed_window;
            if (get_window_rect(window, &rect)) {
                if (context->get_rotation() == ROTATION_90_CW || context->get_rotation() == ROTATION_90_CCW) {
                    sclint temp = rect.width;
                    rect.width = rect.height;
                    rect.height = temp;
                }

                /* Now convert the global coordinate to appropriate local coordinate */
                SclPoint coords = get_rotated_local_coords(ev->root.x, ev->root.y, context->get_rotation(), &rect);
                controller->mouse_release(window, coords.x, coords.y, ev->multi.device);
                processed = TRUE;
            }
        }

        mouse_pressed = FALSE;
    }

    return TRUE;
    //CSCLController *controller = CSCLController::get_instance();
    //CSCLWindows *windows = CSCLWindows::get_instance();
    //controller->mouse_release((sclwindow)data, (int)ev->output.x, (int)ev->output.y);
    //controller->mouse_release((sclwindow)data, (int)ev->x, (int)ev->y);
}

#ifdef HANDLE_KEY_EVENTS
Eina_Bool key_pressed(void *data, int type, void *event_info)
{
    LOGD("=-=-=-=- key_pressed \n");
    CSCLController *controller = CSCLController::get_instance();
    Ecore_Event_Key *ev = (Ecore_Event_Key *)event_info;
    const char *ckey_val = ev->key;
    LOGD("=-=-=-=- ev->key(char) = %c \n",ev->key);
    LOGD("=-=-=-=- ev->key(string) = %s \n",ev->key);
    LOGD("=-=-=-=- ev->keyname(char) = %c \n",ev->keyname);
    LOGD("=-=-=-=- ev->keyname(string) = %s \n",ev->keyname);
    LOGD("=-=-=-=- ev->string(char) = %c \n",ev->string);
    LOGD("=-=-=-=- ev->string(string) = %s \n",ev->string);

    CSCLResourceCache *cache = CSCLResourceCache::get_instance();
    SclButtonContext *prev_button_context = NULL;
    const SclLayoutKeyCoordinate *prevcoordinate = NULL;
    SclButtonContext *button_context = NULL;
    const SclLayoutKeyCoordinate *coordinate = NULL;

    CSCLWindows *windows = CSCLWindows::get_instance();
    sclwindow window = windows->get_base_window();
    CSCLKeyFocusHandler* focus_handler = CSCLKeyFocusHandler::get_instance();

    sclbyte current_key_index = focus_handler->get_current_key_index();
    sclbyte key_index = current_key_index;

    if (strcmp(ev->keyname, "Right") == 0) {
        key_index = focus_handler->get_next_key_index(NAVIGATE_RIGHT);
    } else if (strcmp(ev->keyname, "Left") == 0) {
        key_index = focus_handler->get_next_key_index(NAVIGATE_LEFT);
    } else if (strcmp(ev->keyname, "Up") == 0) {
        key_index = focus_handler->get_next_key_index(NAVIGATE_UP);
    } else if (strcmp(ev->keyname, "Down") == 0) {
        key_index = focus_handler->get_next_key_index(NAVIGATE_DOWN);
    } else if ((strcmp(ev->keyname, "Return") == 0)||(strcmp(ev->keyname, "Enter") == 0)) {
        button_context = cache->get_cur_button_context(window, current_key_index);
        coordinate = cache->get_cur_layout_key_coordinate(window, current_key_index);
        button_context->state = BUTTON_STATE_NORMAL;
        controller->mouse_press(window, coordinate->x, coordinate->y, TRUE);
        controller->mouse_release(window, coordinate->x, coordinate->y, TRUE);
        if (KEY_TYPE_MODECHANGE != coordinate->key_type) {
            button_context->state = BUTTON_STATE_PRESSED;
            windows->update_window(window, coordinate->x, coordinate->y, coordinate->width, coordinate->height);
        } else {
            focus_handler->init_key_index();
        }
        return TRUE;
    }

    if (current_key_index != key_index) {
        button_context = cache->get_cur_button_context(window, key_index);
        prev_button_context = cache->get_cur_button_context(window, current_key_index);
        prevcoordinate = cache->get_cur_layout_key_coordinate(window, current_key_index);
        coordinate = cache->get_cur_layout_key_coordinate(window, key_index);
        prev_button_context->state = BUTTON_STATE_NORMAL;
        button_context->state = BUTTON_STATE_PRESSED;
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
        windows->update_window(window, x, y, width, height);

    } else {
    }

    return TRUE;
}
#endif /*HANDLE_KEY_EVENTS*/

//int mouse_move (void *data, Evas *e, Evas_Object *object, void *event_info)
Eina_Bool mouse_move (void *data, int type, void *event_info)
{
    SCL_DEBUG();

    CSCLController *controller = CSCLController::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLContext *context = CSCLContext::get_instance();
    CSCLResourceCache *cache = CSCLResourceCache::get_instance();

    //Evas_Event_Mouse_Move *ev = (Evas_Event_Mouse_Move*)event_info;
    Ecore_Event_Mouse_Move *ev = (Ecore_Event_Mouse_Move*)event_info;

    //if (!mouse_pressed) return FALSE;

    if (controller && windows && context && cache && ev) {
        sclbyte index = 0;
        sclboolean processed = FALSE;
        sclwindow window = SCLWINDOW_INVALID;
        SclRectangle rect;

        LOGD("mouse_move : %d %d, %d %d\n", ev->root.x, ev->root.y, ev->x, ev->y);

        if (context->get_cur_pressed_window(ev->multi.device) != SCLWINDOW_INVALID &&
            get_window_rect(context->get_cur_pressed_window(ev->multi.device), &rect)) {
            sclint winwidth = rect.width;
            sclint winheight = rect.height;
            if (context->get_display_mode() != DISPLAYMODE_PORTRAIT) {
                rect.height = winwidth;
                rect.width = winheight;
            }
            SclPoint coords = get_rotated_local_coords(ev->root.x, ev->root.y, context->get_rotation(), &rect);

            controller->mouse_move(context->get_cur_pressed_window(ev->multi.device), coords.x, coords.y, ev->multi.device);
            processed = TRUE;
        } else {
            do {
                window = windows->get_nth_window_in_Z_order_list(index);
                if (window) {
                    SclWindowContext *window_context = windows->get_window_context(window);
                    if (window_context) {
                        windows->get_window_rect(window, &(window_context->geometry));
                        if (get_window_rect(window, &rect)) {
                            int adjustx = ev->root.x;
                            int adjusty = ev->root.y;

                            SclResParserManager *sclres_manager = SclResParserManager::get_instance();
                            PSclDefaultConfigure default_configure = NULL;
                            if (sclres_manager) {
                                default_configure = sclres_manager->get_default_configure();
                            }
                            if (default_configure) {
                                SCLDisplayMode display_mode = context->get_display_mode();
                                CSCLErrorAdjustment *adjustment = CSCLErrorAdjustment::get_instance();
                                if (adjustment && scl_check_arrindex(display_mode, DISPLAYMODE_MAX)) {
                                    adjustment->apply_touch_offset(default_configure->touch_offset_level[display_mode], &adjustx, &adjusty);
                                }
                            }

                            sclint winwidth = rect.width;
                            sclint winheight = rect.height;
                            if (context->get_display_mode() != DISPLAYMODE_PORTRAIT) {
                                rect.height = winwidth;
                                rect.width = winheight;
                            }

                            sclboolean process_event = FALSE;
                            if ((adjustx >= rect.x && adjustx <= (rect.x + winwidth)) &&
                                (adjusty >= rect.y && adjusty <= (rect.y + winheight))) {
                                    process_event = TRUE;
                            }
                            /* Process this event regardless of the coordinate if the top window has the POPUP_GRAB layout style */
                            if (index == SCL_WINDOW_Z_TOP) {
                                const SclLayout *layout = cache->get_cur_layout(window);
                                if (layout) {
                                    if (layout->style == LAYOUT_STYLE_POPUP_GRAB) {
                                        process_event = TRUE;
                                    }
                                }
                            }
                            if (process_event)
                            {
                                /* Now convert the global coordinate to appropriate local coordinate */
                                SclPoint coords = get_rotated_local_coords(ev->root.x, ev->root.y, context->get_rotation(), &rect);

                                controller->mouse_move(window, coords.x, coords.y, ev->multi.device);
                                processed = TRUE;
                            }
                        }
                    }
                }
                index++;
            } while (index < MAX_ZORDER_NUM && window != SCLWINDOW_INVALID && !processed);
        }

        if (!processed) {
            window = pressed_window;
            if (get_window_rect(window, &rect)) {
                /* Now convert the global coordinate to appropriate local coordinate */
                SclPoint coords = get_rotated_local_coords(ev->root.x, ev->root.y, context->get_rotation(), &rect);
                controller->mouse_move(window, coords.x, coords.y, ev->multi.device);
                processed = TRUE;
            }
        }
    }
    //CSCLController *controller = CSCLController::get_instance();
    //CSCLWindows *windows = CSCLWindows::get_instance();
    //controller->mouse_move((sclwindow)data, (int)ev->cur.output.x, (int)ev->cur.output.y);
    //controller->mouse_move((sclwindow)data, (int)ev->x, (int)ev->y);

    return TRUE;
}

/**
 * Regists a event callback func to given window.
 * In this function, it should call several event functions of CSCLController class whenever an event has occurred
 * The below list shows what event function should be called.
 * - mouse_press (when the user presses mouse button)
 * - mouse_release (when the user releases mouse button)
 * - mouse_move (when the user drags mouse button)
 * - show_base_layout (when the expost event has occurred)
 */
void
CSCLEventsImplEfl::connect_window_events(const sclwindow wnd, const sclint evt)
{
    SCL_DEBUG();

    //evas_object_event_callback_add((Evas_Object*)wnd, EVAS_CALLBACK_MOUSE_DOWN, mouse_press, NULL);
    /*evas_object_event_callback_add((Evas_Object*)wnd, EVAS_CALLBACK_MOUSE_UP, mouse_release, NULL);
    evas_object_event_callback_add((Evas_Object*)wnd, EVAS_CALLBACK_MOUSE_MOVE, mouse_move, NULL);*/
}

Eina_Bool
client_message_cb(void *data, int type, void *event)
{
    Ecore_X_Event_Client_Message *ev = (Ecore_X_Event_Client_Message *)event;
    if (ev->message_type == ECORE_X_ATOM_E_ILLUME_ACCESS_CONTROL) {
        CSCLWindows *windows = CSCLWindows::get_instance();
        CSCLController *controller = CSCLController::get_instance();

        static int last_pos_x = -10000;
        static int last_pos_y = -10000;

        if (windows && controller) {
            Evas_Object *base_win = (Evas_Object *)windows->get_base_window();
            if (base_win == NULL) return FALSE;

            if ((unsigned int)ev->data.l[0] == elm_win_xwindow_get(base_win) ) {
                if ((unsigned int)ev->data.l[1] == ECORE_X_ATOM_E_ILLUME_ACCESS_ACTION_ACTIVATE) {
                // 1 finger double tap
                controller->mouse_press(base_win, last_pos_x, last_pos_y);
                controller->mouse_release(base_win, last_pos_x, last_pos_y);
                } else if ((unsigned int)ev->data.l[1] == ECORE_X_ATOM_E_ILLUME_ACCESS_ACTION_READ) {
                    // 1 finger tap
                    // 1 finger touch & move
                    last_pos_x = ev->data.l[2];
                    last_pos_y = ev->data.l[3];
                    controller->mouse_over(base_win, last_pos_x, last_pos_y);
                }
            }
        }
    }
    return ECORE_CALLBACK_PASS_ON;
}

Eina_Bool timer_event(void *data)
{
    SCL_DEBUG();
    CSCLUtils *utils = CSCLUtils::get_instance();
    CSCLController *controller = CSCLController::get_instance();

    scl32 sendData = static_cast<scl32>(reinterpret_cast<uintptr_t>(data) & 0xffffffff);

    if (controller && utils) {
        scl16 id = SCL_LOWORD(sendData); /* Timer ID */
        Eina_Bool ret = controller->timer_event(sendData);
        if (!ret) {
            utils->log("Returning Timer : %d %d\n", id, ret);
        }
        return ret;
    }
    return TRUE;
}

/**
 * Creates a timer
 * In this function, it should call timer_event of CSCLController class
 */
void
CSCLEventsImplEfl::create_timer(const scl16 id, const scl32 interval, scl16 value, sclboolean addToMap)
{
    SCL_DEBUG();
    sclint data = SCL_MAKELONG(id, value);
    Ecore_Timer *pTimer = ecore_timer_add((double)interval / 1000.0, timer_event, (void*)data);
    if (pTimer) {
        CSCLUtils *utils = CSCLUtils::get_instance();
        if (utils) {
            utils->log("Created Timer : %d %p\n", id, pTimer);
        }
        if (addToMap) {
            idMap[id] = pTimer;
        }
    }
}

/**
 * Destroys the given ID's timer
 */
void
CSCLEventsImplEfl::destroy_timer(const scl32 id)
{
    SCL_DEBUG();
    //for ( std::map<int, Ecore_Timer*>::iterator idx = idMap.begin(); idx != idMap.end(); ++idx) {
        std::map<int, Ecore_Timer*>::iterator idx = idMap.find(id);
        //if ((*idx).first == id) {
        if (idx != idMap.end()) {
            CSCLUtils *utils = CSCLUtils::get_instance();
            if (utils) {
                utils->log("Destoyed Timer : %d %p\n", (*idx).first, (*idx).second);
            }
            ecore_timer_del((*idx).second);
            idMap.erase((*idx).first);
            //break;
        }
    //}
}

/**
 * Destroys all of created timer
 */
void
CSCLEventsImplEfl::destroy_all_timer()
{
    SCL_DEBUG();
    for ( std::map<int, Ecore_Timer*>::iterator idx = idMap.begin(); idx != idMap.end(); ++idx) {
        ecore_timer_del((*idx).second);

        CSCLUtils *utils = CSCLUtils::get_instance();
        if (utils) {
            utils->log("Destroyed Timer : %d %p\n", (*idx).first, (*idx).second);
        }
    }
    idMap.clear();
}

void
CSCLEventsImplEfl::generate_mouse_event(SCLMouseEvent type, scl16 x, scl16 y)
{
    CSCLWindows *windows = CSCLWindows::get_instance();
    SclWindowContext *window_context = NULL;

    static const sclint MAX_DEVICES = 100;
    static sclboolean pressed[MAX_DEVICES] = { FALSE };
    if (windows) {
        switch (type) {
            case SCL_MOUSE_EVENT_PRESS:
            {
                sclboolean generated = FALSE;
                for (sclint loop = 0; !generated && loop < MAX_DEVICES; loop++) {
                    if (pressed[loop] != TRUE) {
                        pressed[loop] = TRUE;
                        Ecore_Event_Mouse_Button evt;
                        evt.window = elm_win_xwindow_get(static_cast<Evas_Object*>(windows->get_base_window()));
                        //window_context = windows->get_window_context(windows->get_base_window(), FALSE);
                        window_context = windows->get_window_context(windows->get_base_window());
                        if (window_context) {
                            evt.root.x = x + window_context->geometry.x;
                            evt.root.y = y + window_context->geometry.y;
                            evt.multi.device = loop;
                            mouse_press(NULL, 0, &evt);
                        }
                        generated = TRUE;
                    }
                }
            }
            break;
            case SCL_MOUSE_EVENT_RELEASE:
            {
                sclboolean generated = FALSE;
                for (sclint loop = 0; !generated && loop < MAX_DEVICES; loop++) {
                    if (pressed[loop] == TRUE) {
                        pressed[loop] = FALSE;
                        Ecore_Event_Mouse_Button evt;
                        evt.window = elm_win_xwindow_get(static_cast<Evas_Object*>(windows->get_base_window()));
                        //window_context = windows->get_window_context(windows->get_base_window(), FALSE);
                        window_context = windows->get_window_context(windows->get_base_window());
                        if (window_context) {
                            evt.root.x = x + window_context->geometry.x;
                            evt.root.y = y + window_context->geometry.y;
                            evt.multi.device = loop;
                            mouse_release(NULL, 0, &evt);
                        }
                        generated = TRUE;
                    }
                }
            }
            break;
            case SCL_MOUSE_EVENT_MOVE:
            {
                sclboolean generated = FALSE;
                for (sclint loop = 0; !generated && loop < MAX_DEVICES; loop++) {
                    if (pressed[loop] == TRUE) {
                        Ecore_Event_Mouse_Move evt;
                        evt.window = elm_win_xwindow_get(static_cast<Evas_Object*>(windows->get_base_window()));
                        //window_context = windows->get_window_context(windows->get_base_window(), FALSE);
                        window_context = windows->get_window_context(windows->get_base_window());
                        if (window_context) {
                            evt.root.x = x + window_context->geometry.x;
                            evt.root.y = y + window_context->geometry.y;
                            evt.multi.device = loop;
                            mouse_move(NULL, 0, &evt);
                        }
                        generated = TRUE;
                    }
                }
            }
            break;
            default:
            break;
        }
    }
}
