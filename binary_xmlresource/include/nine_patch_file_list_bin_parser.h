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

#ifndef __BinNinePatchFileParser__H__
#define __BinNinePatchFileParser__H__
#include "iparserinfo_provider.h"
#include "sclres_type.h"
#include "string_collector.h"
#include "file_storage_impl.h"
#include "_auto_metadata.h"

#ifndef LIBSCL_EXPORT_API
#define LIBSCL_EXPORT_API 
#endif // LIBSCL_EXPORT_API


class LIBSCL_EXPORT_API BinNinePatchFileParser {
    static const int MAX_NINE_PATCH_FILE_LIST = 128;
    public:
        ~BinNinePatchFileParser();
        static BinNinePatchFileParser *get_instance();
    void init(const FileStorage& storage, int, int, IParserInfo_Provider*);
    bool get_nine_patch_info(const char *filename, SclNinePatchInfo *info);
    SclNinePatchInfo* get_nine_patch_list();
    private:
    BinNinePatchFileParser();
    void parsing_nine_patch_file_list();
    private:
        SclNinePatchInfo m_nine_patch_file_list[MAX_NINE_PATCH_FILE_LIST];
        int m_size;
        StringCollector m_string_collector;
        FileStorage m_storage;
        IParserInfo_Provider* parser_info_provider;
};


#endif
