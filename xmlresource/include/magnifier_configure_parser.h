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

#ifndef __MagnifierConfigParser__H__
#define __MagnifierConfigParser__H__
#include "sclres_type.h"

#ifndef LIBSCL_EXPORT_API
#define LIBSCL_EXPORT_API 
#endif // LIBSCL_EXPORT_API


typedef SclMagnifierWndConfigure *PSclMagnifierWndConfigure;

class MagnifierConfigureParserImpl;

class LIBSCL_EXPORT_API MagnifierConfigParser {
    MagnifierConfigureParserImpl *m_impl;
public:
    int init(const char* file);
    PSclMagnifierWndConfigure get_magnifier_configure();
public:
    ~MagnifierConfigParser();
    static MagnifierConfigParser *get_instance();
private:
    MagnifierConfigParser();
};


#endif
