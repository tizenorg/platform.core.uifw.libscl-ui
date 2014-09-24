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

#include "scluiimpl.h"
#include "scldebug.h"
#include "scluibuilder.h"
#include "sclcontroller.h"
#include "sclresourcecache.h"
#include "sclerroradjustment.h"
#include "sclres_manager.h"
#include "scleventhandler.h"
#include "sclkeyfocushandler.h"
#include "sclanimator.h"

using namespace scl;

CSCLUIImpl::CSCLUIImpl()
{
    SCL_DEBUG();

    /*CSCLUIBuilder *builder = CSCLUIBuilder::get_instance();
    CSCLController *controller = CSCLController::get_instance();
    if (builder && controller) {
        builder->init(SCLWINDOW_INVALID);
        controller->init();
    }*/
    m_initialized = FALSE;
    /* FIXME whethe need the libscl-ui autocapital the shift state */
    /* if set to FALSE, the libscl-ui will not auto handle the shift state */
    /* the client will handle outside the libscl-ui, default is TRUE */
    m_autocapital_shift_state = TRUE;
}

CSCLUIImpl::~CSCLUIImpl()
{
    SCL_DEBUG();
}

CSCLUIImpl*
CSCLUIImpl::get_instance()
{
    static CSCLUIImpl instance;
    return &instance;
}

sclboolean CSCLUIImpl::init(sclwindow parent, const SCLParserType parser_type, const char *entry_filepath)
{
    SCL_DEBUG();

    SCL_DEBUG_ELAPASED_TIME_START();

    SclResParserManager *sclres_manager = SclResParserManager::get_instance();
    CSCLUIBuilder *builder = CSCLUIBuilder::get_instance();
    CSCLResourceCache *cache = CSCLResourceCache::get_instance();
    CSCLUtils *utils = CSCLUtils::get_instance();

    if (sclres_manager && builder && cache && utils) {
        sclres_manager->init(parser_type, entry_filepath);

        utils->init();
        cache->init();
        builder->init(parent);

        PSclDefaultConfigure default_configure = sclres_manager->get_default_configure();
        if (default_configure) {
            set_cur_sublayout(default_configure->default_sub_layout);
        }

        m_initialized = TRUE;
    }

    SCL_DEBUG_ELAPASED_TIME_END();

    return TRUE;
}

void
CSCLUIImpl::fini()
{
    CSCLUtils *utils = CSCLUtils::get_instance();
    if (utils) {
        utils->fini();
    }

    CSCLGwes *gwes = CSCLGwes::get_instance();
    if (gwes) {
        gwes->fini();
    }
}

/**
 * Shows the SCL main window
 * For displaying the SCL UI, you should explicitly call this function after CSCLUIImpl class is created
 */
void
//CSCLUIImpl::show(sclboolean auto_relocate /* = TRUE */ )
CSCLUIImpl::show()
{
    SCL_DEBUG();

    if (m_initialized) {
        CSCLUtils *utils = CSCLUtils::get_instance();
        CSCLWindows *windows = CSCLWindows::get_instance();
        CSCLContext *context = CSCLContext::get_instance();
        CSCLController *controller = CSCLController::get_instance();

        if (windows && controller && context) {
            //if (auto_relocate) {
            if (TRUE) {
                /* Let's relocate our base window - bottomed center aligned */
                SclRectangle rect = get_main_window_rect();

                sclint scrx, scry;
                utils->get_screen_resolution(&scrx, &scry);

                sclint pos_x = (scrx - rect.width) / 2;
                sclint pos_y = (scry - rect.height);

                windows->move_window(windows->get_base_window(), pos_x, pos_y);
            }

            sclwindow window = windows->get_base_window();
            controller->handle_engine_signal(SCL_SIG_SHOW);
            windows->show_window(window);
        }
    }
}

/**
 * Hides the SCL main window
 * The real hide action does not work about base window because that is child of the active window
 */
