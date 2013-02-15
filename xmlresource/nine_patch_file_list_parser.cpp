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

#include "nine_patch_file_list_parser.h"
#include "main_entry_parser.h"
#include "xml_parser_utils.h"
#include <memory.h>
#include <assert.h>

Nine_patch_file_list_Parser* Nine_patch_file_list_Parser::m_instance = NULL;

Nine_patch_file_list_Parser::Nine_patch_file_list_Parser() {
    m_size = 0;
    memset(m_nine_patch_file_list, 0x00, sizeof(SclNinePatchInfo) * MAX_NINE_PATCH_FILE_LIST);
}

Nine_patch_file_list_Parser::~Nine_patch_file_list_Parser() {
    for(int i = 0; i < m_size; ++i) {
        if (m_nine_patch_file_list[i].image_path)
            delete m_nine_patch_file_list[i].image_path;
        m_nine_patch_file_list[i].image_path = NULL;
    }
    m_size = 0;
}
Nine_patch_file_list_Parser* Nine_patch_file_list_Parser::get_instance() {
    if (m_instance == NULL) {
        m_instance = new Nine_patch_file_list_Parser();
    }
    return m_instance;
}
void Nine_patch_file_list_Parser::init() {
    parsing_nine_patch_file_list();
}

void Nine_patch_file_list_Parser::parsing_nine_patch_file_list() {
    xmlDocPtr doc;
    xmlNodePtr cur_node;

    char input_file[_POSIX_PATH_MAX] = {0};
    Main_Entry_Parser::get_file_full_path(input_file, "nine_patch_file_list");

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
    if (0 != xmlStrcmp(cur_node->name, (const xmlChar*)"nine_patch_file_list"))
    {
       printf("root name %s error!\n", cur_node->name);
       xmlFreeDoc(doc);
       exit(1);
    }

    cur_node = cur_node->xmlChildrenNode;

    assert(m_size == 0);
    while (cur_node != NULL) {
        if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"file")) {
            m_nine_patch_file_list[m_size].image_path = (const char*)xmlNodeGetContent(cur_node);
            get_prop_number(cur_node, "left", &(m_nine_patch_file_list[m_size].left));
            get_prop_number(cur_node, "right", &(m_nine_patch_file_list[m_size].right));
            get_prop_number(cur_node, "top", &(m_nine_patch_file_list[m_size].top));
            get_prop_number(cur_node, "bottom", &(m_nine_patch_file_list[m_size].bottom));
            assert(m_nine_patch_file_list[m_size].image_path != NULL);
            m_size++;
        }

        cur_node = cur_node->next;
    }
    xmlFreeDoc(doc);
}

bool
Nine_patch_file_list_Parser::get_nine_patch_info(const char* filename, SclNinePatchInfo *info) {
    if (filename == NULL) return false;

    for(int i = 0; i < MAX_NINE_PATCH_FILE_LIST && i < m_size; ++i) {
        if ( m_nine_patch_file_list[i].image_path != NULL &&
            0 == strcmp(m_nine_patch_file_list[i].image_path, filename) ) {
                if (info) {
                    *info = m_nine_patch_file_list[i];
                }
            return true;
        }
    }

    return false;

}

SclNinePatchInfo* Nine_patch_file_list_Parser::get_nine_patch_list() {
    return m_nine_patch_file_list;
}
