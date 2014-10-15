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

#include "autopopup_configure_bin_parser.h"
#include <assert.h>
using namespace std;
#include "put_record.h"

BinAutoPopupConfigParser::BinAutoPopupConfigParser() {
    memset((void*)&m_autopopup_configure, 0x00, sizeof(SclAutoPopupConfigure));
}

BinAutoPopupConfigParser::~BinAutoPopupConfigParser() {
}

BinAutoPopupConfigParser* BinAutoPopupConfigParser::get_instance() {
    static BinAutoPopupConfigParser instance;

    return &instance;
}

void
BinAutoPopupConfigParser::decode_color(SclColor& color, int width) {
    if (width <= 0) return;

    color.r = m_storage.get<sint_t>(width);
    color.g = m_storage.get<sint_t>(width);
    color.b = m_storage.get<sint_t>(width);
    color.a = m_storage.get<sint_t>(width);
}

void BinAutoPopupConfigParser::init(const FileStorage& storage, int offset, int size, IParserInfo_Provider* provider) {
    m_storage.set_str_provider(provider);
    m_storage.get_storage(storage, offset, size);
    this->parser_info_provider = provider;

    parsing_autopopup_configure();
}

void BinAutoPopupConfigParser::parsing_autopopup_configure() {
    Autopopup_configure_width record_width;
    set_autopopup_configure_width(*parser_info_provider, record_width);

    PSclAutoPopupConfigure cur = &m_autopopup_configure;

    // skip data_size
    m_storage.advance(8);

    //bg_image_path
    m_storage.get_str(&(cur->bg_image_path), record_width.bg_image_path, m_string_collector);

    //bg_color
    decode_color(cur->bg_color, record_width.bg_color);
    //double value
        cur->bg_line_width = m_storage.get<float_t>(record_width.bg_line_width);

    //bg_line_color
        decode_color(cur->bg_line_color, record_width.bg_line_color);

    //bg_padding
    cur->bg_padding = m_storage.get<sint_t>(record_width.bg_padding);

    //button_image_path
    for (int i = 0; i < SCL_BUTTON_STATE_MAX; ++i) {
        m_storage.get_str(&(cur->button_image_path[i]), record_width.button_image_path, m_string_collector);
    }

    //sw_button_style
    cur->sw_button_style = m_storage.get<sint_t>(record_width.sw_button_style);

    //button_width
    cur->button_width = m_storage.get<sint_t>(record_width.button_width);

    //button_height
    cur->button_height = m_storage.get<sint_t>(record_width.button_height);

    //button_spacing
    cur->button_spacing = m_storage.get<sint_t>(record_width.button_spacing);

    m_storage.get_str(&(cur->label_type), record_width.label_type, m_string_collector);

    for (int i = 0; i < MAX_WND_DECORATOR; ++i) {
        m_storage.get_str(&(cur->decoration_image_path[i]), record_width.decoration_image_path, m_string_collector);
    }

    //decoration_size
    cur->decoration_size = m_storage.get<sint_t>(record_width.decoration_size);

    //max_column
    cur->max_column = m_storage.get<sint_t>(record_width.max_column);

    //add_grab_left
    cur->add_grab_left = m_storage.get<sint_t>(record_width.add_grab_left);

    //add_grab_right
    cur->add_grab_right = m_storage.get<sint_t>(record_width.add_grab_right);

    //add_grab_top
    cur->add_grab_top = m_storage.get<sint_t>(record_width.add_grab_top);

    //add_grab_bottom
    cur->add_grab_bottom = m_storage.get<sint_t>(record_width.add_grab_bottom);
#ifdef __SCL_TXT_DEBUG
    put_autopopup_configure(DECODE, m_autopopup_configure);
#endif
}

PSclAutoPopupConfigure BinAutoPopupConfigParser::get_autopopup_configure() {
    return &m_autopopup_configure;
}
