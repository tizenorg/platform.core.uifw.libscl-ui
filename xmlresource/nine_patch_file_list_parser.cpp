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

#include <dlog.h>
#ifndef LOG_TAG
#define LOG_TAG "LIBSCL_UI"
#endif
#include <memory.h>
#include <assert.h>
#include <libxml/parser.h>

#include "nine_patch_file_list_parser.h"
#include "xml_parser_utils.h"

class NinePatchFileListParserImpl {
    public:
        NinePatchFileListParserImpl() {
            m_size = 0;
            memset(m_nine_patch_file_list, 0x00, sizeof(SclNinePatchInfo) * MAX_NINE_PATCH_FILE_LIST);
        }
        ~NinePatchFileListParserImpl() {
            for(int i = 0; i < m_size; ++i) {
                if (m_nine_patch_file_list[i].image_path)
                    delete m_nine_patch_file_list[i].image_path;
                m_nine_patch_file_list[i].image_path = NULL;
            }
            m_size = 0;
        }

        int parsing_nine_patch_file_list(const char* input_file) {
            xmlDocPtr doc;
            xmlNodePtr cur_node;

            doc = xmlReadFile(input_file, NULL, 0);
            if (doc == NULL) {
                LOGD("Could not load file: %s\n", input_file);
                return -1;
            }

            cur_node = xmlDocGetRootElement(doc);
            if (cur_node == NULL) {
                LOGD("Nine_Patch_File_Parser: empty document.\n");
                xmlFreeDoc(doc);
                return -1;
            }
            if (0 != xmlStrcmp(cur_node->name, (const xmlChar*)"nine_patch_file_list"))
            {
                LOGD("%s: root name error: %s.\n", __FUNCTION__, (char *)cur_node->name);
                xmlFreeDoc(doc);
                return -1;
            }

            cur_node = cur_node->xmlChildrenNode;

            assert(m_size == 0);
            while (cur_node != NULL) {
                if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"file")) {
                    assert(m_size >= 0 && m_size < MAX_NINE_PATCH_FILE_LIST);
                    m_nine_patch_file_list[m_size].image_path = (const char*)xmlNodeGetContent(cur_node);
                    get_prop_number(cur_node, "left", &(m_nine_patch_file_list[m_size].left));
                    get_prop_number(cur_node, "right", &(m_nine_patch_file_list[m_size].right));
                    get_prop_number(cur_node, "top", &(m_nine_patch_file_list[m_size].top));
                    get_prop_number(cur_node, "bottom", &(m_nine_patch_file_list[m_size].bottom));
                    if (m_nine_patch_file_list[m_size].image_path == NULL) {
                        LOGD("NinePatchFileParser: image_path should be not NULL");
                    }
                    m_size++;
                    if (m_size >= MAX_NINE_PATCH_FILE_LIST) {
                        LOGD("No Space for nine patch file list record.");
                        break;
                    }
                }

                cur_node = cur_node->next;
            }
            xmlFreeDoc(doc);

            return 0;

        }
        SclNinePatchInfo m_nine_patch_file_list[MAX_NINE_PATCH_FILE_LIST];
        int m_size;

};

NinePatchFileParser* NinePatchFileParser::m_instance = NULL;

NinePatchFileParser::NinePatchFileParser() {
    m_impl = new NinePatchFileListParserImpl;
}

NinePatchFileParser::~NinePatchFileParser() {
    if (m_impl) {
        LOGD("~NinePatchFileParser() has called.");
        delete m_impl;
        m_impl = NULL;
    }
}
NinePatchFileParser*
NinePatchFileParser::get_instance() {
    if (m_instance == NULL) {
        m_instance = new NinePatchFileParser();
    }
    return m_instance;
}

int
NinePatchFileParser::init(const char* file) {
    return m_impl->parsing_nine_patch_file_list(file);
}

bool
NinePatchFileParser::get_nine_patch_info(const char* filename, SclNinePatchInfo *info) {
    if (filename == NULL) {
        LOGD("get_nine_patch_info() has failed.");
        return false;
    }

    SclNinePatchInfo *nine_patch_list = get_nine_patch_list();
    if (nine_patch_list == NULL) {
        LOGD("get_nine_patch_info() has failed.");
        return false;
    }

    for(int i = 0; i < MAX_NINE_PATCH_FILE_LIST && i < m_impl->m_size; ++i) {
        if ( nine_patch_list[i].image_path != NULL &&
            0 == strcmp(nine_patch_list[i].image_path, filename) ) {
                if (info) {
                    *info = nine_patch_list[i];
                }
            return true;
        }
    }

    LOGD("get_nine_patch_info() has failed.");
    return false;

}

SclNinePatchInfo*
NinePatchFileParser::get_nine_patch_list() {
    return m_impl->m_nine_patch_file_list;
}
