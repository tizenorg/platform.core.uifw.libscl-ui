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

#include "sclconfig.h"

#ifndef __SCL_ACTIONSTATE_H__
#define __SCL_ACTIONSTATE_H__

//SCL_BEGIN_DECLS

namespace scl
{

#define MAX_ACTION_BUF 50

/**
 * @brief The base class to work as a soft-based keyboard
 *
 * This class implements all functions for working as a soft-based keyboard
 * In side of ISE developer, they can modify it by their requirements.
 */
class CSCLActionState
{
private:
    CSCLActionState();
public:
    ~CSCLActionState();

    /* for singleton */
    static CSCLActionState* get_instance();

    /* reset the action state to init status */
    void reset();
    void clear_action_buf();
    SCLActionState get_cur_action_state() {
        return m_cur_action_state;
    }
    void set_cur_action_state(SCLActionState state) {
        m_cur_action_state = state;
    }

public:
    /* stores the state information */
    SCLActionState m_action_buffer[MAX_ACTION_BUF];
    SCLActionState m_cur_action_state;
};

}

//SCL_END_DECLS

#endif //__SCL_ACTIONSTATE_H__
