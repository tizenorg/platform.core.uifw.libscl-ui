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

#include <libxml/parser.h>
#include <assert.h>
#include <limits.h>
#include <string.h>
#include "xmlresource.h"
#include "simple_debug.h"

using namespace xmlresource;

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

XMLResource*
XMLResource::get_instance() {
    static XMLResource instance;
    return &instance;
}

static void
get_layout_files(char **layout_files, int* size) {
    if (layout_files == NULL) return;
    int layout_file_size = 0;

    InputModeConfigParser *input_mode_configure_parser = InputModeConfigParser::get_instance();
    for(int inputmode = 0; inputmode < MAX_SCL_INPUT_MODE; inputmode++) {
        int empty_index;
        const sclchar* cur_layout_file;
        PSclInputModeConfigure configure_table = input_mode_configure_parser->get_input_mode_configure_table();
        SclInputModeConfigure& cur_input_mode_configure = configure_table[inputmode];

        for(int display = 0; display < DISPLAYMODE_MAX; display++) {
            char *cur_layout_file = cur_input_mode_configure.layouts[display];
            if (cur_layout_file == NULL) {
                continue;
            }
            bool found = false;
            int empty_index = NOT_USED;

            for(int checkidx = 0; checkidx < MAX_SCL_LAYOUT; checkidx++) {
                // not found (assert array end with null)
                if (layout_files[checkidx] == NULL) {
                    empty_index = checkidx;
                    break;
                }
                // found layout
                else if (strncmp(layout_files[checkidx], cur_layout_file, _POSIX_PATH_MAX) == 0) {
                    found = true;
                    break;
                }
            }

            if (found) {
                continue;
            }

            if (empty_index == NOT_USED) {
                SCLLOG(SclLog::ERROR, "No space for new layout %s\n", cur_layout_file);
            }

            if (empty_index != NOT_USED) {
                layout_file_size++;
                layout_files[empty_index] = strdup(cur_layout_file);
            }
        }
    }
    if (size) {
        *size = layout_file_size;
    }
}
void
XMLResource::init(const char *entry_filepath) {
    if (m_main_entry_parser == NULL) {
        m_main_entry_parser = MainEntryParser::get_instance();
        char input_file[_POSIX_PATH_MAX] = {0};
        snprintf(input_file, _POSIX_PATH_MAX, "%s/%s", get_resource_directory(), entry_filepath);
        if ( 0 != m_main_entry_parser->init(input_file)) {
            /* main entry is necessary */
            SCLLOG(SclLog::ERROR, "main entry init");
        }
    }

    /* get each type of xml file name */
    XMLFiles& xml_files = m_main_entry_parser->get_xml_files();

    SCLLOG(SclLog::MESSAGE, "init inputmode configure\n\n");
    if (m_input_mode_configure_parser == NULL) {
        m_input_mode_configure_parser = InputModeConfigParser::get_instance();
        char input_file[_POSIX_PATH_MAX] = {0};
        snprintf(input_file, _POSIX_PATH_MAX, "%s/%s", get_resource_directory(), xml_files.input_mode_configure);
        if (0 != m_input_mode_configure_parser->init(input_file)) {
            /* input mode configure is necessary */
            SCLLOG(SclLog::ERROR, "input mode configure init");
        }
    }

    SCLLOG(SclLog::MESSAGE, "init default_configure\n\n");
    if (m_default_configure_parser == NULL) {
        m_default_configure_parser = DefaultConfigParser::get_instance();
        char input_file[_POSIX_PATH_MAX] = {0};
        snprintf(input_file, _POSIX_PATH_MAX, "%s/%s", get_resource_directory(), xml_files.default_configure);

        if (0 != m_default_configure_parser->init(input_file)) {
            /* default configure is not necessary */
            SCLLOG(SclLog::WARNING, "default configure init");
        }
    }

    SCLLOG(SclLog::MESSAGE, "init modifier_decoration\n\n");
    if (m_modifier_decoration_parser == NULL) {
        m_modifier_decoration_parser = ModifierDecorationParser::get_instance();
        char input_file[_POSIX_PATH_MAX] = {0};
        snprintf(input_file, _POSIX_PATH_MAX, "%s/%s", get_resource_directory(), xml_files.modifier_decoration);

        if (0 != m_modifier_decoration_parser->init(input_file)) {
            /* modifier decoration is not necessary */
            SCLLOG(SclLog::WARNING, "modifier decoration init");
        }
    }
    SCLLOG(SclLog::MESSAGE, "init label_properties\n\n");
    if (m_label_properties_parser == NULL) {
        m_label_properties_parser = LabelPropertyParser::get_instance();
        char input_file[_POSIX_PATH_MAX] = {0};
        snprintf(input_file, _POSIX_PATH_MAX, "%s/%s", get_resource_directory(), xml_files.key_label_property);

        if (0 != m_label_properties_parser->init(input_file)) {
            /* label properties is not necessary */
            SCLLOG(SclLog::WARNING, "label properties init");
        }
    }

    SCLLOG(SclLog::MESSAGE, "init autopopup_configure\n\n");
    if (m_autopopup_configure_parser == NULL) {
        m_autopopup_configure_parser = AutoPopupConfigParser::get_instance();
        char input_file[_POSIX_PATH_MAX] = {0};
        snprintf(input_file, _POSIX_PATH_MAX, "%s/%s", get_resource_directory(), xml_files.autopopup_configure);
        if (0 != m_autopopup_configure_parser->init(input_file)) {
            /* autopopup configure is not necessary */
            SCLLOG(SclLog::WARNING, "autopopup configure init");
        }
    }
    SCLLOG(SclLog::MESSAGE, "init magnifier_configure\n\n");
    if (m_magnifier_configure_parser == NULL) {
        m_magnifier_configure_parser = MagnifierConfigParser::get_instance();
        char input_file[_POSIX_PATH_MAX] = {0};
        snprintf(input_file, _POSIX_PATH_MAX, "%s/%s", get_resource_directory(), xml_files.magnifier_configure);

        if (0 != m_magnifier_configure_parser->init(input_file)) {
            /* magnifier configure is not necessary */
            SCLLOG(SclLog::WARNING, "magnifier configure init");
        }
    }

    SCLLOG(SclLog::MESSAGE, "init nine_patch_file_list\n\n");
    if (m_nine_patch_file_list_parser == NULL) {
        m_nine_patch_file_list_parser = NinePatchFileParser::get_instance();
        char input_file[_POSIX_PATH_MAX] = {0};
        snprintf(input_file, _POSIX_PATH_MAX, "%s/%s", get_resource_directory(), xml_files.nine_patch_file_list);

        if (0 != m_nine_patch_file_list_parser->init(input_file)) {
            /* nine patch file list is not necessary */
            SCLLOG(SclLog::WARNING, "nine patch file list init");
        }
    }

    SCLLOG(SclLog::MESSAGE, "init layout\n\n");
    if (m_layout_parser == NULL) {
        m_layout_parser = LayoutParser::get_instance();
        char **layout_files = NULL;
        int layout_file_size = 0;

        layout_files = (char**)malloc(sizeof(char*) * MAX_SCL_LAYOUT);
        if (layout_files == NULL) {
            SCLLOG(SclLog::ERROR, "layout init");
            exit(1);
        }
        memset(layout_files, 0, sizeof(char*) * MAX_SCL_LAYOUT);

        get_layout_files(layout_files, &layout_file_size);
        if ( 0 != m_layout_parser->init(get_resource_directory(), layout_files, layout_file_size)) {
            /* layout is necessary */
            SCLLOG(SclLog::ERROR, "layout init");
        }
        for (int i = 0; i < layout_file_size; ++i) {
            free(layout_files[i]);
        }

        free(layout_files);
    }

    SCLLOG(SclLog::MESSAGE, "init Text XML resources OK.\n\n");
}


