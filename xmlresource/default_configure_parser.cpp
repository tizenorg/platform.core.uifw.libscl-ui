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

#include "default_configure_parser.h"
#include "main_entry_parser.h"
#include <assert.h>
#include "xml_parser_utils.h"
using namespace std;

Default_Configure_Parser* Default_Configure_Parser::m_instance = NULL;

Default_Configure_Parser::Default_Configure_Parser() {
    memset((void*)&m_default_configure, 0x00, sizeof(SclDefaultConfigure));
}

Default_Configure_Parser::~Default_Configure_Parser() {
    /* Let's create de-initializing function for this resource releasement */
    if (m_default_configure.input_mode) {
        xmlFree(m_default_configure.input_mode);
        m_default_configure.input_mode = NULL;
    }
    if (m_default_configure.input_mode) {
        xmlFree(m_default_configure.input_mode);
        m_default_configure.input_mode = NULL;
    }
    if (m_default_configure.image_file_base_path) {
        xmlFree(m_default_configure.image_file_base_path);
        m_default_configure.image_file_base_path = NULL;
    }
}

Default_Configure_Parser* Default_Configure_Parser::get_instance() {
    if (m_instance == NULL) {
        m_instance = new Default_Configure_Parser();
    }
    return m_instance;
}

void Default_Configure_Parser::init() {
    parsing_default_configure();
}

void Default_Configure_Parser::parsing_touch_offset(const xmlNodePtr cur_node) {
    assert(cur_node != NULL);

    xmlNodePtr child_node = cur_node->xmlChildrenNode;
    while (child_node!=NULL) {
        if (0 == xmlStrcmp(child_node->name, (const xmlChar *)"portrait") ) {
            xmlNodePtr grandChildNode = child_node->xmlChildrenNode;
            while (grandChildNode!=NULL) {
                if (0 == xmlStrcmp(grandChildNode->name, (const xmlChar*)"x") ) {
                    m_default_configure.touch_offset[DISPLAYMODE_PORTRAIT].x = get_content_int(grandChildNode);
                } else if (0 == xmlStrcmp(grandChildNode->name, (const xmlChar*)"y") ) {
                    m_default_configure.touch_offset[DISPLAYMODE_PORTRAIT].y = get_content_int(grandChildNode);
                }
                grandChildNode = grandChildNode->next;
            }
        } else if (0 == xmlStrcmp(child_node->name, (const xmlChar *)"landscape") ) {
            xmlNodePtr grandChildNode = child_node->xmlChildrenNode;
            while (grandChildNode!=NULL) {
                if (0 == xmlStrcmp(grandChildNode->name, (const xmlChar*)"x") ) {
                    m_default_configure.touch_offset[DISPLAYMODE_LANDSCAPE].x = get_content_int(grandChildNode);
                } else if (0 == xmlStrcmp(grandChildNode->name, (const xmlChar*)"y") ) {
                    m_default_configure.touch_offset[DISPLAYMODE_LANDSCAPE].y = get_content_int(grandChildNode);
                }
                grandChildNode = grandChildNode->next;
            }
        }

        child_node = child_node->next;
    }
}

void Default_Configure_Parser::parsing_dim_color(const xmlNodePtr cur_node) {
    assert(cur_node != NULL);

    xmlNodePtr child_node = cur_node->xmlChildrenNode;
    while (child_node!=NULL) {
        if (0 == xmlStrcmp(child_node->name, (const xmlChar *)"r") ) {
            m_default_configure.dim_color.r = get_content_int(child_node);
        }
        else if (0 == xmlStrcmp(child_node->name, (const xmlChar *)"g") ) {
            m_default_configure.dim_color.g = get_content_int(child_node);
        }
        else if (0 == xmlStrcmp(child_node->name, (const xmlChar *)"b") ) {
            m_default_configure.dim_color.b = get_content_int(child_node);
        }
        else if (0 == xmlStrcmp(child_node->name, (const xmlChar *)"a") ) {
            m_default_configure.dim_color.a = get_content_int(child_node);
        }

        child_node = child_node->next;
    }
}


