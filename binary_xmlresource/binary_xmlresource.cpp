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

#include "binary_xmlresource.h"
#include "string_bin_parser.h"
#include "metadata_bin_parser.h"
#include "parserinfo_provider.h"
#include "string_provider.h"
#include "metadata_provider.h"
#include <assert.h>
#include "simple_debug.h"

using namespace binary_xmlresource;

BinResource::BinResource() {
    m_input_mode_configure_parser = NULL;
    m_layout_parser = NULL;
    m_key_coordinate_frame_parser = NULL;
    m_modifier_decoration_parser = NULL;
    m_label_properties_parser = NULL;
    m_default_configure_parser = NULL;
    m_autopopup_configure_parser = NULL;
    m_magnifier_configure_parser = NULL;
    m_nine_patch_file_list_parser = NULL;
}

BinResource::~BinResource() {
    m_input_mode_configure_parser = NULL;
    m_layout_parser = NULL;
    m_key_coordinate_frame_parser = NULL;
    m_modifier_decoration_parser = NULL;
    m_label_properties_parser = NULL;
    m_default_configure_parser = NULL;
    m_autopopup_configure_parser = NULL;
    m_magnifier_configure_parser = NULL;
    m_nine_patch_file_list_parser = NULL;
}

BinResource* BinResource::get_instance() {
    static BinResource instance;
    return &instance;
}

void BinResource::init(const char *entry_filepath) {
    SCLLOG_TIME_BEGIN();

    char path[_POSIX_PATH_MAX] = {0};


    snprintf(path, _POSIX_PATH_MAX, "%s/%s", get_resource_directory(), "sclresource.bin");

    FileStorage storageAllData;
    if ( storageAllData.loadFile(path) <= 0) {
        return;
    }

    const int SIZE_WIDTH = 8;
    const int OFFSET_WIDTH = 8;
    int size_table[MAX_DATATYPE] = {0};
    int offset_table[MAX_DATATYPE] = {0};
    {
        FileStorage headerStorage;
        headerStorage.get_storage(storageAllData, 0, MAX_DATATYPE * (SIZE_WIDTH + OFFSET_WIDTH));
        for (int i = ALLDATA; i < MAX_DATATYPE; ++i) {
            size_table[i] = headerStorage.get<sint_t>(SIZE_WIDTH);
            offset_table[i] = headerStorage.get<sint_t>(OFFSET_WIDTH);
        }
    }

    for ( int i = ALLDATA; i < MAX_DATATYPE; ++i) {
        m_info[i].offset = offset_table[i];
        m_info[i].size = size_table[i];
    }

    String_Bin_Parser stringBinParser(storageAllData, m_info[STRING].offset, m_info[STRING].size);
    Metadata_Bin_Parser metadataBinParser(storageAllData, m_info[METADATA].offset, m_info[METADATA].size);

    String_Provider stringProvider(&stringBinParser);
    Metadata_Provider metadataProvider(&metadataBinParser);
    ParserInfo_Provider parser_info_provider(&metadataProvider, &stringProvider);

    if (m_input_mode_configure_parser == NULL) {
        m_input_mode_configure_parser = BinInputModeConfigParser::get_instance();
        m_input_mode_configure_parser->init(storageAllData, m_info[INPUT_MODE_CONFIGURE].offset, m_info[INPUT_MODE_CONFIGURE].size, &parser_info_provider);
    }
    if (m_default_configure_parser == NULL) {
        m_default_configure_parser = BinDefaultConfigParser::get_instance();
        m_default_configure_parser->init(storageAllData, m_info[DEFAULT_CONFIGURE].offset, m_info[DEFAULT_CONFIGURE].size, &parser_info_provider);
    }
   if (m_key_coordinate_frame_parser == NULL) {
        m_key_coordinate_frame_parser = BinKeyCoordFrameParser::get_instance();
        m_key_coordinate_frame_parser->init(storageAllData, m_info[KEY_COORDINATE_FRAME].offset, m_info[KEY_COORDINATE_FRAME].size, &parser_info_provider);
    }

   if (m_layout_parser == NULL) {
        m_layout_parser = BinLayoutParser::get_instance();
        m_layout_parser->init(storageAllData, m_info[LAYOUT].offset, m_info[LAYOUT].size, &parser_info_provider);
    }
    if (m_modifier_decoration_parser == NULL) {
        m_modifier_decoration_parser = BinModifierDecorationParser::get_instance();
        m_modifier_decoration_parser->init(storageAllData, m_info[MODIFIER_DECORATION].offset, m_info[MODIFIER_DECORATION].size, &parser_info_provider);
    }
    if (m_label_properties_parser == NULL) {
        m_label_properties_parser = BinLabelPropertyParser::get_instance();
        m_label_properties_parser->init(storageAllData, m_info[LABEL_PROPERTIES_FRAME].offset, m_info[LABEL_PROPERTIES_FRAME].size, &parser_info_provider);
    }

    if (m_autopopup_configure_parser == NULL) {
        m_autopopup_configure_parser = BinAutoPopupConfigParser::get_instance();
        m_autopopup_configure_parser->init(storageAllData, m_info[AUTOPOPUP_CONFIGURE].offset, m_info[AUTOPOPUP_CONFIGURE].size, &parser_info_provider);
    }
    if (m_magnifier_configure_parser == NULL) {
        m_magnifier_configure_parser = BinMagnifierConfigParser::get_instance();
        m_magnifier_configure_parser->init(storageAllData, m_info[MAGNIFIER_CONFIGURE].offset, m_info[MAGNIFIER_CONFIGURE].size, &parser_info_provider);
    }

    if (m_nine_patch_file_list_parser == NULL) {
        m_nine_patch_file_list_parser = BinNinePatchFileParser::get_instance();
        m_nine_patch_file_list_parser->init(storageAllData, m_info[NINE_PATCH].offset, m_info[NINE_PATCH].size, &parser_info_provider);
    }

    SCLLOG_TIME_END("Parsing binary XML files");
}

