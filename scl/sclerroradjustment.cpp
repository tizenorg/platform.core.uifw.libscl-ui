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

#include <stdio.h>
#include <stdlib.h>

#include "sclerroradjustment.h"
#include "scldebug.h"
#include "sclresourcecache.h"
#include "sclutils.h"

#define OFFSET_MAX 			10
#define TWO 				2

using namespace scl;

CSCLErrorAdjustment* CSCLErrorAdjustment::get_instance()
{
    static CSCLErrorAdjustment instance;
    return &instance;
}

CSCLErrorAdjustment::CSCLErrorAdjustment()
{
    SCL_DEBUG();

    m_enabled = TRUE;
}

CSCLErrorAdjustment::~CSCLErrorAdjustment()
{
    SCL_DEBUG();
}

void CSCLErrorAdjustment::enable_touch_offset(sclboolean enabled)
{
    CSCLUtils *utils = CSCLUtils::get_instance();
    if (utils) {
        utils->log("TOUCH_OFFSET is %s!!!!\n", (enabled ? "enabled" : "disabled"));
    }
    m_enabled = enabled;
}

sclboolean
CSCLErrorAdjustment::apply_touch_offset(SCLTouchOffsetLevel level, sclint *x, sclint *y)
{
    SCL_DEBUG();
    CSCLEvents* events = CSCLEvents::get_instance();
    CSCLUtils *utils = CSCLUtils::get_instance();
    SclPoint *pos;
    pos = events->get_touch_event_offset();

    if (m_enabled) {
        if (level == TOUCH_OFFSET_LEVEL_1 ) {
            *x = *x + utils->get_scaled_x(pos->x);
        } else if (level == TOUCH_OFFSET_LEVEL_2) {
            *x = *x + utils->get_scaled_x(pos->x);
            ///* CODE for landscape	 CODE for landscape*/
            //if (*x < utils->get_scale_x(scl_layout[LYT_LANDSCAPE_QTY_DEFAULT].width/TWO)) {
            //    *x = *x + utils->get_scale_x(OFFSET_MAX +((*x - utils->get_scale_x(scl_layout[LYT_LANDSCAPE_QTY_DEFAULT].width/TWO)) /(utils->get_scale_x(scl_layout[LYT_LANDSCAPE_QTY_DEFAULT].width/TWO)/OFFSET_MAX)));
            //} else {
            //    *x = *x - utils->get_scale_x(OFFSET_MAX -((*x - utils->get_scale_x(scl_layout[LYT_LANDSCAPE_QTY_DEFAULT].width/TWO)) /(utils->get_scale_x(scl_layout[LYT_LANDSCAPE_QTY_DEFAULT].width/TWO)/OFFSET_MAX)));
            //}
        }
        *y = *y + utils->get_scaled_y(pos->y);
    }

    return TRUE;
}


