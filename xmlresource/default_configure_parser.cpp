/*
 * Copyright (c) 2012 - 2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <assert.h>
#include <string.h>
#include <libxml/parser.h>
#include "default_configure_parser.h"
#include "xml_parser_utils.h"
#include "simple_debug.h"
using namespace std;

class DefaultConfigureParserImpl {
    public:
        DefaultConfigureParserImpl() {
            memset((void*)&m_default_configure, 0x00, sizeof(SclDefaultConfigure));
        }

        ~DefaultConfigureParserImpl() {
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

        int parsing_default_configure(const char* input_file) {
            xmlDocPtr doc;
            xmlNodePtr cur_node;

            doc = xmlReadFile(input_file, NULL, 0);
            if (doc == NULL) {
                SCLLOG(SclLog::DEBUG, "Could not load file: %s.", input_file);
                return -1;
            }

            cur_node = xmlDocGetRootElement(doc);
            if (cur_node == NULL) {
                SCLLOG(SclLog::DEBUG, "DefaultConfigParser: empty document.\n");
                xmlFreeDoc(doc);
                return -1;
            }
            if (0 != xmlStrcmp(cur_node->name, (const xmlChar*)"default_configure"))
            {
                SCLLOG(SclLog::DEBUG, "DefaultConfigParser: root name error: %s\n!", (char *)cur_node->name);
                xmlFreeDoc(doc);
                return -1;
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
                cur_node = cur_node->next;
            }
            xmlFreeDoc(doc);

            return 0;

        }

        void parsing_touch_offset(const xmlNodePtr cur_node) {
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

        void parsing_dim_color(const xmlNodePtr cur_node) {
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

        SCLDisplayMode get_content_display_mode(const xmlNodePtr cur_node) {
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
        SclDefaultConfigure m_default_configure;
};

DefaultConfigParser*
DefaultConfigParser::get_instance() {
    static DefaultConfigParser instance;
    return &instance;
}
DefaultConfigParser::DefaultConfigParser() {
    m_impl = new DefaultConfigureParserImpl;
}

DefaultConfigParser::~DefaultConfigParser() {
    if (m_impl) {
        SCLLOG(SclLog::MESSAGE, "~DefaultConfigParser() has called");
        delete m_impl;
        m_impl = NULL;
    }
}

int
DefaultConfigParser::init(const char* file) {
        return m_impl->parsing_default_configure(file);
}

PSclDefaultConfigure
DefaultConfigParser::get_default_configure() {
    return &m_impl->m_default_configure;
}
