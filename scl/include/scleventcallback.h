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

#ifndef __SCL_EVENTCALLBACK_H__
#define __SCL_EVENTCALLBACK_H__

//SCL_BEGIN_DECLS
/* FIXME : Need to check the dependancy cause by the next include statement */

#include "scltypes.h"
#include "sclconfig.h"
#include "sclstructs.h"

namespace scl
{

typedef struct {
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
} SclUIEventDesc;

/**
* @brief The callback interface to handle SCL events
*
* ISE developers can inherit this interface and register it to the SCLCore class
* to handle SCL events as they want.
*/
struct ISCLUIEventCallback {
    virtual SCLEventReturnType on_event_key_clicked(SclUIEventDesc ui_event_desc) { return SCL_EVENT_PASS_ON; }
    virtual SCLEventReturnType on_event_drag_state_changed(SclUIEventDesc ui_event_desc) { return SCL_EVENT_PASS_ON; }
    virtual SCLEventReturnType on_event_notification(SCLUINotiType noti_type, sclint etc_info) { return SCL_EVENT_PASS_ON; }
};

}

//SCL_END_DECLS

#endif //__SCL_EVENTCALLBACK_H__
