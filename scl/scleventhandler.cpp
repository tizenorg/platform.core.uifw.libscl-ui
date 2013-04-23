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

#include "sclresourcecache.h"
#include "scleventhandler.h"
#include "scldebug.h"
#include "sclkeydefines.h"
#include "scluiimpl.h"

using namespace scl;

CSCLEventHandler* CSCLEventHandler::m_instance = NULL; /* For singleton */

CSCLEventHandler::CSCLEventHandler()
{
    SCL_DEBUG();

    m_default_event_callback = NULL;
    m_cur_input_mode_event_callback = NULL;
}

CSCLEventHandler::~CSCLEventHandler()
{
    SCL_DEBUG();
}

CSCLEventHandler*
CSCLEventHandler::get_instance()
{
    if (!m_instance) {
        m_instance = new CSCLEventHandler();
    }
    return (CSCLEventHandler*)m_instance;
}

static void handle_shift_button_click_event(SclUIEventDesc ui_event_desc)
{
    CSCLUIImpl *uiimpl = CSCLUIImpl::get_instance();
    CSCLContext *context = CSCLContext::get_instance();

    if (uiimpl && context) {
        if (ui_event_desc.key_type == KEY_TYPE_CONTROL && ui_event_desc.key_event == MVK_Shift_L) {
            switch (uiimpl->get_shift_state()) {
                case SCL_SHIFT_STATE_OFF: {
                    uiimpl->set_shift_state(SCL_SHIFT_STATE_ON);
                }
                break;
                case SCL_SHIFT_STATE_ON: {
                    /* The default next state should be LOCK state */
                    SCLShiftState next_state = SCL_SHIFT_STATE_LOCK;
                    if (context->get_shift_multi_touch_enabled()) {
                        CSCLContext *context = CSCLContext::get_instance();
                        if (context) {
                            if (context->get_shift_multi_touch_state() == SCL_SHIFT_MULTITOUCH_ON_PRESSED) {
                                /* If the shift multi touch state is ON_PRESSED, don't leave ON state now */
                                next_state = SCL_SHIFT_STATE_ON;
                            } else if (context->get_shift_multi_touch_state() == SCL_SHIFT_MULTITOUCH_ON_KEY_ENTERED) {
                                /* If some keys were already entered while shift key was in pressed state, move to OFF */
                                next_state = SCL_SHIFT_STATE_OFF;
                            }
                        }
                    }
                    uiimpl->set_shift_state(next_state);
                }
                break;
                case SCL_SHIFT_STATE_LOCK: {
                    uiimpl->set_shift_state(SCL_SHIFT_STATE_OFF);
                }
                break;
            }
        }
    }
}

static void handle_shift_state_on_button_click_event(SclUIEventDesc ui_event_desc)
{
    CSCLUIImpl *uiimpl = CSCLUIImpl::get_instance();

    /* do not need the libscl-ui auto-captial the shift state  */
    if (FALSE == uiimpl->get_autocapital_shift_state()) {
        return;
    }

    CSCLContext *context = CSCLContext::get_instance();

    sclboolean turn_shift_off = TRUE;
    if (ui_event_desc.key_event == MVK_Shift_L || ui_event_desc.key_event == MVK_Caps_Lock) {
        turn_shift_off = FALSE;
    }
    if (ui_event_desc.key_type == KEY_TYPE_MODECHANGE) {
        turn_shift_off = FALSE;
    }
    /* If we are in ON_PRESSED or ON_KEY_ENTERED mode of shift multi touch state, do not turn it off now */
    if (context->get_shift_multi_touch_enabled() && turn_shift_off) {
        CSCLContext *context = CSCLContext::get_instance();
        if (context) {
            if (context->get_shift_multi_touch_state() == SCL_SHIFT_MULTITOUCH_ON_PRESSED) {
                context->set_shift_multi_touch_state(SCL_SHIFT_MULTITOUCH_ON_KEY_ENTERED);
                turn_shift_off = FALSE;
            } else if (context->get_shift_multi_touch_state() == SCL_SHIFT_MULTITOUCH_ON_KEY_ENTERED) {
                turn_shift_off = FALSE;
            }
        }
    }
    if (turn_shift_off) {
        if (uiimpl->get_shift_state() == SCL_SHIFT_STATE_ON) {
            uiimpl->set_shift_state(SCL_SHIFT_STATE_OFF);
        }
    }
}

