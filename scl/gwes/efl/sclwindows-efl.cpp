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

#include "sclwindows-efl.h"
#include "scldebug.h"
#include "sclcontroller.h"
#include "sclcontext.h"
#include "sclresourcecache.h"
#include "scluibuilder.h"
#include "sclwindows.h"

#include <glib.h>
#include <Elementary.h>
#include <Ecore_X.h>
#include <malloc.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <utilX.h>
#include <dlog.h>
#include "sclkeyfocushandler.h"

using namespace scl;

static Ecore_X_Atom ATOM_WM_CLASS = 0;
static Ecore_X_Window app_window = 0;

const sclint rotation_values_EFL[ROTATION_MAX] = {
    0, // ROTATION_0
    90, // ROTATION_90_CW
    180, // ROTATION_180
    270, // ROTATION_90_CCW
};

void release_all(Evas_Object *win);

#include "sclgraphics-efl.h"
#include <utilX.h>
#ifdef TEST_NEWBACKEND
#include <Ecore_Evas.h>
#include <Ecore.h>
#include <vector>
extern std::vector<ImageCache> g_ImageCache;
extern std::vector<TextCache> g_TextCache;
#else
#endif

/**
 * Constructor
 */
CSCLWindowsImplEfl::CSCLWindowsImplEfl()
{
    SCL_DEBUG();
    /* Initializes all window resources */
    ATOM_WM_CLASS = ecore_x_atom_get("WM_CLASS");
}

/**
 * De-constructor
 */
CSCLWindowsImplEfl::~CSCLWindowsImplEfl()
{
    SCL_DEBUG();
}

static Ecore_Event_Handler *_candidate_show_handler         = NULL;

static Eina_Bool x_event_window_show_cb (void *data, int ev_type, void *event)
{
    CSCLWindows *windows = CSCLWindows::get_instance();
    Evas_Object *window = (Evas_Object *)windows->get_base_window();
    Ecore_X_Event_Window_Show *e = (Ecore_X_Event_Window_Show*)event;

    if(e->win == elm_win_xwindow_get(window)) {
        LOGD("INSIDE =-=-=-=- x_event_window_show_cb, Trying to Grab Key Board : \n");
#ifdef USING_KEY_GRAB
        CSCLKeyFocusHandler* focus_handler = CSCLKeyFocusHandler::get_instance();
        focus_handler->grab_keyboard(windows->get_base_window());
#endif
    }
    return ECORE_CALLBACK_RENEW;
}


void CSCLWindowsImplEfl::init()
{

}

void CSCLWindowsImplEfl::fini()
{

}

/**
 * Create a content window and binds it into given parent window as a child
 */
sclwindow
CSCLWindowsImplEfl::create_base_window(const sclwindow parent, SclWindowContext *window_context, scl16 width, scl16 height)
{
    SCL_DEBUG();

    sclwindow ret = SCLWINDOW_INVALID;

    if (window_context) {
        window_context->etc_info = NULL;
        window_context->window = parent;

    //Adding window show event handler
    _candidate_show_handler = ecore_event_handler_add (ECORE_X_EVENT_WINDOW_SHOW, x_event_window_show_cb, NULL);

#ifndef APPLY_WINDOW_MANAGER_CHANGE
        ecore_x_icccm_name_class_set(elm_win_xwindow_get(static_cast<Evas_Object*>(parent)), "Virtual Keyboard", "ISF" );

        set_window_accepts_focus(parent, FALSE);
#else
        if (parent) {
            evas_object_show((Evas_Object*)parent);
            //elm_win_keyboard_mode_set((Evas_Object*)parent, ELM_WIN_KEYBOARD_OFF);
        }
#endif

        ret = window_context->window;
    }

    CSCLUtils *utils = CSCLUtils::get_instance();
    if (utils) {
        utils->log("WinEfl_createbasewin %p %p, %d %d\n",
                parent, elm_win_xwindow_get(static_cast<Evas_Object*>(parent)), width, height);
    }

    return ret;
}

/**
 * Creates a window
 */
