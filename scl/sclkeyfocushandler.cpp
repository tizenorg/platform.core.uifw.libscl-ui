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
#include "sclkeyfocushandler.h"
#include <Elementary.h>
#include <Evas.h>
#include <dlog.h>
#include <Ecore_X.h>
#include <utilX.h>
#include "sclres_type.h"
#include "scldebug.h"
#include "sclcontext.h"
#include "sclresourcecache.h"
#include "sclwindows.h"
#include "scleventhandler.h"
#include "sclres_manager.h"

using namespace scl;

/**
 * Constructor
 */
CSCLKeyFocusHandler::CSCLKeyFocusHandler()
{
    SCL_DEBUG();
#ifdef USING_KEY_GRAB
    m_keyboard_grabbed = FALSE;
#endif
    m_focus_key = NOT_USED;
    m_focus_window = SCLWINDOW_INVALID;

#ifdef TARGET_EMULATOR
    create_sniffer_window();
#endif
}

/**
 * Destructor
 */
CSCLKeyFocusHandler::~CSCLKeyFocusHandler()
{
    SCL_DEBUG();
}

CSCLKeyFocusHandler*
CSCLKeyFocusHandler::get_instance()
{
    static CSCLKeyFocusHandler instance;
    return &instance;
}

#ifdef USING_KEY_GRAB
/**
 * Grabs the navigation and Return keys
 */
bool
CSCLKeyFocusHandler::grab_keyboard(const sclwindow parent)
{
    Evas_Object *window = (Evas_Object *)parent;
    Ecore_X_Window x_window = elm_win_xwindow_get(window);

    Display *x_display = (Display *)ecore_x_display_get();
    int grab_result;

    grab_result = utilx_grab_key(x_display, x_window, "Return", EXCLUSIVE_GRAB);
    if (0 == grab_result) {
        LOGD("Return Key Grabbed successfully\n");
    } else if (EXCLUSIVE_GRABBED_ALREADY == grab_result) {
        LOGD("Return Key already grabbed in Exclusiv mode\n");
    } else {
        LOGD("Failed to Grab Return key\n");
    }
    m_keyboard_grabbed = TRUE;
    return TRUE;
}

/**
 * UnGrabs the navigation and Return keys
 */
void
CSCLKeyFocusHandler::ungrab_keyboard(const sclwindow parent)
{
    Evas_Object *window = (Evas_Object *)parent;
    Ecore_X_Window x_window = elm_win_xwindow_get(window);
    Display *x_display = (Display *)ecore_x_display_get();
    int grab_result;
    grab_result = utilx_ungrab_key(x_display, x_window, "Return");
    if(0 == grab_result) {
        LOGD("Return Key UnGrabbed successfully\n");
    } else {
        LOGD("Failed to UnGrab Return key\n");
    }
    m_keyboard_grabbed = FALSE;
}

#endif /*USING_KEY_GRAB*/


void
CSCLKeyFocusHandler::popup_opened(sclwindow window)
{

}

void
CSCLKeyFocusHandler::popup_closed(sclwindow window)
{
    sclshort layout = NOT_USED;
    CSCLContext *context = CSCLContext::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();
    SclResParserManager *sclres_manager = SclResParserManager::get_instance();
    PSclLayoutKeyCoordinatePointerTable sclres_layout_key_coordinate_pointer_frame = NULL;

    if (context && windows && sclres_manager) {
        sclres_layout_key_coordinate_pointer_frame = sclres_manager->get_key_coordinate_pointer_frame();
        layout = context->get_popup_layout(m_focus_window);
    }
    if (sclres_layout_key_coordinate_pointer_frame &&
        scl_check_arrindex(layout, MAX_SCL_LAYOUT) && scl_check_arrindex(m_focus_key, MAX_KEY)) {
            SclLayoutKeyCoordinatePointer cur = sclres_layout_key_coordinate_pointer_frame[layout][m_focus_key];
            SclWindowContext *winctx = windows->get_window_context(m_focus_window);
            SclRectangle cur_key_coordinate;
            if (winctx) {
                cur_key_coordinate.x = cur->x + winctx->geometry.x;
                cur_key_coordinate.y = cur->y + winctx->geometry.y;
                cur_key_coordinate.width = cur->width;
                cur_key_coordinate.height = cur->height;
            }

            m_focus_window = windows->get_base_window();
            m_focus_key = get_next_candidate_key(HIGHLIGHT_NAVIGATE_NONE, cur_key_coordinate, windows->get_base_window()).candidate;
    }
}