void
CSCLUIImpl::hide()
{
    SCL_DEBUG();

    if (m_initialized) {
        CSCLController *controller = CSCLController::get_instance();
        CSCLWindows *windows = CSCLWindows::get_instance();
        if (controller && windows) {
            controller->handle_engine_signal(SCL_SIG_HIDE);
            windows->hide_window(windows->get_base_window());
        }
    }
}


/**
 * Regists an event callback function
 * so that the user which uses SCL can recevies all events occuring in running
 */
void
CSCLUIImpl::set_ui_event_callback(ISCLUIEventCallback *callback, const sclchar *input_mode)
{
    SCL_DEBUG();

    if (m_initialized) {
        CSCLEventHandler *handler = CSCLEventHandler::get_instance();
        if (handler) {
            handler->set_event_callback(callback, input_mode);
        }
    }
}

/**
 * Sets the current input mode to the given mode
 * @Usage
 * gCore->set_input_mode("INPUT_MODE_SYMBOL");
 */
sclboolean
CSCLUIImpl::set_input_mode(const sclchar *input_mode)
{
    SCL_DEBUG();
    SCL_DEBUG_ELAPASED_TIME_START();

    sclboolean ret = FALSE;

    if (m_initialized) {
        CSCLController *controller = CSCLController::get_instance();
        CSCLWindows *windows = CSCLWindows::get_instance();
        CSCLEventHandler *handler = CSCLEventHandler::get_instance();
        CSCLKeyFocusHandler* focus_handler = CSCLKeyFocusHandler::get_instance();

        scl8 mode = NOT_USED;

        SclResParserManager *sclres_manager = SclResParserManager::get_instance();
        if (sclres_manager) {
            mode = sclres_manager->get_inputmode_id(input_mode);
        }

        if (controller && windows && handler && focus_handler && mode != NOT_USED) {
            handler->set_input_mode(input_mode);
            ret = controller->process_input_mode_change(mode);
            windows->update_window(windows->get_base_window());
            focus_handler->init_key_index();
        }
    }

    SCL_DEBUG_ELAPASED_TIME_END();
    return ret;
}

/**
 * Returns the current input mode
 */
const sclchar*
CSCLUIImpl::get_input_mode()
{
    SCL_DEBUG();

    const sclchar *ret = NULL;
    if (m_initialized) {
        CSCLContext *context = CSCLContext::get_instance();
        SclResParserManager *sclres_manager = SclResParserManager::get_instance();
        if (context && sclres_manager) {
            scl8 inputmode_id = context->get_input_mode();
            ret = sclres_manager->get_inputmode_name(inputmode_id);
        }
    }
    return ret;
}


/**
 * Sets the given popup window's input mode to the given mode
 * @Usage
 * gCore->set_input_mode("INPUT_MODE_SYMBOL");
 */
sclboolean
CSCLUIImpl::set_popup_input_mode(sclwindow window, const sclchar *input_mode)
{
    SCL_DEBUG();
    SCL_DEBUG_ELAPASED_TIME_START();

    sclboolean ret = FALSE;

    if (m_initialized) {
        CSCLWindows *windows = CSCLWindows::get_instance();
        CSCLResourceCache *cache = CSCLResourceCache::get_instance();
        CSCLContext *context = CSCLContext::get_instance();

        scl8 mode = NOT_USED;
        sclshort layout = NOT_USED;
        SclWindowContext *window_context = NULL;

        SclResParserManager *sclres_manager = SclResParserManager::get_instance();
        if (sclres_manager && windows && context) {
            SCLDisplayMode display_mode = context->get_display_mode();
            PSclInputModeConfigure sclres_input_mode_configure = sclres_manager->get_input_mode_configure_table();
            mode = sclres_manager->get_inputmode_id(input_mode);
            window_context = windows->get_window_context(window);
            if (sclres_input_mode_configure &&
                scl_check_arrindex(mode, MAX_SCL_INPUT_MODE) &&
                scl_check_arrindex(display_mode, DISPLAYMODE_MAX)) {
                    layout = sclres_manager->get_layout_id(sclres_input_mode_configure[mode].layouts[display_mode]);
            }
        }

        if (cache && windows && window_context) {
            if (mode != NOT_USED && mode != window_context->inputmode && layout != NOT_USED) {
                window_context->inputmode = mode;
                window_context->layout = layout;
                cache->recompute_layout(window);
                windows->update_window(window);
                ret = TRUE;
            }
        }
    }

    SCL_DEBUG_ELAPASED_TIME_END();
    return ret;
}

