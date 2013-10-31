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

#include "sclwindows.h"
#ifdef  __WIN32__
#include "sclwindows-win32.h"
#elif defined(__EFL__)
#include "sclwindows-efl.h"
#else
#include "sclwindows-gtk.h"
#endif
#include "scldebug.h"
#include "sclevents.h"
#include "sclresourcecache.h"
#include "sclcontroller.h"
#include "sclactionstate.h"
#include "sclres_manager.h"

using namespace scl;

CSCLWindows::CSCLWindows()
{
    SCL_DEBUG();

    sclint loop;

    m_impl = NULL;

    memset(&m_base_winctx, 0x00, sizeof(SclWindowContext));
    memset(&m_magnifier_winctx, 0x00, sizeof(SclWindowContext));
    memset(&m_dim_winctx, 0x00, sizeof(SclWindowContext));
    memset(m_popup_winctx, 0x00, sizeof(SclWindowContext) * MAX_POPUP_WINDOW);

    m_pending_update = FALSE;

    for (loop = 0;loop < MAX_ZORDER_NUM;loop++) {
        m_Z_order_list[loop] = SCLWINDOW_INVALID;
    }
}

CSCLWindows::~CSCLWindows()
{
    SCL_DEBUG();

    if (m_impl) {
        delete m_impl;
        m_impl = NULL;
    }
}

void CSCLWindows::init()
{
    int loop;
    CSCLWindowsImpl *impl = get_scl_windows_impl();
    if (impl) {
        impl->init();
    }
    for (loop = 0;loop < MAX_ZORDER_NUM;loop++) {
        m_Z_order_list[loop] = SCLWINDOW_INVALID;
    }
    m_initialized = TRUE;
}

void CSCLWindows::fini()
{
    CSCLWindowsImpl* impl = get_scl_windows_impl();

    if (impl) {
        impl->fini();

        if (SCLWINDOW_INVALID != m_base_winctx.window) {
            impl->destroy_window(m_base_winctx.window);
            m_base_winctx.window = SCLWINDOW_INVALID;
        }

        if (SCLWINDOW_INVALID != m_magnifier_winctx.window) {
            impl->destroy_window(m_magnifier_winctx.window);
            m_magnifier_winctx.window = SCLWINDOW_INVALID;
        }

        if (SCLWINDOW_INVALID != m_dim_winctx.window) {
            impl->destroy_window(m_dim_winctx.window);
            m_dim_winctx.window = SCLWINDOW_INVALID;
        }

        for (int loop = 0;loop < MAX_POPUP_WINDOW;loop++) {
            if (m_popup_winctx[loop].window != SCLWINDOW_INVALID) {
                if (!m_popup_winctx[loop].is_virtual) {
                    impl->destroy_window(m_popup_winctx[loop].window);
                }
                m_popup_winctx[loop].window = SCLWINDOW_INVALID;
            }
        }
    }

    m_initialized = FALSE;
}

CSCLWindowsImpl*
CSCLWindows::get_scl_windows_impl()
{
    if (m_impl == 0) {
#ifdef  __WIN32__
        m_impl = new CSCLWindowsImplWin32;
#elif defined(__EFL__)
        m_impl = new CSCLWindowsImplEfl;
#else
        m_impl = new CSCLWindowsImplGtk;
#endif
    }
    return m_impl;
}

CSCLWindows*
CSCLWindows::get_instance()
{
    static CSCLWindows instance;
    return &instance;
}

