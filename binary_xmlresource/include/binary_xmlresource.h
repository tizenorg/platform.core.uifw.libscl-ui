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

#ifndef __BIN_XMLRESOURCE__
#define __BIN_XMLRESOURCE__

#include <limits.h>
#include <libxml/parser.h>
#include "input_mode_configure_bin_parser.h"
#include "layout_bin_parser.h"
#include "key_coordinate_frame_bin_parser.h"
#include "modifier_decoration_bin_parser.h"
#include "label_properties_bin_parser.h"
#include "default_configure_bin_parser.h"
#include "autopopup_configure_bin_parser.h"
#include "magnifier_configure_bin_parser.h"
#include "nine_patch_file_list_bin_parser.h"
#include "sclres.h"
struct resource_info_t{
    int offset;
    int size;
};


enum{
    ALLDATA = 0,
    METADATA,
    INPUT_MODE_CONFIGURE,
    LAYOUT,
    KEY_COORDINATE_FRAME,
    LABEL_PROPERTIES_FRAME,
    MODIFIER_DECORATION,
    DEFAULT_CONFIGURE,
    AUTOPOPUP_CONFIGURE,
    MAGNIFIER_CONFIGURE,
    NINE_PATCH,
    STRING,
    MAX_DATATYPE
};
namespace binary_xmlresource{
class BinResource: public sclres::SclRes{
    public:
    ~BinResource();
    static BinResource* get_instance();
    void init(const char *entry_filepath);

    /* These functions are for dynamic (lazy) loading layouts */
    void load(int layout_id);
    void unload();
    bool loaded(int layout_id);

    public:
    PSclInputModeConfigure get_input_mode_configure_table();
    PSclLayout get_layout_table();
    PSclLayoutKeyCoordinatePointerTable get_key_coordinate_pointer_frame();
    PSclModifierDecoration get_modifier_decoration_table();
    PSclLabelPropertiesTable get_label_properties_frame();
    PSclDefaultConfigure get_default_configure();
    PSclAutoPopupConfigure get_autopopup_configure();
    PSclMagnifierWndConfigure get_magnifier_configure();

    int get_inputmode_id(const char *name);
    const char* get_inputmode_name(int id);
    int get_inputmode_size();
    int get_layout_id(const char* name);
    int get_layout_size();
    int get_labelproperty_size();
    int get_modifier_decoration_id(const char *name);
    bool get_nine_patch_info(const char *filename, SclNinePatchInfo *info);

    const char* get_name() {
        return "binary_xmlparser";
    }

    void destroy();
    struct resource_info_t m_info[MAX_DATATYPE];
    private:
    BinResource();

    private:
    BinInputModeConfigParser *m_input_mode_configure_parser;
    BinLayoutParser *m_layout_parser;
    BinKeyCoordFrameParser *m_key_coordinate_frame_parser;
    BinModifierDecorationParser *m_modifier_decoration_parser;
    BinLabelPropertyParser *m_label_properties_parser;
    BinDefaultConfigParser *m_default_configure_parser;
    BinAutoPopupConfigParser *m_autopopup_configure_parser;
    BinMagnifierConfigParser *m_magnifier_configure_parser;
    BinNinePatchFileParser *m_nine_patch_file_list_parser;
};
}

#endif

