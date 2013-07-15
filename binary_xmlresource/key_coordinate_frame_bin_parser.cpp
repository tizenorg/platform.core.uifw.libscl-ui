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

#include "key_coordinate_frame_bin_parser.h"
#include "sclutils.h"

#include "string_bin_parser.h"
#include "metadata_bin_parser.h"
#include "parserinfo_provider.h"
#include "string_provider.h"
#include "metadata_provider.h"
#include <algorithm>
#include "put_record.h"
#include <malloc.h>
#include "binary_xmlresource.h"
#include "simple_debug.h"
using namespace binary_xmlresource;

using namespace std;
using namespace scl;
BinKeyCoordFrameParser* BinKeyCoordFrameParser::m_instance = NULL;

BinKeyCoordFrameParser::BinKeyCoordFrameParser() {
    memset(m_key_coordinate_pointer_frame, 0x00, sizeof(SclLayoutKeyCoordinatePointer) * MAX_SCL_LAYOUT * MAX_KEY);
}
BinKeyCoordFrameParser::~BinKeyCoordFrameParser() {
    for (int i = 0; i < MAX_SCL_LAYOUT; ++i) {
        for (int j = 0; j < MAX_KEY; ++j) {
            free(m_key_coordinate_pointer_frame[i][j]);
            m_key_coordinate_pointer_frame[i][j] = NULL;
        }
    }
}
BinKeyCoordFrameParser* BinKeyCoordFrameParser::get_instance() {
    if (m_instance == NULL) {
        m_instance = new BinKeyCoordFrameParser();
    }
    return m_instance;
}

void
BinKeyCoordFrameParser::
load(int layout_id)
{
    BinResource *bin_xmlres = BinResource::get_instance();
    int layout_data_offset = bin_xmlres->info[LAYOUT].offset;

    char path[_POSIX_PATH_MAX] = {0};

    snprintf(path, _POSIX_PATH_MAX, "%s/%s", bin_xmlres->get_resource_directory(), "sclresource.bin");

    FileStorage storage;
    FileStorage alldata;
    alldata.loadFile(path);

    String_Bin_Parser stringBinParser(alldata, bin_xmlres->info[STRING].offset, bin_xmlres->info[STRING].size);
    Metadata_Bin_Parser metadataBinParser(alldata, bin_xmlres->info[METADATA].offset, bin_xmlres->info[METADATA].size);

    String_Provider stringProvider(&stringBinParser);
    Metadata_Provider metadataProvider(&metadataBinParser);
    ParserInfo_Provider parser_info_provider(&metadataProvider, &stringProvider);
    storage.set_str_provider(&parser_info_provider);
    this->parser_info_provider = &parser_info_provider;
    storage.get_storage(
        alldata,
        bin_xmlres->info[KEY_COORDINATE_FRAME].offset,
        bin_xmlres->info[KEY_COORDINATE_FRAME].size);

    // 4 byte (range[0-4,294,967,295))
    const int DATA_SIZE_BYTES = 4;
    // 1 byte (range[0-128))
    const int REC_NUM_BYTES = 1;
    const int KEY_NUM_BYTES = 1;
    // 2 byte (range[0-65536))
    const int KEY_COORDIANTE_REC_DATA_SIZE_BYTES = 2;

    // skip data_size
    storage.advance(DATA_SIZE_BYTES);

    // rec_num
    int layout_num = storage.get<uint_t>(REC_NUM_BYTES);
    int *pKey_num_array = new int[layout_num];
    memset(pKey_num_array, 0x00, layout_num * sizeof(int));
    for (int i = 0; i < layout_num; ++i) {
        pKey_num_array[i] = storage.get<uint_t>(KEY_NUM_BYTES);
    }

    // key_coordinate_rec_data_size
    int key_coordinate_rec_data_size = storage.get<uint_t>(KEY_COORDIANTE_REC_DATA_SIZE_BYTES);
    Key_coordinate_record_width record_width;
    set_key_coordinate_record_width(parser_info_provider, record_width);

    for (int i = 0; i < layout_num; ++i) {
        if (layout_id == i) {
            for (int j = 0; j < pKey_num_array[i]; ++j) {
                SclLayoutKeyCoordinatePointer curPointer = (SclLayoutKeyCoordinatePointer)malloc(sizeof(SclLayoutKeyCoordinate));
                if (curPointer == NULL) {
                    SCLLOG(SclLog::ERROR, "Memory malloc error.\n");
                    assert(0);
                }
                memset(curPointer, 0x00, sizeof(SclLayoutKeyCoordinate));
                decode_key_coordinate_record(storage, curPointer, record_width);
                m_key_coordinate_pointer_frame[i][j] = curPointer;
            }
            break;
        }else{
            storage.advance(pKey_num_array[i] * key_coordinate_rec_data_size);
        }
    }

    delete []pKey_num_array;
}

