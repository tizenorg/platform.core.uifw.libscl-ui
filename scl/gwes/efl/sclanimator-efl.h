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

#include "sclanimator.h"

#ifndef __SCL_ANIMATOR_EFL_H__
#define __SCL_ANIMATOR_EFL_H__

#include <Evas.h>

namespace scl
{
class CSCLAnimatorImplEfl : public CSCLAnimatorImpl
{
public :
    CSCLAnimatorImplEfl();
    ~CSCLAnimatorImplEfl();

    void init();
    void fini();

    sclboolean check_animation_supported();
    void animator_timer(SclAnimationState *state);

private:
    Evas_Object *m_highlight_ui_object;
    Evas_Object *m_highlight_ui_object_alternate;
};
} /* End of scl namespace */
#endif //__SCL_ANIMATOR_EFL_H__
