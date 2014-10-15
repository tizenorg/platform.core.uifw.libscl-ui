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

#ifndef __SCLRES__
#define __SCLRES__
#include "sclres_type.h"
#include <string>

namespace sclres{
class SclRes{
public:
    SclRes();
    virtual ~SclRes();

    /* Inform the SclRes where the resource files are */
    virtual void set_resource_directory(const char *directory);
    /* Returns the current filepath where the resource files are */
    virtual const char* get_resource_directory();

public: /* These are the functions need to be implemented by the descendants of SclRes */

    /* Initialize SclRes, with the given entry file that exists under the resource directory */
    virtual void init(const char *entry_filename) = 0;

    /* These functions are for dynamic (lazy) loading layouts */
    virtual void load(int layout_id) = 0;
    virtual void unload() = 0;
    virtual bool loaded(int layout_id) = 0;

    /* Functions returning the SCL structure containing the information described in resource files */
    virtual PSclInputModeConfigure get_input_mode_configure_table() = 0;
    virtual PSclLayout get_layout_table() = 0;
    virtual PSclLayoutKeyCoordinatePointerTable get_key_coordinate_pointer_frame() = 0;
    virtual PSclModifierDecoration get_modifier_decoration_table() = 0;
    virtual PSclLabelPropertiesTable get_label_properties_frame() = 0;
    virtual PSclDefaultConfigure get_default_configure() = 0;
    virtual PSclAutoPopupConfigure get_autopopup_configure() = 0;
    virtual PSclMagnifierWndConfigure get_magnifier_configure() = 0;

    /* Functions returning specific data described in resource files */
    virtual int get_inputmode_id(const char* name) = 0;
    virtual const char* get_inputmode_name(int id) = 0;
    virtual int get_inputmode_size() = 0;
    virtual int get_layout_id(const char* name) = 0;
    virtual int get_layout_size() = 0;
    virtual int get_labelproperty_size() = 0;
    virtual int get_modifier_decoration_id(const char *name) = 0;
    virtual bool get_nine_patch_info(const char *filename, SclNinePatchInfo *info) = 0;

    /* Function returning the name of current SclRes instance */
    virtual const char* get_name() = 0;

private:
    std::string m_resource_directory;
};
}

#endif

