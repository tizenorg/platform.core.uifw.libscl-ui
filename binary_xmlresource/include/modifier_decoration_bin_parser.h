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

#ifndef __BinModifierDecorationParser__H__
#define __BinModifierDecorationParser__H__
#include <libxml/parser.h>
#include "sclres_type.h"
#include "iparserinfo_provider.h"
#include <vector>
#include "string_collector.h"
#include "file_storage_impl.h"
#include "_auto_metadata.h"

#ifndef LIBSCL_EXPORT_API
#define LIBSCL_EXPORT_API 
#endif // LIBSCL_EXPORT_API


class LIBSCL_EXPORT_API BinModifierDecorationParser {
    public:
        ~BinModifierDecorationParser();
        static BinModifierDecorationParser *get_instance();
    void init(const FileStorage& storage, int, int, IParserInfo_Provider*);
    PSclModifierDecoration get_modifier_decoration_table();
    int get_modifier_decoration_id(const char *name);
    private:
    BinModifierDecorationParser();
    void parsing_modifier_decoration_table();

    private:
        SclModifierDecoration m_modifier_decoration_table[MAX_SCL_MODIFIER_DECORATION_NUM];
        IParserInfo_Provider *parser_info_provider;

        StringCollector m_string_collector;
        FileStorage m_storage;
};


#endif