sclwindow
CSCLWindowsImplEfl::create_window(const sclwindow parent, SclWindowContext *window_context, scl16 width, scl16 height)
{
    SCL_DEBUG();

    Evas_Object *win = NULL;
    win = elm_win_add(static_cast<Evas_Object*>(parent), "SCLPopup", ELM_WIN_BASIC);

    elm_win_borderless_set(win, EINA_TRUE);
    elm_win_alpha_set(win, EINA_TRUE);
    elm_win_title_set(win, "Keyboard Popup Window");

    scl16 new_width;
    scl16 new_height;
    CSCLContext *context = CSCLContext::get_instance();
    if(context->get_rotation() == ROTATION_90_CW || context->get_rotation() == ROTATION_90_CCW) {
        new_width = height;
        new_height = width;
    } else {
        new_width = width;
        new_height = height;
    }

    ecore_x_e_window_rotation_geometry_set(elm_win_xwindow_get(win),
        rotation_values_EFL[ROTATION_0], 0, 0, new_width, new_height);
    ecore_x_e_window_rotation_geometry_set(elm_win_xwindow_get(win),
        rotation_values_EFL[ROTATION_90_CW], 0, 0, new_width, new_height);
    ecore_x_e_window_rotation_geometry_set(elm_win_xwindow_get(win),
        rotation_values_EFL[ROTATION_180], 0, 0, new_width, new_height);
    ecore_x_e_window_rotation_geometry_set(elm_win_xwindow_get(win),
        rotation_values_EFL[ROTATION_90_CCW], 0, 0, new_width, new_height);

    int rots[4] = {0, 90, 180, 270};
    elm_win_wm_rotation_available_rotations_set(win, rots, 4);
#ifndef FULL_SCREEN_TEST
    //evas_object_resize(win, width, height);
#endif

    const char *szProfile[] = {"mobile", ""};
    elm_win_profiles_set(win, szProfile, 1);

#ifndef APPLY_WINDOW_MANAGER_CHANGE
    ecore_x_icccm_name_class_set(elm_win_xwindow_get(static_cast<Evas_Object*>(win)), "ISF Popup", "ISF");

    set_window_accepts_focus(win, FALSE);

    Ecore_X_Atom ATOM_WINDOW_EFFECT_ENABLE  = 0;
    unsigned int effect_state = 0; // 0 -> disable effect 1-> enable effect

    CSCLUtils *utils = CSCLUtils::get_instance();
    ATOM_WINDOW_EFFECT_ENABLE = ecore_x_atom_get("_NET_CM_WINDOW_EFFECT_ENABLE");
    if (ATOM_WINDOW_EFFECT_ENABLE) {
        ecore_x_window_prop_card32_set(elm_win_xwindow_get(static_cast<Evas_Object*>(win)), ATOM_WINDOW_EFFECT_ENABLE, &effect_state, 1);
    } else {
        if (utils) {
            utils->log("Could not get _NET_CM_WINDOW_EFFECT_ENABLE ATOM \n");
        }
    }

#endif

    set_window_rotation(win, context->get_rotation());

    //elm_win_override_set(win, EINA_TRUE);
    if (utils) {
        utils->log("WinEfl_createwin %p %p, %d %d\n",
                win, elm_win_xwindow_get(static_cast<Evas_Object*>(win)), width, height);
    }

    return win;
}


/**
* Creates a window
*/
sclwindow
CSCLWindowsImplEfl::create_magnifier_window(const sclwindow parent, SclWindowContext *window_context, scl16 width, scl16 height)
{
    SCL_DEBUG();

    Evas_Object *win = NULL;
    win = elm_win_add(static_cast<Evas_Object*>(parent), "Magnifier", ELM_WIN_BASIC);

    elm_win_borderless_set(win, EINA_TRUE);
    elm_win_alpha_set(win, EINA_TRUE);
    elm_win_title_set(win, "Keyboard Magnifier Window");

#ifdef DO_NOT_MOVE_MAGNIFIER_WINDOW
    CSCLUtils *utils = CSCLUtils::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();
    sclint scrx, scry, winx, winy;
    utils->get_screen_resolution(&scrx, &scry);
    SclWindowContext *window_context = windows->get_window_context(windows->get_base_window());
    evas_object_resize(win, scrx, height + window_context->height);
#else
    //evas_object_resize(win, width, height);
#endif

    const char *szProfile[] = {"mobile", ""};
    elm_win_profiles_set(win, szProfile, 1);

    ecore_x_e_window_rotation_geometry_set(elm_win_xwindow_get(win),
        rotation_values_EFL[ROTATION_0], 0, 0, width, height);
    ecore_x_e_window_rotation_geometry_set(elm_win_xwindow_get(win),
        rotation_values_EFL[ROTATION_90_CW], 0, 0, height, width);
    ecore_x_e_window_rotation_geometry_set(elm_win_xwindow_get(win),
        rotation_values_EFL[ROTATION_180], 0, 0, width, height);
    ecore_x_e_window_rotation_geometry_set(elm_win_xwindow_get(win),
        rotation_values_EFL[ROTATION_90_CCW], 0, 0, height, width);

    int rots[4] = {0,90,180,270};
    elm_win_wm_rotation_available_rotations_set(win, rots, 4);

#ifndef APPLY_WINDOW_MANAGER_CHANGE
    ecore_x_icccm_name_class_set(elm_win_xwindow_get(static_cast<Evas_Object*>(win)), "Key Magnifier", "ISF");

    set_window_accepts_focus(win, FALSE);

    Ecore_X_Atom ATOM_WINDOW_EFFECT_ENABLE  = 0;
    unsigned int effect_state = 0; // 0 -> disable effect 1-> enable effect

    CSCLUtils *utils = CSCLUtils::get_instance();
    ATOM_WINDOW_EFFECT_ENABLE = ecore_x_atom_get("_NET_CM_WINDOW_EFFECT_ENABLE");
    if (ATOM_WINDOW_EFFECT_ENABLE) {
        ecore_x_window_prop_card32_set(elm_win_xwindow_get(static_cast<Evas_Object*>(win)), ATOM_WINDOW_EFFECT_ENABLE, &effect_state, 1);
    } else {
        if (utils) {
            utils->log("Could not get _NET_CM_WINDOW_EFFECT_ENABLE ATOM \n");
        }
    }
#endif

    //evas_font_path_prepend(evas_object_evas_get(win), "/usr/share/SLP/fonts");

    CSCLContext *context = CSCLContext::get_instance();
    set_window_rotation(win, context->get_rotation());

    /*Ecore_X_Display* dpy;
    dpy = ecore_x_display_get();
    utilx_set_window_effect_state((Display*)dpy, elm_win_xwindow_get(static_cast<Evas_Object*>(win)), EINA_TRUE);*/

    //elm_win_override_set(win, EINA_TRUE);
    if (utils) {
        utils->log("WinEfl_createmagwin %p %p, %d %d\n",
            win, elm_win_xwindow_get(static_cast<Evas_Object*>(win)), width, height);
    }

    return win;
}