/**
 * Resets the navigation info
 */
void
CSCLKeyFocusHandler::reset_key_navigation_info(sclwindow window)
{
    CSCLWindows *windows = CSCLWindows::get_instance();
    if (windows) {
        if (windows->is_base_window(window)) {
            m_focus_window = window;
            m_focus_key = 0;
        }
    }
}

/**
 * Compares the sub-layout values
 */
inline bool
CSCLKeyFocusHandler::sub_layout_match(sclchar *layout1,sclchar *layout2)
{
    if (layout1) {
        if (layout2) {
            if (strcmp(layout1, layout2) == 0) {
                return TRUE;
            }
        }
    } else if (layout2) {
        return FALSE;
    } else {
        return TRUE;
    }
    return FALSE;
}

/**
 * Builds the key navigation info
 */
void
CSCLKeyFocusHandler::update_key_navigation_info(sclwindow window, scl8 index, SclLayoutKeyCoordinatePointer p_next_key)
{

}

/**
 * Finalize the navigation info
 */
void
CSCLKeyFocusHandler::finalize_key_navigation_info(sclwindow window)
{

}

/**
 * Initializes the key index to first key of first row
 */
void
CSCLKeyFocusHandler::init_key_index()
{

}

/**
 * Returns the currently focused key index
 */
scl8
CSCLKeyFocusHandler::get_current_focus_key(void)
{
    return m_focus_key;
}

/**
 * Returns the currently focused window
 */
sclwindow
CSCLKeyFocusHandler::get_current_focus_window(void)
{
    return m_focus_window;
}

#ifndef min
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif
/* If 2 lines overlap, this will return minus value of overlapping length,
    and return positive distance value otherwise */
int calculate_distance(int start_1, int end_1, int start_2, int end_2)
{
    return -1 * (min(end_1, end_2) - max(start_1, start_2));
}

/**
 * Computes and Returns the key index for next focussed key depending upon the navigation direction
 */
