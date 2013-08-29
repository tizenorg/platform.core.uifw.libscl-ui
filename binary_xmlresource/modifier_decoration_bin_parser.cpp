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

#include "modifier_decoration_bin_parser.h"
#include <memory.h>
#include "put_record.h"

BinModifierDecorationParser::BinModifierDecorationParser() {
    memset(m_modifier_decoration_table, 0x00, sizeof(SclModifierDecoration) * MAX_SCL_MODIFIER_DECORATION_NUM);
}

BinModifierDecorationParser::~BinModifierDecorationParser() {
}

BinModifierDecorationParser*
BinModifierDecorationParser::get_instance() {
    static BinModifierDecorationParser instance;
    return &instance;
}
void
BinModifierDecorationParser::init(const FileStorage& storage, int offset, int size, IParserInfo_Provider* parser_info_provider) {
    m_storage.set_str_provider(parser_info_provider);
    m_storage.get_storage(storage, offset, size);
    this->parser_info_provider = parser_info_provider;

    parsing_modifier_decoration_table();
}

/* recompute_layout will change the table */
PSclModifierDecoration BinModifierDecorationParser::get_modifier_decoration_table() {
    return m_modifier_decoration_table;
}

void
BinModifierDecorationParser::parsing_modifier_decoration_table() {
    PSclModifierDecoration cur = m_modifier_decoration_table;

    Modifier_decoration_width record_width;
    set_modifier_decoration_width(*parser_info_provider, record_width);

    // skip data_size
    m_storage.advance(8);

    int size = m_storage.get<sint_t>(4);
    for (int i = 0; i < size; ++i) {
        cur->valid = (sclboolean)true;
        m_storage.get<sint_t>(record_width.extract_background);
        m_storage.get_str(&(cur->name), record_width.name, m_string_collector);
        for (int m = 0; m < DISPLAYMODE_MAX; ++m) {
            for (int n = 0; n < KEY_MODIFIER_MAX; ++n) {
                m_storage.get_str(&(cur->bg_image_path[m][n]), record_width.bg_image_path, m_string_collector);
            }
        }

        cur++;
    }
#ifdef __SCL_TXT_DEBUG
    put_modifier_decoration(DECODE, m_modifier_decoration_table);
#endif
}

int
BinModifierDecorationParser::get_modifier_decoration_id( const char *name )
{
    if (name == NULL) return -1;

    for(int i = 0; i < MAX_SCL_MODIFIER_DECORATION_NUM; ++i) {
        if ( m_modifier_decoration_table[i].name) {
            if ( 0 == strcmp(m_modifier_decoration_table[i].name, name) ) {
                return i;
            }
        }
    }

    return -1;
}