/**
 * Returns the given window's input mode
 */
const sclchar*
CSCLUIImpl::get_popup_input_mode(sclwindow window)
{
    SCL_DEBUG();

    const sclchar *ret = NULL;

    if (m_initialized) {
        CSCLWindows *windows = CSCLWindows::get_instance();
        SclResParserManager *sclres_manager = SclResParserManager::get_instance();
        if (windows && sclres_manager) {
            SclWindowContext *window_context = windows->get_window_context(window);
            if (window_context) {
                if (scl_check_arrindex(window_context->inputmode, MAX_SCL_INPUT_MODE)) {
                    ret = sclres_manager->get_inputmode_name(window_context->inputmode);
                }
            }
        }
    }

    return ret;
}

/**
 * Sets the current rotation
 */
sclboolean
CSCLUIImpl::set_rotation(SCLRotation rotation)
{
    SCL_DEBUG();

    sclboolean ret = FALSE;

    if (m_initialized) {
        CSCLUtils *utils = CSCLUtils::get_instance();
        CSCLWindows *windows = CSCLWindows::get_instance();
        CSCLController *controller = CSCLController::get_instance();

        if (utils && windows && controller) {
            ret = controller->process_rotation_change(rotation);

            //if (auto_relocate) {
            if (TRUE) {
                /* Let's relocate our base window - bottomed center aligned */
                SclRectangle rect = get_main_window_rect();

                sclint scrx, scry;
                utils->get_screen_resolution(&scrx, &scry);

                sclint pos_x = (scrx - rect.width) / 2;
                sclint pos_y = (scry - rect.height);

                windows->move_window(windows->get_base_window(), pos_x, pos_y);
            }
        }
    }

    return ret;
}

/**
 * Returns the current rotation
 */
SCLRotation
CSCLUIImpl::get_rotation()
{
    SCL_DEBUG();

    SCLRotation ret = ROTATION_0;

    CSCLContext *context = CSCLContext::get_instance();
    if (context) {
        ret = context->get_rotation();
    }

    return ret;
}

/**
 * Returns the current display mode
 */
SCLDisplayMode
CSCLUIImpl::get_display_mode()
{
    SCL_DEBUG();

    SCLDisplayMode ret = DISPLAYMODE_MAX;
    if (m_initialized) {
        CSCLContext *context = CSCLContext::get_instance();
        if (context) {
            ret = context->get_display_mode();
        }
    }
    return ret;
}

/**
 * Sets a private key to the current context
 * The other properties except given parameters will keep to the orginal value.
 * @Usage
 * gCore->set_private_key(INPUT_MODE_NUMBER, LYT_PORTRAIT_NOW_3x4, 0, "private", 999, TRUE);
 *
 * @param fRedraw If true, it will redraw the current key
 */
sclint
CSCLUIImpl::set_private_key(const sclchar* custom_id, sclchar* label, sclchar* imagelabel[SCL_BUTTON_STATE_MAX], sclchar* imagebg[SCL_BUTTON_STATE_MAX], sclulong key_event, sclchar *key_value, sclboolean fRedraw)
{
    SCL_DEBUG();
    sclint ret = NOT_USED;
    if (m_initialized) {
        CSCLWindows *windows = CSCLWindows::get_instance();
        CSCLResourceCache *cache = CSCLResourceCache::get_instance();
        if (windows && cache) {
            ret = cache->set_private_key(custom_id, label, imagelabel, imagebg,
                key_event, key_value, fRedraw, windows->get_update_pending());
        }
    }
    return ret;
}