NEXT_CANDIDATE_INFO
CSCLKeyFocusHandler::get_next_candidate_key(SCLHighlightNavigationDirection direction, SclRectangle cur, sclwindow window)
{
    NEXT_CANDIDATE_INFO ret;
    ret.candidate = NOT_USED;
    ret.candidate_otherside = NOT_USED;

    int candidate = NOT_USED;
    int candidate_distance_x = INT_MAX;
    int candidate_distance_y = INT_MAX;

    int otherside_candidate = NOT_USED;

    sclshort layout = NOT_USED;

    CSCLContext *context = CSCLContext::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();

    SclResParserManager *sclres_manager = SclResParserManager::get_instance();
    PSclLayoutKeyCoordinatePointerTable sclres_layout_key_coordinate_pointer_frame = NULL;

    if (context && windows && sclres_manager) {
        sclres_layout_key_coordinate_pointer_frame = sclres_manager->get_key_coordinate_pointer_frame();

        if (windows->is_base_window(window)) {
            layout = context->get_base_layout();
        } else {
            layout = context->get_popup_layout(window);
        }
    }
    if (sclres_layout_key_coordinate_pointer_frame) {
        for (sclint loop = 0;loop < MAX_KEY; loop++) {
            SclLayoutKeyCoordinatePointer p = sclres_layout_key_coordinate_pointer_frame[layout][loop];
            if (p && (loop != m_focus_key || window != m_focus_window)) {
                if (p->sub_layout && context->get_cur_sublayout()) {
                    if (!sub_layout_match(p->sub_layout, context->get_cur_sublayout())) {
                        continue;
                    }
                }
                SclWindowContext *winctx = windows->get_window_context(window);
                SclRectangle btn;
                if (winctx) {
                    btn.x = p->x + winctx->geometry.x;
                    btn.y = p->y + winctx->geometry.y;
                    btn.width = p->width;
                    btn.height = p->height;
                }

                int temp_distance_x;
                int temp_distance_y;

                switch(direction) {
                    case HIGHLIGHT_NAVIGATE_LEFT:
                        temp_distance_y = calculate_distance(btn.y, btn.y + btn.height, cur.y, cur.y + cur.height);
                        //if (temp_distance_y <= candidate_distance_y) {
                        if (temp_distance_y <= candidate_distance_y && temp_distance_y < 0) {
                            /* If the button is closer in Y axis, consider this to be the closer regardless of X */
                            if (temp_distance_y < candidate_distance_y) {
                                candidate_distance_x = INT_MAX;
                            }
                            /* Save for otherside */
                            otherside_candidate = loop;
                            int temp_distance_x = calculate_distance(btn.x, btn.x + btn.width, cur.x, cur.x + cur.width);
                            if (temp_distance_x < candidate_distance_x) {
                                if (btn.x < cur.x) {
                                    candidate = loop;
                                    candidate_distance_x = temp_distance_x;
                                    candidate_distance_y = temp_distance_y;
                                }
                            }
                        }
                        break;
                    case HIGHLIGHT_NAVIGATE_RIGHT:
                        temp_distance_y = calculate_distance(btn.y, btn.y + btn.height, cur.y, cur.y + cur.height);
                        //if (temp_distance_y <= candidate_distance_y) {
                        if (temp_distance_y <= candidate_distance_y && temp_distance_y < 0) {
                            /* If the button is closer in Y axis, consider this to be the closer regardless of X */
                            if (temp_distance_y < candidate_distance_y) {
                                candidate_distance_x = INT_MAX;
                            }
                            /* Save for otherside */
                            if (otherside_candidate == NOT_USED) {
                                otherside_candidate = loop;
                            }
                            temp_distance_x = calculate_distance(btn.x, btn.x + btn.width, cur.x, cur.x + cur.width);
                            if (temp_distance_x < candidate_distance_x) {
                                if (btn.x > cur.x) {
                                    candidate = loop;
                                    candidate_distance_x = temp_distance_x;
                                    candidate_distance_y = temp_distance_y;
                                }
                            }
                        }
                        break;
                    case HIGHLIGHT_NAVIGATE_UP:
                        temp_distance_x = calculate_distance(btn.x, btn.x + btn.width, cur.x, cur.x + cur.width);
                        if (temp_distance_x <= candidate_distance_x) {
                            /* If the button is closer in X axis, consider this to be the closer regardless of Y */
                            if (temp_distance_x < candidate_distance_x) {
                                candidate_distance_y = INT_MAX;
                            }
                            temp_distance_y = calculate_distance(btn.y, btn.y + btn.height, cur.y, cur.y + cur.height);
                            if (temp_distance_y < candidate_distance_y) {
                                if (btn.y < cur.y) {
                                    candidate = loop;
                                    candidate_distance_x = temp_distance_x;
                                    candidate_distance_y = temp_distance_y;
                                }
                            }
                        }
                        break;
                    case HIGHLIGHT_NAVIGATE_DOWN:
                        temp_distance_x = calculate_distance(btn.x, btn.x + btn.width, cur.x, cur.x + cur.width);
                        if (temp_distance_x <= candidate_distance_x) {
                            /* If the button is closer in X axis, consider this to be the closer regardless of Y */
                            if (temp_distance_x < candidate_distance_x) {
                                candidate_distance_y = INT_MAX;
                            }
                            temp_distance_y = calculate_distance(btn.y, btn.y + btn.height, cur.y, cur.y + cur.height);
                            if (temp_distance_y < candidate_distance_y) {
                                if (btn.y > cur.y) {
                                    candidate = loop;
                                    candidate_distance_x = temp_distance_x;
                                    candidate_distance_y = temp_distance_y;
                                }
                            }
                        }
                        break;
                    default:
                        temp_distance_y = calculate_distance(btn.y, btn.y + btn.height, cur.y, cur.y + cur.height);
                        if (temp_distance_y <= candidate_distance_y) {
                            int temp_distance_x = calculate_distance(btn.x, btn.x + btn.width, cur.x, cur.x + cur.width);
                            if (temp_distance_x <= candidate_distance_x) {
                                candidate = loop;
                                candidate_distance_x = temp_distance_x;
                                candidate_distance_y = temp_distance_y;
                            }
                        }
                        break;
                }
            }
        }

        ret.candidate = candidate;
        ret.candidate_otherside = otherside_candidate;
    }

    return ret;
}

