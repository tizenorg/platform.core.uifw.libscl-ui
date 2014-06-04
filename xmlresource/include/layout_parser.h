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

#ifndef __LAYOUT_PARSER__H__
#define __LAYOUT_PARSER__H__

#include "sclres_type.h"
#include <vector>

class LayoutParserImpl;

class LayoutParser {
    LayoutParserImpl *m_impl;
    public:
    ~LayoutParser();
    static LayoutParser *get_instance();
    int init(const char *dir,
        const std::vector<std::string> &vec_layout_file_name);

    void load(int layout_id);
    void unload();
    bool loaded(int layout_id);

    int get_layout_index(const char *name);
    int get_layout_size();
    PSclLayout get_layout_table();
    PSclLayoutKeyCoordinatePointerTable get_key_coordinate_pointer_frame();

    private:
    LayoutParser();
};


#endif
