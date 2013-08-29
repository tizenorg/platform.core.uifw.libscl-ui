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
#ifdef  __WIN32__
#include "sclevents-win32.h"
#elif defined(__EFL__)
#include "sclevents-efl.h"
#else
#include "sclevents-gtk.h"
#endif
#include "scldebug.h"
#include "sclwindows.h"

using namespace scl;

CSCLEvents::CSCLEvents()
{
    SCL_DEBUG();
    m_impl = 0;
    m_touch_event_offset.x = m_touch_event_offset.y = 0;
}

CSCLEvents::~CSCLEvents()
{
    SCL_DEBUG();

    fini();

    if (m_impl) {
        delete m_impl;
        m_impl = NULL;
    }
}

void CSCLEvents::init()
{
    CSCLEventsImpl *impl = get_scl_events_impl();
    if (impl) {
        impl->init();
    }
}

void CSCLEvents::fini()
{
    CSCLEventsImpl *impl = get_scl_events_impl();
    if (impl) {
        impl->fini();
    }
}

CSCLEventsImpl* CSCLEvents::get_scl_events_impl()
{
    SCL_DEBUG();
    if (m_impl == 0) {
#ifdef  __WIN32__
        m_impl = new CSCLEventsImplWin32;
#elif defined(__EFL__)
        m_impl = new CSCLEventsImplEfl;
#else
        m_impl = new CSCLEventsImplGtk;
#endif
    }
    return m_impl;
}

CSCLEvents* CSCLEvents::get_instance()
{
    static CSCLEvents instance;
    return &instance;
}

void
CSCLEvents::connect_window_events( sclwindow wnd, const sclint evt )
{
    CSCLWindows *windows = CSCLWindows::get_instance();
    //SclWindowContext *winctx = windows->get_window_context(wnd, FALSE);
    SclWindowContext *winctx = windows->get_window_context(wnd);
    if (winctx) {
        if (!(winctx->is_virtual)) {
            get_scl_events_impl()->connect_window_events(wnd, evt);
        }
    }
}

void
CSCLEvents::set_touch_event_offset(const SclPoint pos)
{
    m_touch_event_offset = pos;
}

SclPoint*
CSCLEvents::get_touch_event_offset()
{
    return &m_touch_event_offset;
}


