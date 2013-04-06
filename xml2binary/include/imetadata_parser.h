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

#ifndef __INTERFACE_METADATA_PARSER_H__
#define __INTERFACE_METADATA_PARSER_H__
#include "metadata.h"
class IMetaData_Parser{
    public:
        virtual const MetaData* get_metadata() const= 0;
        virtual ~IMetaData_Parser() {}
};
#endif