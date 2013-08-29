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

#include <memory.h>
#include <libxml/parser.h>
#include "label_properties_parser.h"
#include "xml_parser_utils.h"
#include "simple_debug.h"
#include "put_record.h"
static int
match_alignment(const char* key) {
  assert(key != NULL);

    typedef struct _match_table_t{
        int value;
        const char* key;
    }Match_table_t;
  static Match_table_t table[] = {
        {LABEL_ALIGN_LEFT_TOP,          "left_top"      },
        {LABEL_ALIGN_CENTER_TOP,        "center_top"    },
        {LABEL_ALIGN_RIGHT_TOP,         "right_top"     },

        {LABEL_ALIGN_LEFT_MIDDLE,       "left_middle"   },
        {LABEL_ALIGN_CENTER_MIDDLE,     "center_middle" },
        {LABEL_ALIGN_RIGHT_MIDDLE,      "right_middle"  },

        {LABEL_ALIGN_LEFT_BOTTOM,       "left_bottom"   },
        {LABEL_ALIGN_CENTER_BOTTOM,     "center_bottom" },
        {LABEL_ALIGN_RIGHT_BOTTOM,      "right_bottom"  }
    };

    int value = LABEL_ALIGN_LEFT_TOP;

    for(int i = 0; i < sizeof(table)/sizeof(Match_table_t); ++i) {
        if (0 == strcmp(key, table[i].key))
        {
            value = table[i].value;
            break;
        }
    }
    return value;
}
static int
match_shadow_direction(const char* key) {
  assert(key != NULL);

    typedef struct _match_table_t{
        int value;
        const char* key;
    }Match_table_t;
  static Match_table_t table[] = {
        {SHADOW_DIRECTION_NONE,             "shadow_direction_none"},

        {SHADOW_DIRECTION_LEFT_TOP,         "shadow_direction_left_top"},
        {SHADOW_DIRECTION_CENTER_TOP,       "shadow_direction_center_top"},
        {SHADOW_DIRECTION_RIGHT_TOP,        "shadow_direction_right_top"},

        {SHADOW_DIRECTION_LEFT_MIDDLE,      "shadow_direction_left_middle"},
        {SHADOW_DIRECTION_CENTER_MIDDLE,    "shadow_direction_center_middle"},
        {SHADOW_DIRECTION_RIGHT_MIDDLE,     "shadow_direction_right_middle"},

        {SHADOW_DIRECTION_LEFT_BOTTOM,     "shadow_direction_left_bottom"},
        {SHADOW_DIRECTION_CENTER_BOTTOM,     "shadow_direction_center_bottom"},
        {SHADOW_DIRECTION_RIGHT_BOTTOM,     "shadow_direction_right_bottom"}
    };

    int value = SHADOW_DIRECTION_NONE;

    for(int i = 0; i < sizeof(table)/sizeof(Match_table_t); ++i) {
        if (0 == strcmp(key, table[i].key))
        {
            value = table[i].value;
            break;
        }
    }
    return value;
}
class LabelPropertiesParserImpl {
    public:
        LabelPropertiesParserImpl() {
            m_size = 0;
            memset(m_label_properties_frame, 0, sizeof(SclLabelProperties) * MAX_SCL_LABEL_PROPERTIES * MAX_SIZE_OF_LABEL_FOR_ONE);
        }

        ~LabelPropertiesParserImpl() {
            for(int i = 0; i < MAX_SCL_LABEL_PROPERTIES && i < m_size; ++i) {
                for(int j = 0; j < MAX_SIZE_OF_LABEL_FOR_ONE; ++j) {
                    if (m_label_properties_frame[i][j].font_name)
                        delete m_label_properties_frame[i][j].font_name;
                    m_label_properties_frame[i][j].font_name = NULL;
                    if (m_label_properties_frame[i][j].label_type)
                        delete m_label_properties_frame[i][j].label_type;
                    m_label_properties_frame[i][j].label_type = NULL;
                }
            }
            m_size = 0;

        }