/**
 * Creates the dim window
 */
sclwindow
CSCLWindowsImplEfl::create_dim_window(const sclwindow parent, SclWindowContext *window_context, scl16 width, scl16 height)
{
    SCL_DEBUG();

    Evas_Object *win = NULL;
    win = elm_win_add(static_cast<Evas_Object*>(parent), "SCLPopup", ELM_WIN_BASIC);

    elm_win_borderless_set(win, EINA_TRUE);
    elm_win_alpha_set(win, EINA_TRUE);
    elm_win_title_set(win, "Keyboard Dim Window");

    evas_object_resize(win, width, height);

#ifndef APPLY_WINDOW_MANAGER_CHANGE
    ecore_x_icccm_name_class_set(elm_win_xwindow_get(static_cast<Evas_Object*>(win)), "ISF Popup", "ISF");

    set_window_accepts_focus(win, FALSE);

    Ecore_X_Atom ATOM_WINDOW_EFFECT_ENABLE  = 0;
    unsigned int effect_state = 0; // 0 -> disable effect 1-> enable effect

    CSCLUtils *utils = CSCLUtils::get_instance();
    ATOM_WINDOW_EFFECT_ENABLE = ecore_x_atom_get("_NET_CM_WINDOW_EFFECT_ENABLE");
    if (ATOM_WINDOW_EFFECT_ENABLE) {
        ecore_x_window_prop_card32_set(elm_win_xwindow_get(static_cast<Evas_Object*>(win)), ATOM_WINDOW_EFFECT_ENABLE, &effect_state, 1);
    } else {
        if (utils) {
            utils->log("Could not get _NET_CM_WINDOW_EFFECT_ENABLE ATOM \n");
        }
    }
#endif

    const char *szProfile[] = {"mobile", ""};
    elm_win_profiles_set(win, szProfile, 1);

    CSCLContext *context = CSCLContext::get_instance();
    set_window_rotation(win, context->get_rotation());

    /*Evas_Object *bg;
    bg = elm_bg_add(win);
    elm_win_resize_object_add(win, bg);
    evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    SclColor color = scl_dim_window_configure.dim_color;
    evas_object_color_set(bg, color.r, color.g, color.b, color.a);
    evas_object_show(bg);*/

    hide_window(win);

    if (utils) {
        utils->log("WinEfl_createdimwin %p %p, %d %d\n",
            win, elm_win_xwindow_get(static_cast<Evas_Object*>(win)), width, height);
    }

    return win;
}

/**
 * Make a window relation between parent and child
 * Caution: Currently, If we use transient_for them the ISE will occure some crash. It needs to check X11
 */
void
CSCLWindowsImplEfl::set_parent(const sclwindow parent, const sclwindow window)
{
    SCL_DEBUG();

    if (parent && window) {
        ecore_x_icccm_transient_for_set(elm_win_xwindow_get(static_cast<Evas_Object*>(window)),
                elm_win_xwindow_get(static_cast<Evas_Object*>(parent)));
    }
}

Eina_Bool destroy_later(void *data)
{
    evas_object_hide((Evas_Object*)data);
    evas_object_del((Evas_Object*)data);
    return ECORE_CALLBACK_CANCEL;
}

/**
 * Destroys the given window
 */