sclwindow CSCLWindows::open_popup(const SclWindowOpener opener, const SclRectangle &geometry, sclshort inputmode, sclshort layout, SCLPopupType popup_type, sclboolean is_virtual, sclboolean use_dim_window, sclint img_offset_x, sclint img_offset_y, sclint timeout)
{
    sclwindow window = SCLWINDOW_INVALID;

    CSCLEvents *events = CSCLEvents::get_instance();
    CSCLActionState *state = CSCLActionState::get_instance();
    CSCLController *controller = CSCLController::get_instance();
    CSCLResourceCache *cache = CSCLResourceCache::get_instance();
    CSCLContext *context = CSCLContext::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLUtils *utils = CSCLUtils::get_instance();

    if (events && state && controller && cache && context && windows && utils) {
        for (sclint loop = 0;loop < MAX_POPUP_WINDOW;loop++) {
            if (m_popup_winctx[loop].window != SCLWINDOW_INVALID) {
                if (m_popup_winctx[loop].layout == layout) return SCLWINDOW_INVALID;
            }
        }

        sclwindow window = create_window(opener, geometry, inputmode, layout, popup_type, is_virtual, img_offset_x, img_offset_y, timeout);
        events->destroy_timer(SCL_TIMER_POPUP_TIMEOUT);
        if (timeout > 0) {
            events->create_timer(SCL_TIMER_POPUP_TIMEOUT, timeout, layout);
        }

        printf("create window (%p) x: %d, y:%d, width:%d, height:%d , layout:%d, popuptype:%d\n",
            window, geometry.x, geometry.y, geometry.width, geometry.height, layout, popup_type);

        events->connect_window_events(window, SCL_EVENT_MOUSE | SCL_EVENT_EXPOSE);
        controller->handle_engine_signal(SCL_SIG_POPUP_SHOW, window);

        /* Shows the dim window if it uses the dimwindow */
        if (use_dim_window) {
            sclwindow dimWindow = get_dim_window();

            /* Currently, get_window_rect does not work normally (need to check X). So I have commented it*/
            SclRectangle rect;
            get_window_rect(get_base_window(), &rect);
            resize_window(dimWindow, rect.width, rect.height);
            move_window(dimWindow, rect.x, rect.y);
            events->connect_window_events(dimWindow, SCL_EVENT_MOUSE);
            /*If we use transient_for them the ISE will occure some crash. It needs to check X11*/
            set_parent(opener.window, dimWindow);
            SclWindowContext *dimctx = get_window_context(get_dim_window());
            if (dimctx) {
                if (dimctx->is_virtual) {
                    set_parent(opener.window, window);
                } else {
                    set_parent(dimWindow, window);
                }
            }
            show_window(dimWindow);
        } else {
            /*If we use transient_for them the ISE will occure some crash. It needs to check X11*/
            set_parent(opener.window, window);
        }
        show_window(window);

        push_window_in_Z_order_list(window);

        state->set_cur_action_state(ACTION_STATE_POPUP_INIT);

        const SclLayout *layout = cache->get_cur_layout(window);
        if (layout) {
            /* If the newly opened popup window has POPUP_GRAB style, lets press the nearest button on the new window */
            if (layout->style == LAYOUT_STYLE_POPUP_GRAB) {
                sclwindow pressed_window = context->get_cur_pressed_window(context->get_last_touch_device_id());
                sclbyte pressed_key = context->get_cur_pressed_key(context->get_last_touch_device_id());

                const SclLayoutKeyCoordinate* coordinate = cache->get_cur_layout_key_coordinate(pressed_window, pressed_key);

                sclwindow move_window = context->get_cur_move_window(context->get_last_touch_device_id());
                SclPoint move_point = context->get_cur_move_point(context->get_last_touch_device_id());
                SclWindowContext *move_ctx = windows->get_window_context(move_window);
                SclWindowContext *popup_ctx = windows->get_window_context(window);
                if (move_ctx && popup_ctx) {
                    move_point.x = (move_ctx->geometry.x - popup_ctx->geometry.x) + move_point.x;
                    move_point.y = (move_ctx->geometry.y - popup_ctx->geometry.y) + move_point.y;
                }

                /* Find the nearest button on the autopopup window */
                sclboolean ended = FALSE;
                sclfloat min_dist = (float)((sclu32)(-1));
                sclint min_dist_index = NOT_USED;
                for (sclint loop = 0;loop < MAX_KEY && !ended;loop++) {
                    SclButtonContext *popup_btncontext = cache->get_cur_button_context(window, loop);
                    const SclLayoutKeyCoordinate *popup_coordinate = cache->get_cur_layout_key_coordinate(window, loop);
                    if (popup_btncontext && popup_coordinate) {
                        if (!(popup_btncontext->used)) {
                            ended = TRUE;
                        } else if (popup_btncontext->state != BUTTON_STATE_DISABLED &&
                            popup_coordinate->button_type != BUTTON_TYPE_UIITEM) {
                                if (popup_coordinate) {
                                    float dist = utils->get_approximate_distance(move_point.x, move_point.y,
                                        popup_coordinate->x + (popup_coordinate->width / 2) - layout->mouse_manipulate_x,
                                        popup_coordinate->y + (popup_coordinate->height / 2) - layout->mouse_manipulate_y);
                                    if (dist < min_dist) {
                                        min_dist_index = loop;
                                        min_dist = dist;
                                    }
                                }
                        }
                    }
                }
                /* When we found the nearest button, make it pressed */
                if (min_dist_index != NOT_USED) {
                    const SclLayoutKeyCoordinate *popup_coordinate =
                        cache->get_cur_layout_key_coordinate(window, min_dist_index);
                    if (popup_coordinate) {
                        sclint x = popup_coordinate->x + (popup_coordinate->width / 2) - layout->mouse_manipulate_x;
                        sclint y = popup_coordinate->y + (popup_coordinate->height / 2) - layout->mouse_manipulate_y;
                        controller->mouse_press(window, x, y, context->get_last_touch_device_id());
                    }
                }

                /* The below code block seems unnecessary since we already invoked mouse_press() */
                /*context->set_cur_pressed_window(context->get_last_touch_device_id(), window);
                context->set_cur_pressed_key(context->get_last_touch_device_id(), min_dist_index);
                if (btncontext) {
                    btncontext->state = BUTTON_STATE_NORMAL;
                }*/

                windows->update_window(window, coordinate->x, coordinate->y, coordinate->width, coordinate->height);
            }
        }
    }

    return window;
}