void
CSCLKeyFocusHandler::process_navigation(SCLHighlightNavigationDirection direction)
{
    CSCLContext *context = CSCLContext::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLEventHandler *handler = CSCLEventHandler::get_instance();
    SclResParserManager *sclres_manager = SclResParserManager::get_instance();
    sclshort layout = NOT_USED;

    sclwindow prev_window = m_focus_window;
    scl8 prev_key = m_focus_key;
    sclwindow next_window = m_focus_window;
    scl8 next_key = m_focus_key;

    PSclLayoutKeyCoordinatePointerTable sclres_layout_key_coordinate_pointer_frame = NULL;
    
    if (context && windows && handler && sclres_manager) {
        sclres_layout_key_coordinate_pointer_frame = sclres_manager->get_key_coordinate_pointer_frame();

        if (windows->is_base_window(m_focus_window)) {
            layout = context->get_base_layout();
        } else {
            layout = context->get_popup_layout(m_focus_window);
        }
    }

    if (sclres_layout_key_coordinate_pointer_frame &&
        scl_check_arrindex(layout, MAX_SCL_LAYOUT) && scl_check_arrindex(m_focus_key, MAX_KEY)) {
        SclLayoutKeyCoordinatePointer cur = sclres_layout_key_coordinate_pointer_frame[layout][m_focus_key];

        /* To compare with buttons in popup window, let's convert to global coordinates */
        SclWindowContext *winctx = windows->get_window_context(m_focus_window);
        SclRectangle cur_key_coordinate;
        if (winctx && cur) {
            cur_key_coordinate.x = cur->x + winctx->geometry.x;
            cur_key_coordinate.y = cur->y + winctx->geometry.y;
            cur_key_coordinate.width = cur->width;
            cur_key_coordinate.height = cur->height;
        }

        NEXT_CANDIDATE_INFO base_candidate;
        base_candidate.candidate = base_candidate.candidate_otherside = NOT_USED;
        NEXT_CANDIDATE_INFO popup_candidate;
        popup_candidate.candidate = popup_candidate.candidate_otherside = NOT_USED;
        sclboolean search_in_base_window = TRUE;
        sclboolean exclude_popup_covered_area = FALSE;

        if (!windows->is_base_window(windows->get_nth_window_in_Z_order_list(SCL_WINDOW_Z_TOP))) {
            CSCLResourceCache *cache = CSCLResourceCache::get_instance();
            if (cache) {
                const SclLayout *layout =
                    cache->get_cur_layout(windows->get_nth_window_in_Z_order_list(SCL_WINDOW_Z_TOP));
                if (!(layout->use_sw_background) || layout->bg_color.a != 0) {
                    const PSclInputModeConfigure sclres_input_mode_configure =
                        sclres_manager->get_input_mode_configure_table();
                    if (sclres_input_mode_configure[winctx->inputmode].use_dim_window) {
                        search_in_base_window = FALSE;
                    } else {
                        exclude_popup_covered_area = TRUE;
                    }
                }
            }
            popup_candidate = get_next_candidate_key(direction, cur_key_coordinate,
                windows->get_nth_window_in_Z_order_list(SCL_WINDOW_Z_TOP));
        }
        /* Now search buttons in base window */
        if (search_in_base_window) {
            base_candidate = get_next_candidate_key(direction, cur_key_coordinate, windows->get_base_window());
        }

        if (popup_candidate.candidate == NOT_USED && base_candidate.candidate != NOT_USED) {
            next_window = windows->get_base_window();
            next_key = base_candidate.candidate;
        } else if (popup_candidate.candidate != NOT_USED && base_candidate.candidate == NOT_USED) {
            next_window = windows->get_nth_window_in_Z_order_list(SCL_WINDOW_Z_TOP);
            next_key = popup_candidate.candidate;
        } else if (popup_candidate.candidate == NOT_USED && base_candidate.candidate == NOT_USED) {
            if (base_candidate.candidate_otherside != NOT_USED) {
                next_window = windows->get_base_window();
                next_key = base_candidate.candidate_otherside;
            } else if (popup_candidate.candidate_otherside != NOT_USED) {
                next_window = windows->get_nth_window_in_Z_order_list(SCL_WINDOW_Z_TOP);
                next_key = popup_candidate.candidate;
            }
        } else {
            /* Compare those 2 candidates */
            sclwindow popup_window = windows->get_nth_window_in_Z_order_list(SCL_WINDOW_Z_TOP);
            sclshort base_layout = context->get_base_layout();
            sclshort popup_layout = context->get_popup_layout(popup_window);

            SclLayoutKeyCoordinatePointer base_key =
                sclres_layout_key_coordinate_pointer_frame[base_layout][base_candidate.candidate];
            SclLayoutKeyCoordinatePointer popup_key =
                sclres_layout_key_coordinate_pointer_frame[popup_layout][popup_candidate.candidate];

            SclWindowContext *base_winctx = windows->get_window_context(windows->get_base_window());
            SclWindowContext *popup_winctx = windows->get_window_context(popup_window);

            SclRectangle base_key_coordinate;
            if (base_winctx) {
                base_key_coordinate.x = base_key->x + base_winctx->geometry.x;
                base_key_coordinate.y = base_key->y + base_winctx->geometry.y;
                base_key_coordinate.width = base_key->width;
                base_key_coordinate.height = base_key->height;
            }

            SclRectangle popup_key_coordinate;
            if (popup_winctx) {
                popup_key_coordinate.x = popup_key->x + popup_winctx->geometry.x;
                popup_key_coordinate.y = popup_key->y + popup_winctx->geometry.y;
                popup_key_coordinate.width = popup_key->width;
                popup_key_coordinate.height = popup_key->height;
            }

            if (exclude_popup_covered_area) {
                CSCLUtils *utils = CSCLUtils::get_instance();
                if (utils) {
                    /* If the base candidate key is covered by popup window, do not choose it */
                    if (utils->is_rect_overlap(base_key_coordinate, popup_winctx->geometry)) {
                        base_candidate.candidate = NOT_USED;
                    }
                }
            }
            /* If the base candidate key wasn't excluded */
            if (base_candidate.candidate != NOT_USED) {
                int base_distance_x = INT_MAX;
                int base_distance_y = INT_MAX;
                int popup_distance_x = INT_MAX;
                int popup_distance_y = INT_MAX;

                base_distance_x = calculate_distance(
                    cur_key_coordinate.x, cur_key_coordinate.x + cur_key_coordinate.width,
                    base_key_coordinate.x, base_key_coordinate.x + base_key_coordinate.width);
                base_distance_y = calculate_distance(
                    cur_key_coordinate.y, cur_key_coordinate.y + cur_key_coordinate.height,
                    base_key_coordinate.y, base_key_coordinate.y + base_key_coordinate.height);

                popup_distance_x = calculate_distance(
                    cur_key_coordinate.x, cur_key_coordinate.x + cur_key_coordinate.width,
                    popup_key_coordinate.x, popup_key_coordinate.x + popup_key_coordinate.width);
                popup_distance_y = calculate_distance(
                    cur_key_coordinate.y, cur_key_coordinate.y + cur_key_coordinate.height,
                    popup_key_coordinate.y, popup_key_coordinate.y + popup_key_coordinate.height);

                if (direction == HIGHLIGHT_NAVIGATE_LEFT || direction == HIGHLIGHT_NAVIGATE_RIGHT) {
                    int minimum_distance = -1 * (cur_key_coordinate.height / 3);
                    if (base_distance_y > minimum_distance && popup_distance_y > minimum_distance) {
                        minimum_distance = 0;
                    }
                    if (base_distance_y < minimum_distance && popup_distance_y < minimum_distance) {
                        if (base_distance_x < popup_distance_x) {
                            next_window = windows->get_base_window();
                            next_key = base_candidate.candidate;
                        } else {
                            next_window = windows->get_nth_window_in_Z_order_list(SCL_WINDOW_Z_TOP);
                            next_key = popup_candidate.candidate;
                        }
                    } else if (base_distance_y < minimum_distance) {
                        next_window = windows->get_base_window();
                        next_key = base_candidate.candidate;
                    } else {
                        next_window = windows->get_nth_window_in_Z_order_list(SCL_WINDOW_Z_TOP);
                        next_key = popup_candidate.candidate;
                    }
                } else if (direction == HIGHLIGHT_NAVIGATE_UP || direction == HIGHLIGHT_NAVIGATE_DOWN) {
                    int minimum_distance = -1 * (cur_key_coordinate.width / 3);
                    if (base_distance_x > minimum_distance && popup_distance_x > minimum_distance) {
                        minimum_distance = 0;
                    }
                    if (base_distance_x < minimum_distance && popup_distance_x < minimum_distance) {
                        if (base_distance_y < popup_distance_y) {
                            next_window = windows->get_base_window();
                            next_key = base_candidate.candidate;
                        } else {
                            next_window = windows->get_nth_window_in_Z_order_list(SCL_WINDOW_Z_TOP);
                            next_key = popup_candidate.candidate;
                        }
                    } else if (base_distance_x < minimum_distance) {
                        next_window = windows->get_base_window();
                        next_key = base_candidate.candidate;
                    } else {
                        next_window = windows->get_nth_window_in_Z_order_list(SCL_WINDOW_Z_TOP);
                        next_key = popup_candidate.candidate;
                    }
                }
            }
        }
    }
}

