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

#include "encode_magnifier_configure.h"
#include "xmlresource.h"
#include "resource_storage_impl.h"
#include "put_record.h"
#include "_auto_metadata.h"
using namespace xmlresource;
static void
encode_magnifier_configure_record(ResourceStorage& storage, const PSclMagnifierWndConfigure cur, const Magnifier_configure_width& record_width) {
    //style
    storage.put<sint_t>(cur->style, record_width.style);
    //width
    storage.put<sint_t>(cur->width, record_width.width);
    //height
    storage.put<sint_t>(cur->height, record_width.height);
    //label_area_record.left
    storage.put<sint_t>(cur->label_area_rect.left, record_width.label_area_rect);
    //label_area_rect.right
    storage.put<sint_t>(cur->label_area_rect.right, record_width.label_area_rect);

    //label_area_record.top
    storage.put<sint_t>(cur->label_area_rect.top, record_width.label_area_rect);
    //label_area_rect.bottom
    storage.put<sint_t>(cur->label_area_rect.bottom, record_width.label_area_rect);
    //bg_image_path
    storage.encode_string(cur->bg_image_path, record_width.bg_image_path);

    //bg_shift_image_path
    storage.encode_string(cur->bg_shift_image_path, record_width.bg_shift_image_path);

    //bg_shift_lock_image_path
    storage.encode_string(cur->bg_shift_lock_image_path, record_width.bg_shift_lock_image_path);

    //bg_long_key_image_path
    storage.encode_string(cur->bg_long_key_image_path, record_width.bg_long_key_image_path);
    //use_actual_window
    storage.put<sint_t>(cur->use_actual_window, record_width.use_actual_window);
    //label_name
    storage.encode_string(cur->label_type, record_width.label_type);
    //padding_x
    storage.put<sint_t>(cur->padding_x, record_width.padding_x);
    //padding_y
    storage.put<sint_t>(cur->padding_y, record_width.padding_y);
    //show_shift_label
    storage.put<sint_t>(cur->show_shift_label, record_width.show_shift_label);
}

int
encode_magnifier_configure_file(ResourceStorage& storage, IMetaData_Helper& md_helper) {

    int init_size = storage.get_size();

    XMLResource *xmlresource = XMLResource::get_instance();
    PSclMagnifierWndConfigure magnifierConfigure = xmlresource->get_magnifier_configure();


#ifdef __SCL_TXT_DEBUG
    put_magnifier_wnd_configure(ENCODE, *magnifierConfigure);

#endif
    Magnifier_configure_width record_width;
    set_magnifier_configure_width(md_helper, record_width);

    storage.reserve(8);
    encode_magnifier_configure_record(storage, magnifierConfigure, record_width);

    int advance_size = storage.get_size() - init_size;

    storage.random_put<sint_t>(advance_size, 8, init_size);

    return storage.get_size();
}

int
encode_magnifier_configure_file(const char* file, int& offset, IMetaData_Helper& md_helper) {
    if (!file) return 0;


    ResourceStorage storage;
    encode_magnifier_configure_file(storage, md_helper);
    storage.toFile(file, offset);

    return storage.get_size();
}

int
encode_magnifier_configure_file(const char* file, IMetaData_Helper& md_helper) {
    if (!file) return 0;


    ResourceStorage storage;
    encode_magnifier_configure_file(storage, md_helper);
    storage.toFile(file);

    return storage.get_size();
}
