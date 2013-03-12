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

#ifndef __DEFAULT_CONFIGURE_PARSER__H__
#define __DEFAULT_CONFIGURE_PARSER__H__
#include <libxml/parser.h>
#include "sclres_type.h"

typedef SclDefaultConfigure *PSclDefaultConfigure;

class Default_Configure_Parser {
public:
    ~Default_Configure_Parser();
    static Default_Configure_Parser *get_instance();
    void init();

    PSclDefaultConfigure get_default_configure();

    //just for debug
    void print_default_configure_table();

    //These private methods for parsing
private:
    Default_Configure_Parser();
    void parsing_default_configure();
    void parsing_touch_offset(const xmlNodePtr);
    void parsing_dim_color(const xmlNodePtr);

    SCLDisplayMode get_content_display_mode(const xmlNodePtr cur_node);

private:
    static Default_Configure_Parser *m_instance;
    SclDefaultConfigure m_default_configure;

private:
    class DestructHelper {
    public:
        ~DestructHelper() {
            if (Default_Configure_Parser::m_instance != NULL)
                delete m_instance;
        }
    };
    static DestructHelper des;
};


#endif