/**
* Unsets a private key to the current context
*/
void
CSCLUIImpl::unset_private_key(const sclchar* custom_id)
{
    SCL_DEBUG();
    if (m_initialized) {
        CSCLResourceCache *cache = CSCLResourceCache::get_instance();
        if (cache) {
            cache->unset_private_key(custom_id);
        }
    }
}

/**
* Sets the current theme
*/
/* FIXME : If setting themename is not allowed before initializing,
           the default theme has to be loaded regardless of current theme name
           and the appropriate current theme has to be loaded afterwards, which is very inefficient */
sclboolean
CSCLUIImpl::set_cur_themename(const sclchar *themename)
{
    sclboolean ret = FALSE;

    if (m_initialized) {
        CSCLResourceCache *cache = CSCLResourceCache::get_instance();
        CSCLWindows *windows = CSCLWindows::get_instance();
        if (cache && windows) {
            cache->set_cur_themename(themename);
            windows->update_window(windows->get_base_window());
            int loop = 0;
            sclwindow window;
            do {
                window = windows->get_nth_popup_window(loop);
                if (window) {
                    windows->update_window(window);
                }
                loop++;
            } while (window);
        }
        ret = TRUE;
    }

    return ret;
}

SCLShiftState
CSCLUIImpl::get_shift_state()
{
    SCLShiftState ret = SCL_SHIFT_STATE_OFF;
    if (m_initialized) {
        CSCLContext *context = CSCLContext::get_instance();
        if (context) {
            ret = context->get_shift_state();
        }
    }
    return ret;
}

void
CSCLUIImpl::set_shift_state(SCLShiftState state)
{
    if (m_initialized) {
        CSCLUtils *utils = CSCLUtils::get_instance();
        CSCLContext *context = CSCLContext::get_instance();
        CSCLWindows *windows = CSCLWindows::get_instance();

        /*inform the client that the shift state changed */
        CSCLEventHandler *handler = CSCLEventHandler::get_instance();
        if (handler) {
            SclNotiShiftStateChangeDesc desc;
            desc.ui_event_desc = NULL;
            desc.shift_state = state;

            SCLEventReturnType ret = handler->on_event_notification(SCL_UINOTITYPE_SHIFT_STATE_CHANGE, &desc);
            if (ret == SCL_EVENT_DONE) {
                return;
            }
        }

        if (context && windows && utils) {
            SCLShiftState current_state = context->get_shift_state();
            context->set_shift_state(state);
            if (state != current_state) {
                windows->update_window(windows->get_base_window());
                if (context->get_tts_enabled()) {
                    if (state == SCL_SHIFT_STATE_ON) {
                        utils->play_tts(SCL_SHIFT_STATE_ON_HINT_STRING);
                    } else if (state == SCL_SHIFT_STATE_LOCK) {
                        utils->play_tts(SCL_SHIFT_STATE_LOCK_HINT_STRING);
                    } else {
                        utils->play_tts(SCL_SHIFT_STATE_OFF_HINT_STRING);
                    }
                }
            }
        }
    }
}

sclboolean
CSCLUIImpl::get_caps_lock_mode()
{
    sclboolean ret = FALSE;
    if (m_initialized) {
        CSCLContext *context = CSCLContext::get_instance();
        if (context) {
            ret = context->get_caps_lock_mode();
        }
    }
    return ret;
}

void
CSCLUIImpl::set_caps_lock_mode(sclboolean mode)
{
    if (m_initialized) {
        CSCLContext *context = CSCLContext::get_instance();
        CSCLWindows *windows = CSCLWindows::get_instance();

        if (context && windows) {
            sclboolean current_mode = context->get_caps_lock_mode();
            if (mode != current_mode) {
                context->set_caps_lock_mode(mode);
                windows->update_window(windows->get_base_window());
            }
        }
    }
}

