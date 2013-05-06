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

#include "encode_default_configure.h"
#include "xmlresource.h"
#include "resource_storage_impl.h"
using namespace xmlresource;

#include "put_record.h"
#include "_auto_metadata.h"
static void _encode_color(ResourceStorage& storage, const SclColor& color, int width) {
    if (width <= 0) return;

    storage.put<sint_t>(color.r, width);
    storage.put<sint_t>(color.g, width);
    storage.put<sint_t>(color.b, width);
    storage.put<sint_t>(color.a, width);
}
static void
encode_default_configure_record(ResourceStorage& storage, const PSclDefaultConfigure cur, const Default_configure_width& record_width) {
    //display_mode
    storage.put<sint_t>((int)cur->display_mode, record_width.display_mode);

    //input_mode
    storage.encode_string(cur->input_mode, record_width.input_mode);

    //image_file_base_path
    storage.encode_string(cur->image_file_base_path, record_width.image_file_base_path);

    //target_screen_width
    storage.put<sint_t>(cur->target_screen_width, record_width.target_screen_width);

    //target_screen_height
    storage.put<sint_t>(cur->target_screen_height, record_width.target_screen_height);

    //auto_detect_landscape
    storage.put<sint_t>(cur->auto_detect_landscape, record_width.auto_detect_landscape);

    //use_magnifier_window
    storage.put<sint_t>(cur->use_magnifier_window, record_width.use_magnifier_window);

    //use_auto_popup
    storage.put<sint_t>(cur->use_auto_popup, record_width.use_auto_popup);

    //use_zoom_window
    storage.put<sint_t>(cur->use_zoom_window, record_width.use_zoom_window);

    //on_error_noti_send
    storage.put<sint_t>(cur->on_error_noti_send, record_width.on_error_noti_send);

    //use_word_deletion
    storage.put<sint_t>(cur->use_word_deletion, record_width.use_word_deletion);

    //sw_button_style
    storage.put<sint_t>(cur->sw_button_style, record_width.sw_button_style);

    //touch_offset_level
    for (int i = 0; i < DISPLAYMODE_MAX; ++i) {
        storage.put<sint_t>((int)cur->touch_offset_level[i], record_width.touch_offset_level);
    }

    //touch_offset
    for (int i = 0; i < DISPLAYMODE_MAX; ++i) {
        storage.put<sint_t>((int)cur->touch_offset[i].x, record_width.touch_offset);
        storage.put<sint_t>((int)cur->touch_offset[i].y, record_width.touch_offset);
    }

    //default_sub_layout
    storage.encode_string(cur->default_sub_layout, record_width.default_sub_layout);

    //use_actual_dim_window
    storage.put<sint_t>(cur->use_actual_dim_window, record_width.use_actual_dim_window);

    //dim_color
    _encode_color(storage, cur->dim_color, record_width.dim_color);
}

int
encode_default_configure_file(ResourceStorage& storage, IMetaData_Helper& md_helper) {
    int init_size = storage.size();

    XMLResource *xmlresource = XMLResource::get_instance();
    PSclDefaultConfigure defaultConfigure = xmlresource->get_default_configure();

#ifdef __SCL_TXT_DEBUG
    put_default_configure(ENCODE, *defaultConfigure);

#endif
    Default_configure_width record_width;
    set_default_configure_width(md_helper, record_width);

    storage.reserve(8);
    encode_default_configure_record(storage, defaultConfigure, record_width);

    int size = storage.size() - init_size;
    storage.random_put<sint_t>(size, 8, init_size);
    return storage.size();
}

int
encode_default_configure_file(const char* file, IMetaData_Helper& md_helper) {
    if (!file) return 0;

    ResourceStorage storage;
    encode_default_configure_file(storage, md_helper);
    storage.toFile(file);

    return storage.size();
}

int
encode_default_configure_file(const char* file, int& offset, IMetaData_Helper& md_helper) {
    if (!file) return 0;

    ResourceStorage storage;
    encode_default_configure_file(storage, md_helper);
    storage.toFile(file, offset);

    return storage.size();
}
