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

#ifndef __INPUT_MODE_PARSER__H__
#define __INPUT_MODE_PARSER__H__
#include <libxml/parser.h>
#include "sclres_type.h"

/** Example of input mode XML file :
 * <?xml version="1.0"?>
 * <input_mode_table>
 *   <mode name="ENGLISH_QTY">
 *       <keyset>PORTRAIT_QTY_DEFAULT</keyset>
 *       <keyset>LANDSCAPE_QTY_DEFAULT</keyset>
 *   </mode>
 *   <mode name="PUNCTUATION_POPUP" dim_window="true">
 *       <keyset>PORTRAIT_PUNCTUATION_POPUP</keyset>
 *       <keyset>LANDSCAPE_PUNCTUATION_POPUP</keyset>
 *   </mode>
 * </input_mode_table>
 */

#define INPUT_MODE_CONFIGURE_TABLE_TAG "input_mode_table"

#define INPUT_MODE_CONFIGURE_MODE_TAG "mode"
#define INPUT_MODE_CONFIGURE_MODE_NAME_ATTRIBUTE "name"
#define INPUT_MODE_CONFIGURE_MODE_DIM_WINDOW_ATTRIBUTE "dim_window"
#define INPUT_MODE_CONFIGURE_MODE_VIRTUAL_WINDOW_ATTRIBUTE "virtual_window"

#define INPUT_MODE_CONFIGURE_LAYOUT_TAG "layouts"
#define INPUT_MODE_CONFIGURE_LAYOUT_PORTRAIT_TAG "portrait"
#define INPUT_MODE_CONFIGURE_LAYOUT_LANDSCAPE_TAG "landscape"

class Input_Mode_Configure_Parser {
    public:
        ~Input_Mode_Configure_Parser();
        static Input_Mode_Configure_Parser *get_instance();
        void init();
        PSclInputModeConfigure get_input_mode_configure_table();
        int get_inputmode_id(const char *name);
        const char* get_inputmode_name(int id);
        int get_inputmode_size();

    private:
        Input_Mode_Configure_Parser();
        void parsing_input_mode_configure_table();
        void set_input_mode_configure_default_record(const PSclInputModeConfigure);
        void parsing_mode_node(const xmlNodePtr, const PSclInputModeConfigure);
        void parsing_layouts(const xmlNodePtr, const PSclInputModeConfigure);

    private:
        static Input_Mode_Configure_Parser *m_instance;
        int m_inputmode_size;
        SclInputModeConfigure m_input_mode_configure_table[MAX_SCL_INPUT_MODE];

    private:
        class DestructHelper {
            public:
                ~DestructHelper() {
                    if (Input_Mode_Configure_Parser::m_instance != NULL)
                        delete m_instance;
                }
        };
        static DestructHelper des;
};
#endif
