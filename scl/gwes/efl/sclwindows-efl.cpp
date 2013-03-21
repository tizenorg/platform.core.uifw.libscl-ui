/*
 * Copyright 2012-2013 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Flora License, Version 1.0 (the "License");
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

#include "sclwindows-efl.h"
#include "scldebug.h"
#include "sclcontroller.h"
#include "sclcontext.h"
#include "sclresourcecache.h"
#include "scluibuilder.h"
#include "sclwindows.h"

#include <Elementary.h>
#include <Ecore_X.h>
#include <malloc.h>
#include <X11/Xlib.h>

using namespace scl;

Ecore_X_Atom ATOM_WM_CLASS = 0;

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


/**
 * Create a content window and binds it into given parent window as a child
 */
sclwindow
CSCLWindowsImplEfl::create_base_window(const sclwindow parent, SclWindowContext *winctx, const scl16 width, const scl16 height)
{
    SCL_DEBUG();

    sclwindow ret = SCLWINDOW_INVALID;

    if (winctx) {
        winctx->etc_info = NULL;
        winctx->window = parent;

#ifndef APPLY_WINDOW_MANAGER_CHANGE
        ecore_x_icccm_name_class_set(elm_win_xwindow_get(static_cast<Evas_Object*>(parent)), "Virtual Keyboard", "ISF" );

        set_window_accepts_focus(parent, FALSE);
#else
        if (parent) {
            evas_object_show((Evas_Object*)parent);
            //elm_win_keyboard_mode_set((Evas_Object*)parent, ELM_WIN_KEYBOARD_OFF);
        }
#endif

        ret = winctx->window;
    }

    CSCLUtils *utils = CSCLUtils::get_instance();
    if (utils) {
        utils->log("%p, %d %d\n", parent, width, height);
    }

    return ret;
}

/**
 * Creates a window
 */
sclwindow
CSCLWindowsImplEfl::create_window(const sclwindow parent, SclWindowContext *winctx, const scl16 width, const scl16 height)
{
    SCL_DEBUG();

    Evas_Object *win = NULL;
    win = elm_win_add(static_cast<Evas_Object*>(parent), "SCLPopup", ELM_WIN_BASIC);
    Ecore_X_Display* dpy;
    dpy = ecore_x_display_get();

    elm_win_borderless_set(win, EINA_TRUE);
    elm_win_alpha_set(win, EINA_TRUE);

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

#ifndef FULL_SCREEN_TEST
    //evas_object_resize(win, width, height);
#endif

    const char *szProfile[] = {"mobile", ""};
    elm_win_profiles_set(win, szProfile, 1);

#ifndef APPLY_WINDOW_MANAGER_CHANGE
    ecore_x_icccm_name_class_set(elm_win_xwindow_get(static_cast<Evas_Object*>(win)), "ISF Popup", "ISF");

    set_window_accepts_focus(win, FALSE);

    /*ecore_x_icccm_transient_for_set(elm_win_xwindow_get(static_cast<Evas_Object*>(win)), 
        elm_win_xwindow_get(static_cast<Evas_Object*>(parent)));*/

    utilx_set_window_effect_state((Display*)dpy, elm_win_xwindow_get(static_cast<Evas_Object*>(win)), 0);
    //utilx_set_window_effect_style((Display*)dpy, elm_win_xwindow_get(static_cast<Evas_Object*>(win)), UTILX_EFFECT_TYPE_MAP, UTILX_EFFECT_STYLE_NONE);
#endif

    set_window_rotation(win, context->get_rotation());

    //elm_win_override_set(win, EINA_TRUE);
    CSCLUtils *utils = CSCLUtils::get_instance();
    if (utils) {
        utils->log("%p, %d %d\n", win, width, height);
    }

    return win;
}


