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

#ifndef __DEFAULT_CONFIGURE_PARSER__H__
#define __DEFAULT_CONFIGURE_PARSER__H__
#include "sclres_type.h"

typedef SclDefaultConfigure *PSclDefaultConfigure;

class DefaultConfigureParserImpl;

class DefaultConfigParser {
    DefaultConfigureParserImpl *m_impl;
public:
    int init(const char* file);
    PSclDefaultConfigure get_default_configure();
public:
    ~DefaultConfigParser();
    static DefaultConfigParser *get_instance();
private:
    DefaultConfigParser();
};


#endif
