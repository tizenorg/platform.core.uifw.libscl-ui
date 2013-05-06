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

#define __GTK__ 1

#include "sclgraphics.h"
#ifdef  __WIN32__
#include "sclgraphics-win32.h"
#elif defined(__EFL__)
#include "sclgraphics-efl.h"
#elif __GTK__
#include "sclgraphics-gtk.h"
#else
#include "sclgraphics-cairo.h"
#endif
#include "scldebug.h"

using namespace scl;

CSCLGraphics* CSCLGraphics::m_instance = NULL; /* For singleton */

CSCLGraphics::CSCLGraphics()
{
    SCL_DEBUG();
    m_impl = 0;
}

CSCLGraphics::~CSCLGraphics()
{
    SCL_DEBUG();
}

CSCLGraphicsImpl* CSCLGraphics::get_scl_graphics_impl()
{
    SCL_DEBUG();
    if (m_impl == 0) {
#ifdef  __WIN32__
        m_impl = new CSCLGraphicsImplWin32;
#elif defined(__EFL__)
        m_impl = new CSCLGraphicsImplEfl;
#elif __GTK__
        m_impl = new CSCLGraphicsImplGtk;
#else
        m_impl = new CSCLGraphicsImplCairo;
#endif
    }
    return m_impl;
}

CSCLGraphics* CSCLGraphics::get_instance()
{
    if (!m_instance) {
        m_instance = new CSCLGraphics();
    }
    return (CSCLGraphics*)m_instance;
}