static void handle_mode_change_button_click_event(SclUIEventDesc ui_event_desc)
{
    CSCLUIImpl *uiimpl = CSCLUIImpl::get_instance();

    if (uiimpl) {
        if (ui_event_desc.key_type == KEY_TYPE_MODECHANGE) {
            uiimpl->set_input_mode(ui_event_desc.key_value);
        }
    }
}

SCLEventReturnType
CSCLEventHandler::on_event_key_clicked(SclUIEventDesc ui_event_desc)
{
    SCLEventReturnType ret = SCL_EVENT_PASS_ON;

    pre_process_ui_event(ui_event_desc);

    if (m_cur_input_mode_event_callback) {
        ret = m_cur_input_mode_event_callback->on_event_key_clicked(ui_event_desc);
    }
    if (ret == SCL_EVENT_PASS_ON) {
        if (m_default_event_callback) {
            ret = m_default_event_callback->on_event_key_clicked(ui_event_desc);
        }
    }

    if (ret == SCL_EVENT_PASS_ON) {
        /* Here we can put the fallback processing of this UIEvent */

        /* General requirement - 1 */
        /* When the SHIFT button was clicked, we change the shift state to OFF -> ON -> LOCK -> OFF ... */
        handle_shift_button_click_event(ui_event_desc);

        /* General requirement - 2 */
        /* If a key was clicked but it is neither a SHIFT nor a CAPSLOCK, we just turn the shift off, if it is on */
        handle_shift_state_on_button_click_event(ui_event_desc);

        /* General requirement - 3 */
        /* If the key type is KEY_TYPE_MODECHANGE, change the current input mode to given key_value */
        handle_mode_change_button_click_event(ui_event_desc);
    }

    return ret;
}

SCLEventReturnType
CSCLEventHandler::on_event_drag_state_changed(SclUIEventDesc ui_event_desc)
{
    SCLEventReturnType ret = SCL_EVENT_PASS_ON;

    pre_process_ui_event(ui_event_desc);

    if (m_cur_input_mode_event_callback) {
        ret = m_cur_input_mode_event_callback->on_event_drag_state_changed(ui_event_desc);
    }
    if (ret == SCL_EVENT_PASS_ON) {
        if (m_default_event_callback) {
            ret = m_default_event_callback->on_event_drag_state_changed(ui_event_desc);
        }
    }

    if (ret == SCL_EVENT_PASS_ON) {
        /* Here we can put the fallback processing of this UIEvent */
        CSCLUIImpl *uiimpl = CSCLUIImpl::get_instance();
        CSCLContext *context = CSCLContext::get_instance();

        /* General requirement - 1 */
        /* When the SHIFT button was 'pressed' and shift button multitouch action is enabled,
           we change the current shift multitouch state to 'ON_PRESSED' */
        if (uiimpl && context) {
            if (context->get_shift_multi_touch_enabled()) {
                if (ui_event_desc.event_type == EVENT_TYPE_PRESS) {
                    if (context) {
                        if (ui_event_desc.key_event == MVK_Shift_L) {
                            if (context->get_shift_multi_touch_state() == SCL_SHIFT_MULTITOUCH_OFF) {
                                uiimpl->set_shift_state(SCL_SHIFT_STATE_ON);
                                context->set_shift_multi_touch_state(SCL_SHIFT_MULTITOUCH_ON_PRESSED);
                            }
                        }
                    }
                }
            }
        }
    }

    return ret;
}