/**
 * This function will be called by the user which uses SCL when the context of the focus application is changed
 * ISE user should explicitly call this function when the context of application is changed.
 * For instance, focus-changed, application-changed,, and so on.
 * This function will call CSCLController to init the related variables.
 */
void
CSCLUIImpl::notify_app_focus_changed()
{
    SCL_DEBUG();
    if (m_initialized) {
        CSCLController *controller = CSCLController::get_instance();
        if (controller) {
            controller->handle_engine_signal(SCL_SIG_FOCUS_CHANGE);
        }
    }
}

void
CSCLUIImpl::reset_popup_timeout()
{
    SCL_DEBUG();
    if (m_initialized) {
        CSCLEvents *events = CSCLEvents::get_instance();
        CSCLWindows *windows = CSCLWindows::get_instance();

        if (events && windows) {
            events->destroy_timer(SCL_TIMER_POPUP_TIMEOUT);

            sclbyte index = 0;
            sclboolean timerset = FALSE;
            sclwindow window = SCLWINDOW_INVALID;
            SclWindowContext *window_context = NULL;
            do {
                window = windows->get_nth_window_in_Z_order_list(index);
                //window_context = windows->get_window_context(window, FALSE);
                window_context = windows->get_window_context(window);
                if (window_context) {
                    if (window_context->timeout != 0) {
                        events->create_timer(SCL_TIMER_POPUP_TIMEOUT, window_context->timeout, 0, TRUE);
                        timerset = TRUE;
                    }
                    index++;
                }
            } while (index < MAX_ZORDER_NUM && window != SCLWINDOW_INVALID && !timerset);
        }
    }
}

void
CSCLUIImpl::close_all_popups()
{
    SCL_DEBUG();
    if (m_initialized) {
        CSCLWindows *windows = CSCLWindows::get_instance();
        if (windows) {
            windows->close_all_popups();
        }
    }
}

/**
 * Returns a scale rate (see default screen resolution in sclconfig.h file)
 */
sclfloat
CSCLUIImpl::get_scale_rate()
{
    sclfloat ret = 0.0f;
    if (m_initialized) {
        CSCLUtils *utils = CSCLUtils::get_instance();
        if (utils) {
            ret = utils->get_smallest_scale_rate();
        }
    }
    return ret;
}

/**
 * Returns a calculated x value according to the current screen resolution
 */
scl16
CSCLUIImpl::get_scaled_x(scl16 x)
{
    scl16 ret = 0;
    if (m_initialized) {
        CSCLUtils *utils = CSCLUtils::get_instance();
        if (utils) {
            ret = utils->get_scaled_x(x);
        }
    }
    return ret;
}

/**
 * Returns a calculated y value according to the current screen resolution
 */
scl16
CSCLUIImpl::get_scaled_y(scl16 y)
{
    scl16 ret = 0;
    if (m_initialized) {
        CSCLUtils *utils = CSCLUtils::get_instance();
        if (utils) {
            ret = utils->get_scaled_y(y);
        }
    }
    return ret;
}

/**
 * Returns the current rate x value for resizing the keyboard's content
 */
sclfloat
CSCLUIImpl::get_custom_scale_rate_x()
{
    sclfloat ret = 1.0f;
    if (m_initialized) {
        CSCLUtils *utils = CSCLUtils::get_instance();
        if (utils) {
            ret = utils->get_custom_scale_rate_x();
        }
    }
    return ret;
}

/**
 * Returns the current rate y value for resizing the keyboard's content
 */
sclfloat
CSCLUIImpl::get_custom_scale_rate_y()
{
    sclfloat ret = 1.0f;
    if (m_initialized) {
        CSCLUtils *utils = CSCLUtils::get_instance();
        if (utils) {
            ret = utils->get_custom_scale_rate_y();
        }
    }
    return ret;
}

/**
 * Sets the custom rate value for resizing the keyboard's content
 */