void BinResource::load(int layout_id) {
    //m_layout_parser->load(layout_id);
    m_key_coordinate_frame_parser->load(layout_id);
}
void BinResource::unload() {
    //m_layout_parser->unload();
    m_key_coordinate_frame_parser->unload();
}
bool BinResource::loaded(int layout_id) {
    //return m_layout_parser->loaded(layout_id);
    return m_key_coordinate_frame_parser->loaded(layout_id);
}

int BinResource::get_labelproperty_size() {
    return m_label_properties_parser->get_size();
}
bool BinResource::get_nine_patch_info(const char *filename, SclNinePatchInfo *info) {
    return m_nine_patch_file_list_parser->get_nine_patch_info(filename, info);
}
int BinResource::get_inputmode_id(const char *name) {
    return m_input_mode_configure_parser->get_inputmode_id(name);
}
const char* BinResource::get_inputmode_name(int id) {
    return m_input_mode_configure_parser->get_inputmode_name(id);
}
int BinResource::get_inputmode_size() {
    return m_input_mode_configure_parser->get_inputmode_size();
}
PSclInputModeConfigure BinResource::get_input_mode_configure_table() {
    return m_input_mode_configure_parser->get_input_mode_configure_table();
}
int BinResource::get_layout_id(const char *name) {
    return m_layout_parser->get_layout_index(name);
}
int BinResource::get_layout_size() {
    return m_layout_parser->get_layout_size();
}
int BinResource::get_modifier_decoration_id(const char *name) {
    return m_modifier_decoration_parser->get_modifier_decoration_id(name);
}
PSclLayout BinResource::get_layout_table() {
    return m_layout_parser->get_layout_table();
}
PSclLayoutKeyCoordinatePointerTable BinResource::get_key_coordinate_pointer_frame() {
    return m_layout_parser->get_key_coordinate_pointer_frame();
}
PSclModifierDecoration BinResource::get_modifier_decoration_table() {
    return m_modifier_decoration_parser->get_modifier_decoration_table();
}

PSclLabelPropertiesTable BinResource::get_label_properties_frame() {
    return m_label_properties_parser->get_label_properties_frame();
}

PSclDefaultConfigure BinResource::get_default_configure() {
    return m_default_configure_parser->get_default_configure();
}

PSclAutoPopupConfigure BinResource::get_autopopup_configure() {
    return m_autopopup_configure_parser->get_autopopup_configure();
}

PSclMagnifierWndConfigure BinResource::get_magnifier_configure() {
    return m_magnifier_configure_parser->get_magnifier_configure();
}