bool CSCLWindows::close_popup(sclwindow window)
{
    printf("close_popup window (%p) \n",window);

    pop_window_in_Z_order_list(window);

    hide_window(get_dim_window());
    hide_window(window);
    return destroy_window(window);
}

bool CSCLWindows::close_all_popups(sclwindow skip_window /* = SCLWINDOW_INVALID */)
{
    sclboolean all_closed = TRUE;
    /* Close all the popup windows except the targetWindow */
    int loop = 0;
    sclwindow window;
    do {
        window = get_nth_popup_window(loop);
        if (window) {
            if (window != skip_window) {
                close_popup(window);
            } else {
                all_closed = FALSE;
            }
        }
        loop++;
    } while (window);

    /* If there is a popup still opened, don't destroy POPUP_TIMEOUT timer */
    return all_closed;
}

sclwindow
CSCLWindows::create_base_window(const sclwindow parent, scl16 width, scl16 height)
{
    SCL_DEBUG();

    if (m_initialized) {
        m_base_winctx.hidden = TRUE;
        m_base_winctx.geometry.width = width;
        m_base_winctx.geometry.height = height;
        m_base_winctx.is_virtual = FALSE;
        m_base_winctx.popup_type = POPUP_TYPE_NONE;
        m_base_winctx.opener.window = parent;
        m_base_winctx.geometry.x = m_base_winctx.geometry.y = 0;
        m_base_winctx.etc_info = NULL;
        m_base_winctx.window = get_scl_windows_impl()->create_base_window(parent, &m_base_winctx, width, height);

        push_window_in_Z_order_list(m_base_winctx.window);
    }

    // Update the position information
    //get_window_context(parent, TRUE);

    return m_base_winctx.window;
}

/**
 * Creates a new top-level window
 *
 * @Code
 *  CSCLGwes* gwes = CSCLGwes::get_instance();
 *  sclwindow popupWindow = gwes->m_windows->create_window(window, 100, 500, 200, 100, 4, POPUP_TYPE_BTN_RELEASE_POPUP, FALSE);
 *  if (popupWindow != NULL) {
 *     gwes->m_events->connect_window_events(popupWindow, SCL_EVENT_MOUSE | SCL_EVENT_EXPOSE);
 *      cache->recompute_layout(popupWindow);
 *      gwes->m_windows->show_window(popupWindow);
 *  }
 */
