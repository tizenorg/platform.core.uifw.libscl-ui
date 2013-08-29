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

#include "label_properties_bin_parser.h"
#include "simple_debug.h"
#include "put_record.h"

BinLabelPropertyParser::BinLabelPropertyParser() {
    m_size = 0;
    memset(m_label_properties_frame, 0, sizeof(SclLabelProperties) * MAX_SCL_LABEL_PROPERTIES * MAX_SIZE_OF_LABEL_FOR_ONE);
}

BinLabelPropertyParser::~BinLabelPropertyParser() {
    m_size = 0;
}

BinLabelPropertyParser* BinLabelPropertyParser::get_instance() {
    static BinLabelPropertyParser instance;
    return &instance;
}
void BinLabelPropertyParser::init(const FileStorage& storage, int offset, int size, IParserInfo_Provider* parser_info_provider) {
    m_storage.set_str_provider(parser_info_provider);
    m_storage.get_storage(storage, offset, size);
    this->parser_info_provider = parser_info_provider;
    parsing_label_properties_frame();
}
const int
BinLabelPropertyParser::get_size() {
    return m_size;
}

//recompute_layout will change the table
PSclLabelPropertiesTable BinLabelPropertyParser::get_label_properties_frame() {
    return m_label_properties_frame;
}

void BinLabelPropertyParser::parsing_label_properties_frame() {
    // skip data_size
    m_storage.advance(8);

    m_size = m_storage.get<sint_t>(4);
    int maxj = m_storage.get<sint_t>(4);

    if (parser_info_provider == NULL) {
        SCLLOG(SclLog::ERROR, "Error parser_info_provider is NULL\n");
        return;
    }

    Label_properties_record_width record_width;
    set_label_properties_record_width(*parser_info_provider, record_width);
    for (int i = 0; i < m_size; ++i) {
        for (int j = 0; j < maxj; ++j) {
            SclLabelProperties &cur = m_label_properties_frame[i][j];
            decode_label_properties_record(&cur, record_width);
        }
    }
#ifdef __SCL_TXT_DEBUG
    put_label_properties_frame(DECODE, m_label_properties_frame);
#endif
}

void
BinLabelPropertyParser::decode_color(SclColor& color, int width) {
    if (width <= 0) return;

    color.r = m_storage.get<sint_t>(width);
    color.g = m_storage.get<sint_t>(width);
    color.b = m_storage.get<sint_t>(width);
    color.a = m_storage.get<sint_t>(width);
}
void
BinLabelPropertyParser::decode_label_properties_record(const PSclLabelProperties cur, const Label_properties_record_width& record_width) {
    cur->valid = (sclboolean)true;

    //label_name
    m_storage.get_str(&(cur->label_type), record_width.label_type, m_string_collector);
    //font_name
    m_storage.get_str(&(cur->font_name), record_width.font_name, m_string_collector);

    //font_size
    cur->font_size = m_storage.get<sint_t>(record_width.font_size);
    //font color
        for (int i = 0; i < SCL_SHIFT_STATE_MAX; ++i) {
            for (int j = 0; j < SCL_BUTTON_STATE_MAX; ++j) {
                decode_color(cur->font_color[i][j], record_width.font_color);
            }
        }

    //alignment
    cur->alignment = (SCLLabelAlignment)m_storage.get<sint_t>(record_width.alignment);

    //padding_x
    cur->padding_x = m_storage.get<sint_t>(record_width.padding_x);

    //padding_y
    cur->padding_y = m_storage.get<sint_t>(record_width.padding_y);

    //inner_width
    cur->inner_width = m_storage.get<sint_t>(record_width.inner_width);

    //inner_height
    cur->inner_height = m_storage.get<sint_t>(record_width.inner_height);

    //shadow_distance
    cur->shadow_distance = m_storage.get<sint_t>(record_width.shadow_distance);

    //shadow_direction
    cur->shadow_direction = (SCLShadowDirection)m_storage.get<sint_t>(record_width.shadow_direction);

    //shadow_color
        for (int i = 0; i < SCL_SHIFT_STATE_MAX; ++i) {
            for (int j = 0; j < SCL_BUTTON_STATE_MAX; ++j) {
                decode_color(cur->shadow_color[i][j], record_width.shadow_color);
            }
        }
}
