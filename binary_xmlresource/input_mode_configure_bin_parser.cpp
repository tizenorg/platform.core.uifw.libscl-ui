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

#include "input_mode_configure_bin_parser.h"
#include "simple_debug.h"
#include "put_record.h"
using namespace std;

BinInputModeConfigParser::BinInputModeConfigParser() {
    m_inputmode_size = 0;
    memset(m_input_mode_configure_table, 0x00, sizeof(SclInputModeConfigure) * MAX_SCL_INPUT_MODE);
}

BinInputModeConfigParser::~BinInputModeConfigParser() {
}

BinInputModeConfigParser*
BinInputModeConfigParser::get_instance() {
    static BinInputModeConfigParser instance;
    return &instance;
}

void
BinInputModeConfigParser::init(const FileStorage& storage, int offset, int size, IParserInfo_Provider* provider) {
    m_storage.set_str_provider(parser_info_provider);
    m_storage.get_storage(storage, offset, size);
    this->parser_info_provider = provider;
    parsing_input_mode_configure_table();
}

int
BinInputModeConfigParser::get_inputmode_id(const char *name) {
    if (name == NULL) return -1;

    for(int i = 0; i < MAX_SCL_INPUT_MODE; ++i) {
        if ( m_input_mode_configure_table[i].name) {
            if ( 0 == strcmp(m_input_mode_configure_table[i].name, name) ) {
                return i;
            }
        }
    }

    return -1;
}

const char*
BinInputModeConfigParser::get_inputmode_name(int id) {
    if (id >= 0 && id < MAX_SCL_INPUT_MODE) {
        return m_input_mode_configure_table[id].name;
    }

    return NULL;
}

int
BinInputModeConfigParser::get_inputmode_size() {
    return m_inputmode_size;
}

void
BinInputModeConfigParser::parsing_input_mode_configure_table() {

    // skip data_size
    m_storage.advance(8);

    //rec_num
    m_inputmode_size = m_storage.get<sint_t>(4);

    Input_mode_configure_width record_width;
    set_input_mode_configure_width(*parser_info_provider, record_width);

    PSclInputModeConfigure cur = m_input_mode_configure_table;

    for (int i = 0; i < m_inputmode_size; ++i) {
        //name
        m_storage.get_str(&(cur->name), record_width.name, m_string_collector);
        //layout_portrait
        m_storage.get_str(&(cur->layouts[0]), record_width.layout_portrait, m_string_collector);
        //layout_landscape
        m_storage.get_str(&(cur->layouts[1]), record_width.layout_landscape, m_string_collector);
        //use_virtual_window
        cur->use_virtual_window = m_storage.get<sint_t>(record_width.use_virtual_window);
        //use_dim_window
        cur->use_dim_window = m_storage.get<sint_t>(record_width.use_dim_window);

        cur->timeout = m_storage.get<sint_t>(record_width.timeout);

        cur++;
    }

#ifdef __SCL_TXT_DEBUG
    put_input_mode_configure_table(DECODE, m_input_mode_configure_table);
#endif
}

PSclInputModeConfigure
BinInputModeConfigParser::get_input_mode_configure_table() {
    return m_input_mode_configure_table;
}

