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

#include "magnifier_configure_bin_parser.h"
#include "file_storage.h"
#include <assert.h>
using namespace std;
#include "put_record.h"

BinMagnifierConfigParser::BinMagnifierConfigParser() {
    memset((void*)&m_magnifier_configure, 0x00, sizeof(SclMagnifierWndConfigure));
}

BinMagnifierConfigParser::~BinMagnifierConfigParser() {
}

BinMagnifierConfigParser* BinMagnifierConfigParser::get_instance() {
    static BinMagnifierConfigParser instance;
    return &instance;
}

void BinMagnifierConfigParser::init(const FileStorage& storage, int offset, int size, IParserInfo_Provider* parser_info_provider) {
    m_storage.set_str_provider(parser_info_provider);
    m_storage.get_storage(storage, offset, size);
    this->parser_info_provider = parser_info_provider;
    parsing_magnifier_configure();
}


void BinMagnifierConfigParser::parsing_magnifier_configure() {
    Magnifier_configure_width record_width;
    set_magnifier_configure_width(*parser_info_provider, record_width);

    PSclMagnifierWndConfigure cur = &m_magnifier_configure;

    // skip data_size
    m_storage.advance(8);

    //style
    cur->style = (SCLMagnifierStyle)m_storage.get<sint_t>(record_width.style);
    //width
    cur->width = m_storage.get<sint_t>(record_width.width);
    //height
    cur->height = m_storage.get<sint_t>(record_width.height);
    //label_area_record.left
    cur->label_area_rect.left = m_storage.get<sint_t>(record_width.label_area_rect);
    //label_area_rect.right
    cur->label_area_rect.right = m_storage.get<sint_t>(record_width.label_area_rect);

    //label_area_record.top
    cur->label_area_rect.top = m_storage.get<sint_t>(record_width.label_area_rect);
    //label_area_rect.bottom
    cur->label_area_rect.bottom = m_storage.get<sint_t>(record_width.label_area_rect);
    //bg_image_path
    m_storage.get_str(&(cur->bg_image_path), record_width.bg_image_path, m_string_collector);

    //bg_shift_image_path
    m_storage.get_str(&(cur->bg_shift_image_path), record_width.bg_shift_image_path, m_string_collector);

    //bg_shift_lock_image_path
    m_storage.get_str(&(cur->bg_shift_lock_image_path), record_width.bg_shift_lock_image_path, m_string_collector);

    //bg_long_key_image_path
    m_storage.get_str(&(cur->bg_long_key_image_path), record_width.bg_long_key_image_path, m_string_collector);
    //use_actual_window
    cur->use_actual_window = m_storage.get<sint_t>(record_width.use_actual_window);
    //label_name
    m_storage.get_str(&(cur->label_type), record_width.label_type, m_string_collector);
    //padding_x
    cur->padding_x = m_storage.get<sint_t>(record_width.padding_x);
    //padding_y
    cur->padding_y = m_storage.get<sint_t>(record_width.padding_y);
    //show_shift_label
    cur->show_shift_label = m_storage.get<sint_t>(record_width.show_shift_label);

#ifdef __SCL_TXT_DEBUG
    put_magnifier_wnd_configure(DECODE, m_magnifier_configure);
#endif
}

PSclMagnifierWndConfigure BinMagnifierConfigParser::get_magnifier_configure() {
    return &m_magnifier_configure;
}

