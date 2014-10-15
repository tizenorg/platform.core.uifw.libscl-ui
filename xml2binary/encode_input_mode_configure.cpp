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
#include "encode_input_mode_configure.h"
#include <string>
#include <libxml/parser.h>
#include "xmlresource.h"
#include "imetadata_helper.h"
#include "put_record.h"
#include "_auto_metadata.h"
using namespace std;
using namespace xmlresource;

static void
encode_input_mode_configure_record(ResourceStorage& storage, const PSclInputModeConfigure cur, const Input_mode_configure_width& record_width) {
    //name
    storage.encode_string(cur->name, record_width.name);
    //layout_portrait
    storage.encode_string(cur->layouts[0], record_width.layout_portrait);
    //layout_landscape
    storage.encode_string(cur->layouts[1], record_width.layout_landscape);
    //use_virtual_window
    storage.put<sint_t>(cur->use_virtual_window, record_width.use_virtual_window);
    //use_dim_window
    storage.put<sint_t>(cur->use_dim_window, record_width.use_dim_window);
    //timeout
    storage.put<sint_t>(cur->timeout, record_width.timeout);
}

int encode_input_mode_configure_file(ResourceStorage& storage, IMetaData_Helper& md_helper) {

    int init_size = storage.get_size();
    /*size 4bytes*/
    const int INPUT_MODE_CONFIGURE_SIZE_WIDTH = 4;


    XMLResource *xmlresource = XMLResource::get_instance();
    PSclInputModeConfigure inputModeConfigureTable = xmlresource->get_input_mode_configure_table();
    int size = xmlresource->get_inputmode_size();
#ifdef __SCL_TXT_DEBUG
    put_input_mode_configure_table(ENCODE, inputModeConfigureTable);

#endif
    PSclInputModeConfigure cur = inputModeConfigureTable;

    //first 8 byte is the size of the block
    storage.reserve(8);
    storage.put<sint_t>(size, INPUT_MODE_CONFIGURE_SIZE_WIDTH);

    Input_mode_configure_width record_width;
    set_input_mode_configure_width(md_helper, record_width);
    for ( int i = 0; i < size; ++i) {
        encode_input_mode_configure_record(storage, cur, record_width);
        cur++;
    }

    int advance_size = storage.get_size() - init_size;
    storage.random_put<sint_t>(advance_size, 8, init_size);
    return storage.get_size();
}

int encode_input_mode_configure_file(const char* file, int& offset, IMetaData_Helper& md_helper) {
    if (!file) return 0;

    ResourceStorage storage;
    encode_input_mode_configure_file(storage, md_helper);
    storage.toFile(file, offset);

    return storage.get_size();
}

int encode_input_mode_configure_file(const char* file, IMetaData_Helper& md_helper) {
    if (!file) return 0;

    ResourceStorage storage;
    encode_input_mode_configure_file(storage, md_helper);
    storage.toFile(file);

    return storage.get_size();
}
