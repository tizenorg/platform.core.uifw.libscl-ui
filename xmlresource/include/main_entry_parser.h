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

#include <libxml/parser.h>

#ifndef __MainEntryParser__H__
#define __MainEntryParser__H__
class XMLFiles{
    public:
    char* input_mode_configure;
    char* layout;
    char* default_configure;
    char* autopopup_configure;
    char* magnifier_configure;
    char* key_label_property;
    char* modifier_decoration;
    char* nine_patch_file_list;

    public:
    XMLFiles() {
        input_mode_configure = NULL;
        layout = NULL;
        key_label_property = NULL;
        modifier_decoration = NULL;
        default_configure = NULL;
        autopopup_configure = NULL;
        magnifier_configure = NULL;
        nine_patch_file_list = NULL;
    }
    ~XMLFiles() {
        if (input_mode_configure)
            xmlFree(input_mode_configure);
        if (layout)
            xmlFree(layout);
        if (key_label_property)
            xmlFree(key_label_property);
        if (modifier_decoration)
            xmlFree(modifier_decoration);
        if (default_configure)
            xmlFree(default_configure);
        if (autopopup_configure)
            xmlFree(autopopup_configure);
        if (magnifier_configure)
            xmlFree(magnifier_configure);
        if (nine_patch_file_list)
            xmlFree(nine_patch_file_list);
    }
};

class MainEntryParserImpl;
class MainEntryParser {
    MainEntryParserImpl *m_impl;
    public:
    /* parsing xml file, path is assigned */
    int init(const char* path);

    XMLFiles& get_xml_files();
    public:
    ~MainEntryParser();
    static MainEntryParser *get_instance();
    private:
    MainEntryParser();
};

#endif
