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

#include "encode_nine_patch.h"
#include <string>
#include "xmlresource.h"
#include "imetadata_helper.h"
#include "resource_storage_impl.h"
#include "put_record.h"
#include "_auto_metadata.h"
using namespace std;
using namespace xmlresource;
static void
encode_nine_patch_info(ResourceStorage& storage, const SclNinePatchInfo* cur, const Nine_patch_width& record_width) {
    assert(cur != NULL);
    storage.encode_string(cur->image_path, record_width.image_path);
    //left
    storage.put<sint_t>(cur->left, record_width.left);
    //right
    storage.put<sint_t>(cur->right, record_width.right);
    //top
    storage.put<sint_t>(cur->top, record_width.top);
    //bottom
    storage.put<sint_t>(cur->bottom, record_width.bottom);
}

int
encode_nine_patch_file(ResourceStorage& storage, IMetaData_Helper& md_helper) {

    int init_size = storage.get_size();

    XMLResource *xmlresource = XMLResource::get_instance();
    if (xmlresource == NULL) {
        printf("Error: failed to get xmlresource instance.\n");
        return 0;
    }
    SclNinePatchInfo* ninePatchInfoTable = xmlresource->get_nine_patch_list();
    if (ninePatchInfoTable == NULL) {
        printf("Warning: nine patch list is null\n");
        return 0;
    }
#ifdef __SCL_TXT_DEBUG
    put_nine_patch_info(ENCODE, ninePatchInfoTable);

#endif
    storage.reserve(8);
    storage.put<sint_t>(MAX_NINE_PATCH_FILE_LIST, 4);

    Nine_patch_width record_width;
    set_nine_patch_width(md_helper, record_width);
    SclNinePatchInfo* cur = ninePatchInfoTable;
    for (int i = 0; i < MAX_NINE_PATCH_FILE_LIST; ++i ) {
        encode_nine_patch_info(storage, cur, record_width);
        cur++;
    }

    int size = storage.get_size() - init_size;
    storage.random_put<sint_t>(size, 8, init_size);

    return storage.get_size();
}

int
encode_nine_patch_file(const char* file, IMetaData_Helper& md_helper) {
    if (!file) return 0;

    ResourceStorage storage;
    encode_nine_patch_file(storage, md_helper);
    storage.toFile(file);

    return storage.get_size();
}

int
encode_nine_patch_file(const char* file, int& offset,  IMetaData_Helper& md_helper) {
    if (!file) return 0;

    ResourceStorage storage;
    encode_nine_patch_file(storage, md_helper);
    storage.toFile(file, offset);

    return storage.get_size();
}
