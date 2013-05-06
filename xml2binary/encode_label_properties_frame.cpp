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

#include "resource_storage_impl.h"
#include "encode_label_properties_frame.h"
#include <string>
#include "xmlresource.h"
#include "put_record.h"
#include "_auto_metadata.h"
using namespace xmlresource;
using namespace std;
static void _encode_color(ResourceStorage& storage, const SclColor& color, int width) {
    if (width <= 0) return;
    storage.put<sint_t>(color.r, width);
    storage.put<sint_t>(color.g, width);
    storage.put<sint_t>(color.b, width);
    storage.put<sint_t>(color.a, width);
}

static void
encode_label_properties_record(ResourceStorage& storage, const PSclLabelProperties cur, const Label_properties_record_width& record_width) {
    if (cur == NULL) return;

    //label_name
    storage.encode_string(cur->label_type, record_width.label_type);

    //font_name
    storage.encode_string(cur->font_name, record_width.font_name);

    //font_size
    storage.put<sint_t>(cur->font_size, record_width.font_size);

    //font color
    for (int i = 0; i < SCL_SHIFT_STATE_MAX; ++i) {
        for (int j = 0; j < SCL_BUTTON_STATE_MAX; ++j) {
            _encode_color(storage, cur->font_color[i][j], record_width.font_color);
        }
    }

    //alignment
    storage.put<sint_t>(cur->alignment, record_width.alignment);

    //padding_x
    storage.put<sint_t>(cur->padding_x, record_width.padding_x);

    //padding_y
    storage.put<sint_t>(cur->padding_y, record_width.padding_y);

    //inner_width
    storage.put<sint_t>(cur->inner_width, record_width.inner_width);

    //inner_height
    storage.put<sint_t>(cur->inner_height, record_width.inner_height);

    //shadow_distance
    storage.put<sint_t>(cur->shadow_distance, record_width.shadow_distance);

    //shadow_direction
    storage.put<sint_t>(cur->shadow_direction, record_width.shadow_direction);

    //shadow_color
    for (int i = 0; i < SCL_SHIFT_STATE_MAX; ++i) {
        for (int j = 0; j < SCL_BUTTON_STATE_MAX; ++j) {
            _encode_color(storage, cur->shadow_color[i][j], record_width.shadow_color);
        }
    }
}

static bool isEmptyRecord(const SclLayoutKeyCoordinate& cur) {
    return (cur.x == 0 && cur.y == 0 && cur.width == 0 && cur.height == 0);
}

int
encode_label_properties_frame_file(ResourceStorage& storage, IMetaData_Helper& md_helper) {

    int init_size = storage.size();

    /*size 4bytes*/
    const int SIZE_WIDTH = 4;

    XMLResource *xmlresource = XMLResource::get_instance();
    PSclLabelPropertiesTable labelPropertiesFrame = xmlresource->get_label_properties_frame();

#ifdef __SCL_TXT_DEBUG
    put_label_properties_frame(ENCODE, labelPropertiesFrame);

#endif
    int size = xmlresource->get_labelproperty_size();
    int maxj = MAX_SIZE_OF_LABEL_FOR_ONE;

    storage.reserve(8);

    storage.put<sint_t>(size, SIZE_WIDTH);
    storage.put<sint_t>(maxj, SIZE_WIDTH);

    Label_properties_record_width record_width;
    set_label_properties_record_width(md_helper, record_width);
    for ( int i = 0; i < size; ++i) {
        for ( int j = 0; j < maxj; ++j) {
            SclLabelProperties cur = labelPropertiesFrame[i][j];
            encode_label_properties_record(storage, &cur, record_width);
        }
    }

    int advance_size = storage.size() - init_size;
    storage.random_put<sint_t>(advance_size, 8, init_size);

    return storage.size();
}

int
encode_label_properties_frame_file(const char* file, int& offset, IMetaData_Helper& md_helper) {
    if (!file) return 0;


    ResourceStorage storage;
    encode_label_properties_frame_file(storage, md_helper);
    storage.toFile(file, offset);

    return storage.size();
}

int
encode_label_properties_frame_file(const char* file, IMetaData_Helper& md_helper) {
    if (!file) return 0;


    ResourceStorage storage;
    encode_label_properties_frame_file(storage, md_helper);
    storage.toFile(file);

    return storage.size();
}
