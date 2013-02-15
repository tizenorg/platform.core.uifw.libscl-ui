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

#include "main_entry_parser.h"
#include <assert.h>
#include "xmlresource.h"
#include "xml_parser_utils.h"

using namespace std;
using namespace xmlresource;

static int get_keyset_id(const xmlNodePtr cur_node);
static int get_layout_id(const xmlNodePtr cur_node);

Main_Entry_Parser* Main_Entry_Parser::m_instance = NULL;

Main_Entry_Parser::Main_Entry_Parser() {
}

Main_Entry_Parser::~Main_Entry_Parser() {
}
Main_Entry_Parser* Main_Entry_Parser::get_instance() {
    if (m_instance == NULL) {
        m_instance = new Main_Entry_Parser();
    }
    return m_instance;
}
void
Main_Entry_Parser::Main_Entry_Parser::init(const char *entry_filepath) {
    char input_file[_POSIX_PATH_MAX] = {0};
    XMLResource *xml_resource = XMLResource::get_instance();
    snprintf(input_file, _POSIX_PATH_MAX, "%s/%s",
        xml_resource->get_resource_directory(), entry_filepath);
    parsing_main_entry(input_file);
}

XMLFiles& Main_Entry_Parser::get_xml_files() {
    return m_xml_files;
}

void Main_Entry_Parser::parsing_main_entry(const char *entry_filepath) {
    xmlDocPtr doc;
    xmlNodePtr cur_node;

    doc = xmlReadFile(entry_filepath, NULL, 0);
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
    if (0 != xmlStrcmp(cur_node->name, (const xmlChar*)"main-entry"))
    {
       printf("root name %s error!\n", cur_node->name);
       xmlFreeDoc(doc);
       exit(1);
    }

    make_xml_files(cur_node);

    xmlFreeDoc(doc);
}

void
Main_Entry_Parser::make_xml_files(const xmlNodePtr p_node) {
    assert(p_node != NULL);
    xmlNodePtr node = p_node->xmlChildrenNode;
    while (node != NULL) {
        if (0 == xmlStrcmp(node->name, (const xmlChar *)"files")) {
            parsing_files_node(node);
        }
        node = node->next;
    }
}

