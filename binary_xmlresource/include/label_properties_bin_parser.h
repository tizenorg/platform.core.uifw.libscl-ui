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

#ifndef __Label_properties_bin_Parser__H__
#define __Label_properties_bin_Parser__H__
#include "sclres_type.h"
#include "iparserinfo_provider.h"
#include <vector>
#include "string_collector.h"
#include "file_storage_impl.h"
#include "_auto_metadata.h"

class Label_properties_bin_Parser {
    public:
        ~Label_properties_bin_Parser();
        static Label_properties_bin_Parser *get_instance();
    void init(const FileStorage& storage, int, int, IParserInfo_Provider*);
    PSclLabelPropertiesTable get_label_properties_frame();
    const int get_size();

    private:
    Label_properties_bin_Parser();
    void parsing_label_properties_frame();
    void decode_label_properties_record(const PSclLabelProperties cur, const Label_properties_record_width&);

    void decode_color(SclColor& color, int width);

    private:
        static Label_properties_bin_Parser *m_instance;
        SclLabelProperties m_label_properties_frame[MAX_SCL_LABEL_PROPERTIES][MAX_SIZE_OF_LABEL_FOR_ONE];
        int m_size;
        IParserInfo_Provider *parser_info_provider;
        FileStorage m_storage;
        StringCollector m_string_collector;
    private:
    class DestructHelper {
        public:
        ~DestructHelper() {
            if (Label_properties_bin_Parser::m_instance != NULL)
                delete m_instance;
        }
    };
    static DestructHelper des;
};


#endif
