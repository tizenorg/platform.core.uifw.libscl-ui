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

#include "xmlresource.h"
#include <assert.h>
#include "simple_debug.h"

using namespace xmlresource;

XMLResource* XMLResource::m_instance = NULL;

XMLResource::XMLResource() {
    m_main_entry_parser = NULL;
    m_input_mode_configure_parser = NULL;
    m_layout_parser = NULL;
    m_modifier_decoration_parser = NULL;
    m_label_properties_parser = NULL;
    m_default_configure_parser = NULL;
    m_autopopup_configure_parser = NULL;
    m_magnifier_configure_parser = NULL;
    m_nine_patch_file_list_parser = NULL;
}

XMLResource::~XMLResource() {
    if (m_main_entry_parser)
        delete m_main_entry_parser;
    m_main_entry_parser = NULL;
    if (m_input_mode_configure_parser)
        delete m_input_mode_configure_parser;
    m_input_mode_configure_parser = NULL;
    if (m_layout_parser)
        delete m_layout_parser;
    m_layout_parser = NULL;
    if (m_modifier_decoration_parser)
        delete m_modifier_decoration_parser;
    m_modifier_decoration_parser = NULL;

    if (m_label_properties_parser)
        delete m_label_properties_parser;
    m_label_properties_parser = NULL;

    if (m_default_configure_parser)
        delete m_default_configure_parser;
    m_default_configure_parser = NULL;

    if (m_autopopup_configure_parser)
        delete m_autopopup_configure_parser;
    m_autopopup_configure_parser = NULL;
    if (m_magnifier_configure_parser)
        delete m_magnifier_configure_parser;
    m_magnifier_configure_parser = NULL;

    if (m_nine_patch_file_list_parser)
        delete m_nine_patch_file_list_parser;
    m_nine_patch_file_list_parser = NULL;
}

XMLResource* XMLResource::get_instance() {
    if (m_instance == NULL) {
        m_instance = new XMLResource();
    }
    return m_instance;
}

void XMLResource::init(const char *entry_filepath) {
    if (m_main_entry_parser == NULL) {
        m_main_entry_parser = Main_Entry_Parser::get_instance();
        m_main_entry_parser->init(entry_filepath);
    }
    SCLLOG(SclLog::MESSAGE, "init inputmode configure\n\n");
    if (m_input_mode_configure_parser == NULL) {
        m_input_mode_configure_parser = Input_Mode_Configure_Parser::get_instance();
        m_input_mode_configure_parser->init();
    }

    SCLLOG(SclLog::MESSAGE, "init default_configure\n\n");
    if (m_default_configure_parser == NULL) {
        m_default_configure_parser = Default_Configure_Parser::get_instance();
        m_default_configure_parser->init();
    }

    SCLLOG(SclLog::MESSAGE, "init modifier_decoration\n\n");
    if (m_modifier_decoration_parser == NULL) {
        m_modifier_decoration_parser = Modifier_decoration_Parser::get_instance();
        m_modifier_decoration_parser->init();
    }
    SCLLOG(SclLog::MESSAGE, "init label_properties\n\n");
    if (m_label_properties_parser == NULL) {
        m_label_properties_parser = Label_properties_Parser::get_instance();
        m_label_properties_parser->init();
    }

    SCLLOG(SclLog::MESSAGE, "init autopopup_configure\n\n");
    if (m_autopopup_configure_parser == NULL) {
        m_autopopup_configure_parser = AutoPopup_Configure_Parser::get_instance();
        m_autopopup_configure_parser->init();
    }
    SCLLOG(SclLog::MESSAGE, "init magnifier_configure\n\n");
    if (m_magnifier_configure_parser == NULL) {
        m_magnifier_configure_parser = Magnifier_Configure_Parser::get_instance();
        m_magnifier_configure_parser->init();
    }

    SCLLOG(SclLog::MESSAGE, "init nine_patch_file_list\n\n");
    if (m_nine_patch_file_list_parser == NULL) {
        m_nine_patch_file_list_parser = Nine_patch_file_list_Parser::get_instance();
        m_nine_patch_file_list_parser->init();
    }

    SCLLOG(SclLog::MESSAGE, "init layout\n\n");
    if (m_layout_parser == NULL) {
        m_layout_parser = Layout_Parser::get_instance();
        m_layout_parser->init();
    }

    SCLLOG(SclLog::MESSAGE, "init Text XML resources OK.\n\n");
}


void XMLResource::load(int layout_id) {
    m_layout_parser->load(layout_id);
}
void XMLResource::unload() {
    m_layout_parser->unload();
}
bool XMLResource::loaded(int layout_id) {
    return m_layout_parser->loaded(layout_id);
}

SclNinePatchInfo* XMLResource::get_nine_patch_list() {
    return m_nine_patch_file_list_parser->get_nine_patch_list();
}
XMLFiles& XMLResource::get_xml_files() {
    return m_main_entry_parser->get_xml_files();
}
int XMLResource::get_labelproperty_size() {
    return m_label_properties_parser->get_size();
}
bool XMLResource::get_nine_patch_info(const char *filename, SclNinePatchInfo *info) {
    return m_nine_patch_file_list_parser->get_nine_patch_info(filename, info);
}
int XMLResource::get_inputmode_id(const char *name) {
    return m_input_mode_configure_parser->get_inputmode_id(name);
}
const char* XMLResource::get_inputmode_name(int id) {
    return m_input_mode_configure_parser->get_inputmode_name(id);
}
int XMLResource::get_inputmode_size() {
    return m_input_mode_configure_parser->get_inputmode_size();
}
PSclInputModeConfigure XMLResource::get_input_mode_configure_table() {
    return m_input_mode_configure_parser->get_input_mode_configure_table();
}
int XMLResource::get_layout_id(const char *name) {
    return m_layout_parser->get_layout_index(name);
}
int XMLResource::get_layout_size() {
    return m_layout_parser->get_layout_size();
}
int XMLResource::get_modifier_decoration_id(const char *name) {
    return m_modifier_decoration_parser->get_modifier_decoration_id(name);
}
PSclLayout XMLResource::get_layout_table() {
    return m_layout_parser->get_layout_table();
}
PSclLayoutKeyCoordinatePointerTable XMLResource::get_key_coordinate_pointer_frame() {
    return m_layout_parser->get_key_coordinate_pointer_frame();
}
PSclModifierDecoration XMLResource::get_modifier_decoration_table() {
    return m_modifier_decoration_parser->get_modifier_decoration_table();
}

PSclLabelPropertiesTable XMLResource::get_label_properties_frame() {
    return m_label_properties_parser->get_label_properties_frame();
}

PSclDefaultConfigure XMLResource::get_default_configure() {
    return m_default_configure_parser->get_default_configure();
}

PSclAutoPopupConfigure XMLResource::get_autopopup_configure() {
    return m_autopopup_configure_parser->get_autopopup_configure();
}

PSclMagnifierWndConfigure XMLResource::get_magnifier_configure() {
    return m_magnifier_configure_parser->get_magnifier_configure();
}
