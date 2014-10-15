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

#include "encode_autopopup_configure.h"
#include "xmlresource.h"
#include "resource_storage_impl.h"
#include "put_record.h"
#include "_auto_metadata.h"
using namespace xmlresource;
static void _encode_color(ResourceStorage& storage, const SclColor& color, int width) {
    if (width <= 0) return;

    storage.put<sint_t>(color.r, width);
    storage.put<sint_t>(color.g, width);
    storage.put<sint_t>(color.b, width);
    storage.put<sint_t>(color.a, width);
}
static void
encode_autopopup_configure_record(ResourceStorage& storage, const PSclAutoPopupConfigure cur, const Autopopup_configure_width& record_width) {
    //bg_image_path
    storage.encode_string(cur->bg_image_path, record_width.bg_image_path);

    //bg_color
    _encode_color(storage, cur->bg_color, record_width.bg_color);

    //double value
    storage.put<float_t>(cur->bg_line_width, record_width.bg_line_width);

    //bg_line_color
        _encode_color(storage, cur->bg_line_color, record_width.bg_line_color);

    //bg_padding
    storage.put<sint_t>(cur->bg_padding, record_width.bg_padding);

    //button_image_path
    for (int i = 0; i < SCL_BUTTON_STATE_MAX; ++i) {
        storage.encode_string(cur->button_image_path[i], record_width.button_image_path);
    }

    //sw_button_style
    storage.put<sint_t>(cur->sw_button_style, record_width.sw_button_style);

    //button_width
    storage.put<sint_t>(cur->button_width, record_width.button_width);

    //button_height
    storage.put<sint_t>(cur->button_height, record_width.button_height);

    //button_spacing
    storage.put<sint_t>(cur->button_spacing, record_width.button_spacing);

    storage.encode_string(cur->label_type, record_width.label_type);

    for (int i = 0; i < MAX_WND_DECORATOR; ++i) {
        storage.encode_string(cur->decoration_image_path[i], record_width.decoration_image_path);
    }

    //decoration_size
    storage.put<sint_t>(cur->decoration_size, record_width.decoration_size);

    //max_column
    storage.put<sint_t>(cur->max_column, record_width.max_column);

    //add_grab_left
    storage.put<sint_t>(cur->add_grab_left, record_width.add_grab_left);

    //add_grab_right
    storage.put<sint_t>(cur->add_grab_right, record_width.add_grab_right);

    //add_grab_top
    storage.put<sint_t>(cur->add_grab_top, record_width.add_grab_top);

    //add_grab_bottom
    storage.put<sint_t>(cur->add_grab_bottom, record_width.add_grab_bottom);
}

int
encode_autopopup_configure_file(ResourceStorage& storage, IMetaData_Helper& md_helper) {
    int init_size = storage.get_size();

    XMLResource *xmlresource = XMLResource::get_instance();
    PSclAutoPopupConfigure autopopupConfigure = xmlresource->get_autopopup_configure();

#ifdef __SCL_TXT_DEBUG
    put_autopopup_configure(ENCODE, *autopopupConfigure);

#endif

    Autopopup_configure_width record_width;
    set_autopopup_configure_width(md_helper, record_width);

    storage.reserve(8);

    encode_autopopup_configure_record(storage, autopopupConfigure, record_width);

    int size = storage.get_size() - init_size;
    storage.random_put<sint_t>(size, 8, init_size);

    return storage.get_size();
}

int
encode_autopopup_configure_file(const char* file, IMetaData_Helper& md_helper) {
    if (!file) return 0;
    ResourceStorage storage;
    encode_autopopup_configure_file(storage, md_helper);
    storage.toFile(file);
    return storage.get_size();
}
int
encode_autopopup_configure_file(const char* file, int &offset, IMetaData_Helper& md_helper) {
    if (!file) return 0;
    ResourceStorage storage;
    encode_autopopup_configure_file(storage, md_helper);
    storage.toFile(file, offset);

    return storage.get_size();
}