bool
CSCLWindowsImplEfl::destroy_window(sclwindow window)
{
    SCL_DEBUG();

    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLUtils *utils = CSCLUtils::get_instance();

    SclWindowContext *window_context = NULL;
    if (windows && window && utils) {
        window_context = windows->get_window_context(window);
        if (window_context) {
            utils->log("WinEfl_destroywin %p %p (basewin %p mag %p)\n", window,
                (!(window_context->is_virtual)) ? elm_win_xwindow_get(static_cast<Evas_Object*>(window)) : 0x01,
                windows->get_base_window(), windows->get_magnifier_window());
            if (window_context->etc_info) {
                Eina_List *list = (Eina_List*)(window_context->etc_info);
                Eina_List *iter = NULL;
                Eina_List *iter_next = NULL;
                void *data = NULL;

                EINA_LIST_FOREACH_SAFE(list, iter, iter_next, data) {
                    if (data) {
                        EFLObject *object = (EFLObject*)(data);
                        if (object) {
                            Evas_Object* eo = object->object;
                            if (object->extracted) {
                                //evas_object_image_data_set(eo, NULL);
                                void *image_data = evas_object_image_data_get(eo, 1);
                                if (image_data) {
                                    free(image_data);
                                }
                            }
                            if (eo) {
                                evas_object_del(eo);
                                object->object = NULL;
                            }
                            if (object->type == EFLOBJECT_TEXTBLOCK) {
                                Evas_Textblock_Style *st = (Evas_Textblock_Style*)(object->data);
                                if (st) {
                                    evas_textblock_style_free(st);
                                }
#ifdef TEST_NEWBACKEND
                                for(sclint loop = 0;loop < g_TextCache.size();loop++) {
                                    if (g_TextCache[loop].text == object->object) {
                                        g_TextCache[loop].used = FALSE;
                                    }
                                }
#endif
                            } else if (object->type == EFLOBJECT_IMAGE) {
#ifdef TEST_NEWBACKEND
                                for(sclint loop = 0;loop < g_ImageCache.size();loop++) {
                                    if (g_ImageCache[loop].image == object->object) {
                                        g_ImageCache[loop].used = FALSE;
                                    }
                                }
#endif
                            }
                            delete object;
                        }
                    }
                    list = eina_list_remove_list(list, iter);
                }
                window_context->etc_info = NULL;
            }

            if (!(window_context->is_virtual)) {
                /* FIXME : A workaround for the bug that event on a window being hidden is delivered to
                    e17, instead of the window itself or the window right below - Should report to WM */
                if (window == windows->get_nth_popup_window(SCL_WINDOW_Z_TOP)) {
                    ecore_timer_add(0.1f, destroy_later, (void*)window);
                } else {
                    Evas_Object *win = (Evas_Object*)window;
                    evas_object_hide(win);
                    evas_object_del(win);
                }
            }
            utils->log("WinEfl_destroywin %p %p (basewin %p mag %p)\n", window,
                (window_context && !(window_context->is_virtual)) ? elm_win_xwindow_get(static_cast<Evas_Object*>(window)) : 0x01,
                windows->get_base_window(), windows->get_magnifier_window());
        }
    }

    return TRUE;
}

/**
 * Shows the given window
 */
void
CSCLWindowsImplEfl::show_window(const sclwindow window, sclboolean queue)
{
    SCL_DEBUG();
    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLContext *context = CSCLContext::get_instance();
    CSCLUtils *utils = CSCLUtils::get_instance();
    if (windows && context && window) {
        SclWindowContext *window_context = windows->get_window_context(window);
        if (!(context->get_hidden_state())) {
            if (window_context) {
                if (!(window_context->is_virtual)) {
                    evas_object_show((Evas_Object*)window);
                }
#ifdef APPLY_WINDOW_MANAGER_CHANGE
                if (windows->is_base_window(window)) {
                    elm_win_keyboard_mode_set((Evas_Object*)window, ELM_WIN_KEYBOARD_ON);
                }
#endif
                if (!(windows->get_update_pending())) {
                    update_window(window);
                }
            }
        }
#ifndef APPLY_WINDOW_MANAGER_CHANGE
        if (windows->get_base_window() == window) {
            int  ret = 0;
            Atom type_return;
            int  format_return;
            unsigned long    nitems_return;
            unsigned long    bytes_after_return;
            unsigned char   *data = NULL;

            Ecore_X_Window win = elm_win_xwindow_get(static_cast<Evas_Object*>(window));
            ret = XGetWindowProperty ((Display *)ecore_x_display_get (),
                    ecore_x_window_root_get (win),
                    ecore_x_atom_get ("_ISF_ACTIVE_WINDOW"),
                    0, G_MAXLONG, False, XA_WINDOW, &type_return,
                    &format_return, &nitems_return, &bytes_after_return,
                    &data);

            if (ret == Success) {
                if ((type_return == XA_WINDOW) && (format_return == 32) && (data)) {
                    app_window = *(Window *)data;
                    if (data)
                        XFree (data);
                }
            }
        }
        scl8 popup_index = windows->find_popup_window_index(window);
        if (windows->get_magnifier_window() == window || popup_index != NOT_USED) {
            /*
             * FIXME a solution to make magnifier window always on top
             * N_SE-17689: magnifier window showing behind of candidate window
             *
             * N_SE-52548: ...and modified if() for other popup windows as well...
             */
            if (window_context && !(window_context->is_virtual)) {
                ecore_x_icccm_transient_for_set
                    (elm_win_xwindow_get(static_cast<Evas_Object*>(window)), app_window);
                elm_win_raise((Evas_Object *)window);
            }
        }
#endif
        utils->log("WinEfl_showwin %p %p (basewin %p mag %p)\n",
            window,
            (window_context && !(window_context->is_virtual)) ? elm_win_xwindow_get(static_cast<Evas_Object*>(window)) : 0x01,
            windows->get_base_window(), windows->get_magnifier_window());
    }
}

/**
 * Hides the given window
 */
