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

#ifndef __Magnifier_Configure_Parser__H__
#define __Magnifier_Configure_Parser__H__
#include <libxml/parser.h>
#include "sclres_type.h"

typedef SclMagnifierWndConfigure *PSclMagnifierWndConfigure;

class Magnifier_Configure_Parser {
public:
    ~Magnifier_Configure_Parser();
    static Magnifier_Configure_Parser *get_instance();
    void init();

    PSclMagnifierWndConfigure get_magnifier_configure();
private:
    Magnifier_Configure_Parser();
    void parsing_magnifier_configure();
    void parsing_label_area(const xmlNodePtr);
    void parsing_background_images(const xmlNodePtr);
    void parsing_padding_values(const xmlNodePtr);

private:
    static Magnifier_Configure_Parser *m_instance;
    SclMagnifierWndConfigure m_magnifier_configure;

private:
    class DestructHelper {
    public:
        ~DestructHelper() {
            if (Magnifier_Configure_Parser::m_instance != NULL)
                delete m_instance;
        }
    };
    static DestructHelper des;
};


#endif
