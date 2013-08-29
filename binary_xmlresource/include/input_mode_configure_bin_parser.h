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

#ifndef __INPUT_MODE_BIN_PARSER__H__
#define __INPUT_MODE_BIN_PARSER__H__
#include <libxml/parser.h>
#include "sclres_type.h"
#include "iparserinfo_provider.h"
#include <vector>
#include "file_storage_impl.h"
#include "string_collector.h"
#include "_auto_metadata.h"

class BinInputModeConfigParser {
    public:
        ~BinInputModeConfigParser();
        static BinInputModeConfigParser *get_instance();
        void init(const FileStorage&, int, int, IParserInfo_Provider* parser_info_provider);
        PSclInputModeConfigure get_input_mode_configure_table();
        int get_inputmode_id(const char *name);
        const char* get_inputmode_name(int id);
        int get_inputmode_size();

        void decode_string(FileStorage &storage, char** p, int width);
    private:
        BinInputModeConfigParser();
        void parsing_input_mode_configure_table();
        void set_input_mode_configure_default_record(const PSclInputModeConfigure);

        StringCollector m_string_collector;
    private:
        int m_inputmode_size;
        SclInputModeConfigure m_input_mode_configure_table[MAX_SCL_INPUT_MODE];

        FileStorage m_storage;
        IParserInfo_Provider* parser_info_provider;
};

#endif
