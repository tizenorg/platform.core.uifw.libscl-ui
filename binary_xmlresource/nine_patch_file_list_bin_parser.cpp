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

#include "nine_patch_file_list_bin_parser.h"
#include <memory.h>
#include <assert.h>
#include "put_record.h"

BinNinePatchFileParser::BinNinePatchFileParser() {
    m_size = 0;
    memset(m_nine_patch_file_list, 0x00, sizeof(SclNinePatchInfo) * MAX_NINE_PATCH_FILE_LIST);
}

BinNinePatchFileParser::~BinNinePatchFileParser() {
    m_size = 0;
}
BinNinePatchFileParser* BinNinePatchFileParser::get_instance() {
    static BinNinePatchFileParser instance;
    return &instance;
}
void BinNinePatchFileParser::init(const FileStorage& storage, int offset, int size, IParserInfo_Provider* parser_info_provider) {
    m_storage.set_str_provider(parser_info_provider);
    m_storage.get_storage(storage, offset, size);
    this->parser_info_provider = parser_info_provider;

    parsing_nine_patch_file_list();
}

void BinNinePatchFileParser::parsing_nine_patch_file_list() {

    Nine_patch_width record_width;
    set_nine_patch_width(*parser_info_provider, record_width);

    // skip data_size
    m_storage.advance(8);

    SclNinePatchInfo* cur = m_nine_patch_file_list;
    m_size = m_storage.get<sint_t>(4);
    for (int i = 0; i < m_size; ++i) {
        char* temp = NULL;
        m_storage.get_str(&(temp), record_width.image_path, m_string_collector);
        cur->image_path = (const char*)temp;

        cur->left = m_storage.get<sint_t>(record_width.left);
        cur->right = m_storage.get<sint_t>(record_width.right);
        cur->top = m_storage.get<sint_t>(record_width.top);
        cur->bottom = m_storage.get<sint_t>(record_width.bottom);
        cur++;
    }
#ifdef __SCL_TXT_DEBUG
    put_nine_patch_info(DECODE, m_nine_patch_file_list);
#endif
}

SclNinePatchInfo*
BinNinePatchFileParser::get_nine_patch_list() {
    return m_nine_patch_file_list;
}
bool
BinNinePatchFileParser::get_nine_patch_info(const char* filename, SclNinePatchInfo *info) {
    if (filename == NULL) return false;

    for(int i = 0; i < MAX_NINE_PATCH_FILE_LIST && i < m_size; ++i) {
        if ( m_nine_patch_file_list[i].image_path != NULL &&
            0 == strcmp(m_nine_patch_file_list[i].image_path, filename) ) {
                if (info) {
                    *info = m_nine_patch_file_list[i];
                }
            return true;
        }
    }

    return false;

}
