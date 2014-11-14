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

#ifndef __ModifierDecorationParser__H__
#define __ModifierDecorationParser__H__
#include "sclres_type.h"

class ModifierDecorationParserImpl;

class ModifierDecorationParser {
    ModifierDecorationParserImpl *m_impl;
    public:
        int init(const char* file);
        PSclModifierDecoration get_modifier_decoration_table();
        int get_modifier_decoration_id(const char *name);
    public:
        ~ModifierDecorationParser();
        static ModifierDecorationParser *get_instance();
    private:
        ModifierDecorationParser();
};


#endif