void
CSCLWindowsImplEfl::hide_window(const sclwindow window,  sclboolean fForce)
{
    SCL_DEBUG();
    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLUtils *utils = CSCLUtils::get_instance();
    SclWindowContext *window_context = NULL;

    if (windows && window) {
#ifdef USING_KEY_GRAB
    if (window == windows->get_base_window()) {
        CSCLKeyFocusHandler* focus_handler = CSCLKeyFocusHandler::get_instance();
        focus_handler->ungrab_keyboard(window);
    }
#endif

        window_context = windows->get_window_context(window);
        if (window_context) {
            if (!(window_context->is_virtual)) {
                Evas_Object *win = (Evas_Object*)window;
#ifdef APPLY_WINDOW_MANAGER_CHANGE
                if (windows->is_base_window(window)) {
                    elm_win_keyboard_mode_set(win, ELM_WIN_KEYBOARD_OFF);
                } else {
#endif
                    /* FIXME : A workaround for the bug that event on a window being hidden is delivered to
                        e17, instead of the window itself or the window right below - Should report to WM */
                    if (window == windows->get_nth_popup_window(SCL_WINDOW_Z_TOP)) {
                        evas_object_move(win, -10000, -10000);
                    } else {
                        evas_object_hide(win);
                    }
#ifdef APPLY_WINDOW_MANAGER_CHANGE
                }
#endif
            }
        }
    }

    if (windows && utils && window) {
        // Memory optimization */
        if (window == windows->get_magnifier_window() || window == windows->get_dim_window()) {
            if (window_context) {
                if (window_context->etc_info) {

                    Eina_List *list = (Eina_List*)(window_context->etc_info);
                    Eina_List *iter = NULL;
                    Eina_List *iter_next = NULL;
                    void *data = NULL;
                    int iIndex = 0;
#ifdef TEST_NEWBACKEND
#else
                    EINA_LIST_FOREACH_SAFE(list, iter, iter_next, data) {
                        if (data) {
                            EFLObject *object = (EFLObject*)(data);
                            if (object) {
                                sclboolean bShouldRemove = FALSE;
                                bShouldRemove = TRUE;
                                if (bShouldRemove) {
                                    Evas_Object* eo = object->object;
                                    if (object->extracted) {
                                        void *image_data = evas_object_image_data_get(eo, 1);
                                        if (image_data) {
                                            free(image_data);
                                        }
                                    }
                                    if (eo) {
                                        evas_object_del(eo);
                                        object->object = NULL;
                                    }
                                    if (object->type == EFLOBJECT_TEXTBLOCK) {
                                        Evas_Textblock_Style *st = (Evas_Textblock_Style*)(object->data);
                                        if (st) {
                                            evas_textblock_style_free(st);
                                        }
#ifdef TEST_NEWBACKEND
                                        for(sclint loop = 0;loop < g_TextCache.size();loop++) {
                                            if (g_TextCache[loop].text == object->object) {
                                                g_TextCache[loop].used = FALSE;
                                            }
                                        }
#endif
                                    } else if (object->type == EFLOBJECT_IMAGE) {
#ifdef TEST_NEWBACKEND
                                        for(sclint loop = 0;loop < g_ImageCache.size();loop++) {
                                            if (g_ImageCache[loop].image == object->object) {
                                                g_ImageCache[loop].used = FALSE;
                                            }
                                        }
#endif
                                    }
                                    delete object;
                                    list = eina_list_remove_list(list, iter);
                                }
                            }
                            iIndex++;
                        }
                    }
                    window_context->etc_info = list;
#endif
                }
            }
            //Evas *evas = evas_object_evas_get((Evas_Object*)window);
            //evas_render_idle_flush(evas);
        }
        if (window == windows->get_base_window()) {
            elm_cache_all_flush();
            malloc_trim(0);
        }
        utils->log("WinEfl_hidewin %p %p (basewin %p mag %p)\n",
            window,
            (window_context && !(window_context->is_virtual)) ? elm_win_xwindow_get(static_cast<Evas_Object*>(window)) : 0x01,
            windows->get_base_window(), windows->get_magnifier_window());
    }
}

/**
 * Moves the window to the given position
 */
sclint magnifierx, magnifiery;
void
CSCLWindowsImplEfl::move_window(const sclwindow window, scl16 x, scl16 y)
{
    SCL_DEBUG();
    CSCLUtils *utils = CSCLUtils::get_instance();
    CSCLContext *context = CSCLContext::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();

    if (utils && context && windows && window) {
        SclWindowContext *window_context = windows->get_window_context(window);
        unsigned short win_width = 0;
        unsigned short win_height = 0;
        if (window_context) {
            win_width = window_context->geometry.width;
            win_height = window_context->geometry.height;
        }

        scl16 rotatex = x;
        scl16 rotatey = y;
        scl16 orgx = x;
        scl16 orgy = y;

        sclint scr_w, scr_h;
        /* get window size */
        utils->get_screen_resolution(&scr_w, &scr_h);

#ifdef DO_NOT_MOVE_MAGNIFIER_WINDOW
        if (window == windows->get_magnifier_window()) {
            SclWindowContext *base_window_context = windows->get_window_context(windows->get_base_window(), FALSE);
            rotatex = orgx = 0;
            if (context->get_rotation_degree() == 90 || context->get_rotation_degree() == 270) {
                rotatey = orgy = scr_w - base_window_context->height - win_height;
                win_width = base_window_context->width;
                win_height = base_window_context->height + win_height;
            } else {
                rotatey = orgy = scr_h - base_window_context->height - win_height;
                win_width = base_window_context->width;
                win_height = base_window_context->height + win_height;
            }
            magnifierx = x;
            magnifiery = y - orgy;
        }
#endif

        switch (context->get_rotation()) {
            case ROTATION_90_CW: {
                rotatex = orgy;
                rotatey = scr_w - orgx - win_width;
            }
            break;
            case ROTATION_180: {
                rotatex = scr_w - orgx - win_width;
                rotatey = scr_h - orgy - win_height;
            }
            break;
            case ROTATION_90_CCW: {
                rotatex = scr_h - orgy - win_height;
                rotatey = orgx;
            }
            break;
            case ROTATION_0: break;
            default: break;
        }

    #ifdef DO_NOT_MOVE_MAGNIFIER_WINDOW
        if (window == windows->get_magnifier_window()) {
            if (rotatex == window_context->x && rotatey == windonw_context->y) return;
        }
    #endif

        Evas_Object *win = (Evas_Object*)window;
#ifndef FULL_SCREEN_TEST
        if (window != windows->get_base_window()) {
            evas_object_move(win, rotatex, rotatey);
        }
#endif
        //Evas_Object *window_object = (Evas_Object*)window;
        //Evas *evas = evas_object_evas_get(window_object);
        //evas_render_idle_flush(evas);

        utils->log("WinEfl_movewin %p %p %d %d %d %d (basewin %p mag %p)\n",
            window,
            (window_context && !(window_context->is_virtual)) ? elm_win_xwindow_get(static_cast<Evas_Object*>(window)) : 0x01,
            x, y, rotatex, rotatey,
            windows->get_base_window(), windows->get_magnifier_window());
    }
}

