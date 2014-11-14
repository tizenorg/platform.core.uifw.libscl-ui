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

#ifndef __BinLabelPropertyParser__H__
#define __BinLabelPropertyParser__H__
#include "sclres_type.h"
#include "iparserinfo_provider.h"
#include <vector>
#include "string_collector.h"
#include "file_storage_impl.h"
#include "_auto_metadata.h"

class BinLabelPropertyParser {
    public:
        ~BinLabelPropertyParser();
        static BinLabelPropertyParser *get_instance();
    void init(const FileStorage& storage, int, int, IParserInfo_Provider*);
    PSclLabelPropertiesTable get_label_properties_frame();
    const int get_size();

    private:
    BinLabelPropertyParser();
    void parsing_label_properties_frame();
    void decode_label_properties_record(const PSclLabelProperties cur, const Label_properties_record_width&);

    void decode_color(SclColor& color, int width);

    private:
        SclLabelProperties m_label_properties_frame[MAX_SCL_LABEL_PROPERTIES][MAX_SIZE_OF_LABEL_FOR_ONE];
        int m_size;
        IParserInfo_Provider *parser_info_provider;
        FileStorage m_storage;
        StringCollector m_string_collector;
};


#endif
