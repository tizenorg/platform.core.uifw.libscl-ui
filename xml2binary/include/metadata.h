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

#ifndef __META_FIELD_H__
#define __META_FIELD_H__
//each field:
//1. the name
//2. the width of the field

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <vector>
const int FIELD_NAME_WIDTH = 128;
const int FIELD_TYPE_WIDTH = 32;
const int FIELD_SIZE_WIDTH = 2;
class MetaData_Field{
    public:
        MetaData_Field():m_width(0) {
            memset(m_name, 0, FIELD_NAME_WIDTH);
            memset(m_type, 0, FIELD_TYPE_WIDTH);
        }
       MetaData_Field(const char*name, const char* type, const int width) {
            assert(name);
            assert(type);
            assert(width >= 0 );

            memset(m_name, 0, FIELD_NAME_WIDTH);
            memset(m_type, 0, FIELD_TYPE_WIDTH);

            strncpy(m_name, name, FIELD_NAME_WIDTH - 1);
            strncpy(m_type, type, FIELD_TYPE_WIDTH -1);

            m_width = width;
        }

        MetaData_Field& operator=(const MetaData_Field& other) {
                if (&other == this) return *this;

                //field name
                memset(m_name, 0, FIELD_NAME_WIDTH);
                strncpy(m_name, other.m_name, FIELD_NAME_WIDTH - 1);
                //field type
                memset(m_type, 0, FIELD_TYPE_WIDTH);
                strncpy(m_type, other.m_name, FIELD_TYPE_WIDTH - 1);
                //field width
                m_width = other.m_width;
                return *this;
            }

        public:
        char m_name[FIELD_NAME_WIDTH];
        char m_type[FIELD_TYPE_WIDTH];
        int m_width;
};

const int RECORD_NAME_WIDTH = 128;
const int RECORD_SIZE_WIDTH = 2;

class MetaData_Record{
    public:
        MetaData_Record() {};
        char m_name[RECORD_NAME_WIDTH];
        std::vector<MetaData_Field> vField;
};

class MetaData{
    public:
        char m_version[RECORD_NAME_WIDTH];
        std::vector<MetaData_Record> m_vec_metadata_record;
};

#endif