sclwindow
CSCLWindows::create_window(const SclWindowOpener opener, const SclRectangle &geometry, sclshort inputmode, sclshort layout, SCLPopupType popup_type, sclboolean is_virtual, sclint img_offset_x, sclint img_offset_y, sclint timeout)
{
    SCL_DEBUG();

    CSCLWindowsImpl* impl = get_scl_windows_impl();
    sclwindow window = SCLWINDOW_INVALID;

    if (impl) {
        for (sclint loop = 0;loop < MAX_POPUP_WINDOW;loop++) {
            if (m_popup_winctx[loop].window == SCLWINDOW_INVALID) {
                m_popup_winctx[loop].hidden = TRUE;
                m_popup_winctx[loop].opener = opener;
                m_popup_winctx[loop].etc_info = NULL;
                m_popup_winctx[loop].inputmode = inputmode;
                m_popup_winctx[loop].layout = layout;
                m_popup_winctx[loop].popup_type = popup_type;
                m_popup_winctx[loop].is_virtual = is_virtual;

                m_popup_winctx[loop].geometry = geometry;

                m_popup_winctx[loop].layout_image_offset.x = img_offset_x;
                m_popup_winctx[loop].layout_image_offset.y = img_offset_y;

                m_popup_winctx[loop].timeout = timeout;

                if (!is_virtual) {
                    window = impl->create_window(opener.window, &(m_popup_winctx[loop]), geometry.width, geometry.height);
                } else {
                    window = reinterpret_cast<sclwindow>(loop + 1);
                }
                if (window) {
                    m_popup_winctx[loop].window = window;
                }
                //set_window_rotation(window, context->get_rotation_degree());
                if (!m_popup_winctx[loop].is_virtual) {
                    impl->move_window(window, geometry.x, geometry.y);
                }
                break;
            }
        }

        // Update the position information
        //get_window_context(window, TRUE);

        if (window == NULL) {
            printf("Failed to create a new window. The size of window buffer has exeeded.\n");
        }
    }
    return window;
}

sclwindow
CSCLWindows::create_magnifier_window(const sclwindow parent, scl16 x, scl16 y, scl16 width, scl16 height)
{
    SCL_DEBUG();

    CSCLWindowsImpl* impl = get_scl_windows_impl();
    sclwindow window = SCLWINDOW_INVALID;

    if (impl && m_initialized) {
        if (m_magnifier_winctx.window == SCLWINDOW_INVALID) {
            window = impl->create_magnifier_window(parent, &m_magnifier_winctx, width, height);
            impl->set_keep_above(window, TRUE);
            if (window) {
                m_magnifier_winctx.window = window;
                m_magnifier_winctx.geometry.width = width;
                m_magnifier_winctx.geometry.height = height;
                m_magnifier_winctx.hidden = TRUE;
            }
        } else {
            window = m_magnifier_winctx.window;
        }
        set_parent(parent, window);

        if (window == NULL) {
            printf("Failed to create a new window. The size of window buffer has exeeded.\n");
        } else {
            printf("Magnifier Window %p created\n", window);
        }
    }

    // Update the position information
    //get_window_context(window, TRUE);

    return window;
}

sclwindow
CSCLWindows::get_magnifier_window()
{
    SCL_DEBUG();
    return m_magnifier_winctx.window;
}

sclwindow
CSCLWindows::create_dim_window(const sclwindow parent, SclWindowContext *winctx, scl16 width, scl16 height)
{
    SCL_DEBUG();

    CSCLWindowsImpl* impl = get_scl_windows_impl();
    sclwindow window = SCLWINDOW_INVALID;

    SclResParserManager *sclres_manager = SclResParserManager::get_instance();
    PSclDefaultConfigure default_configure = NULL;
    if (sclres_manager) {
        default_configure = sclres_manager->get_default_configure();
    }

    if (impl && m_initialized && default_configure) {
        if (m_dim_winctx.window == NULL) {
            m_dim_winctx.hidden = TRUE;
            if (default_configure->use_actual_dim_window) {
                window = impl->create_dim_window(parent, &m_dim_winctx, width, height);
            } else {
                window = reinterpret_cast<sclwindow>(SCLWINDOW_VIRTUAL_DIM);
                m_dim_winctx.is_virtual = TRUE;
            }
            if (window) {
                m_dim_winctx.window = window;
            }
        } else {
            window = m_dim_winctx.window;
        }

        if (window == NULL) {
            printf("Failed to create a new window. The size of window buffer has exeeded.\n");
        }
    }

    return window;
}

sclwindow
CSCLWindows::get_dim_window()
{
    SCL_DEBUG();
    return m_dim_winctx.window;
}

bool
CSCLWindows::destroy_window(sclwindow window)
{
    SCL_DEBUG();

    CSCLWindowsImpl* impl = get_scl_windows_impl();
    sclboolean ret = FALSE;

    if (impl) {
        if (window == m_base_winctx.window) {
            impl->destroy_window(window);
            memset(&m_base_winctx, 0x00, sizeof(SclWindowContext));
            m_base_winctx.window = SCLWINDOW_INVALID;
            ret = TRUE;
        } else if (window == m_magnifier_winctx.window) {
            impl->destroy_window(window);
            memset(&m_magnifier_winctx, 0x00, sizeof(SclWindowContext));
            m_magnifier_winctx.window = SCLWINDOW_INVALID;
            ret = TRUE;
        } else if (window == m_dim_winctx.window) {
            impl->destroy_window(window);
            memset(&m_dim_winctx, 0x00, sizeof(SclWindowContext));
            m_dim_winctx.window = SCLWINDOW_INVALID;
            ret = TRUE;
        } else {
            for (sclint loop = 0;loop < MAX_POPUP_WINDOW;loop++) {
                if (m_popup_winctx[loop].window == window) {
                    impl->destroy_window(window);
                    memset(&m_popup_winctx[loop], 0x00, sizeof(SclWindowContext));
                    ret = TRUE;
                    m_popup_winctx[loop].window = SCLWINDOW_INVALID;
                    break;
                }
            }
        }
    }

    return ret;
}

