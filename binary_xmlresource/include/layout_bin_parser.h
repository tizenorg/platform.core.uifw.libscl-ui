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

#ifndef __LAYOUT_BIN_PARSER__H__
#define __LAYOUT_BIN_PARSER__H__
#include "sclres_type.h"
#include "iparserinfo_provider.h"
#include <vector>
#include "string_collector.h"
#include "file_storage_impl.h"
#include "_auto_metadata.h"

#ifndef LIBSCL_EXPORT_API
#define LIBSCL_EXPORT_API 
#endif // LIBSCL_EXPORT_API

class LIBSCL_EXPORT_API BinLayoutParser {
    public:
        static BinLayoutParser *get_instance();
    void init(const FileStorage& storage, int, int, IParserInfo_Provider*);

    int get_layout_index(const char *name);
    int get_layout_size();
    PSclLayout get_layout_table();
    PSclLayoutKeyCoordinatePointerTable get_key_coordinate_pointer_frame();

    ~BinLayoutParser();
    private:
    BinLayoutParser();
    void parsing_layout_table();
    void decode_layout_record(SclLayout& cur, const Layout_width&);
    void decode_color(SclColor&, int width);

    private:
        int m_layout_size;
        SclLayout m_layout_table[MAX_SCL_LAYOUT];

        FileStorage m_storage;
        IParserInfo_Provider *parser_info_provider;
        StringCollector m_string_collector;
};


#endif
