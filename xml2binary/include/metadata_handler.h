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

#ifndef __METADATA_HANDLER_H__
#define __METADATA_HANDLER_H__
#include <stdio.h>
#include <libxml/parser.h>
#include <string.h>
#include "encode_metadata.h"
#include "imetadata_encoder.h"
#include "imetadata_parser.h"
#include "imetadata_helper.h"
class MetaData_Handler:private IMetaData_Encoder, IMetaData_Parser, public IMetaData_Helper{
    private:
        typedef struct __Metadata_Width{
            int string_id_width;
        }MetaData_Width;
    public:
        MetaData_Handler(const char* file);
        void set_current_metadata_record(const char* record_name);
        //encode all strings and output
        // bin file is assigned by parameter "file"
        int encode(const char* file)const;

        //encode all strings and output
        // bin file is assigned by parameter "file"
        int encode(const char* file, int&offset)const;

        // encode all strings and output to storage
        int encode(ResourceStorage& storage)const;

        const unsigned short get_width(const char* field_name) const;
    private:
        const unsigned short
            get_width(const char* name, const MetaData_Record& metadata_record) const;
        const int find_metadata_record_index(const char* name)const;
        int parsing_field(const xmlNodePtr node, MetaData_Field& data, const MetaData_Width& metadataWidth);
        int parsing_record(const xmlNodePtr curNode, MetaData_Record& metadataRecord, const MetaData_Width& metadataWidth);
        int parsing_metadata_type(const xmlNodePtr curNode, MetaData_Width& metadataWidth);
        void parsing_metadata();
        const MetaData* get_metadata()const;
    private:
        MetaData metadata;
        int m_current_metadata_record_id;
        const char* file_name;
};
#endif