        int parsing_label_properties_frame(const char* input_file) {
            xmlDocPtr doc;
            xmlNodePtr cur_node;

            doc = xmlReadFile(input_file, NULL, 0);
            if (doc == NULL) {
                SCLLOG(SclLog::DEBUG, "Could not load file: %s.", input_file);
                return -1;
            }

            cur_node = xmlDocGetRootElement(doc);
            if (cur_node == NULL) {
                SCLLOG(SclLog::DEBUG, "Label_Properties_Parser: empty document.\n");
                xmlFreeDoc(doc);
                return -1;
            }
            if (0 != xmlStrcmp(cur_node->name, (const xmlChar*)"label_properties_frame"))
            {
                SCLLOG(SclLog::DEBUG, "Label_Properties_Parser: root name error: %s\n!", (char *)cur_node->name);
                xmlFreeDoc(doc);
                return -1;
            }

            cur_node = cur_node->xmlChildrenNode;

            assert(m_size == 0);
            PSclLabelPropertiesTable curTable = m_label_properties_frame;
            while (cur_node != NULL) {
                if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"label_properties_table")) {
                    parsing_label_properties_table( cur_node, curTable);
                    xmlChar* key = xmlGetProp(cur_node, (const xmlChar*)"label_type");
                    PSclLabelProperties cur_rec = (PSclLabelProperties)curTable;
                    cur_rec->label_type = (sclchar *)key;
                    m_size++;
                    curTable++;
                    if (m_size >= MAX_SCL_LABEL_PROPERTIES) {
                        SCLLOG(SclLog::ERROR, "No Space for label properties record.");
                        break;
                    }
                }

