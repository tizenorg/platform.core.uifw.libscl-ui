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

#include "sclgwes.h"
#include "sclutils.h"
#include "sclresource.h"

#ifndef __SCL_RESOURCE_CACHE_H__
#define __SCL_RESOURCE_CACHE_H__

//SCL_BEGIN_DECLS

namespace scl
{

typedef struct _SclButtonContext {
    sclboolean used;
    sclbyte multikeyIdx;
    sclboolean toggled;
    SCLButtonState state;
} SclButtonContext;
/**
* @brief The class to store current state of key layout
*
* This class implements all functions for working as a soft-based keyboard
* In side of ISE developer, they can modify it by their requirements.
*/
class CSCLResourceCache
{
private:
    CSCLResourceCache();

public:
    ~CSCLResourceCache();

    static CSCLResourceCache* get_instance();

    sclboolean init();
    sclboolean recompute_layout(sclwindow window);

    const SclLayout* get_cur_layout(sclwindow window) const;
    SclLayoutKeyCoordinate* get_cur_layout_key_coordinate(sclwindow window, sclbyte key_index);

    const SclLabelProperties* get_label_properties(sclchar *label_type, sclbyte index) const;

    SclButtonContext* get_cur_button_context(sclwindow window, sclbyte key_index);

    sclboolean clear_private_keys();
    sclint add_private_key(SclPrivateKeyProperties* properties, sclboolean *fNeedInvaild);
    sclboolean remove_private_key(sclint id);

    sclboolean copy_from_privatekeyproperties(const SclPrivateKeyProperties* privProperties, SclLayoutKeyCoordinate* coordinate);
    sclboolean copy_to_privatekeyproperties(const SclLayoutKeyCoordinate *coordinate, SclPrivateKeyProperties* privProperties);
    sclboolean copy_privatekeyproperties(const SclPrivateKeyProperties* source, SclPrivateKeyProperties* target);
    sclboolean clear_privatekeyproperties(SclPrivateKeyProperties* privProperties);

    sclboolean set_cur_themename(const sclchar *themename);
    const sclchar* get_cur_themename();

    sclint set_private_key(SclPrivateKeyProperties* properties, sclboolean fRedraw, sclboolean fPendingUpdate);
    sclint set_private_key(const sclchar* custom_id, sclchar* label, sclchar* imagelabel[SCL_BUTTON_STATE_MAX], sclchar* imagebg[SCL_BUTTON_STATE_MAX], sclulong key_event, sclchar *key_value, sclboolean fRedraw, sclboolean fPendingUpdate);
    void unset_private_key(sclshort input_mode_index, sclbyte layout_index, sclbyte key_index);
    void unset_private_key(const sclchar* custom_id);

    void clone_keyproperties(SclPrivateKeyProperties* priv, sclbyte input_mode_index, sclbyte layout_index, sclbyte key_index);

    void enable_button(const sclchar* custom_id, sclboolean enabled);

    void set_string_substitution(const sclchar *original, const sclchar *substitute);
    void unset_string_substitution(const sclchar *original);
    const sclchar* find_substituted_string(const sclchar *original);

    void set_custom_starting_coordinates(sclint x, sclint y);
    SclPoint get_custom_starting_coordinates();

    void set_custom_starting_coordinates_option(SCLStartingCoordinatesOption option);
    SCLStartingCoordinatesOption get_custom_starting_coordinates_option();
private:
    sclboolean resize_resource_elements_by_resolution();
    sclboolean resize_layout_by_resolution(sclbyte layout_index, sclboolean resize_key_only = FALSE);
    sclboolean change_by_privatekey(const sclbyte input_mode_index, const sclbyte layout_index, const sclbyte key_index, SclLayoutKeyCoordinate* coordinate);

    void generate_autopopup_layout(const SclLayoutKeyCoordinate *coordinate,
                                   SclLayout *pCurLayout,
                                   SclLayoutKeyCoordinate (*pCurLayoutKeyCoordinates)[MAX_KEY],
                                   SclButtonContext (*pCurButtonContext)[MAX_KEY]);

protected:

    SclLayout mCurBaseLayout;
    SclLayoutKeyCoordinate mCurBaseLayoutKeyCoordinates[MAX_KEY];

    SclButtonContext mCurBaseButtonContext[MAX_KEY];

    SclLayout mCurPopupLayout[MAX_POPUP_WINDOW];
    SclLayoutKeyCoordinate mCurPopupLayoutKeyCoordinates[MAX_POPUP_WINDOW][MAX_KEY];

    SclButtonContext mCurPopupButtonContext[MAX_POPUP_WINDOW][MAX_KEY];

    SclPrivateKeyProperties mPrivateKeyProperties[MAX_PRIVATE_KEY];
    std::string mDisabledKeyList[MAX_DISABLED_KEY];
    std::map<std::string, std::string> mStringSubstitutor;

    sclchar mCurThemename[_POSIX_PATH_MAX];

    SclPoint mCurStartingCoordinates;
    SCLStartingCoordinatesOption mCurStartingCoordinatesOption;
};

}

//SCL_END_DECLS

#endif
