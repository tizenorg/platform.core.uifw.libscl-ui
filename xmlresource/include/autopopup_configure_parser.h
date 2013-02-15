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

#ifndef __AutoPopup_Configure_Parser__H__
#define __AutoPopup_Configure_Parser__H__
#include <libxml/parser.h>
#include "sclres_type.h"

typedef SclAutoPopupConfigure *PSclAutoPopupConfigure;

class AutoPopup_Configure_Parser {
public:
    ~AutoPopup_Configure_Parser();
    static AutoPopup_Configure_Parser *get_instance();
    void init();

    PSclAutoPopupConfigure get_autopopup_configure();

    //just for debug
    void print_default_configure_table();

    //These private methods for parsing
private:
    AutoPopup_Configure_Parser();
    void parsing_autopopup_configure();
    void parsing_background_color(const xmlNodePtr);
    void parsing_background_line_color(const xmlNodePtr);
    void parsing_button_image_path(const xmlNodePtr);
    void parsing_button_size(const xmlNodePtr);
    void parsing_window_decorator_iamge_path(const xmlNodePtr);
    void parsing_grab_area(const xmlNodePtr);

    int get_button_state_prop(const xmlNodePtr cur_node);
    SCLDisplayMode get_content_displaymode(const xmlNodePtr cur_node);

private:
    static AutoPopup_Configure_Parser *m_instance;
    SclAutoPopupConfigure m_autopopup_configure;

private:
    class DestructHelper {
    public:
        ~DestructHelper() {
            if (AutoPopup_Configure_Parser::m_instance != NULL)
                delete m_instance;
        }
    };
    static DestructHelper des;
};


#endif
