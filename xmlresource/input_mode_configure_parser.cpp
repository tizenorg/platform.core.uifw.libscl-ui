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

#include "input_mode_configure_parser.h"
#include "main_entry_parser.h"
#include "xml_parser_utils.h"
#include "simple_debug.h"

Input_Mode_Configure_Parser* Input_Mode_Configure_Parser::m_instance = NULL;

Input_Mode_Configure_Parser::Input_Mode_Configure_Parser() {
    m_inputmode_size = 0;
    memset(m_input_mode_configure_table, 0x00, sizeof(SclInputModeConfigure) * MAX_SCL_INPUT_MODE);
}

Input_Mode_Configure_Parser::~Input_Mode_Configure_Parser() {
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

Input_Mode_Configure_Parser*
Input_Mode_Configure_Parser::get_instance() {
    if (m_instance == NULL) {
        m_instance = new Input_Mode_Configure_Parser();
    }
    return m_instance;
}

void
Input_Mode_Configure_Parser::init() {
    parsing_input_mode_configure_table();
}


void
Input_Mode_Configure_Parser::parsing_layouts( const xmlNodePtr cur_node, const PSclInputModeConfigure cur_rec) {
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

int
Input_Mode_Configure_Parser::get_inputmode_id(const char *name) {
    if (name == NULL) return -1;

    for(int i = 0; i < MAX_SCL_INPUT_MODE; ++i) {
        if ( m_input_mode_configure_table[i].name) {
            if ( 0 == strcmp(m_input_mode_configure_table[i].name, name) ) {
                return i;
            }
        }
    }

    return -1;
}

const char*
Input_Mode_Configure_Parser::get_inputmode_name(int id) {
    if (id >= 0 && id < MAX_SCL_INPUT_MODE) {
        return m_input_mode_configure_table[id].name;
    }

    return NULL;
}

int
Input_Mode_Configure_Parser::get_inputmode_size() {
    return m_inputmode_size;
}

void
Input_Mode_Configure_Parser::set_input_mode_configure_default_record(const PSclInputModeConfigure cur_rec) {
    cur_rec->name=NULL;
    cur_rec->layouts[DISPLAYMODE_PORTRAIT] = NULL;
    cur_rec->layouts[DISPLAYMODE_LANDSCAPE] = NULL;
    cur_rec->use_virtual_window = FALSE;
    cur_rec->use_dim_window = FALSE;
}

void
Input_Mode_Configure_Parser::parsing_mode_node(const xmlNodePtr cur_node, const PSclInputModeConfigure cur_rec) {
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
            LOG("Warning: input_mode_configure.xml has no such attribute.\n");
        }

        child_node = child_node->next;
    }
}

void
Input_Mode_Configure_Parser::parsing_input_mode_configure_table() {
    xmlDocPtr doc;
    xmlNodePtr cur_node;

    xmlChar* key;

    char input_file[_POSIX_PATH_MAX] = {0};
    Main_Entry_Parser::get_file_full_path(input_file, "input_mode_configure");

    doc = xmlReadFile(input_file, NULL, 0);
    if (doc == NULL) {
        printf("Could not load file.\n");
        exit(1);
    }

    cur_node = xmlDocGetRootElement(doc);
    if (cur_node == NULL) {
        printf("empty document.\n");
        xmlFreeDoc(doc);
        exit(1);
    }
    if (0 != xmlStrcmp(cur_node->name, (const xmlChar*)INPUT_MODE_CONFIGURE_TABLE_TAG))
    {
       printf("root name %s error!\n", cur_node->name);
       xmlFreeDoc(doc);
       exit(1);
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
        }
        cur_node = cur_node->next;
    }
    xmlFreeDoc(doc);
}

PSclInputModeConfigure Input_Mode_Configure_Parser::get_input_mode_configure_table() {
    return m_input_mode_configure_table;
}

