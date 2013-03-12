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

#include "modifier_decoration_parser.h"
#include "main_entry_parser.h"
#include <memory.h>
#include "xml_parser_utils.h"
static int get_display_state_prop(const char*);
static int get_key_modifier_state_prop(const char*);

Modifier_decoration_Parser* Modifier_decoration_Parser::m_instance = NULL;

Modifier_decoration_Parser::Modifier_decoration_Parser() {
    memset(m_modifier_decoration_table, 0x00, sizeof(SclModifierDecoration) * MAX_SCL_MODIFIER_DECORATION_NUM);
}

Modifier_decoration_Parser::~Modifier_decoration_Parser() {
    for(int i = 0; i < MAX_SCL_MODIFIER_DECORATION_NUM; ++i) {
        SclModifierDecoration& cur_rec = m_modifier_decoration_table[i];
        for (int display_state = 0; display_state < DISPLAYMODE_MAX; ++display_state ) {
            for( int key_modifier_state = 0; key_modifier_state < KEY_MODIFIER_MAX; ++key_modifier_state) {
                if (cur_rec.bg_image_path[display_state][key_modifier_state])
                    delete cur_rec.bg_image_path[display_state][key_modifier_state];
                cur_rec.bg_image_path[display_state][key_modifier_state] = NULL;
            }
        }

    }
}

Modifier_decoration_Parser* Modifier_decoration_Parser::get_instance() {
    if (m_instance == NULL) {
        m_instance = new Modifier_decoration_Parser();
    }
    return m_instance;
}
void Modifier_decoration_Parser::init() {
    parsing_modifier_decoration_table();
}

/* recompute_layout will change the table */
PSclModifierDecoration Modifier_decoration_Parser::get_modifier_decoration_table() {
    return m_modifier_decoration_table;
}

void Modifier_decoration_Parser::parsing_modifier_decoration_table() {
    xmlDocPtr doc;
    xmlNodePtr cur_node;

    char input_file[_POSIX_PATH_MAX] = {0};
    Main_Entry_Parser::get_file_full_path(input_file, "modifier_decoration");

    doc = xmlReadFile(input_file, NULL, 0);
    if (doc == NULL) {
        printf("Could not load file.");
        exit(1);
    }
    cur_node = xmlDocGetRootElement(doc);
    if (cur_node == NULL) {
        printf("empty document.\n");
        xmlFreeDoc(doc);
        exit(1);
    }
    if (0 != xmlStrcmp(cur_node->name, (const xmlChar*)"modifier_decoration_table"))
    {
       printf("root name error!");
       xmlFreeDoc(doc);
       exit(1);
    }

    cur_node = cur_node->xmlChildrenNode;


    SclModifierDecoration* cur_rec = m_modifier_decoration_table;
    while (cur_node != NULL) {
        if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"modifier_decoration_record")) {
            parsing_modifier_decoration_record( cur_node, cur_rec);
            cur_rec++;
        }

        cur_node = cur_node->next;
    }
    xmlFreeDoc(doc);
}

void
Modifier_decoration_Parser::set_modifier_decoration_default_record(const PSclModifierDecoration cur_rec) {
    cur_rec->valid = FALSE;
    cur_rec->extract_background = false;
    cur_rec->name = NULL;
    for (int display_state = 0; display_state < DISPLAYMODE_MAX; ++display_state ) {
        for( int key_modifier_state = 0; key_modifier_state < KEY_MODIFIER_MAX; ++key_modifier_state) {
            cur_rec->bg_image_path[display_state][key_modifier_state] = NULL;
        }
    }
}

void
Modifier_decoration_Parser::parsing_modifier_decoration_record(const xmlNodePtr cur_node, const PSclModifierDecoration cur_rec) {
    assert(cur_node != NULL);
    assert(cur_rec != NULL);

   set_modifier_decoration_default_record(cur_rec);

    xmlNodePtr child_node = cur_node->xmlChildrenNode;

    while (child_node != NULL) {
        if ( 0 == xmlStrcmp(child_node->name, (const xmlChar*)"name")) {
            xmlChar* temp = xmlNodeGetContent(child_node);
            cur_rec->name = (sclchar *)temp;
            cur_rec->valid = TRUE;
        }
        else if ( 0 == xmlStrcmp(child_node->name, (const xmlChar*)"display_part_background")) {
            cur_rec->extract_background = get_content_bool(child_node);
            cur_rec->valid = TRUE;
        }
        else if ( 0 == xmlStrcmp(child_node->name, (const xmlChar*)"background_image_path")) {
            parsing_background_image_record_node(child_node, cur_rec);
            cur_rec->valid = TRUE;
        }
        child_node = child_node->next;
    }
}

void
Modifier_decoration_Parser::parsing_background_image_record_node(const xmlNodePtr cur_node, const PSclModifierDecoration cur_rec) {
    assert(cur_node != NULL);
    assert(cur_rec != NULL);
    assert(0 == xmlStrcmp(cur_node->name, (const xmlChar*)"background_image_path"));
    xmlNodePtr child_node = cur_node->xmlChildrenNode;

    while (child_node != NULL) {
        if (0 == xmlStrcmp(child_node->name, (const xmlChar*)"image") ) {
            /* FIXME */
            int display_state = DISPLAYMODE_PORTRAIT;
            xmlChar* display_state_xml = xmlGetProp(child_node, (const xmlChar*)"display_state");
            if (display_state_xml != NULL) {
                if (0 == xmlStrcmp(display_state_xml, (const xmlChar*)"landscape")) {
                    display_state = DISPLAYMODE_LANDSCAPE;
                }
                xmlFree(display_state_xml);
            }

            int key_modifier_state = KEY_MODIFIER_NONE;
            xmlChar* key_modifier_state_xml = xmlGetProp(child_node, (const xmlChar*)"key_modifier_state");
            if (key_modifier_state_xml != NULL) {
                key_modifier_state = get_key_modifier_state_prop((const char*)key_modifier_state_xml);
                xmlFree(key_modifier_state_xml);
            }

           if (display_state != -1 && key_modifier_state != -1 ) {
                sclchar* key = (sclchar*)xmlNodeGetContent(child_node);
                cur_rec->bg_image_path[display_state][key_modifier_state] = key;
                //Warning:: Donot xmlFree key
            }
        }
        child_node = child_node->next;
    }
}