void
Main_Entry_Parser::parsing_files_node(const xmlNodePtr p_node) {
    assert(p_node != NULL);
    xmlNodePtr node = p_node->xmlChildrenNode;

    while (node != NULL) {
        if (0 == xmlStrcmp(node->name, (const xmlChar *)"inputmode-name-list")) {
            m_xml_files.inputmode_name_list = (char*)xmlNodeGetContent(node);
        }
        else if (0 == xmlStrcmp(node->name, (const xmlChar *)"inputmode-popup-name-list")) {
            m_xml_files.inputmode_popup_name_list = (char*)xmlNodeGetContent(node);
        }
        else if (0 == xmlStrcmp(node->name, (const xmlChar *)"layout-name-list")) {
            m_xml_files.layout_name_list = (char*)xmlNodeGetContent(node);
        }
        else if (0 == xmlStrcmp(node->name, (const xmlChar *)"keyset-name-list")) {
            m_xml_files.keyset_name_list = (char*)xmlNodeGetContent(node);
        }
        else if (0 == xmlStrcmp(node->name, (const xmlChar *)"input-mode-configure")) {
            m_xml_files.input_mode_configure = (char*)xmlNodeGetContent(node);
        }
        else if (0 == xmlStrcmp(node->name, (const xmlChar *)"input-mode-popup-configure")) {
            m_xml_files.input_mode_popup_configure = (char*)xmlNodeGetContent(node);
        }
        else if (0 == xmlStrcmp(node->name, (const xmlChar *)"layout")) {
            m_xml_files.layout = (char*)xmlNodeGetContent(node);
        }
        else if (0 == xmlStrcmp(node->name, (const xmlChar *)"layout-keyset")) {
            m_xml_files.layout_keyset = (char*)xmlNodeGetContent(node);
        }
        else if (0 == xmlStrcmp(node->name, (const xmlChar *)"configure-files")) {
            parsing_configure_files(node);
        }
        else if (0 == xmlStrcmp(node->name, (const xmlChar *)"coordinate-files")) {
            parsing_coordinate_files(node);
        }
        else if (0 == xmlStrcmp(node->name, (const xmlChar *)"property-files")) {
            parsing_property_files(node);
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

void
Main_Entry_Parser::parsing_configure_files(const xmlNodePtr p_node) {
    assert(p_node != NULL);
    xmlNodePtr node = p_node->xmlChildrenNode;

    while (node != NULL) {
        if (0 == xmlStrcmp(node->name, (const xmlChar *)"file")) {
            xmlChar* keyset = xmlGetProp(node, (const xmlChar*)"keyset");
            assert(keyset != NULL);
            xmlChar* file_name = xmlNodeGetContent(node);
            assert(file_name != NULL);
            m_xml_files.layout_key_configure.insert(Iter_data::value_type((const char*)keyset, (char*)file_name));
        }
        node = node->next;
    }
}
void
Main_Entry_Parser::parsing_property_files(const xmlNodePtr p_node) {
    assert(p_node != NULL);
    xmlNodePtr node = p_node->xmlChildrenNode;

    while (node != NULL) {
        if (0 == xmlStrcmp(node->name, (const xmlChar *)"file")) {
            xmlChar* keyset = xmlGetProp(node, (const xmlChar*)"keyset");
            xmlChar* file_name = xmlNodeGetContent(node);
            assert(keyset != NULL);
            assert(file_name != NULL);

            m_xml_files.layout_key_property.insert(Iter_data::value_type((const char*)keyset, (char*)file_name));
        }
        node = node->next;
    }
}

void
Main_Entry_Parser::parsing_coordinate_files(const xmlNodePtr p_node) {
    assert(p_node != NULL);
    xmlNodePtr node = p_node->xmlChildrenNode;

    while (node != NULL) {
        if (0 == xmlStrcmp(node->name, (const xmlChar *)"file")) {
            xmlChar* layout = xmlGetProp(node, (const xmlChar*)"layout");
            xmlChar* file_name = xmlNodeGetContent(node);
            assert(layout != NULL);
            assert(file_name != NULL);

            m_xml_files.layout_key_coordinate.insert(Iter_data::value_type((const char*)layout, (char*)file_name));
        }
        node = node->next;
    }
}

void
Main_Entry_Parser::get_file_full_path(char* dest_path, const char* file_type) {
    assert(file_type != NULL);
    assert(dest_path != NULL);

    Main_Entry_Parser* main_entry_parser = Main_Entry_Parser::get_instance();
    XMLFiles& xml_files = main_entry_parser->get_xml_files();

    const char* input_file = NULL;
    if (0 == strcmp(file_type, "input_mode_configure")) {
        input_file = xml_files.input_mode_configure;
    }
    else if (0 == strcmp(file_type, "layout")) {
        input_file = xml_files.layout;
    }
    else if (0 == strcmp(file_type, "label_property")) {
        input_file = xml_files.key_label_property;
    }
    else if (0 == strcmp(file_type, "modifier_decoration")) {
        input_file = xml_files.modifier_decoration;
    }
    else if (0 == strcmp(file_type, "default_configure")) {
        input_file = xml_files.default_configure;
    }
    else if (0 == strcmp(file_type, "autopopup_configure")) {
        input_file = xml_files.autopopup_configure;
    }
    else if (0 == strcmp(file_type, "magnifier_configure")) {
        input_file = xml_files.magnifier_configure;
    }
    else if (0 == strcmp(file_type, "nine_patch_file_list")) {
        input_file = xml_files.nine_patch_file_list;
    } else {
        printf("file_type : %s is not exist.\n", file_type);
        assert(0);
    }

    if (input_file && dest_path) {
        XMLResource *xml_resource = XMLResource::get_instance();
        snprintf(dest_path, _POSIX_PATH_MAX, "%s/%s",
            xml_resource->get_resource_directory(), input_file);
    }
}
