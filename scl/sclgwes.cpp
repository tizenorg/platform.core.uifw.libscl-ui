/*
 * Copyright 2012-2013 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Flora License, Version 1.0 (the "License");
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

#include "sclgwes.h"
#include "scldebug.h"

using namespace scl;

CSCLGwes* CSCLGwes::m_instance = NULL; /* For singleton */

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
    if (m_windows) delete(m_windows);
    if (m_graphics) delete(m_graphics);
    if (m_events) delete(m_events);
}

void CSCLGwes::init(sclwindow parent, scl16 width, scl16 height)
{
    SCL_DEBUG();
    if (m_windows == NULL) m_windows = CSCLWindows::get_instance();
    if (m_graphics == NULL) m_graphics = CSCLGraphics::get_instance();
    if (m_events == NULL) m_events = CSCLEvents::get_instance();

    sclwindow wnd = m_windows->create_base_window(parent, width, height);
    m_events->connect_window_events(wnd, SCL_EVENT_MOUSE | SCL_EVENT_EXPOSE);
}

CSCLGwes* CSCLGwes::get_instance()
{
    if (!m_instance) {
        m_instance = new CSCLGwes();
    }
    return (CSCLGwes*)m_instance;
}