int
Modifier_decoration_Parser::get_modifier_decoration_id( const char *name )
{
    if (name == NULL) return -1;

    for(int i = 0; i < MAX_SCL_MODIFIER_DECORATION_NUM; ++i) {
        if ( m_modifier_decoration_table[i].name) {
            if ( 0 == strcmp(m_modifier_decoration_table[i].name, name) ) {
                return i;
            }
        }
    }

    return -1;
}

//UTILS
typedef struct _Modifier_decoration_state_match_table{
    int modifier_decoration_state;
    const char* key;
}Modifier_decoration_state_match_table_t;

typedef struct _Key_modifier_state_match_table{
    int key_modifier_state;
    const char* key;
}Key_modifier_state_match_table_t;

static int
get_key_modifier_state_prop(const char* key ) {
  static Key_modifier_state_match_table_t table[] = {
    {KEY_MODIFIER_NONE,                             "none"                          },
    {KEY_MODIFIER_LONGKEY,                          "longkey"                       },
    {KEY_MODIFIER_MULTITAP_START,                   "multitap_start"                },
    {KEY_MODIFIER_MULTITAP_REPEAT,                  "multitap_repeat"               },
    {KEY_MODIFIER_DIRECTION_LEFT,                   "direction_left"                },
    {KEY_MODIFIER_DIRECTION_RIGHT,                  "direction_right"               },
    {KEY_MODIFIER_DIRECTION_UP,                     "direction_up"                  },
    {KEY_MODIFIER_DIRECTION_DOWN,                   "direction_down"                },
    {KEY_MODIFIER_DIRECTION_UP_LEFT,                "direction_up_left"             },
    {KEY_MODIFIER_DIRECTION_UP_RIGHT,               "direction_up_right"            },
    {KEY_MODIFIER_DIRECTION_DOWN_LEFT,              "direction_down_left"           },
    {KEY_MODIFIER_DIRECTION_DOWN_RIGHT,             "direction_down_right"          },

    {KEY_MODIFIER_DIRECTION_LEFT_LONG,              "direction_left_long"           },
    {KEY_MODIFIER_DIRECTION_RIGHT_LONG,             "direction_right_long"          },
    {KEY_MODIFIER_DIRECTION_UP_LONG,                "direction_up_long"             },
    {KEY_MODIFIER_DIRECTION_DOWN_LONG,              "direction_down_long"           },
    {KEY_MODIFIER_DIRECTION_UP_LEFT_LONG,           "direction_up_left_long"        },
    {KEY_MODIFIER_DIRECTION_UP_RIGHT_LONG,          "direction_up_right_long"       },
    {KEY_MODIFIER_DIRECTION_DOWN_LEFT_LONG,         "direction_down_left_long"      },
    {KEY_MODIFIER_DIRECTION_DOWN_RIGHT_LONG,        "direction_down_right_long"     },

    {KEY_MODIFIER_DIRECTION_LEFT_RETURN,            "direction_left_return"         },
    {KEY_MODIFIER_DIRECTION_RIGHT_RETURN,           "direction_right_return"        },
    {KEY_MODIFIER_DIRECTION_UP_RETURN,              "direction_up_return"           },
    {KEY_MODIFIER_DIRECTION_DOWN_RETURN,            "direction_down_return"         },
    {KEY_MODIFIER_DIRECTION_UP_LEFT_RETURN,         "direction_up_left_return"      },
    {KEY_MODIFIER_DIRECTION_UP_RIGHT_RETURN,        "direction_up_right_return"     },
    {KEY_MODIFIER_DIRECTION_DOWN_LEFT_RETURN,       "direction_down_left_return"    },
    {KEY_MODIFIER_DIRECTION_DOWN_RIGHT_RETURN,      "direction_down_right_return"   },

    {KEY_MODIFIER_DIRECTION_CURVE_UP_LEFT,          "direction_curve_up_left"       },
    {KEY_MODIFIER_DIRECTION_CURVE_UP_RIGHT,         "direction_curve_up_right"      },
    {KEY_MODIFIER_DIRECTION_CURVE_DOWN_LEFT,        "direction_curve_down_left"     },
    {KEY_MODIFIER_DIRECTION_CURVE_DOWN_RIGHT,       "direction_curve_down_right"    },
    {KEY_MODIFIER_DIRECTION_CURVE_LEFT_UP,          "direction_curve_left_up"       },
    {KEY_MODIFIER_DIRECTION_CURVE_LEFT_DOWN,        "direction_curve_left_down"     },
    {KEY_MODIFIER_DIRECTION_CURVE_RIGHT_UP,         "direction_curve_right_up"      },
    {KEY_MODIFIER_DIRECTION_CURVE_RIGHT_DOWN,       "direction_curve_right_down"    }
    };

    int key_modifier_state = KEY_MODIFIER_NONE;

    for(int i = 0; i < sizeof(table)/sizeof(Key_modifier_state_match_table_t); ++i) {
        if (0 == strcmp(key, table[i].key) )
        {
            key_modifier_state = table[i].key_modifier_state;
            break;
        }
    }
    return key_modifier_state;
}
