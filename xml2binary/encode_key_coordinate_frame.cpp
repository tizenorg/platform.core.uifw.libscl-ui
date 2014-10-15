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
#include "encode_key_coordinate_frame.h"
#include <string>
#include "xmlresource.h"
#include "put_record.h"
#include "_auto_metadata.h"
using namespace xmlresource;
using namespace std;
static void
encode_key_coordinate_record(ResourceStorage& storage, const PSclLayoutKeyCoordinate cur, const Key_coordinate_record_width& record_width) {
    if (cur == NULL) return;

    //x
    storage.put<sint_t>(cur->x, record_width.x);

    //y
    storage.put<sint_t>(cur->y, record_width.y);

    //width
    storage.put<sint_t>(cur->width, record_width.width);

    //height
    storage.put<sint_t>(cur->height, record_width.height);

    //add_hit_left
    storage.put<sint_t>(cur->add_hit_left, record_width.add_hit_left);

    //add_hit_right
    storage.put<sint_t>(cur->add_hit_right, record_width.add_hit_right);

    //add_hit_top
    storage.put<sint_t>(cur->add_hit_top, record_width.add_hit_top);

    //add_hit_bottom
    storage.put<sint_t>(cur->add_hit_bottom, record_width.add_hit_bottom);

    //popup_relative_x
    storage.put<sint_t>(cur->popup_relative_x, record_width.popup_relative_x);

    //popup_relative_y
    storage.put<sint_t>(cur->popup_relative_y, record_width.popup_relative_y);

    //extract_offset_x
    storage.put<sint_t>(cur->extract_offset_x, record_width.extract_offset_x);

    //extract_offset_y
    storage.put<sint_t>(cur->extract_offset_y, record_width.extract_offset_y);

    storage.encode_string(cur->sub_layout, record_width.sub_layout);

    //magnifier_offset_x
    storage.put<sint_t>(cur->magnifier_offset_x, record_width.magnifier_offset_x);

    //magnifier_offset_y
    storage.put<sint_t>(cur->magnifier_offset_y, record_width.magnifier_offset_y);

    storage.encode_string(cur->custom_id, record_width.custom_id);

    //button_type
    storage.put<sint_t>(cur->button_type, record_width.button_type);

    //key_type
    storage.put<sint_t>(cur->key_type, record_width.key_type);

    //popup_type
    storage.put<sint_t>(cur->popup_type, record_width.popup_type);

    storage.put<sint_t>(cur->use_magnifier, record_width.use_magnifier);
    storage.put<sint_t>(cur->use_long_key_magnifier, record_width.use_long_key_magnifier);

    //popup_input_mode
    for (int i = 0; i < SCL_DRAG_STATE_MAX; ++i) {
        storage.encode_string(cur->popup_input_mode[i], record_width.popup_input_mode);
    }

    storage.encode_string(cur->sound_style, record_width.sound_style);
    storage.encode_string(cur->vibe_style, record_width.vibe_style);
    storage.put<sint_t>(cur->is_side_button, record_width.is_side_button);

    //label_count
    storage.put<sint_t>(cur->label_count, record_width.label_count);

    //label
    for (int i = 0; i < SCL_SHIFT_STATE_MAX; ++i) {
        for (int j = 0; j < MAX_SIZE_OF_LABEL_FOR_ONE; ++j) {
            storage.encode_string(cur->label[i][j], record_width.label);
        }
    }

    storage.encode_string(cur->label_type, record_width.label_type);

    //label_image_path
    for (int i = 0; i < SCL_SHIFT_STATE_MAX; ++i) {
        for (int j = 0; j < SCL_BUTTON_STATE_MAX; ++j) {
            storage.encode_string(cur->image_label_path[i][j], record_width.image_label_path);
        }
    }

    storage.encode_string(cur->image_label_type, record_width.image_label_type);

    //bg_image_path
    for (int i = 0; i < SCL_SHIFT_STATE_MAX; ++i) {
        for (int j = 0; j < SCL_BUTTON_STATE_MAX; ++j) {
            storage.encode_string(cur->bg_image_path[i][j], record_width.bg_image_path);
        }
    }

    //key_value_count
    storage.put<sint_t>(cur->key_value_count, record_width.key_value_count);

    //key_value
    for (int i = 0; i < SCL_SHIFT_STATE_MAX; ++i) {
        for (int j = 0; j < MAX_SIZE_OF_MULTITAP_CHAR; ++j) {
            storage.encode_string(cur->key_value[i][j], record_width.key_value);
        }
    }
    //key_event
    for (int i = 0; i < SCL_SHIFT_STATE_MAX; ++i) {
        for (int j = 0; j < MAX_SIZE_OF_LABEL_FOR_ONE; ++j) {
            storage.put<sint_t>(cur->key_event[i][j], record_width.key_event);
        }
    }

    storage.put<sint_t>(cur->long_key_type, record_width.long_key_type);

    //long_key_value
    storage.encode_string(cur->long_key_value, record_width.long_key_value);

    //long_key_event
    storage.put<sint_t>(cur->long_key_event, record_width.long_key_event);

    storage.put<sint_t>(cur->use_repeat_key, record_width.use_repeat_key);

    //autopopup_keys
    for (int i = 0; i < SCL_SHIFT_STATE_MAX; ++i) {
        for (int j = 0; j < MAX_SIZE_OF_AUTOPOPUP_STRING; ++j) {
            storage.encode_string(cur->autopopup_key_labels[i][j], record_width.autopopup_key_labels);
            storage.encode_string(cur->autopopup_key_values[i][j], record_width.autopopup_key_values);
            storage.put<sint_t>(cur->autopopup_key_events[i][j], record_width.autopopup_key_events);
        }
    }

    storage.put<sint_t>(cur->dont_close_popup, record_width.dont_close_popup);

    //extra_option
    storage.put<sint_t>(cur->extra_option, record_width.extra_option);

    //multitouch_type
    storage.put<sint_t>(cur->multitouch_type, record_width.multitouch_type);

    //modifier_decorator
    storage.encode_string(cur->modifier_decorator, record_width.modifier_decorator);

    //magnifier_label
    for (int i = 0; i < SCL_SHIFT_STATE_MAX; ++i) {
        for (int j = 0; j < MAX_SIZE_OF_LABEL_FOR_ONE; ++j) {
            storage.encode_string(cur->magnifier_label[i][j], record_width.magnifier_label);
        }
    }

    // hint_string
    for (int i = 0; i < SCL_SHIFT_STATE_MAX; ++i) {
        for (int j = 0; j < MAX_SIZE_OF_MULTITAP_CHAR; ++j) {
            storage.encode_string(cur->hint_string[i][j], record_width.hint_string);
        }
    }
}