#ifdef TARGET_EMULATOR

static Ecore_Event_Handler *_sniffer_win_show_handler         = NULL;

/**
 * ecore event handler deletion
 */
static void delete_sniffer_win_show_handler (void)
{
    if (_sniffer_win_show_handler) {
        ecore_event_handler_del (_sniffer_win_show_handler);
        _sniffer_win_show_handler = NULL;
    }
}

/**
 * callback for window show event (sniffer window)
 */
static Eina_Bool x_event_sniffer_window_show_cb (void *data, int ev_type, void *event)
{

    Evas_Object *evas_window = (Evas_Object *)data;
    Ecore_X_Window x_window = elm_win_xwindow_get(evas_window);
    Ecore_X_Event_Window_Show *e = (Ecore_X_Event_Window_Show*)event;

    if (e->win == x_window) {

        LOGD("INSIDE =-=-=-=- x_event_sniffer_window_show_cb, Trying to Grab Key Board : \n");
        Eina_Bool ret = ecore_x_keyboard_grab(x_window);

        if (EINA_TRUE == ret) {
            LOGD("Keyboard Grabbed successfully by sniffer\n");
        } else {
            LOGD("Failed to Grab keyboard by sniffer\n");
        }
        ecore_event_handler_del(_sniffer_win_show_handler);
        return ECORE_CALLBACK_CANCEL;
    }
    LOGD("Wrong window .. renewing callback\n");
    return ECORE_CALLBACK_RENEW;
}

