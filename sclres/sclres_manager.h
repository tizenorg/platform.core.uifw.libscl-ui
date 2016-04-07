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

#ifndef __SCLRES_MANAGER__
#define __SCLRES_MANAGER__
#include "sclres_type.h"

#ifndef LIBSCL_EXPORT_API
#define LIBSCL_EXPORT_API 
#endif // LIBSCL_EXPORT_API


class LIBSCL_EXPORT_API SclResParserManager{
    public:
        ~SclResParserManager();
        static SclResParserManager* get_instance();

    public:
        void init(const SCLParserType parser_type, const char *entry_filepath);

        /* These functions are for dynamic (lazy) loading layouts */
        void load(int layout_id);
        void unload();
        bool loaded(int layout_id);

        PSclInputModeConfigure get_input_mode_configure_table();
        PSclLayout get_layout_table();
        PSclLayoutKeyCoordinatePointerTable get_key_coordinate_pointer_frame();
        PSclModifierDecoration get_modifier_decoration_table();
        PSclLabelPropertiesTable get_label_properties_frame();
        PSclDefaultConfigure get_default_configure();
        PSclAutoPopupConfigure get_autopopup_configure();
        PSclMagnifierWndConfigure get_magnifier_configure();

        const char* get_resource_directory();
        int get_inputmode_id(const char* name);
        const char* get_inputmode_name(int id);
        int get_inputmode_size();
        int get_layout_size();
        int get_layout_id(const char *name);
        int get_labelproperty_size();
        int get_modifier_decoration_id(const char *name);
        bool get_nine_patch_info(const char *filename, SclNinePatchInfo *info);

        const char* get_name();
        void destroy();

    private:
        SclResParserManager();
        bool m_initialized;
};

#endif
