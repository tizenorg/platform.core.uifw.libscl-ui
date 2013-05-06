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

#include "metadata_bin_parser.h"
Metadata_Bin_Parser::Metadata_Bin_Parser(const FileStorage& storage, int offset, int size) {
    m_storage.get_storage(storage, offset, size);
    parse();
}
Metadata_Bin_Parser::~Metadata_Bin_Parser() {
}

void
Metadata_Bin_Parser::parse() {
    // skip block_size
    m_storage.advance(8);

    metadata.m_version = m_storage.get_str();

    unsigned short num = m_storage.get<sint_t>(RECORD_SIZE_WIDTH);
    for (int i = 0; i < num; ++i) {
        MetaData_Record temp;
        decode_record(temp);
        metadata.m_vec_metadata_record.push_back(temp);
    }
}
void Metadata_Bin_Parser::decode_record(MetaData_Record& metadata_record) {
    metadata_record.m_name = m_storage.get_str();
    unsigned short field_num = 0;
    field_num = m_storage.get<sint_t>(FIELD_NUM_WIDTH);
    for (int i = 0; i < field_num; ++i) {
        MetaData_Field temp;
        temp.m_name = m_storage.get_str();
        temp.m_type = m_storage.get_str();
        temp.m_width = m_storage.get<sint_t>(FIELD_SIZE_WIDTH);
        metadata_record.vField.push_back(temp);
    }
}

