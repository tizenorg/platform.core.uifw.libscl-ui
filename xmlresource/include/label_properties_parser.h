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

#ifndef __Label_properties_Parser__H__
#define __Label_properties_Parser__H__
#include <libxml/parser.h>
#include "sclres_type.h"

class Label_properties_Parser {
    public:
        ~Label_properties_Parser();
        static Label_properties_Parser *get_instance();
    void init();
    PSclLabelPropertiesTable get_label_properties_frame();
    const int get_size();

    private:
    Label_properties_Parser();
    void parsing_label_properties_frame();
    void parsing_label_properties_table(const xmlNodePtr, const PSclLabelPropertiesTable);
    void parsing_label_properties_record(const xmlNodePtr, const PSclLabelProperties);
    void set_label_properties_default_record(const PSclLabelProperties);
    void parsing_font_color_record(const xmlNodePtr cur_node, const PSclLabelProperties cur_rec);
    void parsing_shadow_color_record(const xmlNodePtr cur_node, const PSclLabelProperties cur_rec);
    void parsing_rgb(const xmlNodePtr, SclColor&);
    int get_shift_state_prop(const xmlNodePtr cur_node);
    int get_button_state_prop(const xmlNodePtr cur_node);

    private:
        static Label_properties_Parser *m_instance;
        SclLabelProperties m_label_properties_frame[MAX_SCL_LABEL_PROPERTIES][MAX_SIZE_OF_LABEL_FOR_ONE];
        int m_size;
    private:
    class DestructHelper {
        public:
        ~DestructHelper() {
            if (Label_properties_Parser::m_instance != NULL)
                delete m_instance;
        }
    };
    static DestructHelper des;
};


#endif
