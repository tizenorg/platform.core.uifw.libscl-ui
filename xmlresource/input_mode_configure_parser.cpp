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

#include <string.h>
#include <libxml/parser.h>

#include "input_mode_configure_parser.h"
#include "xml_parser_utils.h"
#include "simple_debug.h"

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

class InputModeConfigureParserImpl {
    public:
        InputModeConfigureParserImpl() {
            m_inputmode_size = 0;
            memset(m_input_mode_configure_table, 0x00, sizeof(SclInputModeConfigure) * MAX_SCL_INPUT_MODE);
        }

        ~InputModeConfigureParserImpl() {
            for(int input_mode = 0; input_mode < MAX_SCL_INPUT_MODE; ++input_mode) {
                SclInputModeConfigure& cur_rec = m_input_mode_configure_table[input_mode];
                if (cur_rec.name) {
                    xmlFree(cur_rec.name);
                    cur_rec.name = NULL;
                }
                if (cur_rec.layouts[DISPLAYMODE_PORTRAIT]) {
                    xmlFree(cur_rec.layouts[DISPLAYMODE_PORTRAIT]);
                    cur_rec.layouts[DISPLAYMODE_PORTRAIT] = NULL;
                }
                if (cur_rec.layouts[DISPLAYMODE_LANDSCAPE]) {
                    xmlFree(cur_rec.layouts[DISPLAYMODE_LANDSCAPE]);
                    cur_rec.layouts[DISPLAYMODE_LANDSCAPE] = NULL;
                }
            }
        }

        int parsing_input_mode_configure_table(const char* input_file) {
            xmlDocPtr doc;
            xmlNodePtr cur_node;

            xmlChar* key;

            doc = xmlReadFile(input_file, NULL, 0);
            if (doc == NULL) {
                SCLLOG(SclLog::DEBUG, "Could not load file: %s.", input_file);
                return -1;
            }

            cur_node = xmlDocGetRootElement(doc);
            if (cur_node == NULL) {
                SCLLOG(SclLog::DEBUG, "InputModeConfigParser: empty document.\n");
                xmlFreeDoc(doc);
                return -1;
            }
            if (0 != xmlStrcmp(cur_node->name, (const xmlChar*)INPUT_MODE_CONFIGURE_TABLE_TAG))
            {
                SCLLOG(SclLog::DEBUG, "InputModeConfigParser: root name error: %s\n!", (char *)cur_node->name);
                xmlFreeDoc(doc);
                return -1;
            }

            m_inputmode_size = 0;
            cur_node = cur_node->xmlChildrenNode;

            SclInputModeConfigure* cur_rec = m_input_mode_configure_table;
            while (cur_node != NULL) {
                if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)INPUT_MODE_CONFIGURE_MODE_TAG)) {
                    set_input_mode_configure_default_record(cur_rec);

                    key = xmlGetProp(cur_node, (const xmlChar*)INPUT_MODE_CONFIGURE_MODE_NAME_ATTRIBUTE);
                    if (key) {
                        cur_rec->name = (sclchar*)key;
                    }

                    get_prop_bool(cur_node, INPUT_MODE_CONFIGURE_MODE_DIM_WINDOW_ATTRIBUTE, &(cur_rec->use_dim_window));
                    get_prop_bool(cur_node, INPUT_MODE_CONFIGURE_MODE_VIRTUAL_WINDOW_ATTRIBUTE, &(cur_rec->use_virtual_window));

                    parsing_mode_node(cur_node, cur_rec);
                    m_inputmode_size++;
                    cur_rec++;
                    if (m_inputmode_size >= MAX_SCL_INPUT_MODE) {
                        SCLLOG(SclLog::ERROR, "No Space for input mode record.");
                        break;
                    }
                }
                cur_node = cur_node->next;
            }
            xmlFreeDoc(doc);

            return 0;

        }

        void set_input_mode_configure_default_record(const PSclInputModeConfigure cur_rec) {
            cur_rec->name=NULL;
            cur_rec->layouts[DISPLAYMODE_PORTRAIT] = NULL;
            cur_rec->layouts[DISPLAYMODE_LANDSCAPE] = NULL;
            cur_rec->use_virtual_window = FALSE;
            cur_rec->use_dim_window = FALSE;
        }

        void parsing_mode_node(const xmlNodePtr cur_node, const PSclInputModeConfigure cur_rec) {
            assert(cur_node != NULL);
            assert(cur_rec != NULL);

            sclboolean val;
            get_prop_bool(cur_node, INPUT_MODE_CONFIGURE_MODE_VIRTUAL_WINDOW_ATTRIBUTE, &val);
            get_prop_bool(cur_node, INPUT_MODE_CONFIGURE_MODE_DIM_WINDOW_ATTRIBUTE, &(cur_rec->use_dim_window));

            xmlChar* temp = xmlGetProp(cur_node, (const xmlChar*)INPUT_MODE_CONFIGURE_MODE_NAME_ATTRIBUTE);
            cur_rec->name = (sclchar *)temp;

            xmlNodePtr child_node = cur_node->xmlChildrenNode;
            while (child_node!=NULL) {
                if (0 == xmlStrcmp(child_node->name, (const xmlChar *)"text") ) {
                    child_node = child_node->next;
                    continue;
                }
                if (0 == xmlStrcmp(child_node->name, (const xmlChar *)INPUT_MODE_CONFIGURE_LAYOUT_TAG) ) {
                    parsing_layouts(child_node, cur_rec);
                } else {
                    SCLLOG(SclLog::WARNING, "input_mode_configure has no such node name: %s\n", (char *)child_node->name);
                }

                child_node = child_node->next;
            }
        }

        void parsing_layouts(const xmlNodePtr cur_node, const PSclInputModeConfigure cur_rec) {
            assert(cur_node != NULL);
            assert(cur_rec != NULL);
            assert(0 == xmlStrcmp(cur_node->name, (const xmlChar*)INPUT_MODE_CONFIGURE_LAYOUT_TAG) );

            xmlNodePtr child_node = cur_node->xmlChildrenNode;
            while (child_node != NULL) {
                if ( 0 == xmlStrcmp(child_node->name, (const xmlChar *)INPUT_MODE_CONFIGURE_LAYOUT_PORTRAIT_TAG) ) {
                    xmlChar* temp = xmlNodeGetContent(child_node);
                    cur_rec->layouts[DISPLAYMODE_PORTRAIT] = (sclchar*)temp;
                }
                else if ( 0 == xmlStrcmp(child_node->name, (const xmlChar *)INPUT_MODE_CONFIGURE_LAYOUT_LANDSCAPE_TAG) ) {
                    xmlChar* temp = xmlNodeGetContent(child_node);
                    cur_rec->layouts[DISPLAYMODE_LANDSCAPE] = (sclchar*)temp;
                }
                child_node = child_node->next;
            }
        }

        int m_inputmode_size;
        SclInputModeConfigure m_input_mode_configure_table[MAX_SCL_INPUT_MODE];
};

