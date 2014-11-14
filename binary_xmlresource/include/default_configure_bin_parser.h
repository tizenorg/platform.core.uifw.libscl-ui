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

#ifndef __DEFAULT_CONFIGURE_BIN_PARSER__H__
#define __DEFAULT_CONFIGURE_BIN_PARSER__H__
#include "sclres_type.h"
#include "string_collector.h"
#include "iparserinfo_provider.h"
#include "file_storage_impl.h"
#include "_auto_metadata.h"
class BinDefaultConfigParser {
public:
    ~BinDefaultConfigParser();
    static BinDefaultConfigParser *get_instance();
    void init(const FileStorage& storage, int, int, IParserInfo_Provider*);

    PSclDefaultConfigure get_default_configure();


    //These private methods for parsing
private:
    BinDefaultConfigParser();
    void parsing_default_configure();
    void decode_color(SclColor&, int width);
private:
    SclDefaultConfigure m_default_configure;
    StringCollector m_string_collector;
    IParserInfo_Provider* parser_info_provider;
    FileStorage m_storage;
};


#endif
