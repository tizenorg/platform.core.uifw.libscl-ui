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

#ifndef __META_FIELD_H__
#define __META_FIELD_H__
//each field:
//1. the name
//2. the width of the field

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <vector>
const int FIELD_SIZE_WIDTH = 2;
class MetaData_Field{
    public:
        MetaData_Field():m_name(NULL), m_type(NULL), m_width(0) {
        }
        public:
        const char* m_name;
        const char* m_type;
        int m_width;
};

const int RECORD_SIZE_WIDTH = 2;
class MetaData_Record{
    public:
        MetaData_Record():m_name(NULL) {}
        const char* m_name;
        std::vector<MetaData_Field> vField;
};

class MetaData{
    public:
        MetaData():m_version(NULL) {}
        const char* m_version;
        std::vector<MetaData_Record> m_vec_metadata_record;
};

#endif
