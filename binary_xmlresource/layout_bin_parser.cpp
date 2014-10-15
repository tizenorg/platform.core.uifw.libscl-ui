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

#include "layout_bin_parser.h"
#include "default_configure_bin_parser.h"
#include "key_coordinate_frame_bin_parser.h"
#include "sclutils.h"

#include "put_record.h"
#include <algorithm>
using namespace std;
using namespace scl;

BinLayoutParser::
BinLayoutParser() {
    m_layout_size = 0;
    memset(m_layout_table, 0x00, sizeof(SclLayout) * MAX_SCL_LAYOUT);
}
BinLayoutParser::
~BinLayoutParser() {
}
BinLayoutParser* BinLayoutParser::
get_instance() {
    static BinLayoutParser instance;
    return &instance;
}
void BinLayoutParser::
init(const FileStorage& storage, int offset, int size, IParserInfo_Provider* provider) {
    m_storage.set_str_provider(provider);
    m_storage.get_storage(storage, offset, size);
    this->parser_info_provider = provider;
    parsing_layout_table();

}

int BinLayoutParser::
get_layout_index(const char *name) {
    int ret = NOT_USED;
    if (name) {
        for(int loop = 0;loop < MAX_SCL_LAYOUT && ret == NOT_USED;loop++) {
            if (m_layout_table[loop].name) {
                if (strcmp(m_layout_table[loop].name, name) == 0) {
                    ret = loop;
                    break;
                }
            }
        }
    }
    return ret;
}

PSclLayout BinLayoutParser::
get_layout_table() {
    return m_layout_table;
}
int BinLayoutParser::
get_layout_size() {
    return m_layout_size;
}


PSclLayoutKeyCoordinatePointerTable BinLayoutParser::
get_key_coordinate_pointer_frame() {
    BinKeyCoordFrameParser *key_coordinate_frame_bin_parser = BinKeyCoordFrameParser::get_instance();
    return key_coordinate_frame_bin_parser->get_key_coordinate_pointer_frame();
}

void BinLayoutParser::
parsing_layout_table() {
    // 4 byte (range[0-4,294,967,295))
    const int DATA_SIZE_BYTES = 4;
    // 1 byte (range[0-128))
    const int REC_NUM_BYTES = 1;
    // 2 byte (range[0-65536))
    const int LAYOUT_REC_DATA_SIZE_BYTES = 2;

    /*open binary file*/
    Layout_width record_width;
    set_layout_width(*parser_info_provider, record_width);

    // skip data size
    m_storage.advance(DATA_SIZE_BYTES);

    // rec_num
    m_layout_size = m_storage.get<sint_t>(REC_NUM_BYTES);

    // layout_rec_data_size
    m_storage.advance(LAYOUT_REC_DATA_SIZE_BYTES);

    for (int i = 0; i < m_layout_size; ++i) {
        SclLayout& cur = m_layout_table[i];
        decode_layout_record(cur, record_width);
    }
#ifdef __SCL_TXT_DEBUG
    put_layout_table(DECODE, m_layout_table);
#endif

}

void
BinLayoutParser::
decode_color(SclColor& color, int width) {
    if (width <= 0) return;

    color.r = m_storage.get<sint_t>(width);
    color.g = m_storage.get<sint_t>(width);
    color.b = m_storage.get<sint_t>(width);
    color.a = m_storage.get<sint_t>(width);
}
void BinLayoutParser::
decode_layout_record(SclLayout& cur, const Layout_width& record_width) {
    //name
    m_storage.get_str(&(cur.name), record_width.name, m_string_collector);
    //display mode
    cur.display_mode = (SCLDisplayMode)m_storage.get<sint_t>(record_width.display_mode);
    //style
    cur.style = (SCLLayoutStyle)m_storage.get<sint_t>(record_width.style);
    //width
    cur.width = m_storage.get<sint_t>(record_width.width);
    //height
    cur.height = m_storage.get<sint_t>(record_width.height);
    //use_sw_button
    cur.use_sw_button = m_storage.get<sint_t>(record_width.use_sw_button);

    //use_magnifier_window
    cur.use_magnifier_window = m_storage.get<sint_t>(record_width.use_magnifier_window);

    //extract_background
    cur.extract_background = m_storage.get<sint_t>(record_width.extract_background);

    //key_width
    cur.key_width = m_storage.get<sint_t>(record_width.key_width);
    //key_height
    cur.key_height = m_storage.get<sint_t>(record_width.key_height);
    //key_spacing
    cur.key_spacing = m_storage.get<sint_t>(record_width.key_spacing);
    //row_spacing
    cur.row_spacing = m_storage.get<sint_t>(record_width.row_spacing);
    //use_sw_background
    cur.use_sw_background = m_storage.get<sint_t>(record_width.use_sw_background);

    decode_color(cur.bg_color, record_width.bg_color);
    //bg_line_width
    cur.bg_line_width = m_storage.get<float_t>(record_width.bg_line_width);

    decode_color(cur.bg_line_color, record_width.bg_line_color);

    //add_grab_left
    cur.add_grab_left = m_storage.get<sint_t>(record_width.add_grab_left);

    //add_grab_right
    cur.add_grab_right = m_storage.get<sint_t>(record_width.add_grab_right);
    //add_grab_top
    cur.add_grab_top = m_storage.get<sint_t>(record_width.add_grab_top);
    //add_grab_bottom
    cur.add_grab_bottom = m_storage.get<sint_t>(record_width.add_grab_bottom);
    //image_path
    for (int i = 0; i < SCL_BUTTON_STATE_MAX; ++i) {
        m_storage.get_str(&(cur.image_path[i]), record_width.image_path, m_string_collector);
    }

    //key_background_image
    for (int i = 0; i < SCL_SHIFT_STATE_MAX; ++i) {
        for (int j = 0; j < SCL_BUTTON_STATE_MAX; ++j) {
            m_storage.get_str(&(cur.key_background_image[i][j]), record_width.key_background_image, m_string_collector);
        }
    }

    //sound_style
    m_storage.get_str(&(cur.sound_style), record_width.sound_style, m_string_collector);

    //vibe_style
    m_storage.get_str(&(cur.vibe_style), record_width.vibe_style, m_string_collector);

    //label_type
    m_storage.get_str(&(cur.label_type), record_width.label_type, m_string_collector);

    //modifier_decorator
    m_storage.get_str(&(cur.modifier_decorator), record_width.modifier_decorator, m_string_collector);
}


