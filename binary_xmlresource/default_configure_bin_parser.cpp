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

#include "default_configure_bin_parser.h"
#include <assert.h>
using namespace std;
#include "put_record.h"

BinDefaultConfigParser::BinDefaultConfigParser() {
    memset((void*)&m_default_configure, 0x00, sizeof(SclDefaultConfigure));
}

BinDefaultConfigParser::~BinDefaultConfigParser() {
}

BinDefaultConfigParser* BinDefaultConfigParser::get_instance() {
    static BinDefaultConfigParser instance;
    return &instance;
}

void BinDefaultConfigParser::init(const FileStorage& storage, int offset, int size,  IParserInfo_Provider* parser_info_provider) {
    m_storage.set_str_provider(parser_info_provider);
    m_storage.get_storage(storage, offset, size);
    this->parser_info_provider = parser_info_provider;
    parsing_default_configure();
}
void
BinDefaultConfigParser::decode_color(SclColor& color, int width) {
    if (width <= 0) return;

    color.r = m_storage.get<sint_t>(width);
    color.g = m_storage.get<sint_t>(width);
    color.b = m_storage.get<sint_t>(width);
    color.a = m_storage.get<sint_t>(width);
}

void BinDefaultConfigParser::parsing_default_configure() {

    PSclDefaultConfigure cur = &m_default_configure;

    Default_configure_width record_width;
    set_default_configure_width(*parser_info_provider, record_width);

    // skip data_size
    m_storage.advance(8);

    //display_mode
    cur->display_mode = (SCLDisplayMode)m_storage.get<sint_t>(record_width.display_mode);

    //input_mode
    m_storage.get_str(&(cur->input_mode), record_width.input_mode, m_string_collector);

    //image_file_base_path
    m_storage.get_str(&(cur->image_file_base_path), record_width.image_file_base_path, m_string_collector);

    //target_screen_width
    cur->target_screen_width = m_storage.get<sint_t>(record_width.target_screen_width);

    //target_screen_height
    cur->target_screen_height = m_storage.get<sint_t>(record_width.target_screen_height);

    //auto_detect_landscape
    cur->auto_detect_landscape = m_storage.get<sint_t>(record_width.auto_detect_landscape);

    //use_magnifier_window
    cur->use_magnifier_window = m_storage.get<sint_t>(record_width.use_magnifier_window);

    //use_auto_popup
    cur->use_auto_popup = m_storage.get<sint_t>(record_width.use_auto_popup);

    //use_zoom_window
    cur->use_zoom_window = m_storage.get<sint_t>(record_width.use_zoom_window);

    //on_error_noti_send
    cur->on_error_noti_send = m_storage.get<sint_t>(record_width.on_error_noti_send);

    //use_word_deletion
    cur->use_word_deletion = m_storage.get<sint_t>(record_width.use_word_deletion);

    //sw_button_style
    cur->sw_button_style = m_storage.get<sint_t>(record_width.sw_button_style);

    //touch_offset_level
    for (int i = 0; i < DISPLAYMODE_MAX; ++i) {
        cur->touch_offset_level[i] = (SCLTouchOffsetLevel)m_storage.get<sint_t>(record_width.touch_offset_level);
    }

    //touch_offset
    for (int i = 0; i < DISPLAYMODE_MAX; ++i) {
        cur->touch_offset[i].x = m_storage.get<sint_t>(record_width.touch_offset);
        cur->touch_offset[i].y = m_storage.get<sint_t>(record_width.touch_offset);
    }

    //default_sub_layout
    m_storage.get_str(&(cur->default_sub_layout), record_width.default_sub_layout, m_string_collector);

    //use_actual_dim_window
    cur->use_actual_dim_window = m_storage.get<sint_t>(record_width.use_actual_dim_window);

    //dim_color
    decode_color(cur->dim_color, record_width.dim_color);

#ifdef __SCL_TXT_DEBUG
    put_default_configure(DECODE, m_default_configure);
#endif
}

PSclDefaultConfigure BinDefaultConfigParser::get_default_configure() {
    return &m_default_configure;
}

