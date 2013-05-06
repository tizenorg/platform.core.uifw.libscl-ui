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

#ifndef __IPARSERINFO_PROVIDER_H__
#define __IPARSERINFO_PROVIDER_H__
#include <string>

/***
* 1. How to get field width, take "input mode configure" for example
    1) set_current_metadata_record("input_mode_configure")
    2) get_width("name");
  2. How to get a string
    1) parsing the binary file to get the string_id
    2) use get_string_by_id(string_id);
*/
#include "istring_provider.h"
class IParserInfo_Provider: public IString_Provider{
    public:
        virtual void set_current_metadata_record(const char* record_name) = 0;
        virtual const int get_width(const char* field_name) const= 0;
        virtual const char* get_string_by_id(const int id) const = 0;
        virtual ~IParserInfo_Provider() {}

};
#endif
