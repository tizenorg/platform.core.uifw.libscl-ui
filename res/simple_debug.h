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

#ifndef __SIMPLE_DEBUG_H__
#define __SIMPLE_DEBUG_H__
#include <stdio.h>

class SclLog {
    public:
        enum LOG_LEVEL {
            MESSAGE = 0,
            WARNING,
            DEBUG,
            ERROR,
            MAX_LOG_LEVEL
        };

        static SclLog* get_instance();
        void log(enum LOG_LEVEL, char* fmt, ...);
        ~SclLog();
    private:
        SclLog();

        FILE *m_flog;
};


void SCLLOG(enum SclLog::LOG_LEVEL level, char* fmt, ...);

void SCLLOG_TIME_BEGIN();
void SCLLOG_TIME_END(char *msg);
#endif
