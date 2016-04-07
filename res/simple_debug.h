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

#ifndef __SIMPLE_DEBUG_H__
#define __SIMPLE_DEBUG_H__
#include <stdio.h>

#ifndef LIBSCL_EXPORT_API
#define LIBSCL_EXPORT_API 
#endif // LIBSCL_EXPORT_API


class LIBSCL_EXPORT_API SclLog {
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


LIBSCL_EXPORT_API void SCLLOG(enum SclLog::LOG_LEVEL level, char* fmt, ...);

LIBSCL_EXPORT_API void SCLLOG_TIME_BEGIN();
LIBSCL_EXPORT_API void SCLLOG_TIME_END(char *msg);
#endif