InputModeConfigParser::InputModeConfigParser() {
    m_impl = new InputModeConfigureParserImpl;
}

InputModeConfigParser::~InputModeConfigParser() {
    if (m_impl) {
        SCLLOG(SclLog::MESSAGE, "~InputModeConfigParser() has called");
        delete m_impl;
        m_impl = NULL;
    }
}

InputModeConfigParser*
InputModeConfigParser::get_instance() {
    static InputModeConfigParser instance;
    return &instance;
}

int
InputModeConfigParser::init(const char* file) {
    return m_impl->parsing_input_mode_configure_table(file);
}

int
InputModeConfigParser::get_inputmode_id(const char *name) {
    if (name == NULL) {
        SCLLOG(SclLog::DEBUG, "get_inputmode_id failed");
        return -1;
    }

    PSclInputModeConfigure config_table = get_input_mode_configure_table();

    if (config_table == NULL) {
        SCLLOG(SclLog::DEBUG, "get_inputmode_id failed");
        return -1;
    }

    for(int i = 0; i < get_inputmode_size(); ++i) {
        if ( config_table[i].name) {
            if ( 0 == strcmp(config_table[i].name, name) ) {
                return i;
            }
        }
    }

    SCLLOG(SclLog::DEBUG, "get_inputmode_id failed");
    return -1;
}

const char*
InputModeConfigParser::get_inputmode_name(int id) {
    if (id >= 0 && id < MAX_SCL_INPUT_MODE) {
        PSclInputModeConfigure config_table = get_input_mode_configure_table();
        if (config_table) {
            return config_table[id].name;
        }
    }

    return NULL;
}

int
InputModeConfigParser::get_inputmode_size() {
    return m_impl->m_inputmode_size;
}

PSclInputModeConfigure
InputModeConfigParser::get_input_mode_configure_table() {
    return m_impl->m_input_mode_configure_table;
}

