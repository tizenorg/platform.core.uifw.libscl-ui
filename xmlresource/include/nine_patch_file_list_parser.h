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

#ifndef __Nine_patch_file_list_Parser__H__
#define __Nine_patch_file_list_Parser__H__
#include <libxml/parser.h>
#include "sclres_type.h"

const int MAX_NINE_PATCH_FILE_LIST = 128;

class Nine_patch_file_list_Parser {
    public:
        ~Nine_patch_file_list_Parser();
        static Nine_patch_file_list_Parser *get_instance();
    void init();
    SclNinePatchInfo* get_nine_patch_list();
    bool get_nine_patch_info(const char *filename, SclNinePatchInfo *info);
    private:
    Nine_patch_file_list_Parser();
    void parsing_nine_patch_file_list();
    private:
        static Nine_patch_file_list_Parser *m_instance;
        SclNinePatchInfo m_nine_patch_file_list[MAX_NINE_PATCH_FILE_LIST];
        int m_size;
    private:
    class DestructHelper {
        public:
        ~DestructHelper() {
            if (Nine_patch_file_list_Parser::m_instance != NULL)
                delete m_instance;
        }
    };
    static DestructHelper des;
};


#endif