void
XMLResource::load(int layout_id) {
    m_layout_parser->load(layout_id);
}

void
XMLResource::unload() {
    m_layout_parser->unload();
}

bool
XMLResource::loaded(int layout_id) {
    return m_layout_parser->loaded(layout_id);
}

SclNinePatchInfo*
XMLResource::get_nine_patch_list() {
    return m_nine_patch_file_list_parser->get_nine_patch_list();
}

int
XMLResource::get_labelproperty_size() {
    return m_label_properties_parser->get_size();
}

bool
XMLResource::get_nine_patch_info(const char *filename, SclNinePatchInfo *info) {
    return m_nine_patch_file_list_parser->get_nine_patch_info(filename, info);
}

int
XMLResource::get_inputmode_id(const char *name) {
    return m_input_mode_configure_parser->get_inputmode_id(name);
}

const char*
XMLResource::get_inputmode_name(int id) {
    return m_input_mode_configure_parser->get_inputmode_name(id);
}

int
XMLResource::get_inputmode_size() {
    return m_input_mode_configure_parser->get_inputmode_size();
}

PSclInputModeConfigure
XMLResource::get_input_mode_configure_table() {
    return m_input_mode_configure_parser->get_input_mode_configure_table();
}

int
XMLResource::get_layout_id(const char *name) {
    return m_layout_parser->get_layout_index(name);
}

int
XMLResource::get_layout_size() {
    return m_layout_parser->get_layout_size();
}

int
XMLResource::get_modifier_decoration_id(const char *name) {
    return m_modifier_decoration_parser->get_modifier_decoration_id(name);
}

PSclLayout
XMLResource::get_layout_table() {
    return m_layout_parser->get_layout_table();
}

PSclLayoutKeyCoordinatePointerTable
XMLResource::get_key_coordinate_pointer_frame() {
    return m_layout_parser->get_key_coordinate_pointer_frame();
}

PSclModifierDecoration
XMLResource::get_modifier_decoration_table() {
    return m_modifier_decoration_parser->get_modifier_decoration_table();
}

PSclLabelPropertiesTable
XMLResource::get_label_properties_frame() {
    return m_label_properties_parser->get_label_properties_frame();
}

PSclDefaultConfigure
XMLResource::get_default_configure() {
    return m_default_configure_parser->get_default_configure();
}

PSclAutoPopupConfigure
XMLResource::get_autopopup_configure() {
    return m_autopopup_configure_parser->get_autopopup_configure();
}

PSclMagnifierWndConfigure
XMLResource::get_magnifier_configure() {
    return m_magnifier_configure_parser->get_magnifier_configure();
}
