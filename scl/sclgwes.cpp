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
#include "scldebug.h"

using namespace scl;

CSCLGwes::CSCLGwes()
{
    SCL_DEBUG();

    m_windows = NULL;
    m_graphics = NULL;
    m_events = NULL;
}

CSCLGwes::~CSCLGwes()
{
    SCL_DEBUG();
}

void CSCLGwes::init(sclwindow parent, scl16 width, scl16 height)
{
    SCL_DEBUG();

    if (m_windows == NULL) m_windows = CSCLWindows::get_instance();
    if (m_graphics == NULL) m_graphics = CSCLGraphics::get_instance();
    if (m_events == NULL) m_events = CSCLEvents::get_instance();

    if (m_windows) {
        m_windows->init();
    }
    if (m_graphics) {
        m_graphics->init();
    }
    if (m_events) {
        m_events->init();
    }

    sclwindow wnd = m_windows->create_base_window(parent, width, height);
    m_events->connect_window_events(wnd, SCL_EVENT_MOUSE | SCL_EVENT_EXPOSE);
}

void CSCLGwes::fini()
{
    SCL_DEBUG();

    if (m_windows) {
        m_windows->fini();
        m_windows = NULL;
    }
    if (m_graphics) {
        m_graphics->fini();
        m_graphics = NULL;
    }
    if (m_events) {
        m_events->fini();
        m_events = NULL;
    }
}

CSCLGwes* CSCLGwes::get_instance()
{
    static CSCLGwes instance;
    return &instance;
}

