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
                SCLLOG(SclLog::DEBUG, "Could not load file: %s.", input_file);
                return -1;
            }

            cur_node = xmlDocGetRootElement(doc);
            if (cur_node == NULL) {
                SCLLOG(SclLog::DEBUG, "Main_Entry_Parser: empty document.\n");
                xmlFreeDoc(doc);
                return -1;
            }
            if (0 != xmlStrcmp(cur_node->name, (const xmlChar*)"main-entry"))
            {
                SCLLOG(SclLog::DEBUG, "Main_Entry_Parser: root name error: %s\n!", (char *)cur_node->name);
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

Main_Entry_Parser* Main_Entry_Parser::m_instance = NULL;

Main_Entry_Parser::Main_Entry_Parser() {
    m_impl = new MainEntryParserImpl;
    if (m_impl == NULL) {
        SCLLOG(SclLog::ERROR, "Create MainEntryParserImpl failed");
        return;
    }
}

Main_Entry_Parser::~Main_Entry_Parser() {
    if (m_impl) {
        SCLLOG(SclLog::MESSAGE, "~Main_Entry_Parser() has called");
        delete m_impl;
        m_impl = NULL;
    }
}

Main_Entry_Parser*
Main_Entry_Parser::get_instance() {
    if (m_instance == NULL) {
        m_instance = new Main_Entry_Parser();
    }
    return m_instance;
}

int
Main_Entry_Parser::init(const char* path) {
    return m_impl->parsing_main_entry(path);
}


XMLFiles&
Main_Entry_Parser::get_xml_files() {
    return m_impl->m_xml_files;
}
