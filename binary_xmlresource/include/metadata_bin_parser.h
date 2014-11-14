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

#ifndef __METADATA_BIN_PARSER_H__
#define __METADATA_BIN_PARSER_H__
#include "metadata.h"
#include "file_storage_impl.h"
#include "string_collector.h"
class Metadata_Bin_Parser{
    public:
        static const int FIELD_NUM_WIDTH = 2;
        Metadata_Bin_Parser(const FileStorage& storage, int, int);
        ~Metadata_Bin_Parser();
        const MetaData* get_metadata()const{
            return &metadata;
        }
    private:
        void parse();
        void decode_record(MetaData_Record& metadata_record);
        MetaData metadata;
        FileStorage m_storage;
};
#endif