/**
* Resizes the window to the given metric
*/
void
CSCLWindowsImplEfl::resize_window(const sclwindow window, scl16 width, scl16 height)
{
    SCL_DEBUG();

    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLUtils *utils = CSCLUtils::get_instance();

#ifdef DO_NOT_MOVE_MAGNIFIER_WINDOW
    if (window == windows->get_magnifier_window()) {
        SclWindowContext *window_context = windows->get_window_context(windows->get_base_window());
        if (window_context->width != width || window_context->height != height) {
            CSCLUtils *utils = CSCLUtils::get_instance();
            sclint scrx, scry, winx, winy;
            utils->get_screen_resolution(&scrx, &scry);
            if (context->get_rotation_degree() == 90 || context->get_rotation_degree() == 270) {
                evas_object_resize((Evas_Object*)window, scry, height + window_context->height);
            } else {
                evas_object_resize((Evas_Object*)window, scrx, height + window_context->height);
            }
        }
        return;
    }
#endif

    Evas_Object *win = (Evas_Object*)window;
#ifndef FULL_SCREEN_TEST
    if (windows && utils && window) {
        SclWindowContext *window_context = windows->get_window_context(window);
        utils->log("WinEfl_resizewin %p %p %d %d (basewin %p mag %p)\n",
            window,
            (window_context && !(window_context->is_virtual)) ? elm_win_xwindow_get(static_cast<Evas_Object*>(window)) : 0x01,
            windows->get_base_window(), windows->get_magnifier_window());
    }
#endif
    //Evas_Object *window_object = (Evas_Object*)window;
    //Evas *evas = evas_object_evas_get(window_object);
    /*CSCLWindows *windows = CSCLWindows::get_instance();
    if (windows) {
        windows->update_window(window);
    }*/
    //evas_render_idle_flush(evas);
    if (windows && window) {
        if (window != windows->get_base_window()) {
            evas_object_resize(win, width, height);
        }

        if (window == windows->get_dim_window()) {
            hide_window(window);
        }
    }
}

/**
* Resizes the window to the given metric
*/
void
CSCLWindowsImplEfl::move_resize_window(const sclwindow window, scl16 x, scl16 y, scl16 width, scl16 height)
{
    SCL_DEBUG();
    Evas_Object *win = (Evas_Object*)window;
#ifndef FULL_SCREEN_TEST
    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLUtils *utils = CSCLUtils::get_instance();
    if (windows && utils && window) {
        if (window != windows->get_base_window()) {
            evas_object_move(win, x, y);
            evas_object_resize(win, width, height);
        }
        utils->log("WinEfl_moveresizewin %p %d %d %d %d (basewin %p mag %p)\n",
            window, x, y, width, height, windows->get_base_window());
    }
#endif
    //Evas_Object *window_object = (Evas_Object*)window;
    //Evas *evas = evas_object_evas_get(window_object);
    /*CSCLWindows *windows = CSCLWindows::get_instance();
    if (windows) {
        windows->update_window(window);
    }*/
    //evas_render_idle_flush(evas);
}

