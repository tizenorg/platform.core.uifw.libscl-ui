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

#include "sclres_manager.h"
#include "xmlresource.h"
#include "binary_xmlresource.h"
#include <assert.h>
using namespace xmlresource;
using namespace binary_xmlresource;
using namespace sclres;
#include "put_record.h"

SclResParserManager* SclResParserManager::m_instance = NULL;
SclResParserManager::~SclResParserManager() {
    if (m_cur)
        delete m_cur;
    m_cur = NULL;
}

SclResParserManager*
SclResParserManager::get_instance() {
    if (m_instance == NULL) {
        m_instance = new SclResParserManager();
    }
    return m_instance;
}

SclResParserManager::SclResParserManager() {
    m_cur = NULL;
}

void
SclResParserManager::init(const SCLParserType parser_type, const char *entry_filepath) {
    if (parser_type == SCL_PARSER_TYPE_XML) {
        printf("Use text xml\n");
        m_cur = XMLResource::get_instance();
    }
    else if (parser_type == SCL_PARSER_TYPE_BINARY_XML) {
        printf("Use binary xml\n");
        m_cur = BinXmlResource::get_instance();
    }

    /* Let's acquire the directory information from filepath */
    std::string str = entry_filepath;
    size_t found;
    found = str.find_last_of("/\\");

    std::string filepath = str.substr(0, found);
    std::string filename = str.substr(found + 1);

    //assert(m_cur != NULL);
    if (m_cur != NULL) {
        /* Assume the directory where the main entry file exists, is the default resource directory */
        m_cur->set_resource_directory(filepath.c_str());
        m_cur->init(filename.c_str());
#ifdef __SCL_TXT_DEBUG
        put_autopopup_configure(PARSER, *(m_cur->get_autopopup_configure()));
        put_default_configure(PARSER, *(m_cur->get_default_configure()));
        put_input_mode_configure_table(PARSER, m_cur->get_input_mode_configure_table());
        put_key_coordinate_frame(PARSER, m_cur->get_key_coordinate_pointer_frame());
        put_label_properties_frame(PARSER, m_cur->get_label_properties_frame());
        put_layout_table(PARSER, m_cur->get_layout_table());
        put_magnifier_wnd_configure(PARSER, *(m_cur->get_magnifier_configure()));
        put_modifier_decoration(PARSER, m_cur->get_modifier_decoration_table());
        //put_nine_patch_info(PARSER, m_cur->get_nine_patch_file_list());
#endif
    }
}

void
SclResParserManager::load(int layout_id) {
    if (m_cur) {
        m_cur->load(layout_id);
    }
}

void
SclResParserManager::unload() {
    if (m_cur) {
        m_cur->unload();
    }
}

bool
SclResParserManager::loaded(int layout_id) {
    if (m_cur == NULL) return false;

    return m_cur->loaded(layout_id);
}

PSclInputModeConfigure
SclResParserManager::get_input_mode_configure_table() {
    if (m_cur == NULL) return NULL;

    return m_cur->get_input_mode_configure_table();
}

PSclLayout
SclResParserManager::get_layout_table() {
    if (m_cur == NULL) return NULL;

    return m_cur->get_layout_table();
}

PSclLayoutKeyCoordinatePointerTable
SclResParserManager::get_key_coordinate_pointer_frame() {
    if (m_cur == NULL) return NULL;

    return m_cur->get_key_coordinate_pointer_frame();
}

PSclModifierDecoration
SclResParserManager::get_modifier_decoration_table() {
    if (m_cur == NULL) return NULL;

    return m_cur->get_modifier_decoration_table();
}

PSclLabelPropertiesTable
SclResParserManager::get_label_properties_frame() {
    if (m_cur == NULL) return NULL;

    return m_cur->get_label_properties_frame();
}

PSclDefaultConfigure
SclResParserManager::get_default_configure() {
    if (m_cur == NULL) return NULL;

    return m_cur->get_default_configure();
}

PSclAutoPopupConfigure
SclResParserManager::get_autopopup_configure() {
    if (m_cur == NULL) return NULL;

    return m_cur->get_autopopup_configure();
}

PSclMagnifierWndConfigure
SclResParserManager::get_magnifier_configure() {
    if (m_cur == NULL) return NULL;

    return m_cur->get_magnifier_configure();
}

const char*
SclResParserManager::get_resource_directory() {
    if (m_cur == NULL) return NULL;

    return m_cur->get_resource_directory();
}

int
SclResParserManager::get_inputmode_id(const char *name) {
    if (m_cur == NULL) return -1;

    return m_cur->get_inputmode_id(name);
}

const char*
SclResParserManager::get_inputmode_name(int id) {
    if (m_cur == NULL) return NULL;

    return m_cur->get_inputmode_name(id);
}

int
SclResParserManager::get_inputmode_size() {
    if (m_cur == NULL) return 0;

    return m_cur->get_inputmode_size();
}

int
SclResParserManager::get_layout_size() {
    if (m_cur == NULL) return 0;

    return m_cur->get_layout_size();
}

int
SclResParserManager::get_layout_id(const char *name) {
    if (m_cur == NULL) return -1;

    return m_cur->get_layout_id(name);
}

int
SclResParserManager::get_labelproperty_size() {
    if (m_cur == NULL) return 0;

    return m_cur->get_labelproperty_size();
}

int
SclResParserManager::get_modifier_decoration_id(const char *name) {
    if (m_cur == NULL) return -1;

    return m_cur->get_modifier_decoration_id(name);
}

bool
SclResParserManager::get_nine_patch_info(const char *filename, SclNinePatchInfo *info) {
    if (m_cur == NULL) return false;

    return m_cur->get_nine_patch_info(filename, info);
}

const char*
SclResParserManager::name() {
    if (m_cur == NULL) return NULL;

    return m_cur->name();
}