/**
* Creates a window
*/
sclwindow
CSCLWindowsImplEfl::create_magnifier_window(const sclwindow parent, SclWindowContext *winctx, const scl16 width, const scl16 height)
{
    SCL_DEBUG();

    Evas_Object *win = NULL;
    win = elm_win_add(static_cast<Evas_Object*>(parent), "Magnfier", ELM_WIN_BASIC);
    Ecore_X_Display* dpy;
    dpy = ecore_x_display_get();

    elm_win_borderless_set(win, EINA_TRUE);
    elm_win_alpha_set(win, EINA_TRUE);

    /*
     * FIXME set override to let window manager responds elm_win_raise request
     */
    //elm_win_override_set(win, EINA_TRUE);

#ifdef DO_NOT_MOVE_MAGNIFIER_WINDOW
    CSCLUtils *utils = CSCLUtils::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();
    sclint scrx, scry, winx, winy;
    utils->get_screen_resolution(&scrx, &scry);
    SclWindowContext *winctx = windows->get_window_context(windows->get_base_window());
    evas_object_resize(win, scrx, height + winctx->height);
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

#ifndef APPLY_WINDOW_MANAGER_CHANGE
    ecore_x_icccm_name_class_set(elm_win_xwindow_get(static_cast<Evas_Object*>(win)), "Key Magnifier", "ISF");

    set_window_accepts_focus(win, FALSE);

    /*ecore_x_icccm_transient_for_set(elm_win_xwindow_get(static_cast<Evas_Object*>(win)), 
        elm_win_xwindow_get(static_cast<Evas_Object*>(parent)));*/

    utilx_set_window_effect_state((Display*)dpy, elm_win_xwindow_get(static_cast<Evas_Object*>(win)), 0);
    //utilx_set_window_effect_style((Display*)dpy, elm_win_xwindow_get(static_cast<Evas_Object*>(win)), UTILX_EFFECT_TYPE_MAP, UTILX_EFFECT_STYLE_NONE);
#endif

    //evas_font_path_prepend(evas_object_evas_get(win), "/usr/share/SLP/fonts");

    CSCLContext *context = CSCLContext::get_instance();
    set_window_rotation(win, context->get_rotation());

    /*Ecore_X_Display* dpy;
    dpy = ecore_x_display_get();
    utilx_set_window_effect_state((Display*)dpy, elm_win_xwindow_get(static_cast<Evas_Object*>(win)), EINA_TRUE);*/

    //elm_win_override_set(win, EINA_TRUE);
    CSCLUtils *utils = CSCLUtils::get_instance();
    if (utils) {
        utils->log("%p, %d %d\n", win, width, height);
    }

    return win;
}

/**
 * Creates the dim window
 */
