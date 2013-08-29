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

#ifndef __NinePatchFileParser__H__
#define __NinePatchFileParser__H__
#include "sclres_type.h"

const int MAX_NINE_PATCH_FILE_LIST = 128;
class NinePatchFileListParserImpl;

class NinePatchFileParser {
    NinePatchFileListParserImpl *m_impl;
    public:
    int init(const char* file);
    SclNinePatchInfo* get_nine_patch_list();
    bool get_nine_patch_info(const char *filename, SclNinePatchInfo *info);
    public:
    ~NinePatchFileParser();
    static NinePatchFileParser *get_instance();
    private:
    NinePatchFileParser();
};


#endif
