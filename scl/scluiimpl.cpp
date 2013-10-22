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

#include "scluiimpl.h"
#include "scldebug.h"
#include "scluibuilder.h"
#include "sclcontroller.h"
#include "sclresourcecache.h"
#include "sclerroradjustment.h"
#include "sclres_manager.h"
#include "scleventhandler.h"

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

    sclboolean ret = FALSE;

    if (m_initialized) {
        CSCLUtils *utils = CSCLUtils::get_instance();
        CSCLWindows *windows = CSCLWindows::get_instance();
        CSCLContext *context = CSCLContext::get_instance();
        CSCLController *controller = CSCLController::get_instance();

        if (windows && controller && context) {
            //if (auto_relocate) {
            if (TRUE) {
                /* Let's relocate our base window - bottomed center aligned */
                sclint width, height;
                //get_layout_size(&width, &height);
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

    sclboolean ret = FALSE;

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

    sclboolean ret = FALSE;

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

        scl8 mode = NOT_USED;

        SclResParserManager *sclres_manager = SclResParserManager::get_instance();
        if (sclres_manager) {
            mode = sclres_manager->get_inputmode_id(input_mode);
        }

        if (controller && windows && handler && mode != NOT_USED) {
            handler->set_input_mode(input_mode);
            ret = controller->process_input_mode_change(mode);
            windows->update_window(windows->get_base_window());
        }
    }

    SCL_DEBUG_ELAPASED_TIME_END();
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
                sclint width, height;
                //get_layout_size(&width, &height);
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
            ret = cache->set_private_key((sclchar*)custom_id, label, imagelabel, imagebg,
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
    sclboolean ret = FALSE;

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
            SCLEventReturnType ret = handler->on_event_notification(SCL_UINOTITYPE_SHIFT_STATE_CHANGE, state);
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
    sclboolean ret = FALSE;

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
    sclboolean ret = FALSE;

    if (m_initialized) {
        CSCLEvents *events = CSCLEvents::get_instance();
        CSCLWindows *windows = CSCLWindows::get_instance();

        if (events && windows) {
            events->destroy_timer(SCL_TIMER_POPUP_TIMEOUT);

            sclbyte index = 0;
            sclboolean timerset = FALSE;
            sclwindow window = SCLWINDOW_INVALID;
            SclWindowContext *winctx = NULL;
            do {
                window = windows->get_nth_window_in_Z_order_list(index);
                //winctx = windows->get_window_context(window, FALSE);
                winctx = windows->get_window_context(window);
                if (winctx) {
                    if (winctx->timeout != 0) {
                        events->create_timer(SCL_TIMER_POPUP_TIMEOUT, winctx->timeout, 0, TRUE);
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
    sclboolean ret = FALSE;

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
 * Returns the scl base window size
 */
SclRectangle
CSCLUIImpl::get_main_window_rect()
{
    SclRectangle ret = {0};

    if (m_initialized) {
        CSCLResourceCache *cache = CSCLResourceCache::get_instance();
        CSCLWindows *windows = CSCLWindows::get_instance();
        if (cache && windows) {
            //const SclLayout *layout  = cache->get_cur_layout(windows->get_base_window());
            SclWindowContext *winctx = windows->get_window_context(windows->get_base_window());
            if (winctx) {
                ret.x = winctx->geometry.x;
                ret.y = winctx->geometry.y;
                ret.width = winctx->geometry.width;
                ret.height = winctx->geometry.height;
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
    SclSize ret = {0};

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
    sclboolean ret = FALSE;

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
    sclboolean ret = FALSE;

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
    sclboolean ret = FALSE;

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
    sclboolean ret = FALSE;

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
    sclboolean ret = FALSE;

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
    sclboolean ret = FALSE;

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
    sclboolean ret = FALSE;

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
    sclboolean ret = FALSE;

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
    sclboolean ret = FALSE;

    if (m_initialized) {
        CSCLContext *context = CSCLContext::get_instance();
        if (context) {
            context->set_shift_multi_touch_enabled(enabled);
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
    sclboolean ret = FALSE;

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