sclwindow
CSCLWindowsImplEfl::create_dim_window(const sclwindow parent, SclWindowContext *winctx, const scl16 width, const scl16 height)
{
    SCL_DEBUG();

    Evas_Object *win = NULL;
    win = elm_win_add(static_cast<Evas_Object*>(parent), "SCLPopup", ELM_WIN_BASIC);
    Ecore_X_Display* dpy;
    dpy = ecore_x_display_get();

    elm_win_borderless_set(win, EINA_TRUE);
    elm_win_alpha_set(win, EINA_TRUE);
    evas_object_resize(win, width, height);

#ifndef APPLY_WINDOW_MANAGER_CHANGE
    ecore_x_icccm_name_class_set(elm_win_xwindow_get(static_cast<Evas_Object*>(win)), "ISF Popup", "ISF");

    set_window_accepts_focus(win, FALSE);

    /*ecore_x_icccm_transient_for_set(elm_win_xwindow_get(static_cast<Evas_Object*>(win)), 
        elm_win_xwindow_get(static_cast<Evas_Object*>(parent)));*/

    utilx_set_window_effect_state((Display*)dpy, elm_win_xwindow_get(static_cast<Evas_Object*>(win)), 0);
    //utilx_set_window_effect_style((Display*)dpy, elm_win_xwindow_get(static_cast<Evas_Object*>(win)), UTILX_EFFECT_TYPE_MAP, UTILX_EFFECT_STYLE_NONE);
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

    //elm_win_override_set(win, EINA_TRUE);
    hide_window(win);

    CSCLUtils *utils = CSCLUtils::get_instance();
    if (utils) {
        utils->log("%p, %d %d\n", win, width, height);
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

    ecore_x_icccm_transient_for_set(elm_win_xwindow_get(static_cast<Evas_Object*>(window)), 
        elm_win_xwindow_get(static_cast<Evas_Object*>(parent)));
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

    SclWindowContext *winctx = NULL;
    if (windows) {
        winctx = windows->get_window_context(window);
    }

    if (windows && utils && winctx) {
        if (winctx->etc_info) {
            while ((Eina_List*)(winctx->etc_info)) {
                EFLObject *object = (EFLObject*)eina_list_data_get((Eina_List*)(winctx->etc_info));
                if (object) {
                    Evas_Object* eo = object->object;
                    if (object->extracted) {
                        //evas_object_image_data_set(eo, NULL);
                        void *data = evas_object_image_data_get(eo, 1);
                        if (data) {
                            free(data);
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
                }
                winctx->etc_info = eina_list_remove_list((Eina_List*)(winctx->etc_info), (Eina_List*)(winctx->etc_info));
                if (object)
                    delete object;
            }
            winctx->etc_info = NULL;
        }

        if (!(winctx->is_virtual)) {
            Evas_Object *win = (Evas_Object*)window;
            evas_object_hide(win);
            evas_object_del(win);
        }
        utils->log("%p (basewin %p mag %p)\n", window,
            windows->get_base_window(), windows->get_magnifier_window());
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
#ifndef APPLY_WINDOW_MANAGER_CHANGE
        if (windows->get_magnifier_window() == window) {
            set_parent(windows->get_base_window(), window);
        }
#endif
        SclWindowContext *winctx = windows->get_window_context(window);
        if (!(context->get_hidden_state())) {
            if (winctx) {
                if (!(winctx->is_virtual)) {
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
        /*
         * FIXME a solution to make magnifier window always on top
         * N_SE-17689: magnifier window showing behind of candidate window
         */
        if (window != windows->get_base_window()) {
            //elm_win_raise((Evas_Object *)window);
        }
        utils->log("%p (basewin %p mag %p)\n", window,
            windows->get_base_window(), windows->get_magnifier_window());
    }
}

int hide_window_timer_event(void *data)
{
    Evas_Object *win = (Evas_Object*)data;
    CSCLUtils *utils = CSCLUtils::get_instance();
    if (utils) {
        utils->log("%p\n", win);
    }
    if (win) {
        evas_object_hide(win);
    }
    return FALSE;
}
/**
 * Hides the given window
 */
void
CSCLWindowsImplEfl::hide_window(const sclwindow window,  const sclboolean fForce)
{
    SCL_DEBUG();
    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLUtils *utils = CSCLUtils::get_instance();
    SclWindowContext *winctx = NULL;

    if (windows) {
        winctx = windows->get_window_context(window);
        if (winctx) {
            if (!(winctx->is_virtual)) {
                Evas_Object *win = (Evas_Object*)window;
#ifdef APPLY_WINDOW_MANAGER_CHANGE
                if (windows->is_base_window(window)) {
                    elm_win_keyboard_mode_set(win, ELM_WIN_KEYBOARD_OFF);
                } else {
#endif
                    evas_object_hide(win);
#ifdef APPLY_WINDOW_MANAGER_CHANGE
                }
#endif
            }
        }
    }

    if (windows && utils) {
        // Memory optimization */
        //if (window == windows->get_magnifier_window() || TRUE) {
        if (window == windows->get_magnifier_window() || window == windows->get_dim_window()) {
            if (winctx) {
                if (winctx->etc_info) {

                    Eina_List *list = (Eina_List*)(winctx->etc_info);
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
                                        void *data = evas_object_image_data_get(eo, 1);
                                        if (data) {
                                            free(data);
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
                    winctx->etc_info = list;
#endif
                }
            }
            //Evas *evas = evas_object_evas_get((Evas_Object*)window);
            //evas_render_idle_flush(evas);
        }
        //ecore_timer_add(5, hide_window_timer_event, window);
        if (window == windows->get_base_window()) {
            elm_cache_all_flush();
            malloc_trim(0);
        }
        utils->log("%p (basewin %p mag %p)\n", window,
            windows->get_base_window(), windows->get_magnifier_window());
    }
}

/**
 * Moves the window to the given position
 */
sclint magnifierx, magnifiery;
void
CSCLWindowsImplEfl::move_window(const sclwindow window, const scl16 x, const scl16 y)
{
    SCL_DEBUG();
    CSCLUtils *utils = CSCLUtils::get_instance();
    CSCLContext *context = CSCLContext::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();

    if (utils && context && windows) {
        //SclWindowContext *winctx = windows->get_window_context(window, FALSE);
        SclWindowContext *winctx = windows->get_window_context(window);
        unsigned short win_width = 0;
        unsigned short win_height = 0;
        if (winctx) {
            win_width = winctx->geometry.width;
            win_height = winctx->geometry.height;
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
            SclWindowContext *basewinctx = windows->get_window_context(windows->get_base_window(), FALSE);
            rotatex = orgx = 0;
            if (context->get_rotation_degree() == 90 || context->get_rotation_degree() == 270) {
                rotatey = orgy = scr_w - basewinctx->height - win_height;
                win_width = basewinctx->width;
                win_height = basewinctx->height + win_height;
            } else {
                rotatey = orgy = scr_h - basewinctx->height - win_height;
                win_width = basewinctx->width;
                win_height = basewinctx->height + win_height;
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
        }

    #ifdef DO_NOT_MOVE_MAGNIFIER_WINDOW
        if (window == windows->get_magnifier_window()) {
            if (rotatex == winctx->x && rotatey == winctx->y) return;
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

        utils->log("%p %d %d %d %d (basewin %p mag %p)\n", window, x, y, rotatex, rotatey,
            windows->get_base_window(), windows->get_magnifier_window());
    }
}

/**
* Resizes the window to the given metric
*/
void
CSCLWindowsImplEfl::resize_window(const sclwindow window, const scl16 width, const scl16 height)
{
    SCL_DEBUG();

    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLUtils *utils = CSCLUtils::get_instance();

#ifdef DO_NOT_MOVE_MAGNIFIER_WINDOW
    if (window == windows->get_magnifier_window()) {
        SclWindowContext *winctx = windows->get_window_context(windows->get_base_window());
        if (winctx->width != width || winctx->height != height) {
            CSCLUtils *utils = CSCLUtils::get_instance();
            sclint scrx, scry, winx, winy;
            utils->get_screen_resolution(&scrx, &scry);
            if (context->get_rotation_degree() == 90 || context->get_rotation_degree() == 270) {
                evas_object_resize((Evas_Object*)window, scry, height + winctx->height);
            } else {
                evas_object_resize((Evas_Object*)window, scrx, height + winctx->height);
            }
        }
        return;
    }
#endif

    Evas_Object *win = (Evas_Object*)window;
#ifndef FULL_SCREEN_TEST
    if (windows && utils) {
        utils->log("%p %d %d (basewin %p mag %p)\n", window, width, height,
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
    if (windows) {
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
CSCLWindowsImplEfl::move_resize_window(const sclwindow window, const scl16 x, const scl16 y, const scl16 width, const scl16 height)
{
    SCL_DEBUG();
    Evas_Object *win = (Evas_Object*)window;
#ifndef FULL_SCREEN_TEST
    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLUtils *utils = CSCLUtils::get_instance();
    if (windows && utils) {
        if (window != windows->get_base_window()) {
            evas_object_move(win, x, y);
            evas_object_resize(win, width, height);
        }
        utils->log("%p %d %d %d %d (basewin %p mag %p)\n",
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
CSCLWindowsImplEfl::update_window(const sclwindow window, const scl16 x, const scl16 y, const scl16 width, const scl16 height)
{
    SCL_DEBUG();

    sclboolean removeall = FALSE;
    SclRectangle updatearea = {x, y, width, height};
    if (x + y + width + height == 0) {
        removeall = TRUE;
    }

    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLUtils *utils = CSCLUtils::get_instance();
    SclWindowContext *winctx = NULL;

    if (windows) {
        //winctx = windows->get_window_context(window, FALSE);
        winctx = windows->get_window_context(window);
    }
    if (windows && utils && winctx) {
        if (winctx->is_virtual) {
            SclWindowContext *basectx = windows->get_window_context(windows->get_base_window());
            if (basectx) {
                updatearea.x += (winctx->geometry.x - basectx->geometry.x);
                updatearea.y += (winctx->geometry.y - basectx->geometry.y);
            }
        }
        if (winctx->etc_info) {

            Eina_List *list = (Eina_List*)(winctx->etc_info);
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
                                if (winctx->width != object->position.width || winctx->height != object->position.height ||
                                    object->type == EFLOBJECT_TEXTBLOCK || window == windows->get_magnifier_window()) {
                                    evas_object_hide(object->object);
                                }
                            } else {
#else
                            if (object->extracted) {
                                void *data = evas_object_image_data_get(eo, 1);
                                if (data) {
                                    free(data);
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
            winctx->etc_info = list;

            /*while ((Eina_List*)(winctx->etc_info))
            {
                EFLObject *object = (EFLObject*)eina_list_data_get((Eina_List*)(winctx->etc_info));
                if (object) {
                    Evas_Object* eo = object->object;
                    if (eo) {
                        evas_object_del(eo);
                        object->object = NULL;
                    }
                }
                winctx->etc_info = eina_list_remove_list((Eina_List*)(winctx->etc_info), (Eina_List*)(winctx->etc_info));
                delete object;
            }
            winctx->etc_info = NULL;*/
        }
    }

    /*evas_image_cache_flush(evas_object_evas_get((Evas_Object*)window));
    elm_cache_all_flush();
    malloc_trim(0);*/
    //edje_file_cache_flush();

    CSCLUIBuilder *builder = CSCLUIBuilder::get_instance();
    builder->show_layout(window, x, y, width, height);
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

    if (utils && context && rect) {
        Window junkwin;
        Ecore_X_Window_Attributes attrs;
        int x, y, width, height;
        sclint scr_w, scr_h;
        //CSCLWindows *windows = CSCLWindows::get_instance();
        ecore_x_window_geometry_get(elm_win_xwindow_get(static_cast<Evas_Object*>(window)), &x, &y, &width, &height);
        ecore_x_window_attributes_get(elm_win_xwindow_get(static_cast<Evas_Object*>(window)), &attrs);
        XTranslateCoordinates((Display*)ecore_x_display_get(), (Drawable)elm_win_xwindow_get(static_cast<Evas_Object*>(window)),
                              attrs.root, -attrs.border, -attrs.border, &x, &y, &junkwin);

        utils->log("%p , %d %d %d %d\n", window, x, y, width, height);

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
CSCLWindowsImplEfl::set_window_rotation(const sclwindow window, const SCLRotation rotation)
{
    SCL_DEBUG();

    CSCLWindows *windows = CSCLWindows::get_instance();
    SclWindowContext *winctx = NULL;

    if (windows) {
        //winctx = windows->get_window_context(window, FALSE);
        winctx = windows->get_window_context(window);
    }

    if (winctx) {
        if (winctx->is_virtual) {
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
    /*if (winctx) {
        windows->resize_window(winctx->window, winctx->width, winctx->height);
    }*/
}


/**
 * Shows a message box
 */
void
CSCLWindowsImplEfl::show_message_box(const sclwindow parent, const scl8 msgType, sclchar* title, sclchar* msg)
{
    SCL_DEBUG();
}


void
CSCLWindowsImplEfl::set_keep_above(const sclwindow window, const sclboolean keepabove)
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
    //if (window == windows->get_magnifier_window()) {
        //SclWindowContext *winctx = windows->get_window_context(win, FALSE);
    SclWindowContext *winctx = windows->get_window_context(win);
        if (winctx) {
            if (winctx->etc_info) {

                Eina_List *list = (Eina_List*)(winctx->etc_info);
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
                                    void *data = evas_object_image_data_get(eo, 1);
                                    if (data) {
                                        free(data);
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
                winctx->etc_info = list;
#endif
            }
        }
        //Evas *evas = evas_object_evas_get((Evas_Object*)win);
        //evas_render_idle_flush(evas);
    //}
    //ecore_timer_add(5, hide_window_timer_event, window);
    /*evas_image_cache_flush(evas_object_evas_get((Evas_Object*)win));
    elm_cache_all_flush();
    malloc_trim(0);*/
}

#ifndef APPLY_WINDOW_MANAGER_CHANGE
void CSCLWindowsImplEfl::set_window_accepts_focus(const sclwindow window, const sclboolean acceptable)
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