const int get_layout_num(const PSclLayoutKeyCoordinatePointerTable keyCoordinatePointerFrame) {
    assert(keyCoordinatePointerFrame != NULL);
    int layout_num = MAX_SCL_LAYOUT;
    for ( int i = 0; i < MAX_SCL_LAYOUT; ++i) {
        // NULL is the end flag
        if (keyCoordinatePointerFrame[i][0] == NULL) {
            layout_num = i;
            break;
        }
    }

    return layout_num;
}

const int get_key_num(const PSclLayoutKeyCoordinatePointerTable keyCoordinatePointerFrame, const int layout_no) {
    assert(keyCoordinatePointerFrame != NULL);
    int key_num = MAX_KEY;

    SclLayoutKeyCoordinatePointer* curLayoutRecordPointers = keyCoordinatePointerFrame[layout_no];
    for (int i = 0; i < MAX_KEY; ++i) {
        if (curLayoutRecordPointers[i] == NULL) {
            key_num = i;
            break;
        }
    }

    return key_num;
}


int
encode_key_coordinate_frame_file(ResourceStorage& storage, IMetaData_Helper& md_helper) {

    XMLResource *xmlresource = XMLResource::get_instance();

    int layout_num = xmlresource->get_layout_size();
    assert(layout_num >= 0);
    SclLayoutKeyCoordinatePointer _key_coordinate_pointer_frame[MAX_SCL_LAYOUT][MAX_KEY];
    memset(_key_coordinate_pointer_frame, 0x00, MAX_KEY * MAX_SCL_LAYOUT * sizeof(SclLayoutKeyCoordinatePointer));

    for (int i = 0; i < layout_num; ++i) {
        xmlresource->load(i);
        for (int j = 0; j < MAX_KEY; ++j) {
            _key_coordinate_pointer_frame[i][j] = xmlresource->get_key_coordinate_pointer_frame()[i][j];
        }
    }
    // 4 byte (range[0-4,294,967,295))
    const int DATA_SIZE_BYTES = 4;
    // 1 byte (range[0-128))
    const int REC_NUM_BYTES = 1;
    const int KEY_NUM_BYTES = 1;
    // 2 byte (range[0-65536))
    const int KEY_COORDIANTE_REC_DATA_SIZE_BYTES = 2;

    int init_size = storage.get_size();
    // data size
    storage.reserve(DATA_SIZE_BYTES);

    //TODO: assert layout_num < REC_NUM_BYTES's range
    storage.put<uint_t>(layout_num, REC_NUM_BYTES);

    int *pKey_num_array = new int[layout_num];
    memset(pKey_num_array, 0x00, layout_num * sizeof(int));
    for (int i = 0; i < layout_num; ++i) {
        pKey_num_array[i] = get_key_num(_key_coordinate_pointer_frame, i);
        //TODO: assert key_num < KEY_NUM_BYTES's range
        storage.put<uint_t>(pKey_num_array[i], KEY_NUM_BYTES);
    }

    // key_coordinate_rec_data_size
    storage.reserve(KEY_COORDIANTE_REC_DATA_SIZE_BYTES);

    Key_coordinate_record_width record_width;
    set_key_coordinate_record_width(&md_helper, record_width);


    int key_coordinate_rec_data_size = 0;
    for ( int i = 0; i < layout_num; ++i) {
        for ( int j = 0; j < pKey_num_array[i]; ++j) {
            int pre_size = storage.get_size();

            SclLayoutKeyCoordinatePointer cur = _key_coordinate_pointer_frame[i][j];
            if (!cur) break;
            encode_key_coordinate_record(storage, cur, record_width);

            int aft_size = storage.get_size();
            if (key_coordinate_rec_data_size == 0) {
                key_coordinate_rec_data_size = aft_size - pre_size;
            }
            assert(key_coordinate_rec_data_size == aft_size - pre_size);
        }
    }

    delete[] pKey_num_array;

    int advance_size = storage.get_size() - init_size;
    storage.random_put<sint_t>(advance_size, DATA_SIZE_BYTES, init_size);

    // random put key_coordinate_rec_data_size
    int key_coordinate_rec_data_offset = init_size +
                                        DATA_SIZE_BYTES +
                                        REC_NUM_BYTES +
                                        layout_num * KEY_NUM_BYTES;
    storage.random_put<uint_t>(key_coordinate_rec_data_size,
                                KEY_COORDIANTE_REC_DATA_SIZE_BYTES,
                                key_coordinate_rec_data_offset);

    return storage.get_size();
}

int
encode_key_coordinate_frame_file(const char* file, int& offset, IMetaData_Helper& md_helper) {
    if (!file) return 0;

    ResourceStorage storage;
    encode_key_coordinate_frame_file(storage, md_helper);
    storage.toFile(file, offset);

    return storage.get_size();
}

int
encode_key_coordinate_frame_file(const char* file, IMetaData_Helper& md_helper) {
    if (!file) return 0;

    ResourceStorage storage;
    encode_key_coordinate_frame_file(storage, md_helper);
    storage.toFile(file);

    return storage.get_size();
}