/**
 * sniffer window creation function, the keyboard would be grabbed by this window in case of Tizen Emulator
 */
void
CSCLKeyFocusHandler::create_sniffer_window(void)
{
    LOGD("CSCLKeyFocusHandler : INSIDE =-=-=-=- create_sniffer_window : \n");	
    Evas_Object *win = NULL;

    win = elm_win_add(NULL, "KEY_SNIFFER", ELM_WIN_UTILITY);

    elm_win_borderless_set(win, EINA_TRUE);
    elm_win_alpha_set(win, EINA_FALSE);
    elm_win_title_set(win, "KEY_SNIFFER");
    elm_win_fullscreen_set(win,EINA_FALSE);
    set_window_accepts_focus(win, FALSE);
    evas_object_show(win);
    evas_object_resize(win, 100, 100);
    m_sniffer = win;
    _sniffer_win_show_handler = ecore_event_handler_add (ECORE_X_EVENT_WINDOW_SHOW, x_event_sniffer_window_show_cb, m_sniffer);
}

void
CSCLKeyFocusHandler::set_window_accepts_focus(const sclwindow window, sclboolean acceptable)
{
    Eina_Bool accepts_focus;
    Ecore_X_Window_State_Hint initial_state;
    Ecore_X_Pixmap icon_pixmap;
    Ecore_X_Pixmap icon_mask;
    Ecore_X_Window icon_window;
    Ecore_X_Window window_group;
    Eina_Bool is_urgent;

    if (window) {
        ecore_x_icccm_hints_get(elm_win_xwindow_get(static_cast<Evas_Object*>(window)),
            &accepts_focus, &initial_state, &icon_pixmap, &icon_mask, &icon_window, &window_group, &is_urgent);
        ecore_x_icccm_hints_set(elm_win_xwindow_get(static_cast<Evas_Object*>(window)),
            acceptable, initial_state, icon_pixmap, icon_mask, icon_window, window_group, is_urgent);
    }
}


#endif


