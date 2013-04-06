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

#ifndef __INTERFACE_METADATA_HELPER_H__
#define __INTERFACE_METADATA_HELPER_H__
class IMetaData_Helper{
    public:
        virtual void set_current_metadata_record(const char* record_name) = 0;
        virtual const unsigned short get_width(const char* field_name) const= 0;
        virtual ~IMetaData_Helper() {}
};
#endif