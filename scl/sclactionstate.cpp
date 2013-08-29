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

#include <memory.h>
#include "sclactionstate.h"
#include "scldebug.h"

using namespace scl;

CSCLActionState::CSCLActionState()
{
    SCL_DEBUG();

    reset();
}

CSCLActionState::~CSCLActionState()
{
    SCL_DEBUG();
}

CSCLActionState*
CSCLActionState::get_instance()
{
    static CSCLActionState instance;
    return &instance;
}


void
CSCLActionState::reset()
{
    m_cur_action_state = ACTION_STATE_BASE_INIT;
    memset(m_action_buffer, 0x00, sizeof(m_action_buffer));
}

void
CSCLActionState::clear_action_buf()
{

}
