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

#include "resource_storage_impl.h"
#include "encode_layout.h"
#include <string>
#include <libxml/parser.h>
#include "xmlresource.h"
#include "put_record.h"
#include "_auto_metadata.h"
using namespace std;
using namespace xmlresource;
extern IString_Encoder& string_encoder;

static void _encode_color(ResourceStorage& storage, const SclColor& color, int width) {
    if (width <= 0) return;

    storage.put<sint_t>(color.r, width);
    storage.put<sint_t>(color.g, width);
    storage.put<sint_t>(color.b, width);
    storage.put<sint_t>(color.a, width);
}

static void
encode_layout_record(ResourceStorage& storage, PSclLayout cur, const Layout_width& record_width) {
    //name
    storage.encode_string((const char*)cur->name, record_width.name);
    //display mode
    storage.put<sint_t>(cur->display_mode, record_width.display_mode);

    //style
    storage.put<sint_t>(cur->style, record_width.style);

    //width
    storage.put<sint_t>(cur->width, record_width.width);

    //height
    storage.put<sint_t>(cur->height, record_width.height);

    //use_sw_button
    storage.put<sint_t>(cur->use_sw_button, record_width.use_sw_button);

    //use_magnifier_window
    storage.put<sint_t>(cur->use_magnifier_window, record_width.use_magnifier_window);

    //extract_background
    storage.put<sint_t>(cur->extract_background, record_width.extract_background);

    //key_width
    storage.put<sint_t>(cur->key_width, record_width.key_width);

    //key_height
    storage.put<sint_t>(cur->key_height, record_width.key_height);
    //key_spacing
    storage.put<sint_t>(cur->key_spacing, record_width.key_spacing);
    //row_spacing
    storage.put<sint_t>(cur->row_spacing, record_width.row_spacing);

    //use_sw_background
    storage.put<sint_t>(cur->use_sw_background, record_width.use_sw_background);

    _encode_color(storage, cur->bg_color, record_width.bg_color);

    //bg_line_width
    storage.put<float_t>(cur->bg_line_width, record_width.bg_line_width);

    _encode_color(storage, cur->bg_line_color, record_width.bg_line_color);

    //add_grab_left
    storage.put<sint_t>(cur->add_grab_left, record_width.add_grab_left);

    //add_grab_right
    storage.put<sint_t>(cur->add_grab_right, record_width.add_grab_right);

    //add_grab_top
    storage.put<sint_t>(cur->add_grab_top, record_width.add_grab_top);

    //add_grab_bottom
    storage.put<sint_t>(cur->add_grab_bottom, record_width.add_grab_bottom);

    //image_path
    for (int i = 0; i < SCL_BUTTON_STATE_MAX; ++i) {
        storage.encode_string(cur->image_path[i], record_width.image_path);
    }

    //key_background_image
    for (int i = 0; i < SCL_SHIFT_STATE_MAX; ++i) {
        for (int j = 0; j < SCL_BUTTON_STATE_MAX; ++j) {
            storage.encode_string(cur->key_background_image[i][j], record_width.key_background_image);
        }
    }

    //sound_style
    storage.encode_string(cur->sound_style, record_width.sound_style);
    //vibe_style
    storage.encode_string(cur->vibe_style, record_width.vibe_style);
    //label_name
    storage.encode_string(cur->label_type, record_width.label_type);
    //modifier_decorator
    storage.encode_string(cur->modifier_decorator, record_width.modifier_decorator);
}

int
encode_layout_file(ResourceStorage& storage, IMetaData_Helper& md_helper) {
    // 4 byte (range[0-4,294,967,295))
    const int DATA_SIZE_BYTES = 4;
    // 1 byte (range[0-128))
    const int REC_NUM_BYTES = 1;
    // 2 byte (range[0-65536))
    const int LAYOUT_REC_DATA_SIZE_BYTES = 2;

    const int init_size = storage.get_size();

    XMLResource *xmlresource = XMLResource::get_instance();
    PSclLayout layoutTable = xmlresource->get_layout_table();

    int size = xmlresource->get_layout_size();

#ifdef __SCL_TXT_DEBUG
    put_layout_table(ENCODE, layoutTable);
#endif

    PSclLayout cur = layoutTable;

    // data size
    storage.reserve(DATA_SIZE_BYTES);

    // rec_num
    storage.put<sint_t>(size, REC_NUM_BYTES);

    // rec data size
    storage.reserve(LAYOUT_REC_DATA_SIZE_BYTES);

    Layout_width record_width;
    set_layout_width(md_helper, record_width);

    int layout_rec_data_size = 0;
    for ( int i = 0; i < size; ++i) {
        int pre_size = storage.get_size();
        encode_layout_record(storage, cur, record_width);

        int aft_size = storage.get_size();
        if (layout_rec_data_size == 0) {
            layout_rec_data_size = aft_size - pre_size;
        }
        assert(layout_rec_data_size == aft_size - pre_size);

        cur++;
    }

    // back write data size
    int advance_size = storage.get_size() - init_size;

    // random put advance_size
    int data_size_offset = init_size;
    storage.random_put<sint_t>(advance_size, DATA_SIZE_BYTES, data_size_offset);

    // random put layout_rec size
    int layout_rec_data_offset = init_size + DATA_SIZE_BYTES + REC_NUM_BYTES;
    storage.random_put<sint_t>(layout_rec_data_size, LAYOUT_REC_DATA_SIZE_BYTES, layout_rec_data_offset);

    return storage.get_size();
}

int
encode_layout_file(const char* file, IMetaData_Helper& md_helper) {
    if (!file) return 0;

    ResourceStorage storage;
    encode_layout_file(storage, md_helper);

    storage.toFile(file);

    return storage.get_size();
}

int
encode_layout_file(const char* file, int& offset, IMetaData_Helper& md_helper) {
    if (!file) return 0;

    ResourceStorage storage;
    encode_layout_file(storage, md_helper);

    storage.toFile(file, offset);

    return storage.get_size();
}
