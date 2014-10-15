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

#include "sclres_manager.h"
#include "xmlresource.h"
#include "binary_xmlresource.h"
#include <assert.h>
#include "simple_debug.h"
using namespace xmlresource;
using namespace binary_xmlresource;
using namespace sclres;
#include "put_record.h"

static sclres::SclRes *_current_parser = NULL;

SclResParserManager::~SclResParserManager() {
    _current_parser = NULL;
    m_initialized = false;
}

SclResParserManager*
SclResParserManager::get_instance() {
    static SclResParserManager instance;
    return &instance;
}

SclResParserManager::SclResParserManager() {
    _current_parser = NULL;
    m_initialized = false;
}

void
SclResParserManager::init(const SCLParserType parser_type, const char *entry_filepath) {
    if (!m_initialized) {
        if (parser_type == SCL_PARSER_TYPE_XML) {
            SCLLOG(SclLog::MESSAGE, "Use text xml\n");
            _current_parser = XMLResource::get_instance();
        }
        else if (parser_type == SCL_PARSER_TYPE_BINARY_XML) {
            SCLLOG(SclLog::MESSAGE, "Use binary xml\n");
            _current_parser = BinResource::get_instance();
        }

        /* Let's acquire the directory information from filepath */
        std::string str = entry_filepath;
        size_t found;
        found = str.find_last_of("/\\");

        std::string filepath = str.substr(0, found);
        std::string filename = str.substr(found + 1);

        //assert(_current_parser != NULL);
        if (_current_parser != NULL) {
            /* Assume the directory where the main entry file exists, is the default resource directory */
            _current_parser->set_resource_directory(filepath.c_str());
            _current_parser->init(filename.c_str());
#ifdef __SCL_TXT_DEBUG
            put_autopopup_configure(PARSER, *(_current_parser->get_autopopup_configure()));
            put_default_configure(PARSER, *(_current_parser->get_default_configure()));
            put_input_mode_configure_table(PARSER, _current_parser->get_input_mode_configure_table());
            put_key_coordinate_frame(PARSER, _current_parser->get_key_coordinate_pointer_frame());
            put_label_properties_frame(PARSER, _current_parser->get_label_properties_frame());
            put_layout_table(PARSER, _current_parser->get_layout_table());
            put_magnifier_wnd_configure(PARSER, *(_current_parser->get_magnifier_configure()));
            put_modifier_decoration(PARSER, _current_parser->get_modifier_decoration_table());
            //put_nine_patch_info(PARSER, _current_parser->get_nine_patch_file_list());
#endif
        }
        m_initialized = true;
    }
}

void
SclResParserManager::load(int layout_id) {
    if (_current_parser) {
        _current_parser->load(layout_id);
    }
}

void
SclResParserManager::unload() {
    if (_current_parser) {
        _current_parser->unload();
    }
}

bool
SclResParserManager::loaded(int layout_id) {
    if (_current_parser == NULL) return false;

    return _current_parser->loaded(layout_id);
}

PSclInputModeConfigure
SclResParserManager::get_input_mode_configure_table() {
    if (_current_parser == NULL) return NULL;

    return _current_parser->get_input_mode_configure_table();
}

PSclLayout
SclResParserManager::get_layout_table() {
    if (_current_parser == NULL) return NULL;

    return _current_parser->get_layout_table();
}

PSclLayoutKeyCoordinatePointerTable
SclResParserManager::get_key_coordinate_pointer_frame() {
    if (_current_parser == NULL) return NULL;

    return _current_parser->get_key_coordinate_pointer_frame();
}

PSclModifierDecoration
SclResParserManager::get_modifier_decoration_table() {
    if (_current_parser == NULL) return NULL;

    return _current_parser->get_modifier_decoration_table();
}

PSclLabelPropertiesTable
SclResParserManager::get_label_properties_frame() {
    if (_current_parser == NULL) return NULL;

    return _current_parser->get_label_properties_frame();
}

PSclDefaultConfigure
SclResParserManager::get_default_configure() {
    if (_current_parser == NULL) return NULL;

    return _current_parser->get_default_configure();
}

PSclAutoPopupConfigure
SclResParserManager::get_autopopup_configure() {
    if (_current_parser == NULL) return NULL;

    return _current_parser->get_autopopup_configure();
}

PSclMagnifierWndConfigure
SclResParserManager::get_magnifier_configure() {
    if (_current_parser == NULL) return NULL;

    return _current_parser->get_magnifier_configure();
}

const char*
SclResParserManager::get_resource_directory() {
    if (_current_parser == NULL) return NULL;

    return _current_parser->get_resource_directory();
}

int
SclResParserManager::get_inputmode_id(const char *name) {
    if (_current_parser == NULL) return -1;

    return _current_parser->get_inputmode_id(name);
}

const char*
SclResParserManager::get_inputmode_name(int id) {
    if (_current_parser == NULL) return NULL;

    return _current_parser->get_inputmode_name(id);
}

int
SclResParserManager::get_inputmode_size() {
    if (_current_parser == NULL) return 0;

    return _current_parser->get_inputmode_size();
}

int
SclResParserManager::get_layout_size() {
    if (_current_parser == NULL) return 0;

    return _current_parser->get_layout_size();
}

int
SclResParserManager::get_layout_id(const char *name) {
    if (_current_parser == NULL) return -1;

    return _current_parser->get_layout_id(name);
}

int
SclResParserManager::get_labelproperty_size() {
    if (_current_parser == NULL) return 0;

    return _current_parser->get_labelproperty_size();
}

int
SclResParserManager::get_modifier_decoration_id(const char *name) {
    if (_current_parser == NULL) return -1;

    return _current_parser->get_modifier_decoration_id(name);
}

bool
SclResParserManager::get_nine_patch_info(const char *filename, SclNinePatchInfo *info) {
    if (_current_parser == NULL) return false;

    return _current_parser->get_nine_patch_info(filename, info);
}

const char*
SclResParserManager::get_name() {
    if (_current_parser == NULL) return NULL;

    return _current_parser->get_name();
}
