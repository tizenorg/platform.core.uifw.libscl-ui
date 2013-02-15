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

#ifndef __Main_Entry_Parser__H__
#define __Main_Entry_Parser__H__
#include <libxml/parser.h>
#include <string.h>
#include <map>
struct ltstr{
    bool operator()(const char* st1, const char* st2)const {
        return strcmp(st1, st2)<0;
    }
};

typedef std::map<const char*, char*, ltstr> MapFile;
typedef MapFile::iterator Iter_data;

class XMLFiles{
    public:
    char* inputmode_name_list;
    char* inputmode_popup_name_list;
    char* layout_name_list;
    char* keyset_name_list;
    char* input_mode_configure;
    char* input_mode_popup_configure;
    char* layout;
    char* layout_keyset;
    char* default_configure;
    char* autopopup_configure;
    char* magnifier_configure;
    MapFile layout_key_configure;
    MapFile layout_key_property;
    MapFile layout_key_coordinate;
    char* key_label_property;
    char* modifier_decoration;
    char* nine_patch_file_list;

    public:
    XMLFiles() {
        inputmode_name_list = NULL;
        inputmode_popup_name_list = NULL;
        layout_name_list = NULL;
        keyset_name_list = NULL;
        input_mode_configure = NULL;
        input_mode_popup_configure = NULL;
        layout = NULL;
        layout_keyset = NULL;
        key_label_property = NULL;
        modifier_decoration = NULL;
        default_configure = NULL;
        autopopup_configure = NULL;
        magnifier_configure = NULL;
        nine_patch_file_list = NULL;
    }
    ~XMLFiles() {
        if (inputmode_name_list)
            delete inputmode_name_list;
        if (inputmode_popup_name_list)
            delete inputmode_popup_name_list;
        if (layout_name_list)
            delete layout_name_list;
        if (keyset_name_list)
            delete keyset_name_list;
        if (input_mode_configure)
            delete input_mode_configure;
        if (input_mode_popup_configure)
            delete input_mode_popup_configure;
        if (layout)
            delete layout;
        if (layout_keyset)
            delete layout_keyset;
        if (key_label_property)
            delete key_label_property;
        if (modifier_decoration)
            delete modifier_decoration;
        if (default_configure)
            delete default_configure;
        if (autopopup_configure)
            delete autopopup_configure;
        if (magnifier_configure)
            delete magnifier_configure;
        if (nine_patch_file_list)
            delete nine_patch_file_list;

        MapFile::iterator it;
        it = layout_key_configure.begin();
        for(; it != layout_key_configure.end(); ++it) {
            if (it->first)
                delete it->first;
            if (it->second)
                delete it->second;
        }
        it = layout_key_property.begin();
        for(; it != layout_key_property.end(); ++it) {
            if (it->first)
                delete it->first;
            if (it->second)
                delete it->second;
        }
        it = layout_key_coordinate.begin();
        for(; it != layout_key_coordinate.end(); ++it) {
            if (it->first)
                delete it->first;
            if (it->second)
                delete it->second;
        }
    }
};

class Main_Entry_Parser {
    public:
        ~Main_Entry_Parser();
        static Main_Entry_Parser *get_instance();
    void init(const char *entry_filepath);
    XMLFiles& get_xml_files();
    static void get_file_full_path(char* full_path, const char* file_type);
    private:
    Main_Entry_Parser();
    void parsing_main_entry(const char *entry_filepath);
    void make_xml_files(const xmlNodePtr);
    void parsing_files_node(const xmlNodePtr);
    void parsing_configure_files(const xmlNodePtr);
    void parsing_property_files(const xmlNodePtr);
    void parsing_coordinate_files(const xmlNodePtr);
    private:
        static Main_Entry_Parser *m_instance;

        XMLFiles m_xml_files;

    private:
    class DestructHelper {
        public:
        ~DestructHelper() {
            if (Main_Entry_Parser::m_instance != NULL)
            delete m_instance;
        }
    };
    static DestructHelper des;
};


#endif
