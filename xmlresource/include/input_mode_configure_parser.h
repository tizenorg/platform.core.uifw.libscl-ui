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

#ifndef __INPUT_MODE_PARSER__H__
#define __INPUT_MODE_PARSER__H__
#include "sclres_type.h"

#ifndef LIBSCL_EXPORT_API
#define LIBSCL_EXPORT_API 
#endif // LIBSCL_EXPORT_API


class InputModeConfigureParserImpl;

class LIBSCL_EXPORT_API InputModeConfigParser {
    InputModeConfigureParserImpl *m_impl;
    public:
        int init(const char* file);
        PSclInputModeConfigure get_input_mode_configure_table();
        int get_inputmode_id(const char *name);
        const char* get_inputmode_name(int id);
        int get_inputmode_size();
    public:
        ~InputModeConfigParser();
        static InputModeConfigParser *get_instance();
    private:
        InputModeConfigParser();
};
#endif
