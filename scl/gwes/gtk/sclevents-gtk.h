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

#include "sclevents.h"
#include <gtk/gtk.h>
#include <map>


#ifndef __SCL_EVENTS_GTK_H__
#define __SCL_EVENTS_GTK_H__

namespace scl
{
class CSCLEventsImplGtk : public CSCLEventsImpl
{
public :
    CSCLEventsImplGtk();
    ~CSCLEventsImplGtk();

    /* Implementation about interface functions */
    void connect_window_events(const sclwindow wnd, const sclint evt);
    void create_timer(const scl16 id, const scl32 interval, scl16 value, sclboolean addToMap);
    void destroy_timer(const scl32 id);
    void destroy_all_timer();

private:
    std::map<int, int> idMap;
};
} /* End of scl namespace */
#endif __SCL_EVENTS_GTK_H__
