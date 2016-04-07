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

#include "sclevents.h"
#include <Ecore_Evas.h>
#include <Ecore.h>
#include <map>


#ifndef __SCL_EVENTS_EFL_H__
#define __SCL_EVENTS_EFL_H__

#ifndef LIBSCL_EXPORT_API
#define LIBSCL_EXPORT_API 
#endif // LIBSCL_EXPORT_API


namespace scl
{
class LIBSCL_EXPORT_API CSCLEventsImplEfl : public CSCLEventsImpl
{
public :
    CSCLEventsImplEfl();
    ~CSCLEventsImplEfl();

    void init();
    void fini();

    /* Implementation about interface functions */
    void connect_window_events(const sclwindow wnd, const sclint evt);
    void create_timer(const scl16 id, const scl32 interval, scl16 value, sclboolean addToMap);
    void destroy_timer(const scl32 id);
    void destroy_all_timer();

    void generate_mouse_event(SCLMouseEvent type, scl16 x, scl16 y);

private:
    std::map<int, Ecore_Timer*> idMap;

    Ecore_Event_Handler *m_mouse_down_handler;
    Ecore_Event_Handler *m_mouse_move_handler;
    Ecore_Event_Handler *m_mouse_up_handler;
    Ecore_Event_Handler *m_xclient_msg_handler;
    Ecore_Event_Handler *m_key_pressed_handler;
};
} /* End of scl namespace */
#endif
