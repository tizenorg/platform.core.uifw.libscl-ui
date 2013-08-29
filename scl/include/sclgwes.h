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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scltypes.h"
#include "sclconfig.h"
#include "sclwindows.h"
#include "sclgraphics.h"
#include "sclevents.h"

#ifndef __SCL_GWES_H__
#define __SCL_GWES_H__

#ifdef __cplusplus
//SCL_BEGIN_DECLS
#endif

namespace scl
{
/**
 * @brief The base class to work as a soft-based keyboard
 *
 * This class implements all functions for working as a soft-based keyboard
 * In side of ISE developer, they can modify it by their requirements.
 */
class CSCLGwes
{
private:
    CSCLGwes();
public :
    ~CSCLGwes();

    static CSCLGwes* get_instance();

private:

public:
    void init(sclwindow parent, scl16 width, scl16 height);
    void fini();

    CSCLGraphics *m_graphics;
    CSCLWindows *m_windows;
    CSCLEvents *m_events;
};

} /* End of scl namespace */

#ifdef __cplusplus
//SCL_END_DECLS
#endif

#endif //__SCL_GWES_H__
