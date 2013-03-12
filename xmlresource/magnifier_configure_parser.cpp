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

#include "magnifier_configure_parser.h"
#include "main_entry_parser.h"
#include <assert.h>
#include "xml_parser_utils.h"
using namespace std;

Magnifier_Configure_Parser* Magnifier_Configure_Parser::m_instance = NULL;

Magnifier_Configure_Parser::Magnifier_Configure_Parser() {
    memset((void*)&m_magnifier_configure, 0x00, sizeof(SclMagnifierWndConfigure));
}

Magnifier_Configure_Parser::~Magnifier_Configure_Parser() {
    /* Let's create de-initializing function for this resource releasement */
    if (m_magnifier_configure.bg_image_path) {
        xmlFree(m_magnifier_configure.bg_image_path);
        m_magnifier_configure.bg_image_path = NULL;
    }
    if (m_magnifier_configure.bg_shift_image_path) {
        xmlFree(m_magnifier_configure.bg_shift_image_path);
        m_magnifier_configure.bg_shift_image_path = NULL;
    }
    if (m_magnifier_configure.bg_shift_lock_image_path) {
        xmlFree(m_magnifier_configure.bg_shift_lock_image_path);
        m_magnifier_configure.bg_shift_lock_image_path = NULL;
    }
    if (m_magnifier_configure.bg_long_key_image_path) {
        xmlFree(m_magnifier_configure.bg_long_key_image_path);
        m_magnifier_configure.bg_long_key_image_path = NULL;
    }
    if (m_magnifier_configure.label_type) {
        xmlFree(m_magnifier_configure.label_type);
        m_magnifier_configure.label_type = NULL;
    }
}

Magnifier_Configure_Parser* Magnifier_Configure_Parser::get_instance() {
    if (m_instance == NULL) {
        m_instance = new Magnifier_Configure_Parser();
    }
    return m_instance;
}

void Magnifier_Configure_Parser::init() {
    parsing_magnifier_configure();
}

void Magnifier_Configure_Parser::parsing_label_area(const xmlNodePtr cur_node) {
    assert(cur_node != NULL);

    xmlNodePtr child_node = cur_node->xmlChildrenNode;
    while (child_node!=NULL) {
        if (0 == xmlStrcmp(child_node->name, (const xmlChar *)"left") ) {
            m_magnifier_configure.label_area_rect.left = get_content_int(child_node);
        }
        else if (0 == xmlStrcmp(child_node->name, (const xmlChar *)"top") ) {
            m_magnifier_configure.label_area_rect.top = get_content_int(child_node);
        }
        else if (0 == xmlStrcmp(child_node->name, (const xmlChar *)"right") ) {
            m_magnifier_configure.label_area_rect.right = get_content_int(child_node);
        }
        else if (0 == xmlStrcmp(child_node->name, (const xmlChar *)"bottom") ) {
            m_magnifier_configure.label_area_rect.bottom = get_content_int(child_node);
        }

        child_node = child_node->next;
    }
}

void Magnifier_Configure_Parser::parsing_background_images(const xmlNodePtr cur_node) {
    assert(cur_node != NULL);

    xmlNodePtr child_node = cur_node->xmlChildrenNode;
    while (child_node!=NULL) {
        if (0 == xmlStrcmp(child_node->name, (const xmlChar *)"image") ) {
            if (equal_prop(child_node, "state", "normal")) {
                xmlChar* temp = xmlNodeGetContent(child_node);
                m_magnifier_configure.bg_image_path = (sclchar *)temp;
            }
            else if (equal_prop(child_node, "state", "shift")) {
                xmlChar* temp = xmlNodeGetContent(child_node);
                m_magnifier_configure.bg_shift_image_path = (sclchar *)temp;
            }
            else if (equal_prop(child_node, "state", "lock")) {
                xmlChar* temp = xmlNodeGetContent(child_node);
                m_magnifier_configure.bg_shift_lock_image_path = (sclchar *)temp;
            }
            else if (equal_prop(child_node, "state", "longkey")) {
                xmlChar* temp = xmlNodeGetContent(child_node);
                m_magnifier_configure.bg_long_key_image_path = (sclchar *)temp;
            }
        }

        child_node = child_node->next;
    }
}

