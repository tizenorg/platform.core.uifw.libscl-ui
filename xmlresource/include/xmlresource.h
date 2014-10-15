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

#ifndef __XMLRESOURCE__
#define __XMLRESOURCE__

#include "main_entry_parser.h"
#include "input_mode_configure_parser.h"
#include "layout_parser.h"
#include "modifier_decoration_parser.h"
#include "label_properties_parser.h"
#include "default_configure_parser.h"
#include "autopopup_configure_parser.h"
#include "magnifier_configure_parser.h"
#include "nine_patch_file_list_parser.h"
#include "sclres.h"

namespace xmlresource{
class XMLResource: public sclres::SclRes{
    public:
    ~XMLResource();
    static XMLResource* get_instance();
    void init(const char *entry_filepath);

    /* These functions are for dynamic (lazy) loading layouts */
    void load(int layout_id);
    void unload();
    bool loaded(int layout_id);

    public:
    XMLFiles& get_xml_files();
    PSclInputModeConfigure get_input_mode_configure_table();
    PSclLayout get_layout_table();
    PSclLayoutKeyCoordinatePointerTable get_key_coordinate_pointer_frame();
    PSclModifierDecoration get_modifier_decoration_table();
    PSclLabelPropertiesTable get_label_properties_frame();
    PSclDefaultConfigure get_default_configure();
    PSclAutoPopupConfigure get_autopopup_configure();
    PSclMagnifierWndConfigure get_magnifier_configure();
    SclNinePatchInfo* get_nine_patch_list();
    int get_inputmode_id(const char *name);
    const char* get_inputmode_name(int id);
    int get_inputmode_size();
    int get_layout_id(const char* name);
    int get_layout_size();
    int get_labelproperty_size();
    int get_modifier_decoration_id(const char *name);
    bool get_nine_patch_info(const char *filename, SclNinePatchInfo *info);

    const char* get_name() {
        return "xmlparser";
    }

    void destroy();
    private:
    XMLResource();

    private:
    MainEntryParser *m_main_entry_parser;
    InputModeConfigParser *m_input_mode_configure_parser;
    LayoutParser *m_layout_parser;
    ModifierDecorationParser *m_modifier_decoration_parser;
    LabelPropertyParser *m_label_properties_parser;
    DefaultConfigParser *m_default_configure_parser;
    AutoPopupConfigParser *m_autopopup_configure_parser;
    MagnifierConfigParser *m_magnifier_configure_parser;
    NinePatchFileParser *m_nine_patch_file_list_parser;
};
}

#endif