/**
* Update the window to redraw given area
*/
void
CSCLWindowsImplEfl::update_window(const sclwindow window, scl16 x, scl16 y, scl16 width, scl16 height)
{
    SCL_DEBUG();

    sclboolean removeall = FALSE;
    SclRectangle updatearea = {x, y, width, height};
    if (x + y + width + height == 0) {
        removeall = TRUE;
    }

    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLUtils *utils = CSCLUtils::get_instance();
    SclWindowContext *window_context = NULL;

    if (windows && window) {
        //window_context = windows->get_window_context(window, FALSE);
        window_context = windows->get_window_context(window);
    }
    if (windows && utils && window_context) {
        if (window_context->is_virtual) {
            SclWindowContext *base_window_context = windows->get_window_context(windows->get_base_window());
            if (base_window_context) {
                updatearea.x += (window_context->geometry.x - base_window_context->geometry.x);
                updatearea.y += (window_context->geometry.y - base_window_context->geometry.y);
            }
        }
        if (window_context->etc_info) {

            Eina_List *list = (Eina_List*)(window_context->etc_info);
            Eina_List *iter = NULL;
            Eina_List *iter_next = NULL;
            void *data = NULL;
            int iIndex = 0;

            EINA_LIST_FOREACH_SAFE(list, iter, iter_next, data) {
                if (data) {
                    EFLObject *object = (EFLObject*)(data);
                    if (object) {
                        Evas_Object* eo = object->object;
                        sclboolean bShouldRemove = FALSE;
                        if (removeall || utils->is_rect_overlap(object->position, updatearea)) {
                            bShouldRemove = TRUE;
                        }
                        if (iIndex == 0 && !removeall) {
                            bShouldRemove = FALSE; // window's background
                        }
                        if (object->type == EFLOBJECT_CLIPOBJECT) {
                            bShouldRemove = FALSE; // Clip objects are removed when the parent image object is removed
                        }
                        if (eo == NULL) { /* If this object is already removed, proceed removing */
                            bShouldRemove = TRUE;
                        }
                        if (bShouldRemove) {
#ifdef TEST_NEWBACKEND
                            //if (object->type == EFLOBJECT_IMAGE) {
                            SCL_DEBUG_ELAPASED_TIME_START();
                            if (TRUE) {
                                if (window_context->width != object->position.width || window_context->height != object->position.height ||
                                    object->type == EFLOBJECT_TEXTBLOCK || window == windows->get_magnifier_window()) {
                                    evas_object_hide(object->object);
                                }
                            } else {
#else
                            if (object->extracted) {
                                void *image_data = evas_object_image_data_get(eo, 1);
                                if (image_data) {
                                    free(image_data);
                                }
                            }
                            if (eo) {
                                evas_object_del(eo);
                                object->object = NULL;
                            }
                            if (object->type == EFLOBJECT_TEXTBLOCK) {
                                Evas_Textblock_Style *st = (Evas_Textblock_Style*)(object->data);
                                if (st) {
                                    evas_textblock_style_free(st);
                                }
#ifdef TEST_NEWBACKEND
                                for(sclint loop = 0;loop < g_TextCache.size();loop++) {
                                    if (g_TextCache[loop].text == object->object) {
                                        g_TextCache[loop].used = FALSE;
                                    }
                                }
#endif
                            } else if (object->type == EFLOBJECT_IMAGE) {
#ifdef TEST_NEWBACKEND
                                for(sclint loop = 0;loop < g_ImageCache.size();loop++) {
                                    if (g_ImageCache[loop].image == object->object) {
                                        g_ImageCache[loop].used = FALSE;
                                    }
                                }
#endif
                                EFLObject *clip_object = (EFLObject*)(object->data);
                                if (clip_object) {
                                    if (clip_object->object) {
                                        evas_object_del(clip_object->object);
                                        clip_object->object = NULL;
                                    }
                                }
                            }
                            delete object;
                            list = eina_list_remove_list(list, iter);
#endif
#ifdef TEST_NEWBACKEND
                            }
#endif
                        }
                    }
                    iIndex++;
                }
            }
            window_context->etc_info = list;

            /*while ((Eina_List*)(window_context->etc_info))
            {
                EFLObject *object = (EFLObject*)eina_list_data_get((Eina_List*)(window_context->etc_info));
                if (object) {
                    Evas_Object* eo = object->object;
                    if (eo) {
                        evas_object_del(eo);
                        object->object = NULL;
                    }
                }
                window_context->etc_info = eina_list_remove_list((Eina_List*)(window_context->etc_info), (Eina_List*)(winctx->etc_info));
                delete object;
            }
            window_context->etc_info = NULL;*/
        }
        CSCLUIBuilder *builder = CSCLUIBuilder::get_instance();
        builder->show_layout(window, x, y, width, height);
    }

    /*evas_image_cache_flush(evas_object_evas_get((Evas_Object*)window));
    elm_cache_all_flush();
    malloc_trim(0);*/
    //edje_file_cache_flush();
}

/**
 * Returns the position of x,y,width,height of the given window
 */
sclboolean
CSCLWindowsImplEfl::get_window_rect(const sclwindow window, SclRectangle *rect)
{
    SCL_DEBUG();
    CSCLUtils *utils = CSCLUtils::get_instance();
    CSCLContext *context = CSCLContext::get_instance();

    if (utils && context && rect && window) {
        Window junkwin;
        Ecore_X_Window_Attributes attrs;
        int x, y, width, height;
        sclint scr_w, scr_h;
        //CSCLWindows *windows = CSCLWindows::get_instance();
        ecore_x_window_geometry_get(elm_win_xwindow_get(static_cast<Evas_Object*>(window)), &x, &y, &width, &height);
        ecore_x_window_attributes_get(elm_win_xwindow_get(static_cast<Evas_Object*>(window)), &attrs);
        XTranslateCoordinates((Display*)ecore_x_display_get(), (Drawable)elm_win_xwindow_get(static_cast<Evas_Object*>(window)),
                              attrs.root, -attrs.border, -attrs.border, &x, &y, &junkwin);

        utils->log("WinEfl_getwinrect %p %p, %d %d %d %d\n",
            window, elm_win_xwindow_get(static_cast<Evas_Object*>(window)),
            x, y, width, height);

        /* get window size */
        utils->get_screen_resolution(&scr_w, &scr_h);

        switch (context->get_rotation())
        {
        case ROTATION_90_CW:
            {
                rect->width = height;
                rect->height = width;
                rect->x = scr_w - y - height;
                rect->y = x;
            }
            break;
        case ROTATION_180:
            {
                rect->x = scr_w - width - x;
                rect->y = scr_h - height - y;
                rect->width = width;
                rect->height = height;
            }
            break;
        case ROTATION_90_CCW:
            {
                rect->width = height;
                rect->height = width;
                rect->x = y;
                rect->y = scr_h - x - width;
            }
            break;
        default:
            {
                rect->x = x;
                rect->y = y;
                rect->width = width;
                rect->height = height;
            }
            break;
        }
    }

    return TRUE;
}