sclwindow
CSCLWindows::get_base_window()
{
    SCL_DEBUG();

    return m_base_winctx.window;
}

sclboolean
CSCLWindows::is_base_window(sclwindow window)
{
    SCL_DEBUG();

    sclboolean ret = FALSE;

    if (window != SCLWINDOW_INVALID) {
        if (window == m_base_winctx.window) {
            ret = TRUE;
        }
    }

    return ret;
}

sclwindow
CSCLWindows::find_by_etcinfo( void* etc_info )
{
    SCL_DEBUG();

    sclwindow ret = SCLWINDOW_INVALID;

    if (etc_info == m_base_winctx.etc_info) {
        ret = m_base_winctx.window;
    } else if (etc_info == m_magnifier_winctx.etc_info) {
        ret = m_magnifier_winctx.window;
    } else if (etc_info == m_dim_winctx.etc_info) {
        ret = m_dim_winctx.window;
    } else {
        for (sclint loop = 0;loop < MAX_POPUP_WINDOW;loop++) {
            if (etc_info == m_popup_winctx[loop].etc_info) {
                ret = m_popup_winctx[loop].window;
                break;
            }
        }
    }

    return ret;
}

SclWindowContext*
//CSCLWindows::get_window_context(sclwindow window, sclboolean geometry_update)
CSCLWindows::get_window_context(sclwindow window)
{
    SCL_DEBUG();

    SclWindowContext* ret = NULL;

    if (window == m_base_winctx.window) {
        /*if (geometry_update) {
            SclRectangle rect;
            get_window_rect(window, &rect);
            m_base_winctx.geometry.x = rect.x;
            m_base_winctx.geometry.y = rect.y;
        }*/
        ret = &m_base_winctx;
    } else if (window == m_magnifier_winctx.window) {
        ret = &m_magnifier_winctx;
    } else if (window == m_dim_winctx.window) {
        ret = &m_dim_winctx;
    } else {
        for (sclint loop = 0;loop < MAX_POPUP_WINDOW;loop++) {
            if (m_popup_winctx[loop].window == window) {
                /*if (geometry_update) {
                    SclRectangle rect;
                    get_window_rect(window, &rect);
                    m_popup_winctx[loop].geometry.x = rect.x;
                    m_popup_winctx[loop].geometry.y = rect.y;
                }*/
                ret = &m_popup_winctx[loop];
                break;
            }
        }
    }

    return ret;
}

void
CSCLWindows::set_window_context(sclwindow window, SclWindowContext* context)
{
    SCL_DEBUG();

    if (window == m_base_winctx.window) {
        memcpy(&m_base_winctx,context, sizeof(SclWindowContext));
    } else {
        for (sclint loop = 0;loop < MAX_POPUP_WINDOW;loop++) {
            if (m_popup_winctx[loop].window == window) {
                memcpy(&m_popup_winctx[loop], context, sizeof(SclWindowContext));
                break;
            }
        }
    }
}

scl8
CSCLWindows::find_popup_window_index(sclwindow window)
{
    SCL_DEBUG();

    scl8 ret = NOT_USED;

    for (sclint loop = 0;loop < MAX_POPUP_WINDOW;loop++) {
        if (m_popup_winctx[loop].window == window) {
            ret = loop;
            break;
        }
    }

    return ret;
}

