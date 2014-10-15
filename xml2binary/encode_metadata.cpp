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

#include "encode_metadata.h"
#include <assert.h>
#include <stdio.h>
#include "metadata.h"
using namespace std;
static const int FIELD_NUM_WIDTH = 2;
static void
encode_metadata_record(ResourceStorage& storage, const MetaData_Record& metadata_record ) {
    storage.put(metadata_record.m_name);
    unsigned short field_num = metadata_record.vField.size();
    storage.put<sint_t>(field_num, FIELD_NUM_WIDTH);

    std::vector<MetaData_Field>::const_iterator it;
    for (it = metadata_record.vField.begin(); it != metadata_record.vField.end(); ++it) {
        storage.put(it->m_name);
        storage.put(it->m_type);
        storage.put<sint_t>(it->m_width, FIELD_SIZE_WIDTH);
    }
}
void
encode_metadata(ResourceStorage& storage, const MetaData& metadata ) {
    int init_size = storage.get_size();

    storage.reserve(8);
    storage.put(metadata.m_version);

    unsigned short size = metadata.m_vec_metadata_record.size();
    storage.put<sint_t>(size, RECORD_SIZE_WIDTH);

    for (int i = 0; i < size; ++i) {
        encode_metadata_record(storage, metadata.m_vec_metadata_record.at(i));
    }

    int block_size = storage.get_size() - init_size;
    storage.random_put<sint_t>(block_size, 8, init_size);
}