void
CSCLUIImpl::set_custom_scale_rate(sclfloat x, sclfloat y)
{
    if (m_initialized) {
        CSCLUtils *utils = CSCLUtils::get_instance();
        CSCLResourceCache *cache = CSCLResourceCache::get_instance();
        CSCLWindows *windows = CSCLWindows::get_instance();
        if (utils && cache && windows) {
            utils->set_custom_scale_rate_x(x);
            utils->set_custom_scale_rate_y(y);

            cache->recompute_layout(windows->get_base_window());
        }
    }
}

/**
 * Sets the custom starting coordinates for drawing the keyboard's content
 */
void
CSCLUIImpl::set_custom_starting_coordinates(sclint x, sclint y)
{
    if (m_initialized) {
        CSCLResourceCache *cache = CSCLResourceCache::get_instance();
        if (cache) {
            cache->set_custom_starting_coordinates(x, y);
        }
    }
}

/**
 * Sets the custom starting coordinates option for drawing the keyboard's content
 */
void
CSCLUIImpl::set_custom_starting_coordinates_option(SCLStartingCoordinatesOption option)
{
    if (m_initialized) {
        CSCLResourceCache *cache = CSCLResourceCache::get_instance();
        if (cache) {
            cache->set_custom_starting_coordinates_option(option);
        }
    }
}

/**
 * Returns the scl base window size
 */
SclRectangle
CSCLUIImpl::get_main_window_rect()
{
    SclRectangle ret = {0,0,0,0};

    if (m_initialized) {
        CSCLResourceCache *cache = CSCLResourceCache::get_instance();
        CSCLWindows *windows = CSCLWindows::get_instance();
        if (cache && windows) {
            //const SclLayout *layout  = cache->get_cur_layout(windows->get_base_window());
            SclWindowContext *window_context = windows->get_window_context(windows->get_base_window());
            if (window_context) {
                ret.x = window_context->geometry.x;
                ret.y = window_context->geometry.y;
                ret.width = window_context->geometry.width;
                ret.height = window_context->geometry.height;
            }
        }
    }

    return ret;
}

/**
 * Returns the scl base window size
 */
SclSize
CSCLUIImpl::get_input_mode_size(const sclchar *input_mode, SCLDisplayMode display_mode)
{
    SclSize ret = {0,0};

    if (m_initialized) {
        CSCLUtils *utils = CSCLUtils::get_instance();
        SclResParserManager *sclres_manager = SclResParserManager::get_instance();
        if (utils && sclres_manager) {
            const PSclInputModeConfigure sclres_input_mode_configure = sclres_manager->get_input_mode_configure_table();
            const PSclLayout sclres_layout = sclres_manager->get_layout_table();
            sclint inputmode = sclres_manager->get_inputmode_id(input_mode);
            if (sclres_input_mode_configure && sclres_layout) {
                sclint layout = sclres_manager->get_layout_id(
                        sclres_input_mode_configure[inputmode].layouts[display_mode]);

                ret.width = sclres_layout[layout].width;
                ret.height = sclres_layout[layout].height;
            }
        }
    }

    return ret;
}

/**
* Returns the screen resolution
*/
void
CSCLUIImpl::get_screen_resolution(sclint *width, sclint *height)
{
    if (m_initialized) {
        CSCLUtils *utils = CSCLUtils::get_instance();
        if (utils && width && height) {
            utils->get_screen_resolution(width, height);
        }
    }
}



void
CSCLUIImpl::set_debug_mode(SCLDebugMode mode)
{
    if (m_initialized) {
        CSCLController *controller = CSCLController::get_instance();
        if (controller) {
            controller->set_debug_mode(mode);
        }
    }
}

SCLDebugMode
CSCLUIImpl::get_debug_mode()
{
    SCLDebugMode ret = DEBUGMODE_DISABLED;
    if (m_initialized) {
        CSCLController *controller = CSCLController::get_instance();
        if (controller) {
            ret = controller->get_debug_mode();
        }
    }
    return ret;
}

void
CSCLUIImpl::set_update_pending(sclboolean pend)
{
    if (m_initialized) {
        CSCLWindows *windows = CSCLWindows::get_instance();
        if (windows) {
            windows->set_update_pending(pend);
        }
    }
}

