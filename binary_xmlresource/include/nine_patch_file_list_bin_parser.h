/*
 * Copyright 2012-2013 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Flora License, Version 1.0 (the "License");
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

#ifndef __Nine_patch_file_list_bin_Parser__H__
#define __Nine_patch_file_list_bin_Parser__H__
#include "iparserinfo_provider.h"
#include "sclres_type.h"
#include "string_collector.h"
#include "file_storage_impl.h"
#include "_auto_metadata.h"

class Nine_patch_file_list_bin_Parser {
    static const int MAX_NINE_PATCH_FILE_LIST = 128;
    public:
        ~Nine_patch_file_list_bin_Parser();
        static Nine_patch_file_list_bin_Parser *get_instance();
    void init(const FileStorage& storage, int, int, IParserInfo_Provider*);
    bool get_nine_patch_info(const char *filename, SclNinePatchInfo *info);
    SclNinePatchInfo* get_nine_patch_list();
    private:
    Nine_patch_file_list_bin_Parser();
    void parsing_nine_patch_file_list();
    private:
        static Nine_patch_file_list_bin_Parser *m_instance;
        SclNinePatchInfo m_nine_patch_file_list[MAX_NINE_PATCH_FILE_LIST];
        int m_size;
        StringCollector m_string_collector;
        FileStorage m_storage;
        IParserInfo_Provider* parser_info_provider;
    private:
    class DestructHelper {
        public:
        ~DestructHelper() {
            if (Nine_patch_file_list_bin_Parser::m_instance != NULL)
                delete m_instance;
        }
    };
    static DestructHelper des;
};


#endif