void
CSCLWindows::move_window(const sclwindow window, scl16 x, scl16 y)
{
    SCL_DEBUG();

    sclboolean is_virtual = FALSE;
    if (window == m_base_winctx.window) {
        m_base_winctx.geometry.x = x;
        m_base_winctx.geometry.y = y;
        is_virtual = m_base_winctx.is_virtual;
    } else if (window == m_magnifier_winctx.window) {
        m_magnifier_winctx.geometry.x = x;
        m_magnifier_winctx.geometry.y = y;
        is_virtual = m_magnifier_winctx.is_virtual;
    } else if (window == m_dim_winctx.window) {
        m_dim_winctx.geometry.x = x;
        m_dim_winctx.geometry.y = y;
        is_virtual = m_dim_winctx.is_virtual;
    } else {
        for (sclint loop = 0;loop < MAX_POPUP_WINDOW;loop++) {
            if (m_popup_winctx[loop].window == window) {
                m_popup_winctx[loop].geometry.x = x;
                m_popup_winctx[loop].geometry.y = y;
                is_virtual = m_popup_winctx[loop].is_virtual;
                break;
            }
        }
    }
    if (!is_virtual) {
        CSCLWindowsImpl* impl = get_scl_windows_impl();
        if (impl) {
            impl->move_window(window, x, y);
        }
    }
}

void
CSCLWindows::resize_window(const sclwindow window, scl16 width, scl16 height)
{
    SCL_DEBUG();

    sclboolean is_virtual = FALSE;
    if (window == m_base_winctx.window) {
        m_base_winctx.geometry.width = width;
        m_base_winctx.geometry.height = height;
        is_virtual = m_base_winctx.is_virtual;
    } else if (window == m_magnifier_winctx.window) {
        m_magnifier_winctx.geometry.width = width;
        m_magnifier_winctx.geometry.height = height;
        is_virtual = m_magnifier_winctx.is_virtual;
    } else if (window == m_dim_winctx.window) {
        m_dim_winctx.geometry.width = width;
        m_dim_winctx.geometry.height = height;
        is_virtual = m_dim_winctx.is_virtual;
    } else {
        for (sclint loop = 0;loop < MAX_POPUP_WINDOW;loop++) {
            if (m_popup_winctx[loop].window == window) {
                m_popup_winctx[loop].geometry.width = width;
                m_popup_winctx[loop].geometry.height = height;
                is_virtual = m_popup_winctx[loop].is_virtual;
                break;
            }
        }
    }
    if (!is_virtual) {
        CSCLWindowsImpl* impl = get_scl_windows_impl();
        if (impl) {
            impl->resize_window(window, width, height);
        }
    }
}

void
CSCLWindows::move_resize_window(const sclwindow window, scl16 x, scl16 y, scl16 width, scl16 height)
{
    SCL_DEBUG();

    sclboolean is_virtual = FALSE;
    if (window == m_base_winctx.window) {
        m_base_winctx.geometry.x = x;
        m_base_winctx.geometry.y = y;
        m_base_winctx.geometry.width = width;
        m_base_winctx.geometry.height = height;
        is_virtual = m_base_winctx.is_virtual;
    } else if (window == m_magnifier_winctx.window) {
        m_magnifier_winctx.geometry.x = x;
        m_magnifier_winctx.geometry.y = y;
        m_magnifier_winctx.geometry.width = width;
        m_magnifier_winctx.geometry.height = height;
        is_virtual = m_magnifier_winctx.is_virtual;
    } else if (window == m_dim_winctx.window) {
        m_dim_winctx.geometry.x = x;
        m_dim_winctx.geometry.y = y;
        m_dim_winctx.geometry.width = width;
        m_dim_winctx.geometry.height = height;
        is_virtual = m_dim_winctx.is_virtual;
    } else {
        for (sclint loop = 0;loop < MAX_POPUP_WINDOW;loop++) {
            if (m_popup_winctx[loop].window == window) {
                m_popup_winctx[loop].geometry.x = x;
                m_popup_winctx[loop].geometry.y = y;
                m_popup_winctx[loop].geometry.width = width;
                m_popup_winctx[loop].geometry.height = height;
                is_virtual = m_popup_winctx[loop].is_virtual;
                break;
            }
        }
    }
    if (!is_virtual) {
        CSCLWindowsImpl* impl = get_scl_windows_impl();
        if (impl) {
            impl->move_resize_window(window, x, y, width, height);
        }
    }
}


/* Push given window into a Z-order list */
void CSCLWindows::push_window_in_Z_order_list(sclwindow window)
{
    SCL_DEBUG();

    sclint loop;

    for (loop = 0;loop < MAX_ZORDER_NUM;loop++) {
        if (m_Z_order_list[loop] == SCLWINDOW_INVALID) {
            m_Z_order_list[loop] = window;
            return;
        }
    }
}