void
CSCLUIImpl::enable_button(const sclchar* custom_id, sclboolean enabled)
{
    if (m_initialized) {
        CSCLResourceCache *cache = CSCLResourceCache::get_instance();
        if (cache) {
            cache->enable_button(custom_id, enabled);
        }
    }
}

sclint
CSCLUIImpl::get_multi_touch_context_num()
{
    sclint ret = 0;

    if (m_initialized) {
        CSCLContext *context = CSCLContext::get_instance();
        if (context) {
            ret = context->get_multi_touch_context_num();
        }
    }
    return ret;
}

sclboolean
CSCLUIImpl::get_multi_touch_event(sclint seqorder, SclUIEventDesc *desc)
{
    sclboolean ret = FALSE;

    if (m_initialized) {
        CSCLContext *context = CSCLContext::get_instance();
        if (context) {
            ret = context->get_multi_touch_event(seqorder, desc);
        }
    }
    return ret;
}

sclboolean
CSCLUIImpl::set_longkey_duration(scllong msc)
{
    sclboolean ret = FALSE;

    if (m_initialized) {
        CSCLController *controller = CSCLController::get_instance();
        if (controller) {
            ret = controller->set_longkey_duration(msc);
        }
    }
    return ret;
}

sclboolean
CSCLUIImpl::set_longkey_cancel_dist(sclshort dist)
{
    sclboolean ret = FALSE;

    if (m_initialized) {
        CSCLController *controller = CSCLController::get_instance();
        if (controller) {
            ret = controller->set_longkey_cancel_dist(dist);
        }
    }
    return ret;
}

sclboolean
CSCLUIImpl::set_repeatkey_duration(scllong msc)
{
    sclboolean ret = FALSE;

    if (m_initialized) {
        CSCLController *controller = CSCLController::get_instance();
        if (controller) {
            ret = controller->set_repeatkey_duration(msc);
        }
    }
    return ret;
}

sclboolean
CSCLUIImpl::set_autopoup_key_duration(scllong msc)
{
    sclboolean ret = FALSE;

    if (m_initialized) {
        CSCLController *controller = CSCLController::get_instance();
        if (controller) {
            ret = controller->set_autopopup_key_duration(msc);
        }
    }
    return ret;
}

sclboolean
CSCLUIImpl::set_button_delay_duration(scllong msc)
{
    sclboolean ret = FALSE;

    if (m_initialized) {
        CSCLController *controller = CSCLController::get_instance();
        if (controller) {
            ret = controller->set_button_delay_duration(msc);
        }
    }
    return ret;
}

void
CSCLUIImpl::enable_magnifier(sclboolean enabled)
{
    if (m_initialized) {
        CSCLContext *context = CSCLContext::get_instance();
        if (context) {
            context->set_magnifier_enabled(enabled);
        }
    }
}

void
CSCLUIImpl::enable_sound(sclboolean enabled)
{
    if (m_initialized) {
        CSCLContext *context = CSCLContext::get_instance();
        if (context) {
            context->set_sound_enabled(enabled);
        }
    }
}

void
CSCLUIImpl::enable_vibration(sclboolean enabled)
{
    if (m_initialized) {
        CSCLContext *context = CSCLContext::get_instance();
        if (context) {
            context->set_vibration_enabled(enabled);
        }
    }
}

void
CSCLUIImpl::enable_tts(sclboolean enabled)
{
    if (m_initialized) {
        CSCLContext *context = CSCLContext::get_instance();
        if (context) {
            context->set_tts_enabled(enabled);
        }
    }
}

void
CSCLUIImpl::enable_shift_multi_touch(sclboolean enabled)
{
    if (m_initialized) {
        CSCLContext *context = CSCLContext::get_instance();
        if (context) {
            context->set_shift_multi_touch_enabled(enabled);
        }
    }
}