void Magnifier_Configure_Parser::parsing_padding_values(const xmlNodePtr cur_node) {
    assert(cur_node != NULL);

    xmlNodePtr child_node = cur_node->xmlChildrenNode;
    while (child_node!=NULL) {
        if (0 == xmlStrcmp(child_node->name, (const xmlChar *)"x") ) {
            m_magnifier_configure.padding_x = get_content_int(child_node);
        }
        else if (0 == xmlStrcmp(child_node->name, (const xmlChar *)"y") ) {
            m_magnifier_configure.padding_y = get_content_int(child_node);
        }

        child_node = child_node->next;
    }
}

SCLMagnifierStyle 
get_content_magnifier_style(const xmlNodePtr cur_node) {
    assert(cur_node != NULL);

    SCLMagnifierStyle style = MAGNIFIER_STYLE_LABEL_ONLY;

    xmlChar* key = xmlNodeGetContent(cur_node);
    if (key!= NULL) {
        if (0 == strcmp("area_capture", (const char*)key)) {
            style = MAGNIFIER_STYLE_SEL_AREA_CAPTURE;
        }
        xmlFree(key);
    }

    return style;
}

void Magnifier_Configure_Parser::parsing_magnifier_configure() {
    xmlDocPtr doc;
    xmlNodePtr cur_node;

    char input_file[_POSIX_PATH_MAX] = {0};
    Main_Entry_Parser::get_file_full_path(input_file, "magnifier_configure");

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
    if (0 != xmlStrcmp(cur_node->name, (const xmlChar*)"magnifier_configure"))
    {
        printf("root name %s error!\n", cur_node->name);
        xmlFreeDoc(doc);
        exit(1);
    }

    cur_node = cur_node->xmlChildrenNode;

    while (cur_node != NULL) {
        if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"magnifier_style")) {
            m_magnifier_configure.style = get_content_magnifier_style(cur_node);
        }
        else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"width")) {
            m_magnifier_configure.width = get_content_int(cur_node);
        }
        else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"height")) {
            m_magnifier_configure.height = get_content_int(cur_node);
        }
        else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"label_area")) {
            parsing_label_area(cur_node);
        }
        else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"background_image_path")) {
            parsing_background_images(cur_node);
        }
        else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"use_window")) {
            m_magnifier_configure.use_actual_window = get_content_bool(cur_node);
        }
        else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"label_type")) {
            xmlChar* temp = xmlNodeGetContent(cur_node);
            m_magnifier_configure.label_type = (sclchar*)temp;
        }
        else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"padding")) {
            parsing_padding_values(cur_node);
        }
        else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"show_shift_label")) {
            m_magnifier_configure.show_shift_label = get_content_bool(cur_node);
        }
        cur_node = cur_node->next;
    }
    //print_default_configure_table();
    xmlFreeDoc(doc);
}

PSclMagnifierWndConfigure Magnifier_Configure_Parser::get_magnifier_configure() {
    return &m_magnifier_configure;
}

void Magnifier_Configure_Parser::print_magnifier_configure_table() {
    //printf("------------------\n");
    //printf("\tdisplay:\t%d\n", m_magnifier_configure.display);
    //printf("\tinputMode:\t%s\n", m_magnifier_configure.input_mode);
    //printf("\tuse_magnifier:\t%d\n", m_magnifier_configure.use_magnifier_window);
    //printf("\tuse_autopopup:\t%d\n", m_magnifier_configure.use_auto_popup);
    //printf("\tuse_zoomwindow:\t%d\n", m_magnifier_configure.use_zoom_window);
    //printf("\tuse_error_sound:\t%d\n", m_magnifier_configure.use_zoom_window);
    //printf("\tuse_word_deletion:\t%d\n", m_magnifier_configure.use_zoom_window);
    //printf("\ttouch_offset:\n");
    //printf("\\t\t\t%d %d %d %d\n",
    //    m_magnifier_configure.touch_offset[0].x, m_magnifier_configure.touch_offset[0].y,
    //    m_magnifier_configure.touch_offset[1].x, m_magnifier_configure.touch_offset[1].y);
    //printf("------------------\n");
}