/* Pop given window from a Z-order list */
void CSCLWindows::pop_window_in_Z_order_list(sclwindow window)
{
    SCL_DEBUG();

    sclint loop;
    sclboolean found = FALSE;

    for (loop = 0;loop < MAX_ZORDER_NUM - 1;loop++) {
        if (m_Z_order_list[loop] == window || found) {
            found = TRUE;
            m_Z_order_list[loop] = m_Z_order_list[loop + 1];
        }
    }

    m_Z_order_list[MAX_ZORDER_NUM - 1] = SCLWINDOW_INVALID;
}

/* Search n-th window in the Z-order stack, starting from the top (TOPMOST window would be the 0 index) */
sclwindow CSCLWindows::get_nth_window_in_Z_order_list(sclbyte index)
{
    SCL_DEBUG();

    sclint loop;

    for (loop = MAX_ZORDER_NUM - 1;loop >= 0;loop--) {
        if (m_Z_order_list[loop] != SCLWINDOW_INVALID) {
            if (index == 0) {
                return m_Z_order_list[loop];
            }
            index--;
        }
    }

    return SCLWINDOW_INVALID;
}

sclwindow CSCLWindows::get_nth_popup_window( sclbyte index )
{
    SCL_DEBUG();

    scl_assert_return_null(index >= 0 && index < MAX_POPUP_WINDOW);

    if (index < MAX_POPUP_WINDOW) {
        return m_popup_winctx[index].window;
    }

    return SCLWINDOW_INVALID;
}

sclbyte CSCLWindows::get_Z_order(sclwindow window)
{
    SCL_DEBUG();

    sclbyte loop;
    for (loop = 0;loop < MAX_ZORDER_NUM;loop++) {
        if (m_Z_order_list[loop] == window) {
            return loop;
        }
    }
    return NOT_USED;
}

void CSCLWindows::set_parent( const sclwindow parent, const sclwindow window )
{
    /* Do not set parent if the window is a virtual window */
    //SclWindowContext *winctx = get_window_context(window, FALSE);
    SclWindowContext *winctx = get_window_context(window);
    if (winctx) {
        if (!(winctx->is_virtual)) {
            CSCLWindowsImpl* impl = get_scl_windows_impl();
            if (impl) {
                impl->set_parent(parent, window);
            }
        }
    }
}

void CSCLWindows::set_window_rotation(const sclwindow window, SCLRotation rotation)
{
    SCL_DEBUG();

    CSCLWindowsImpl* impl = get_scl_windows_impl();

    if (impl) {
        if (window == NULL) {
            impl->set_window_rotation(m_base_winctx.window, rotation);
            if (SCLWINDOW_INVALID != m_magnifier_winctx.window) {
                CSCLUtils *utils = CSCLUtils::get_instance();
                sclfloat scale_rate_x, scale_rate_y;
                if (rotation == ROTATION_90_CW || rotation == ROTATION_90_CCW) {
                    scale_rate_x = utils->get_scale_rate_y();
                    scale_rate_y = utils->get_scale_rate_x();
                } else {
                    scale_rate_x = utils->get_scale_rate_x();
                    scale_rate_y = utils->get_scale_rate_y();
                }

                SclResParserManager *sclres_manager = SclResParserManager::get_instance();
                PSclMagnifierWndConfigure magnifier_configure = NULL;
                if (sclres_manager) {
                    magnifier_configure = sclres_manager->get_magnifier_configure();
                }
                if (magnifier_configure) {
                    m_magnifier_winctx.geometry.width =
                        magnifier_configure->width * utils->get_custom_scale_rate_x();
                    m_magnifier_winctx.geometry.height =
                        magnifier_configure->height * utils->get_custom_scale_rate_y();

                    impl->set_window_rotation(m_magnifier_winctx.window, rotation);
                }
            }

            if (SCLWINDOW_INVALID != m_dim_winctx.window) {
                /* For indivisual window rotation */
                impl->set_window_rotation(m_dim_winctx.window, rotation);
                //resize_window(m_dim_winctx.window, m_base_winctx.width, m_base_winctx.height);
                //move_window(m_dim_winctx.window, m_base_winctx.x, m_base_winctx.y);
                hide_window(m_dim_winctx.window);
            }
            /* For indivisual window rotation
            for (int loop = 0;loop < MAX_POPUP_WINDOW;loop++) {
                if (m_popup_winctx[loop].window != SCLWINDOW_INVALID) {
                    if (!m_popup_winctx[loop].isVirtual) {
                        get_scl_windows_impl()->set_window_rotation(m_popup_winctx[loop].window, degree);
                    }
                }
            }
            */
        } else {
            impl->set_window_rotation(window, rotation);
            printf("## set_window_rotation : %d \n", rotation);
        }
    }

    // Update the position information
    //get_window_context(window, TRUE);
    SclWindowContext *winctx = get_window_context(window);
    if (winctx) {
        get_window_rect(window, &(winctx->geometry));
    }
}