SCLDisplayMode
Default_Configure_Parser::get_content_display_mode(const xmlNodePtr cur_node) {
    assert(cur_node != NULL);

    SCLDisplayMode display_mode = DISPLAYMODE_PORTRAIT;

    xmlChar* key = xmlNodeGetContent(cur_node);
    if (key!= NULL) {
        if (0 == strcmp("landscape", (const char*)key)) {
            display_mode = DISPLAYMODE_LANDSCAPE;
        }
        xmlFree(key);
    }

    return display_mode;
}

void Default_Configure_Parser::parsing_default_configure() {
    xmlDocPtr doc;
    xmlNodePtr cur_node;

    char input_file[_POSIX_PATH_MAX] = {0};
    Main_Entry_Parser::get_file_full_path(input_file, "default_configure");

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
    if (0 != xmlStrcmp(cur_node->name, (const xmlChar*)"default_configure"))
    {
        printf("root name %s error!\n", cur_node->name);
        xmlFreeDoc(doc);
        exit(1);
    }

    cur_node = cur_node->xmlChildrenNode;

    while (cur_node != NULL) {
        if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"display")) {
            m_default_configure.display_mode = get_content_display_mode(cur_node);
        }
        else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"input_mode")) {
            xmlChar* temp = xmlNodeGetContent(cur_node);
            m_default_configure.input_mode = (sclchar *)temp;
        }
        else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"base_screen_width")) {
            m_default_configure.target_screen_width = get_content_int(cur_node);
        }
        else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"base_screen_height")) {
            m_default_configure.target_screen_height = get_content_int(cur_node);
        }
        else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"image_file_base_path")) {
            xmlChar* temp = xmlNodeGetContent(cur_node);
            m_default_configure.image_file_base_path = (sclchar *)temp;
        }
        else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"use_magnifier")) {
            m_default_configure.use_magnifier_window = get_content_bool(cur_node);
        }
        else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"use_autopopup")) {
            m_default_configure.use_auto_popup = get_content_bool(cur_node);
        }
        else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"use_zoomwindow")) {
            m_default_configure.use_zoom_window = get_content_bool(cur_node);
        }
        else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"use_error_sound")) {
            m_default_configure.on_error_noti_send = get_content_bool(cur_node);
        }
        else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"use_word_deletion")) {
            m_default_configure.use_word_deletion = get_content_bool(cur_node);
        }
        else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"touch_offset_level")) {
            // Let's skip this item since it does not seem to be useful anymore
        }
        else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"touch_offset")) {
            // Let's skip this item since it does not seem to be useful anymore
            parsing_touch_offset(cur_node);
        }
        else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"default_sub_layout")) {
            xmlChar* temp = xmlNodeGetContent(cur_node);
            m_default_configure.default_sub_layout = (sclchar *)temp;
        }
        else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"dim_use_window_flag")) {
            m_default_configure.use_actual_dim_window = get_content_bool(cur_node);
        }
        else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"dim_color")) {
            parsing_dim_color(cur_node);
        }
        else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"use_lazy_loading")) {
            m_default_configure.use_lazy_loading = get_content_bool(cur_node);
        }
        cur_node = cur_node->next;
    }
    //print_default_configure_table();
    xmlFreeDoc(doc);
}

PSclDefaultConfigure Default_Configure_Parser::get_default_configure() {
    return &m_default_configure;
}

void Default_Configure_Parser::print_default_configure_table() {
    printf("------------------\n");
    printf("\tdisplay:\t%d\n", m_default_configure.display_mode);
    printf("\tinputMode:\t%s\n", m_default_configure.input_mode);
    printf("\tuse_magnifier:\t%d\n", m_default_configure.use_magnifier_window);
    printf("\tuse_autopopup:\t%d\n", m_default_configure.use_auto_popup);
    printf("\tuse_zoomwindow:\t%d\n", m_default_configure.use_zoom_window);
    printf("\tuse_error_sound:\t%d\n", m_default_configure.use_zoom_window);
    printf("\tuse_word_deletion:\t%d\n", m_default_configure.use_zoom_window);
    printf("\ttouch_offset:\n");
    printf("\\t\t\t%d %d %d %d\n",
        m_default_configure.touch_offset[0].x, m_default_configure.touch_offset[0].y,
        m_default_configure.touch_offset[1].x, m_default_configure.touch_offset[1].y);
    printf("------------------\n");
}