                cur_node = cur_node->next;
            }
            xmlFreeDoc(doc);

            return 0;

        }
        void parsing_label_properties_table(const xmlNodePtr cur_node, const PSclLabelPropertiesTable curTable) {
            assert(cur_node != NULL);
            assert(curTable != NULL);

            xmlNodePtr child_node = cur_node->xmlChildrenNode;

            PSclLabelProperties cur_rec = (PSclLabelProperties)curTable;
            while (NULL != child_node) {
                if (0 == xmlStrcmp(child_node->name, (const xmlChar*)"label_properties")) {
                    parsing_label_properties_record(child_node, cur_rec);
                    cur_rec++;
                }

                child_node = child_node->next;
            }

        }
        void parsing_label_properties_record(const xmlNodePtr cur_node, const PSclLabelProperties cur_rec) {
            assert(cur_node != NULL);
            assert(cur_rec != NULL);

            set_label_properties_default_record(cur_rec);

            xmlChar* key = xmlGetProp(cur_node, (const xmlChar*)"font_name");
            if (NULL != key) {
                cur_rec->font_name = (sclchar*)key;
            }

            get_prop_number(cur_node, "font_size", &(cur_rec->font_size));

            key = xmlGetProp(cur_node, (const xmlChar*)"label_alignment");
            if (NULL != key) {
                cur_rec->alignment = (SCLLabelAlignment)match_alignment((const char*)key);
                xmlFree(key);
            }

            get_prop_number(cur_node, "padding_X", &(cur_rec->padding_x));
            get_prop_number(cur_node, "padding_Y", &(cur_rec->padding_y));
            get_prop_number(cur_node, "inner_width", &(cur_rec->inner_width));
            get_prop_number(cur_node, "inner_height", &(cur_rec->inner_height));

            get_prop_number(cur_node, "shadow_distance", &(cur_rec->shadow_distance));
            key = xmlGetProp(cur_node, (const xmlChar*)"shadow_direction");
            if (NULL != key) {
                cur_rec->shadow_direction = (SCLShadowDirection)match_shadow_direction((const char*)key);
                xmlFree(key);
            }

            xmlNodePtr child_node = cur_node->xmlChildrenNode;

            while (child_node != NULL) {
                if ( 0 == xmlStrcmp(child_node->name, (const xmlChar*)"font_color_record")) {
                    parsing_font_color_record(child_node, cur_rec);
                } else if ( 0 == xmlStrcmp(child_node->name, (const xmlChar*)"shadow_color_record")) {
                    parsing_shadow_color_record(child_node, cur_rec);
                }

                child_node = child_node->next;
            }
        }

        void set_label_properties_default_record(const PSclLabelProperties cur_rec) {
        cur_rec->valid = true;
        cur_rec->font_name = NULL;
        cur_rec->font_size = 0;
        for(int shift_state = 0; shift_state < SCL_SHIFT_STATE_MAX; ++shift_state) {
            for(int button_state = 0; button_state < SCL_BUTTON_STATE_MAX; ++button_state) {
                cur_rec->font_color[shift_state][button_state].r = 0;
                cur_rec->font_color[shift_state][button_state].g = 0;
                cur_rec->font_color[shift_state][button_state].b = 0;
            }
        }

        cur_rec->alignment = LABEL_ALIGN_LEFT_TOP;
        cur_rec->padding_x = 0;
        cur_rec->padding_y = 0;
        cur_rec->inner_width = 0;
        cur_rec->inner_height = 0;
        cur_rec->shadow_distance = 0;

        cur_rec->shadow_direction = SHADOW_DIRECTION_NONE;

        for(int shift_state = 0; shift_state < SCL_SHIFT_STATE_MAX; ++shift_state) {
            for(int button_state = 0; button_state < SCL_BUTTON_STATE_MAX; ++button_state) {
                cur_rec->shadow_color[shift_state][button_state].r = 0;
                cur_rec->shadow_color[shift_state][button_state].g = 0;
                cur_rec->shadow_color[shift_state][button_state].b = 0;
            }
        }

    }

    void parsing_font_color_record(const xmlNodePtr cur_node, const PSclLabelProperties cur_rec) {
        assert(cur_node != NULL);
        assert(cur_rec != NULL);
        assert(0 == xmlStrcmp(cur_node->name, (const xmlChar*)"font_color_record"));
        xmlNodePtr child_node = cur_node->xmlChildrenNode;

        while (child_node != NULL) {
            if (0 == xmlStrcmp(child_node->name, (const xmlChar*)"color") ) {
                SclColor font_color = {0x00, 0x00, 0x00, 0xFF};
                parsing_rgb(child_node, font_color);

                int shift_state = get_shift_state_prop(child_node);
                int button_state = get_button_state_prop(child_node);

                for(int shift_loop = 0;shift_loop < SCL_SHIFT_STATE_MAX;shift_loop++) {
                    for(int button_loop = 0;button_loop < SCL_BUTTON_STATE_MAX;button_loop++) {
                        if ((shift_state == shift_loop || shift_state == -1) &&
                                (button_state == button_loop || button_state == -1)) {
                            cur_rec->font_color[shift_loop][button_loop] = font_color;
                        }
                    }
                }
            }
            child_node = child_node->next;
        }
    }

    void parsing_shadow_color_record(const xmlNodePtr cur_node, const PSclLabelProperties cur_rec) {
        assert(cur_node != NULL);
        assert(cur_rec != NULL);
        assert(0 == xmlStrcmp(cur_node->name, (const xmlChar*)"shadow_color_record"));
        xmlNodePtr child_node = cur_node->xmlChildrenNode;

        while (child_node != NULL) {
            if (0 == xmlStrcmp(child_node->name, (const xmlChar*)"color") ) {
                int shift_state = get_shift_state_prop(child_node);
                int button_state = get_button_state_prop(child_node);
                if (shift_state != -1 && button_state != -1 ) {
                    parsing_rgb(child_node, cur_rec->shadow_color[shift_state][button_state]);
                }
            }
            child_node = child_node->next;
        }
    }

    void parsing_rgb(const xmlNodePtr cur_node, SclColor& cur_color) {
        assert(cur_node != NULL);

        assert(0 == xmlStrcmp(cur_node->name, (const xmlChar*)"color"));
        xmlNodePtr child_node = cur_node->xmlChildrenNode;

        if (child_node) {
            /* FIXME : A hardcoded routine for parsing 0xRRGGBBAA style string */
            xmlChar* key = xmlNodeGetContent(child_node);
            if (key) {
                if (xmlStrlen(key) == 10) {
                    int r, g, b, a;
                    r = g = b = 0;
                    a = 0xff;

                    char temp_string[5] = {"0xFF"};
                    temp_string[2] = key[2];
                    temp_string[3] = key[3];
                    if (sscanf(temp_string, "%x", &r) <= 0) {
                        SCLLOG(SclLog::ERROR, "parsing_rgb() has failed.");
                    }

                    temp_string[2] = key[4];
                    temp_string[3] = key[5];
                    if (sscanf(temp_string, "%x", &g) <= 0) {
                        SCLLOG(SclLog::ERROR, "parsing_rgb() has failed.");
                    }
                    temp_string[2] = key[6];
                    temp_string[3] = key[7];
                    if (sscanf(temp_string, "%x", &b) <= 0) {
                        SCLLOG(SclLog::ERROR, "parsing_rgb() has failed.");
                    }

                    temp_string[2] = key[8];
                    temp_string[3] = key[9];
                    if (sscanf(temp_string, "%x", &a) <= 0) {
                        SCLLOG(SclLog::ERROR, "parsing_rgb() has failed.");
                    }

                    cur_color.r = r;
                    cur_color.g = g;
                    cur_color.b = b;
                    cur_color.a = a;
                }
            }
            xmlFree(key);
        }
    }

    int get_shift_state_prop(const xmlNodePtr cur_node) {
        assert(cur_node != NULL);

        int shift_state = -1;

        if (equal_prop(cur_node, "shift", "on")) {
            shift_state = SCL_SHIFT_STATE_ON;
        } else if (equal_prop(cur_node, "shift", "off")) {
            shift_state = SCL_SHIFT_STATE_OFF;

        } else if (equal_prop(cur_node, "shift", "loc")) {
            shift_state = SCL_SHIFT_STATE_LOCK;
        }
        return shift_state;
    }

    int get_button_state_prop(const xmlNodePtr cur_node) {
        assert(cur_node != NULL);
        int button_state = -1;

        if (equal_prop(cur_node, "button", "pressed")) {
            button_state = BUTTON_STATE_PRESSED;
        } else if (equal_prop(cur_node, "button", "normal")) {
            button_state = BUTTON_STATE_NORMAL;
        }
        else if (equal_prop(cur_node, "button", "disabled")) {
            button_state = BUTTON_STATE_DISABLED;
        }
        return button_state;
    }

    SclLabelProperties m_label_properties_frame[MAX_SCL_LABEL_PROPERTIES][MAX_SIZE_OF_LABEL_FOR_ONE];
    int m_size;
};

LabelPropertyParser::LabelPropertyParser() {
    m_impl = new LabelPropertiesParserImpl;
}

LabelPropertyParser::~LabelPropertyParser() {
    if (m_impl) {
        SCLLOG(SclLog::MESSAGE, "~LabelPropertyParser() has called");
        delete m_impl;
        m_impl = NULL;
    }
}

LabelPropertyParser*
LabelPropertyParser::get_instance() {
    static LabelPropertyParser instance;
    return &instance;
}

int
LabelPropertyParser::init(const char* file) {
    return m_impl->parsing_label_properties_frame(file);
}

int
LabelPropertyParser::get_size() {
    return m_impl->m_size;
}

//recompute_layout will change the table
PSclLabelPropertiesTable
LabelPropertyParser::get_label_properties_frame() {
    return m_impl->m_label_properties_frame;
}