void
CSCLWindows::show_window(const sclwindow window, sclboolean queue /*= FALSE*/)
{
    SCL_DEBUG();

    if (window == m_base_winctx.window) {
        m_base_winctx.hidden = FALSE;
    } else if (window == m_magnifier_winctx.window) {
        m_magnifier_winctx.hidden = FALSE;
    } else if (window == m_dim_winctx.window) {
        m_dim_winctx.hidden = FALSE;
    } else {
        for (sclint loop = 0;loop < MAX_POPUP_WINDOW;loop++) {
            if (m_popup_winctx[loop].window == window) {
                m_popup_winctx[loop].hidden = FALSE;
                break;
            }
        }
    }

    CSCLWindowsImpl* impl = get_scl_windows_impl();
    if (impl) {
        impl->show_window(window, queue);
    }
}

void
CSCLWindows::hide_window(const sclwindow window, sclboolean force /*= FALSE*/)
{
    SCL_DEBUG();

    if (window == m_base_winctx.window) {
        m_base_winctx.hidden = TRUE;
    } else if (window == m_magnifier_winctx.window) {
        m_magnifier_winctx.hidden = TRUE;
    } else if (window == m_dim_winctx.window) {
        m_dim_winctx.hidden = TRUE;
    } else {
        for (sclint loop = 0;loop < MAX_POPUP_WINDOW;loop++) {
            if (m_popup_winctx[loop].window == window) {
                m_popup_winctx[loop].hidden = TRUE;
                break;
            }
        }
    }

    CSCLWindowsImpl* impl = get_scl_windows_impl();
    if (impl) {
        impl->hide_window(window, force);
    }
}

void
CSCLWindows::set_keep_above(const sclwindow window, sclboolean keep_above)
{
    CSCLWindowsImpl* impl = get_scl_windows_impl();
    if (impl) {
        impl->set_keep_above(window, keep_above);
    }
}

void
CSCLWindows::set_update_pending(sclboolean pend)
{
    m_pending_update = pend;
    if (!pend) {
        update_window(m_base_winctx.window);
    }
}

sclboolean
CSCLWindows::get_update_pending()
{
    return m_pending_update;
}

void
CSCLWindows::update_window(const sclwindow window,
                           scl16 x /*= 0*/, scl16 y /*= 0*/, scl16 width /*= 0*/, scl16 height /*= 0*/ )
{
    if (!m_pending_update) {
        CSCLWindowsImpl* impl = get_scl_windows_impl();
        if (impl) {
            impl->update_window(window, x, y, width, height);
        }
    }
}

sclboolean
CSCLWindows::get_window_rect(const sclwindow window, SclRectangle *rect) {
    SCL_DEBUG();

    sclboolean is_virtual = FALSE;
    sclboolean ret = FALSE;
    if (rect) {
        if (window == m_base_winctx.window) {
            is_virtual = m_base_winctx.is_virtual;
            if (is_virtual) {
                *rect = m_base_winctx.geometry;
            }
        } else if (window == m_magnifier_winctx.window) {
            is_virtual = m_magnifier_winctx.is_virtual;
            if (is_virtual) {
                *rect = m_magnifier_winctx.geometry;
            }
        } else if (window == m_dim_winctx.window) {
            is_virtual = m_dim_winctx.is_virtual;
            if (is_virtual) {
                *rect = m_dim_winctx.geometry;
            }
        } else {
            for (sclint loop = 0;loop < MAX_POPUP_WINDOW;loop++) {
                if (m_popup_winctx[loop].window == window) {
                    is_virtual = m_popup_winctx[loop].is_virtual;
                    if (is_virtual) {
                        *rect = m_popup_winctx[loop].geometry;
                    }
                    break;
                }
            }
        }
    }
    if (!is_virtual) {
         CSCLWindowsImpl* impl = get_scl_windows_impl();
         if (impl) {
             ret = impl->get_window_rect(window, rect);
         }
    }

    return ret;
}