bool
BinKeyCoordFrameParser::
loaded(int layout_id)
{
    bool ret = TRUE;
    if (layout_id >= 0 && layout_id < MAX_SCL_LAYOUT) {
        if (m_key_coordinate_pointer_frame[layout_id][0] == NULL) {
            ret = FALSE;
        }
    }
    return ret;
}

void
BinKeyCoordFrameParser::
unload()
{
    for (int i = 0; i < MAX_SCL_LAYOUT; ++i) {
        for (int j = 0; j < MAX_KEY; ++j) {
            free(m_key_coordinate_pointer_frame[i][j]);
            m_key_coordinate_pointer_frame[i][j] = NULL;
        }
    }
}
void
BinKeyCoordFrameParser::init(const FileStorage& storage, int offset, int size, IParserInfo_Provider* parser_info_provider) {
    m_storage.set_str_provider(parser_info_provider);
    m_storage.get_storage(storage, offset, size);
    this->parser_info_provider = parser_info_provider;
}

PSclLayoutKeyCoordinatePointerTable
BinKeyCoordFrameParser::get_key_coordinate_pointer_frame() {
    return m_key_coordinate_pointer_frame;
}

void
BinKeyCoordFrameParser::decode_key_coordinate_record(FileStorage& storage, const PSclLayoutKeyCoordinate cur, const Key_coordinate_record_width& record_width) {
    int width = 0;

    cur->valid = (sclboolean)true;
    //x
    cur->x = storage.get<sint_t>(record_width.x);
    //y
    cur->y = storage.get<sint_t>(record_width.y);

    //width
    cur->width = storage.get<sint_t>(record_width.width);
    //height
    cur->height = storage.get<sint_t>(record_width.height);

    //add_hit_left
    cur->add_hit_left = storage.get<sint_t>(record_width.add_hit_left);
    //add_hit_right
    cur->add_hit_right = storage.get<sint_t>(record_width.add_hit_right);
    //add_hit_top
    cur->add_hit_top = storage.get<sint_t>(record_width.add_hit_top);
    //add_hit_bottom
    cur->add_hit_bottom = storage.get<sint_t>(record_width.add_hit_bottom);
    //popup_relative_x
    cur->popup_relative_x = storage.get<sint_t>(record_width.popup_relative_x);
    //popup_relative_y
    cur->popup_relative_y = storage.get<sint_t>(record_width.popup_relative_y);
    //extract_offset_x
    cur->extract_offset_x = storage.get<sint_t>(record_width.extract_offset_x);
    //extract_offset_y
    cur->extract_offset_y = storage.get<sint_t>(record_width.extract_offset_y);

    storage.get_str(&(cur->sub_layout), record_width.sub_layout, m_string_collector);

    //magnifier_offset_x
    cur->magnifier_offset_x = storage.get<sint_t>(record_width.magnifier_offset_x);
    //magnifier_offset_y
    cur->magnifier_offset_y = storage.get<sint_t>(record_width.magnifier_offset_y);

    storage.get_str(&(cur->custom_id), record_width.custom_id, m_string_collector);

    //button_type
    cur->button_type = (SCLButtonType)storage.get<sint_t>(record_width.button_type);
    //key_type
    cur->key_type = (SCLKeyType)storage.get<sint_t>(record_width.key_type);
    //popup_type
    cur->popup_type = (SCLPopupType)storage.get<sint_t>(record_width.popup_type);

    cur->use_magnifier = storage.get<sint_t>(record_width.use_magnifier);
    cur->use_long_key_magnifier = storage.get<sint_t>(record_width.use_long_key_magnifier);

    //popup_input_mode
    for (int i = 0; i < SCL_DRAG_STATE_MAX; ++i) {
        storage.get_str(&(cur->popup_input_mode[i]), record_width.popup_input_mode, m_string_collector);
    }

    storage.get_str(&(cur->sound_style), record_width.sound_style, m_string_collector);
    storage.get_str(&(cur->vibe_style), record_width.vibe_style, m_string_collector);
    cur->is_side_button = storage.get<sint_t>(record_width.is_side_button);

    //label_count
    cur->label_count = storage.get<sint_t>(record_width.label_count);

    //label
    for (int i = 0; i < SCL_SHIFT_STATE_MAX; ++i) {
        for (int j = 0; j < MAX_SIZE_OF_LABEL_FOR_ONE; ++j) {
            storage.get_str(&(cur->label[i][j]), record_width.label, m_string_collector);
        }
    }

    storage.get_str(&(cur->label_type), record_width.label_type, m_string_collector);

    //label_image_path
    for (int i = 0; i < SCL_SHIFT_STATE_MAX; ++i) {
        for (int j = 0; j < SCL_BUTTON_STATE_MAX; ++j) {
            storage.get_str(&(cur->image_label_path[i][j]), record_width.image_label_path, m_string_collector);
        }
    }

    storage.get_str(&(cur->image_label_type), record_width.image_label_type, m_string_collector);

    //bg_image_path
    for (int i = 0; i < SCL_SHIFT_STATE_MAX; ++i) {
        for (int j = 0; j < SCL_BUTTON_STATE_MAX; ++j) {
            storage.get_str(&(cur->bg_image_path[i][j]), record_width.bg_image_path, m_string_collector);
        }
    }

    //key_value_count
    cur->key_value_count = storage.get<sint_t>(record_width.key_value_count);

    //key_value
    for (int i = 0; i < SCL_SHIFT_STATE_MAX; ++i) {
        for (int j = 0; j < MAX_SIZE_OF_MULTITAP_CHAR; ++j) {
            storage.get_str(&(cur->key_value[i][j]), record_width.key_value, m_string_collector);
        }
    }
    //key_event
    for (int i = 0; i < SCL_SHIFT_STATE_MAX; ++i) {
        for (int j = 0; j < MAX_SIZE_OF_LABEL_FOR_ONE; ++j) {
            cur->key_event[i][j] = storage.get<sint_t>(record_width.key_event);
        }
    }

    //long_key_type
    cur->long_key_type = (SCLKeyType)storage.get<sint_t>(record_width.long_key_type);

    //long_key_value
    storage.get_str(&(cur->long_key_value), record_width.long_key_value, m_string_collector);

    //long_key_event
    cur->long_key_event = storage.get<sint_t>(record_width.long_key_event);

    cur->use_repeat_key = storage.get<sint_t>(record_width.use_repeat_key);

    //autopopup_keys
    for (int i = 0; i < SCL_SHIFT_STATE_MAX; ++i) {
        for (int j = 0; j < MAX_SIZE_OF_AUTOPOPUP_STRING; ++j) {
            storage.get_str(&(cur->autopopup_key_labels[i][j]), record_width.autopopup_key_labels, m_string_collector);
            storage.get_str(&(cur->autopopup_key_values[i][j]), record_width.autopopup_key_values, m_string_collector);
            cur->autopopup_key_events[i][j] = storage.get<sint_t>(record_width.autopopup_key_events);
        }
    }

    cur->dont_close_popup = storage.get<sint_t>(record_width.dont_close_popup);

    //extra_option
    cur->extra_option = storage.get<sint_t>(record_width.extra_option);

    //multitouch_type
    cur->multitouch_type = storage.get<sint_t>(record_width.multitouch_type);

    // modifier_decorator
    storage.get_str(&(cur->modifier_decorator), record_width.modifier_decorator, m_string_collector);

    //magnifier_label
    for (int i = 0; i < SCL_SHIFT_STATE_MAX; ++i) {
        for (int j = 0; j < MAX_SIZE_OF_LABEL_FOR_ONE; ++j) {
            storage.get_str(&(cur->magnifier_label[i][j]), record_width.magnifier_label, m_string_collector);
        }
    }

    // hint_string
    for (int i = 0; i < SCL_SHIFT_STATE_MAX; ++i) {
        for (int j = 0; j < MAX_SIZE_OF_MULTITAP_CHAR; ++j) {
            storage.get_str(&(cur->hint_string[i][j]), record_width.hint_string, m_string_collector);
        }
    }
}

