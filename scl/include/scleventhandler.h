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

#include "sclgwes.h"
#include "sclutils.h"
#include "sclcontext.h"
#include "scleventcallback.h"

#ifndef __SCL_EVENT_HANDLER_H__
#define __SCL_EVENT_HANDLER_H__

#ifndef LIBSCL_EXPORT_API
#define LIBSCL_EXPORT_API 
#endif // LIBSCL_EXPORT_API


//SCL_BEGIN_DECLS

namespace scl
{
/**
 * @brief The default event handler class
 *
 */
class LIBSCL_EXPORT_API CSCLEventHandler : public ISCLUIEventCallback
{
private:
    CSCLEventHandler();
public:
    ~CSCLEventHandler();

    static CSCLEventHandler* get_instance();

    sclboolean set_input_mode(const sclchar *input_mode);
    void set_event_callback(ISCLUIEventCallback *callback, const sclchar *input_mode);

    SCLEventReturnType on_event_key_clicked(SclUIEventDesc ui_event_desc);
    SCLEventReturnType on_event_drag_state_changed(SclUIEventDesc ui_event_desc);
    SCLEventReturnType on_event_notification(SCLUINotiType noti_type, SclNotiDesc *etc_info);

    void pre_process_ui_event(SclUIEventDesc &ui_event_desc);

protected:
    ISCLUIEventCallback* m_default_event_callback;
    ISCLUIEventCallback* m_cur_input_mode_event_callback;

    std::map<std::string, ISCLUIEventCallback*> m_input_mode_event_callbacks;
};

}

//SCL_END_DECLS

#endif //__SCL_EVENT_HANDLER_H__
