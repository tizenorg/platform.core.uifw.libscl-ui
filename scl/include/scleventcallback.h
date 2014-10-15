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

#ifndef __SCL_EVENTCALLBACK_H__
#define __SCL_EVENTCALLBACK_H__

//SCL_BEGIN_DECLS
/* FIXME : Need to check the dependancy cause by the next include statement */

#include "scltypes.h"
#include "sclconfig.h"
#include "sclstructs.h"

namespace scl
{

struct SclUIEventDesc {
    SclUIEventDesc() {
        key_value = NULL;
        key_event = 0;
        key_type = KEY_TYPE_NONE;
        key_modifier = KEY_MODIFIER_NONE;
        touch_id = 0;
        touch_event_order = 0;
        event_type = EVENT_TYPE_NONE;
    }
    const sclchar *key_value;
    sclulong key_event;
    SCLKeyType key_type;
    SCLKeyModifier key_modifier;

    /* Extra information for advanced event processing */
    scltouchdevice touch_id;
    sclint touch_event_order;

    SclPoint mouse_pressed_point;
    SclPoint mouse_current_point;
    SclPoint mouse_farthest_point;

    SCLEventType event_type;
};

struct SclNotiDesc {
    SclUIEventDesc *ui_event_desc;
};

struct SclNotiPopupOpeningDesc : SclNotiDesc {
    const char *input_mode;
}; // SCL_UINOTITYPE_POPUP_OPENING

struct SclNotiPopupOpenedDesc : SclNotiDesc {
    sclwindow window;
    const char *input_mode;
}; // SCL_UINOTITYPE_POPUP_OPENED

struct SclNotiPopupClosingDesc : SclNotiDesc {
    sclwindow window;
    const char *input_mode;
    sclboolean timed_out;
}; // SCL_UINOTITYPE_POPUP_CLOSING

struct SclNotiPopupClosedDesc : SclNotiDesc {
    sclwindow window;
    const char *input_mode;
    sclboolean timed_out;
}; // SCL_UINOTITYPE_POPUP_CLOSED

struct SclNotiGestureFlickDesc : SclNotiDesc {
    SCLDragType drag_type;
}; // SCL_UINOTITYPE_GESTURE_FLICK

struct SclNotiShiftStateChangeDesc : SclNotiDesc {
    SCLShiftState shift_state;
}; // SCL_UINOTITYPE_SHIFT_STATE_CHANGE

struct SclNotiInputModeChangeDesc : SclNotiDesc {
    const char *input_mode;
}; // SCL_UINOTITYPE_INPUT_MODE_CHANGE

struct SclNotiHighlighNavigateDesc : SclNotiDesc {
    SCLHighlightNavigationDirection direction;
    sclwindow window_from;
    scl8 key_from;
    sclwindow window_to;
    scl8 key_to;
}; // SCL_UINOTITYPE_HIGHLIGHT_NAVIGATE

/**
* @brief The callback interface to handle SCL events
*
* ISE developers can inherit this interface and register it to the SCLCore class
* to handle SCL events as they want.
*/
struct ISCLUIEventCallback {
    virtual SCLEventReturnType on_event_key_clicked(SclUIEventDesc ui_event_desc) { return SCL_EVENT_PASS_ON; }
    virtual SCLEventReturnType on_event_drag_state_changed(SclUIEventDesc ui_event_desc) { return SCL_EVENT_PASS_ON; }
    virtual SCLEventReturnType on_event_notification(SCLUINotiType noti_type, SclNotiDesc *etc_info) { return SCL_EVENT_PASS_ON; }
};

}

//SCL_END_DECLS

#endif //__SCL_EVENTCALLBACK_H__
