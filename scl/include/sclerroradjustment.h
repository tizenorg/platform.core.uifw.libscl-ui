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

#include "scltypes.h"
#include "sclstructs.h"

#ifndef __SCL_ERRORADJUSTMENT_H__
#define __SCL_ERRORADJUSTMENT_H__

//SCL_BEGIN_DECLS

#define DEFAULT_VIBE_DURATION 100

namespace scl
{
/**
 * @brief The base class to work as a soft-based keyboard
 *
 * This class implements all functions for working as a soft-based keyboard
 * In side of ISE developer, they can modify it by their requirements.
 */
class CSCLErrorAdjustment
{
public:
    CSCLErrorAdjustment();
    CSCLErrorAdjustment(sclwindow parent);

    ~CSCLErrorAdjustment();
    static CSCLErrorAdjustment* get_instance();

private:

public:
    void enable_touch_offset(sclboolean enable);
    sclboolean apply_touch_offset(SCLTouchOffsetLevel level, sclint *x, sclint *y);

private:
    sclboolean m_enabled;
};

}

//SCL_END_DECLS

#endif //__SCL_ERRORADJUSTMENT_H__