SCLEventReturnType
CSCLEventHandler::on_event_notification(SCLUINotiType noti_type, sclint etc_info)
{
    SCLEventReturnType ret = SCL_EVENT_PASS_ON;

    if (m_cur_input_mode_event_callback) {
        ret = m_cur_input_mode_event_callback->on_event_notification(noti_type, etc_info);
    }
    if (ret == SCL_EVENT_PASS_ON) {
        if (m_default_event_callback) {
            ret = m_default_event_callback->on_event_notification(noti_type, etc_info);
        }
    }

    if (ret == SCL_EVENT_PASS_ON) {
        /* Here we can put the fallback processing of this UIEvent */
    }

    return ret;
}

sclboolean
CSCLEventHandler::set_input_mode(const sclchar *input_mode)
{
    SCL_DEBUG();

    sclboolean ret = FALSE;
    m_cur_input_mode_event_callback = NULL;

    if (input_mode) {
        std::string id = input_mode;
        std::map<std::string, ISCLUIEventCallback*>::iterator iter = m_input_mode_event_callbacks.find(input_mode);
        if (iter != m_input_mode_event_callbacks.end()) {
            m_cur_input_mode_event_callback = (iter->second);
        }
    }

    if (m_cur_input_mode_event_callback) {
        ret = TRUE;
    }

    return ret;
}

void
CSCLEventHandler::set_event_callback(ISCLUIEventCallback *callback, const sclchar *input_mode)
{
    SCL_DEBUG();

    scl_assert_return(callback);

    if (input_mode) {
        std::string id = input_mode;
        m_input_mode_event_callbacks[id] = callback;
    } else {
        m_default_event_callback = callback;
    }
}

void
CSCLEventHandler::pre_process_ui_event(SclUIEventDesc &ui_event_desc)
{
    SCL_DEBUG();

    typedef struct {
        const sclchar *key_value;
        sclulong key_event;
    } KEY_VALUE_EVENT_CONVERT_TABLE;

    KEY_VALUE_EVENT_CONVERT_TABLE control_keys[] = {
        {"Space",       MVK_space       },
        {"BackSpace",   MVK_BackSpace   },
        {"Shift",       MVK_Shift_L     },
        {"CapsLock",    MVK_Caps_Lock   },
        {"Enter",       MVK_Return      },
        {"Left",        MVK_Left        },
        {"Right",       MVK_Right       },
        {"Up",          MVK_Up          },
        {"Down",        MVK_Down        },
    };

    /* Translate key_values only when key_event is 0 and key_value is not NULL */
    if (ui_event_desc.key_value && ui_event_desc.key_event == 0) {
        if (ui_event_desc.key_type == KEY_TYPE_CHAR) {
            /* If the key_value is a string with length 1, and the first character has value between
               SCL_ISCHAR range, provide the corresponding ASCII code in key_event field */
            if (ui_event_desc.key_value[0] != '\0' && ui_event_desc.key_value[1] == '\0') {
                if (SCL_ISCHAR(ui_event_desc.key_value[0])) {
                    ui_event_desc.key_event = ui_event_desc.key_value[0];
                }
            }
        } else if (ui_event_desc.key_type == KEY_TYPE_CONTROL) {
            const scluint control_keys_size = sizeof(control_keys) / sizeof(KEY_VALUE_EVENT_CONVERT_TABLE);

            for (scluint loop = 0;loop < control_keys_size;loop++) {
                if (strncmp(control_keys[loop].key_value, ui_event_desc.key_value, strlen(control_keys[loop].key_value)) == 0) {
                    ui_event_desc.key_event = control_keys[loop].key_event;
                }
            }
        } else if (ui_event_desc.key_type == KEY_TYPE_STRING) {
            CSCLResourceCache *cache = CSCLResourceCache::get_instance();
            if (cache) {
                ui_event_desc.key_value = cache->find_substituted_string(ui_event_desc.key_value);
            }
        }
    }
}
