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

#include "encode_modifier_decoration.h"
#include "xmlresource.h"
#include "resource_storage_impl.h"
#include "put_record.h"
#include "_auto_metadata.h"
using namespace xmlresource;
static void
encode_modifier_decoration_record(ResourceStorage& storage, const PSclModifierDecoration cur, const Modifier_decoration_width& record_width) {
    storage.put<sint_t>(cur->extract_background, record_width.extract_background);
    storage.encode_string(cur->name, record_width.name);
    for (int i = 0; i < DISPLAYMODE_MAX; ++i) {
        for (int j = 0; j < KEY_MODIFIER_MAX; ++j) {
            storage.encode_string(cur->bg_image_path[i][j], record_width.bg_image_path);
        }
    }
}

int
encode_modifier_decoration_file(ResourceStorage& storage, IMetaData_Helper& md_helper) {

    int init_size = storage.get_size();

    XMLResource *xmlresource = XMLResource::get_instance();
    PSclModifierDecoration modifierDecorationTable = xmlresource->get_modifier_decoration_table();
    if (modifierDecorationTable == NULL) {
        printf("Error. modifier decoration table is NULL\n");
        return 0;
    }

#ifdef __SCL_TXT_DEBUG
    put_modifier_decoration(ENCODE, modifierDecorationTable);

#endif
    Modifier_decoration_width record_width;
    set_modifier_decoration_width(md_helper, record_width);

    storage.reserve(8);

    storage.put<sint_t>(MAX_SCL_MODIFIER_DECORATION_NUM, 4);
    PSclModifierDecoration cur = modifierDecorationTable;
    for (int i = 0; i < MAX_SCL_MODIFIER_DECORATION_NUM; ++i) {
        encode_modifier_decoration_record(storage, cur, record_width);
        cur++;
    }

    int advance_size = storage.get_size() - init_size;
    storage.random_put<sint_t>(advance_size, 8, init_size);

    return storage.get_size();
}

int
encode_modifier_decoration_file(const char* file, IMetaData_Helper& md_helper) {
    if (!file) return 0;

    ResourceStorage storage;

    encode_modifier_decoration_file(storage, md_helper);
    storage.toFile(file);

    return storage.get_size();
}

int
encode_modifier_decoration_file(const char* file, int& offset, IMetaData_Helper& md_helper) {
    if (!file) return 0;

    ResourceStorage storage;

    encode_modifier_decoration_file(storage, md_helper);
    storage.toFile(file, offset);

    return storage.get_size();
}