#include <X11/Xutil.h>
/**
 * Sets rotation
 */
void
CSCLWindowsImplEfl::set_window_rotation(const sclwindow window, SCLRotation rotation)
{
    SCL_DEBUG();

    CSCLWindows *windows = CSCLWindows::get_instance();
    SclWindowContext *window_context = NULL;

    if (windows && window) {
        //window_context = windows->get_window_context(window, FALSE);
        window_context = windows->get_window_context(window);

        if (window_context) {
            if (window_context->is_virtual) {
                return;
            }
        }

        if (scl_check_arrindex(rotation, ROTATION_MAX)) {
            elm_win_rotation_with_resize_set(static_cast<Evas_Object*>(window), rotation_values_EFL[rotation]);
        }

        XSizeHints hint;
        long int mask;
        Window win;
        Display *dpy;

        dpy = (Display*)ecore_x_display_get();
        win = elm_win_xwindow_get((Evas_Object*)window);

        if (!XGetWMNormalHints(dpy, win, &hint, &mask))
            memset(&hint, 0, sizeof(XSizeHints));

        hint.flags |= USPosition;

        XSetWMNormalHints(dpy, win, &hint);
        /*if (window_context) {
          windows->resize_window(window_context->window, window_context->width, winctx->height);
          }*/
    }
}


/**
 * Shows a message box
 */
void
CSCLWindowsImplEfl::show_message_box(const sclwindow parent, scl8 msgType, sclchar* title, sclchar* msg)
{
    SCL_DEBUG();
}


void
CSCLWindowsImplEfl::set_keep_above(const sclwindow window, sclboolean keepabove)
{
    SCL_DEBUG();
}

void release_all(Evas_Object *win)
{
    SCL_DEBUG();
    //if (window == windows->get_magnifier_window()) {
    //evas_object_move(win, -10000, -10000);
    //Evas *evas = evas_object_evas_get(win);
    //evas_render_idle_flush(evas);
    /*} else {
    evas_object_hide(win);
    }*/
    //LOGD("HIDE_WINDOW : %p\n", window);

    CSCLWindows *windows = CSCLWindows::get_instance();
    SclWindowContext *window_context = windows->get_window_context(win);
        if (window_context && win) {
            if (window_context->etc_info) {

                Eina_List *list = (Eina_List*)(window_context->etc_info);
                Eina_List *iter = NULL;
                Eina_List *iter_next = NULL;
                void *data = NULL;
                int iIndex = 0;
#ifdef TEST_NEWBACKEND
#else
                EINA_LIST_FOREACH_SAFE(list, iter, iter_next, data) {
                    if (data) {
                        EFLObject *object = (EFLObject*)(data);
                        if (object) {
                            sclboolean bShouldRemove = FALSE;
                            bShouldRemove = TRUE;
                            if (bShouldRemove) {
                                Evas_Object* eo = object->object;
                                if (object->extracted) {
                                    void *image_data = evas_object_image_data_get(eo, 1);
                                    if (image_data) {
                                        free(image_data);
                                    }
                                }
                                if (eo) {
                                    evas_object_del(eo);
                                    object->object = NULL;
                                }
                                if (object->type == EFLOBJECT_TEXTBLOCK) {
                                    Evas_Textblock_Style *st = (Evas_Textblock_Style*)(object->data);
                                    if (st) {
                                        evas_textblock_style_free(st);
                                    }
#ifdef TEST_NEWBACKEND
                                    for(sclint loop = 0;loop < g_TextCache.size();loop++) {
                                        if (g_TextCache[loop].text == object->object) {
                                            g_TextCache[loop].used = FALSE;
                                        }
                                    }
#endif
                                } else if (object->type == EFLOBJECT_IMAGE) {
#ifdef TEST_NEWBACKEND
                                    for(sclint loop = 0;loop < g_ImageCache.size();loop++) {
                                        if (g_ImageCache[loop].image == object->object) {
                                            g_ImageCache[loop].used = FALSE;
                                        }
                                    }
#endif
                                }
                                delete object;
                                list = eina_list_remove_list(list, iter);
                            }
                        }
                        iIndex++;
                    }
                }
                window_context->etc_info = list;
#endif
            }
        }
        //Evas *evas = evas_object_evas_get((Evas_Object*)win);
        //evas_render_idle_flush(evas);
    //}
    /*evas_image_cache_flush(evas_object_evas_get((Evas_Object*)win));
    elm_cache_all_flush();
    malloc_trim(0);*/
}

#ifndef APPLY_WINDOW_MANAGER_CHANGE
void CSCLWindowsImplEfl::set_window_accepts_focus(const sclwindow window, sclboolean acceptable)
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
