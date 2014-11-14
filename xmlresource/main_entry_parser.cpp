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

#include "main_entry_parser.h"
#include "xml_parser_utils.h"
#include "simple_debug.h"

using namespace std;

class MainEntryParserImpl {
    public:
        int parsing_main_entry(const char *input_file) {
            xmlDocPtr doc;
            xmlNodePtr cur_node;

            doc = xmlReadFile(input_file, NULL, 0);
            if (doc == NULL) {
                return -1;
            }

            cur_node = xmlDocGetRootElement(doc);
            if (cur_node == NULL) {
                xmlFreeDoc(doc);
                return -1;
            }
            if (0 != xmlStrcmp(cur_node->name, (const xmlChar*)"main-entry"))
            {
                xmlFreeDoc(doc);
                return -1;
            }

            make_xml_files(cur_node);

            xmlFreeDoc(doc);

            return 0;

        }
        void make_xml_files(const xmlNodePtr p_node) {
            assert(p_node != NULL);
            xmlNodePtr node = p_node->xmlChildrenNode;
            while (node != NULL) {
                if (0 == xmlStrcmp(node->name, (const xmlChar *)"files")) {
                    parsing_files_node(node);
                }
                node = node->next;
            }

        }

        void parsing_files_node(const xmlNodePtr p_node) {
            assert(p_node != NULL);
            xmlNodePtr node = p_node->xmlChildrenNode;

            while (node != NULL) {
                if (0 == xmlStrcmp(node->name, (const xmlChar *)"input-mode-configure")) {
                    m_xml_files.input_mode_configure = (char*)xmlNodeGetContent(node);
                }
                else if (0 == xmlStrcmp(node->name, (const xmlChar *)"layout")) {
                    m_xml_files.layout = (char*)xmlNodeGetContent(node);
                }
                else if (0 == xmlStrcmp(node->name, (const xmlChar *)"key-label-property")) {
                    m_xml_files.key_label_property = (char*)xmlNodeGetContent(node);
                }
                else if (0 == xmlStrcmp(node->name, (const xmlChar *)"modifier-decoration")) {
                    m_xml_files.modifier_decoration = (char*)xmlNodeGetContent(node);
                }
                else if (0 == xmlStrcmp(node->name, (const xmlChar *)"default-configure")) {
                    m_xml_files.default_configure = (char*)xmlNodeGetContent(node);
                }
                else if (0 == xmlStrcmp(node->name, (const xmlChar *)"autopopup-configure")) {
                    m_xml_files.autopopup_configure = (char*)xmlNodeGetContent(node);
                }
                else if (0 == xmlStrcmp(node->name, (const xmlChar *)"magnifier-configure")) {
                    m_xml_files.magnifier_configure = (char*)xmlNodeGetContent(node);
                }
                else if (0 == xmlStrcmp(node->name, (const xmlChar *)"nine-patch-file-list")) {
                    m_xml_files.nine_patch_file_list = (char*)xmlNodeGetContent(node);
                }
                node = node->next;
            }

        }
        XMLFiles m_xml_files;
};

MainEntryParser::MainEntryParser() {
    m_impl = new MainEntryParserImpl;
}

MainEntryParser::~MainEntryParser() {
    if (m_impl) {
        delete m_impl;
    }
}

MainEntryParser*
MainEntryParser::get_instance() {
    static MainEntryParser instance;
    return &instance;
}

int
MainEntryParser::init(const char* path) {
    return m_impl->parsing_main_entry(path);
}


XMLFiles&
MainEntryParser::get_xml_files() {
    return m_impl->m_xml_files;
}
