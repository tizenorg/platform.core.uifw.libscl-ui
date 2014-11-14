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

CSCLGraphics::CSCLGraphics()
{
    SCL_DEBUG();
    m_impl = NULL;
}

CSCLGraphics::~CSCLGraphics()
{
    SCL_DEBUG();

    if (m_impl) {
        delete m_impl;
        m_impl = NULL;
    }
}

void CSCLGraphics::init()
{
    CSCLGraphicsImpl *impl = get_scl_graphics_impl();
    if (impl) {
        impl->init();
    }
}

void CSCLGraphics::fini()
{
    CSCLGraphicsImpl *impl = get_scl_graphics_impl();
    if (impl) {
        impl->fini();
    }
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
    static CSCLGraphics instance;
    return &instance;
}

