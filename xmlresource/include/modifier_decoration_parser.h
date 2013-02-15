/*
 * Copyright 2012-2013 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Flora License, Version 1.0 (the "License");
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

#ifndef __Modifier_decoration_Parser__H__
#define __Modifier_decoration_Parser__H__
#include <libxml/parser.h>
#include "sclres_type.h"

class Modifier_decoration_Parser {
    public:
        ~Modifier_decoration_Parser();
        static Modifier_decoration_Parser *get_instance();
        void init();
        PSclModifierDecoration get_modifier_decoration_table();
        int get_modifier_decoration_id(const char *name);
    private:
        Modifier_decoration_Parser();
        void parsing_modifier_decoration_table();
        void parsing_modifier_decoration_record(const xmlNodePtr, const PSclModifierDecoration);
        void set_modifier_decoration_default_record(const PSclModifierDecoration);
        void parsing_background_image_record_node(const xmlNodePtr cur_node, const PSclModifierDecoration cur_rec);

    private:
        static Modifier_decoration_Parser *m_instance;
        SclModifierDecoration m_modifier_decoration_table[MAX_SCL_MODIFIER_DECORATION_NUM];
    private:
        class DestructHelper {
            public:
                ~DestructHelper() {
                    if (Modifier_decoration_Parser::m_instance != NULL)
                        delete m_instance;
                }
        };
        static DestructHelper des;
};


#endif