void
CSCLUIImpl::enable_highlight_ui(sclboolean enabled)
{
    if (m_initialized) {
        CSCLContext *context = CSCLContext::get_instance();
        CSCLWindows *windows = CSCLWindows::get_instance();
        if (context) {
            context->set_highlight_ui_enabled(enabled);

            sclwindow window = windows->get_nth_window_in_Z_order_list(SCL_WINDOW_Z_TOP);
            windows->update_window(window);
            if (!(windows->is_base_window(window))) {
                windows->update_window(windows->get_base_window());
            }
        }
    }
}

void
CSCLUIImpl::enable_highlight_ui_animation(sclboolean enabled)
{
    if (m_initialized) {
        CSCLContext *context = CSCLContext::get_instance();
        CSCLAnimator *animator = CSCLAnimator::get_instance();
        if (context && animator) {
            context->set_highlight_ui_animation_enabled(enabled);

            if (!enabled) {
                sclint id = animator->find_animator_by_type(ANIMATION_TYPE_HIGHLIGHT_UI);
                if (id != NOT_USED) {
                    animator->destroy_animator(id);
                }
            }
        }
    }
}

void
CSCLUIImpl::enable_touch_offset(sclboolean enabled)
{
    CSCLErrorAdjustment *adjustment = CSCLErrorAdjustment::get_instance();
    if (adjustment) {
        adjustment->enable_touch_offset(enabled);
    }
}

void
CSCLUIImpl::disable_input_events(sclboolean disabled)
{
    if (m_initialized) {
        CSCLController *controller = CSCLController::get_instance();
        if (controller) {
            controller->disable_input_events(disabled);
        }
    }
}

sclboolean
CSCLUIImpl::set_cur_sublayout(const sclchar *sub_layout_name)
{
    sclboolean ret = FALSE;

    if (m_initialized) {
        CSCLContext *context = CSCLContext::get_instance();
        CSCLWindows *windows = CSCLWindows::get_instance();
        if (context && windows) {
            ret = context->set_cur_sublayout(sub_layout_name);
            windows->update_window(windows->get_base_window());
        }
    }
    return ret;
}

const sclchar*
CSCLUIImpl::get_cur_sublayout()
{
    sclchar* ret = NULL;
    if (m_initialized) {
        CSCLContext *context = CSCLContext::get_instance();
        if (context) {
            ret = context->get_cur_sublayout();
        }
    }
    return ret;
}

void
CSCLUIImpl::set_custom_magnifier_label(scltouchdevice touch_id, sclint index, const sclchar* label)
{
    if (m_initialized) {
        CSCLContext *context = CSCLContext::get_instance();
        if (context) {
            context->set_custom_magnifier_label(touch_id, index, label);
        }
    }
}

void
CSCLUIImpl::set_string_substitution(const sclchar *original, const sclchar *substitute)
{
    if (m_initialized) {
        CSCLWindows *windows = CSCLWindows::get_instance();
        CSCLResourceCache *cache = CSCLResourceCache::get_instance();
        if (cache && windows) {
            cache->set_string_substitution(original, substitute);
            windows->update_window(windows->get_base_window());
        }
    }
}

void
CSCLUIImpl::unset_string_substitution(const sclchar *original)
{
    if (m_initialized) {
        CSCLWindows *windows = CSCLWindows::get_instance();
        CSCLResourceCache *cache = CSCLResourceCache::get_instance();
        if (cache && windows) {
            cache->unset_string_substitution(original);
            windows->update_window(windows->get_base_window());
        }
    }
}

void
CSCLUIImpl::set_autocapital_shift_state(sclboolean flag) {
    m_autocapital_shift_state = flag;
}

sclboolean
CSCLUIImpl::get_autocapital_shift_state() {
    return m_autocapital_shift_state;
}

sclboolean
CSCLUIImpl::process_key_event(const char *key)
{
    if (m_initialized) {
        CSCLEvents *events = CSCLEvents::get_instance();
        return events->process_key_event(key);
    }
    return FALSE;
}
