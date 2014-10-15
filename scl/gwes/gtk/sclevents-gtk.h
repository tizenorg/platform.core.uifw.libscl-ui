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
    void connect_window_events(const sclwindow window, const sclint event);
    void create_timer(const scl16 id, const scl32 interval, scl16 value, sclboolean add_to_map);
    void destroy_timer(const scl32 id);
    void destroy_all_timer();

private:
    std::map<int, int> id_map;
};
} /* End of scl namespace */
#endif __SCL_EVENTS_GTK_H__
