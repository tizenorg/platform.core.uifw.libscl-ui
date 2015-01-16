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

#include <stdio.h>
#include <stdlib.h>
#include "sclresourcekeys.h"
#include "sclresourcecache.h"
#include "scldebug.h"
#include "sclcontext.h"

//#include "sclresource.h"
#include "scluibuilder.h"
#include "sclres_manager.h"
#include <assert.h>

//Includes for CSCLKeyFocusHandler
#include "sclkeyfocushandler.h"
using namespace scl;

CSCLResourceCache::CSCLResourceCache()
{
    SCL_DEBUG();

    memset(mCurThemename, 0x00, sizeof(mCurThemename));
}

CSCLResourceCache::~CSCLResourceCache()
{
    SCL_DEBUG();
}

CSCLResourceCache*
CSCLResourceCache::get_instance()
{
    static CSCLResourceCache instance;
    return &instance;
}


sclboolean
CSCLResourceCache::init()
{
    clear_private_keys();

    resize_resource_elements_by_resolution();

    mCurStartingCoordinates.x = 0;
    mCurStartingCoordinates.y = 0;
    mCurStartingCoordinatesOption = SCL_STARTING_COORDINATES_OPTION_ALL;

    return TRUE;
}

/**
 * Returns the current layout data
 */
const SclLayout*
CSCLResourceCache::get_cur_layout(sclwindow window) const
{
    SCL_DEBUG();

    const SclLayout *ret = NULL;
    CSCLWindows *windows = CSCLWindows::get_instance();

    if (windows) {
        if (windows->get_base_window() == window) {
            ret = &mCurBaseLayout;
        } else {
            sclbyte popupindex = windows->find_popup_window_index(window);
            scl_assert_return_false(popupindex < MAX_POPUP_WINDOW);
            if (popupindex < MAX_POPUP_WINDOW) {
                ret = &mCurPopupLayout[popupindex];
            }
        }
    }
    return ret;
}

/**
 * Translates each key's x,y,width,height by the current screen resolution
 * This func should be called when the class init / or after lazy loading
 */
sclboolean
CSCLResourceCache::resize_layout_by_resolution(sclbyte layout_index, sclboolean resize_key_only)
{
    sclint innerLoop;
    CSCLUtils *utils = CSCLUtils::get_instance();

    SclResParserManager *sclres_manager = SclResParserManager::get_instance();
    if (utils && sclres_manager) {
        const PSclDefaultConfigure sclres_default_configure = sclres_manager->get_default_configure();
        const PSclLayout sclres_layout = sclres_manager->get_layout_table();
        const PSclLayoutKeyCoordinatePointerTable sclres_layout_key_coordinate_pointer_frame =
            sclres_manager->get_key_coordinate_pointer_frame();

        assert(sclres_default_configure != NULL);
        assert(sclres_layout != NULL);
        assert(sclres_layout_key_coordinate_pointer_frame != NULL);

        sclboolean invert_display = FALSE;

        if (sclres_default_configure->auto_detect_landscape) {
            sclint width, height;
            utils->get_screen_resolution(&width, &height);
            /* If the width of screen is bigger than the height, switch portrait mode and landscape mode */
            if (width > height) {
                invert_display = TRUE;
            }
        }

        sclfloat scale_value_x, scale_value_y;
        if (invert_display) {
            if (sclres_layout[layout_index].display_mode == DISPLAYMODE_PORTRAIT) {
                scale_value_x = utils->get_scale_rate_y();
                scale_value_y = utils->get_scale_rate_x();
            } else {
                scale_value_x = utils->get_scale_rate_x();
                scale_value_y = utils->get_scale_rate_y();
            }
        } else {
            if (sclres_layout[layout_index].display_mode == DISPLAYMODE_PORTRAIT) {
                scale_value_x = utils->get_scale_rate_x();
                scale_value_y = utils->get_scale_rate_y();
            } else {
                scale_value_x = utils->get_scale_rate_y();
                scale_value_y = utils->get_scale_rate_x();
            }
        }

        ///* FIXME : We should apply this contraint to other scaling routines also! */
        ///* If the current screen resolution Y is bigger than our target height */
        //if (scale_value_y > 1.0f) {
        //    /* And if we have to scale Y-axis more than the X-axis, limit the scale value to X-axis rate */
        //    if (scale_value_y > scale_value_x) {
        //        scale_value_y = scale_value_x;
        //    }
        //} else if (scale_value_y < 1.0f) { /* Or current screen is smaller than our target resolution */
        //    /* And if we have to scale Y-axis more than the X-axis, limit the scale value to X-axis rate */
        //    if (scale_value_y < scale_value_x) {
        //        scale_value_y = scale_value_x;
        //    }
        //}

        if (!resize_key_only) {
            sclres_layout[layout_index].width *= scale_value_x;
            sclres_layout[layout_index].height *= scale_value_y;
        }

        for (innerLoop = 0;innerLoop < MAX_KEY;innerLoop++) {
            SclLayoutKeyCoordinatePointer p = sclres_layout_key_coordinate_pointer_frame[layout_index][innerLoop];
            if (p && p->valid) {
                p->x *= scale_value_x;
                p->y *= scale_value_y;
                p->width *= scale_value_x;
                p->height *= scale_value_y;
                p->add_hit_left *= scale_value_x;
                p->add_hit_right *= scale_value_x;
                p->add_hit_top *= scale_value_y;
                p->add_hit_bottom *= scale_value_y;
                p->popup_relative_x *= scale_value_x;
                p->popup_relative_y *= scale_value_y;
                p->extract_offset_x *= scale_value_x;
                p->extract_offset_y *= scale_value_y;
                p->magnifier_offset_x *= scale_value_x;
                p->magnifier_offset_y *= scale_value_y;
            }
        }
    }

    return TRUE;
}

/**
 * Translates the current x,y,width,height by the current screen resolution
 * This func should be called when the class init
 */
sclboolean
CSCLResourceCache::resize_resource_elements_by_resolution()
{
    sclint loop, innerLoop;
    CSCLUtils *utils = CSCLUtils::get_instance();

    SclResParserManager *sclres_manager = SclResParserManager::get_instance();
    if (utils && sclres_manager) {
        const PSclDefaultConfigure sclres_default_configure = sclres_manager->get_default_configure();
        const PSclInputModeConfigure sclres_input_mode_configure = sclres_manager->get_input_mode_configure_table();
        const PSclModifierDecoration sclres_modifier_decoration = sclres_manager->get_modifier_decoration_table();
        const PSclLabelPropertiesTable sclres_label_properties = sclres_manager->get_label_properties_frame();
        const PSclAutoPopupConfigure sclres_autopopup_configure = sclres_manager->get_autopopup_configure();
        const PSclMagnifierWndConfigure sclres_magnifier_configure = sclres_manager->get_magnifier_configure();

        assert(sclres_default_configure != NULL);
        assert(sclres_input_mode_configure != NULL);
        assert(sclres_modifier_decoration != NULL);
        assert(sclres_label_properties != NULL);
        assert(sclres_autopopup_configure != NULL);
        assert(sclres_magnifier_configure != NULL);

        sclboolean invert_display = FALSE;

        if (sclres_default_configure->auto_detect_landscape) {
            sclint width, height;
            utils->get_screen_resolution(&width, &height);
            /* If the width of screen is bigger than the height, switch portrait mode and landscape mode */
            if (width > height) {
                invert_display = TRUE;
            }
        }

        /* First we recalculate all the coordinates of each keys and sizes of layouts structure */
        /* FIXME */
        //for (loop = 0;loop < MAX_LAYOUT;loop++) {
        for (loop = 0;loop < MAX_SCL_LAYOUT;loop++) {
            resize_layout_by_resolution(loop);
        }

        /* And resize the font labels, adjusting the size of padding also */
        /* FIXME */
        //for (loop = 0;loop < MAX_LABEL_PROPERTIES;loop++) {
        for (loop = 0;loop < MAX_SCL_LABEL_PROPERTIES;loop++) {
            for (innerLoop = 0;innerLoop < MAX_SIZE_OF_LABEL_FOR_ONE;innerLoop++) {
                if (sclres_label_properties[loop][innerLoop].valid)
                    sclres_label_properties[loop][innerLoop].font_size *= utils->get_smallest_scale_rate();
                sclres_label_properties[loop][innerLoop].padding_x *= utils->get_smallest_scale_rate();
                sclres_label_properties[loop][innerLoop].padding_y *= utils->get_smallest_scale_rate();
                sclres_label_properties[loop][innerLoop].inner_width *= utils->get_smallest_scale_rate();
                sclres_label_properties[loop][innerLoop].inner_height *= utils->get_smallest_scale_rate();
                sclres_label_properties[loop][innerLoop].shadow_distance *= utils->get_smallest_scale_rate();
            }
        }

        /* FIXME - Let's check if these variables also need to be calculated by AutoLandscapeDection */
        sclres_autopopup_configure->decoration_size *= utils->get_smallest_scale_rate();
        sclres_autopopup_configure->bg_padding *= utils->get_smallest_scale_rate();
        sclres_autopopup_configure->button_spacing *= utils->get_smallest_scale_rate();
        sclres_autopopup_configure->button_width *= utils->get_scale_rate_x();
        sclres_autopopup_configure->button_height *= utils->get_scale_rate_y();

        sclres_magnifier_configure->width *= utils->get_scale_rate_x();
        sclres_magnifier_configure->height *= utils->get_scale_rate_y();
        sclres_magnifier_configure->label_area_rect.left *= utils->get_scale_rate_x();
        sclres_magnifier_configure->label_area_rect.right *= utils->get_scale_rate_x();
        sclres_magnifier_configure->label_area_rect.top *= utils->get_scale_rate_y();
        sclres_magnifier_configure->label_area_rect.bottom *= utils->get_scale_rate_y();

        if (invert_display) {
            /* FIXME */
            //for (loop = 0;loop < MAX_INPUT_MODE;loop++) {
            for (loop = 0;loop < MAX_SCL_INPUT_MODE;loop++) {
                sclchar *temp = sclres_input_mode_configure[loop].layouts[0];
                sclres_input_mode_configure[loop].layouts[0] = sclres_input_mode_configure[loop].layouts[1];
                sclres_input_mode_configure[loop].layouts[1] = temp;
            }
            /* FIXME */
            //for (loop = 0;loop < MODIFIER_DECORATION_NUM;loop++) {
            for (loop = 0;loop < MAX_SCL_MODIFIER_DECORATION_NUM;loop++) {
                for(innerLoop = 0;innerLoop < KEY_MODIFIER_MAX;innerLoop++) {
                    sclchar *temp;
                    temp = sclres_modifier_decoration[loop].bg_image_path[0][innerLoop];
                    sclres_modifier_decoration[loop].bg_image_path[0][innerLoop] = sclres_modifier_decoration[loop].bg_image_path[1][innerLoop];
                    sclres_modifier_decoration[loop].bg_image_path[1][innerLoop] = temp;
                }
            }
        }
    }

    return TRUE;
}

/**
 * Changes the current key properties by the current screen resolution
 * This func should be called when the class init
 */
sclboolean
CSCLResourceCache::change_by_privatekey(const sclbyte input_mode_index, const sclbyte layout_index, const sclbyte key_index, SclLayoutKeyCoordinate* coordinate)
{
    SCL_DEBUG();
    scl_assert_return_false(coordinate);

    if (coordinate) {
        for (int loop = 0;loop < MAX_PRIVATE_KEY; loop++) {
            if (mPrivateKeyProperties[loop].valid &&
                !(mPrivateKeyProperties[loop].custom_id.empty()) && coordinate->custom_id) {
                if (mPrivateKeyProperties[loop].custom_id.compare(coordinate->custom_id) == 0) {
                    /* sets the current properties to private key properties */
                    copy_from_privatekeyproperties(&mPrivateKeyProperties[loop], coordinate);
                }
            }
        }
    }

    return TRUE;
}

/**
 * Copys the given private properties data to the given key properties
 */
sclboolean
CSCLResourceCache::copy_from_privatekeyproperties(const SclPrivateKeyProperties* privProperties, SclLayoutKeyCoordinate* coordinate)
{
    SCL_DEBUG();
    scl_assert_return_false(privProperties);
    scl_assert_return_false(coordinate);

    sclint loop;
    sclint inner_loop;
    if (privProperties && coordinate) {
        /* Copy customizing-allowed properties only if those properties are valid */
        coordinate->label_count = privProperties->label_count;
        for (loop = 0;loop < SCL_SHIFT_STATE_MAX;loop++) {
            for (inner_loop = 0;inner_loop < MAX_SIZE_OF_LABEL_FOR_ONE;inner_loop++) {
                if (!(privProperties->label[loop][inner_loop].empty())) {
                    coordinate->label[loop][inner_loop] =
                        const_cast<sclchar*>(privProperties->label[loop][inner_loop].c_str());
                }
            }
        }
        for (loop = 0;loop < SCL_SHIFT_STATE_MAX;loop++) {
            for (inner_loop = 0;inner_loop < SCL_BUTTON_STATE_MAX;inner_loop++) {
                if (!(privProperties->image_label_path[loop][inner_loop].empty())) {
                    coordinate->image_label_path[loop][inner_loop] =
                        const_cast<sclchar*>(privProperties->image_label_path[loop][inner_loop].c_str());
                }
            }
        }
        for (loop = 0;loop < SCL_SHIFT_STATE_MAX;loop++) {
            for (inner_loop = 0;inner_loop < SCL_BUTTON_STATE_MAX;inner_loop++) {
                if (!(privProperties->bg_image_path[loop][inner_loop].empty())) {
                    coordinate->bg_image_path[loop][inner_loop] =
                        const_cast<sclchar*>(privProperties->bg_image_path[loop][inner_loop].c_str());
                }
            }
        }

        for (loop = 0;loop < SCL_SHIFT_STATE_MAX;loop++) {
            for (inner_loop = 0;inner_loop < MAX_SIZE_OF_MULTITAP_CHAR;inner_loop++) {
                if (!(privProperties->key_value[loop][inner_loop].empty())) {
                    coordinate->key_value[loop][inner_loop] =
                        const_cast<sclchar*>(privProperties->key_value[loop][inner_loop].c_str());
                }
            }
        }
        for (loop = 0;loop < SCL_SHIFT_STATE_MAX;loop++) {
            for (inner_loop = 0;inner_loop < MAX_SIZE_OF_MULTITAP_CHAR;inner_loop++) {
                if (privProperties->key_event[loop][inner_loop] != 0) {
                    coordinate->key_event[loop][inner_loop] =
                        privProperties->key_event[loop][inner_loop];
                }
            }
        }
        for (loop = 0;loop < SCL_SHIFT_STATE_MAX;loop++) {
            for (inner_loop = 0;inner_loop < MAX_SIZE_OF_MULTITAP_CHAR;inner_loop++) {
                if (!(privProperties->hint_string[loop][inner_loop].empty())) {
                    coordinate->hint_string[loop][inner_loop] =
                        const_cast<sclchar*>(privProperties->hint_string[loop][inner_loop].c_str());
                }
            }
        }
    }

    return TRUE;
}

/**
 * Copys the given properties data to the given private key properties
 */
sclboolean
CSCLResourceCache::copy_to_privatekeyproperties(const SclLayoutKeyCoordinate *coordinate, SclPrivateKeyProperties* privProperties)
{
    SCL_DEBUG();
    scl_assert_return_false(privProperties);
    scl_assert_return_false(coordinate);

    /* sets the current key Properties to private key privProperties */

    sclint loop;
    sclint inner_loop;
    if (privProperties && coordinate) {
        /* Configure */
        if (coordinate->custom_id) {
            privProperties->custom_id = coordinate->custom_id;
        }
        privProperties->button_type = coordinate->button_type;
        privProperties->key_type = coordinate->key_type;
        privProperties->popup_type = coordinate->popup_type;
        privProperties->use_magnifier = coordinate->use_magnifier;
        privProperties->use_long_key_magnifier = coordinate->use_long_key_magnifier;
        if (coordinate->sound_style) {
            privProperties->sound_style = coordinate->sound_style;
        }
        if (coordinate->vibe_style) {
            privProperties->vibe_style = coordinate->vibe_style;
        }

        for (loop = 0;loop < SCL_DRAG_STATE_MAX;loop++) {
            if (coordinate->popup_input_mode[loop]) {
                privProperties->popup_input_mode[loop] = coordinate->popup_input_mode[loop];
            }
        }

        /* Properties */
        privProperties->label_count = coordinate->label_count;
        privProperties->key_value_count = coordinate->key_value_count;
        privProperties->long_key_type = coordinate->long_key_type;
        if (coordinate->long_key_value) {
            privProperties->long_key_value = coordinate->long_key_value;
        }
        privProperties->long_key_event = coordinate->long_key_event;
        privProperties->use_repeat_key = coordinate->use_repeat_key;
        privProperties->dont_close_popup = coordinate->dont_close_popup;
        privProperties->extra_option = coordinate->extra_option;
        if (coordinate->label_type) {
            privProperties->label_type = coordinate->label_type;
        }
        if (coordinate->image_label_type) {
            privProperties->image_label_type = coordinate->image_label_type;
        }

        for (loop = 0;loop < SCL_SHIFT_STATE_MAX;loop++) {
            for (inner_loop = 0;inner_loop < MAX_SIZE_OF_LABEL_FOR_ONE;inner_loop++) {
                if (coordinate->label[loop][inner_loop]) {
                    privProperties->label[loop][inner_loop] =
                        coordinate->label[loop][inner_loop];
                }
            }
        }
        for (loop = 0;loop < SCL_SHIFT_STATE_MAX;loop++) {
            for (inner_loop = 0;inner_loop < SCL_BUTTON_STATE_MAX;inner_loop++) {
                if (coordinate->image_label_path[loop][inner_loop]) {
                    privProperties->image_label_path[loop][inner_loop] =
                        coordinate->image_label_path[loop][inner_loop];
                }
            }
        }
        for (loop = 0;loop < SCL_SHIFT_STATE_MAX;loop++) {
            for (inner_loop = 0;inner_loop < SCL_BUTTON_STATE_MAX;inner_loop++) {
                if (coordinate->bg_image_path[loop][inner_loop]) {
                    privProperties->bg_image_path[loop][inner_loop] =
                        coordinate->bg_image_path[loop][inner_loop];
                }
            }
        }

        for (loop = 0;loop < SCL_SHIFT_STATE_MAX;loop++) {
            for (inner_loop = 0;inner_loop < MAX_SIZE_OF_MULTITAP_CHAR;inner_loop++) {
                if (coordinate->key_value[loop][inner_loop]) {
                    privProperties->key_value[loop][inner_loop] =
                        coordinate->key_value[loop][inner_loop];
                }
            }
        }
        for (loop = 0;loop < SCL_SHIFT_STATE_MAX;loop++) {
            for (inner_loop = 0;inner_loop < MAX_SIZE_OF_MULTITAP_CHAR;inner_loop++) {
                if (coordinate->key_event[loop][inner_loop]) {
                    privProperties->key_event[loop][inner_loop] =
                        coordinate->key_event[loop][inner_loop];
                }
            }
        }

        for (loop = 0;loop < SCL_SHIFT_STATE_MAX;loop++) {
            for (inner_loop = 0;inner_loop < MAX_SIZE_OF_AUTOPOPUP_STRING;inner_loop++) {
                if (coordinate->autopopup_key_labels[loop][inner_loop]) {
                    privProperties->autopopup_key_labels[loop][inner_loop] =
                        coordinate->autopopup_key_labels[loop][inner_loop];
                }
            }
        }
        for (loop = 0;loop < SCL_SHIFT_STATE_MAX;loop++) {
            for (inner_loop = 0;inner_loop < MAX_SIZE_OF_AUTOPOPUP_STRING;inner_loop++) {
                if (coordinate->autopopup_key_events[loop][inner_loop]) {
                    privProperties->autopopup_key_events[loop][inner_loop] =
                        coordinate->autopopup_key_events[loop][inner_loop];
                }
            }
        }
        for (loop = 0;loop < SCL_SHIFT_STATE_MAX;loop++) {
            for (inner_loop = 0;inner_loop < MAX_SIZE_OF_AUTOPOPUP_STRING;inner_loop++) {
                if (coordinate->autopopup_key_values[loop][inner_loop]) {
                    privProperties->autopopup_key_values[loop][inner_loop] =
                        coordinate->autopopup_key_values[loop][inner_loop];
                }
            }
        }
        for (loop = 0;loop < SCL_SHIFT_STATE_MAX;loop++) {
            for (inner_loop = 0;inner_loop < MAX_SIZE_OF_MULTITAP_CHAR;inner_loop++) {
                if (coordinate->hint_string[loop][inner_loop]) {
                    privProperties->hint_string[loop][inner_loop] =
                        coordinate->hint_string[loop][inner_loop];
                }
            }
        }
    }

    return TRUE;
}

/**
 * Copys the given private properties data to the other private properties
 */
sclboolean
CSCLResourceCache::copy_privatekeyproperties(const SclPrivateKeyProperties* source, SclPrivateKeyProperties* target)
{
    SCL_DEBUG();
    scl_assert_return_false(source);
    scl_assert_return_false(target);

    sclint loop;
    sclint inner_loop;
    if (source && target) {
        target->input_mode_index = source->input_mode_index;
        target->layout_index = source->layout_index;
        target->key_index = source->key_index;

        /* Configure */
        target->custom_id = source->custom_id;
        target->button_type = source->button_type;
        target->key_type = source->key_type;
        target->popup_type = source->popup_type;
        target->use_magnifier = source->use_magnifier;
        target->use_long_key_magnifier = source->use_long_key_magnifier;
        target->sound_style = source->sound_style;
        target->vibe_style = source->vibe_style;

        for (loop = 0;loop < SCL_DRAG_STATE_MAX;loop++) {
            target->popup_input_mode[loop] = source->popup_input_mode[loop];
        }

        /* Properties */
        target->label_count = source->label_count;
        target->key_value_count = source->key_value_count;
        target->long_key_type = source->long_key_type;
        target->long_key_value = source->long_key_value;
        target->long_key_event = source->long_key_event;
        target->use_repeat_key = source->use_repeat_key;
        target->dont_close_popup = source->dont_close_popup;
        target->extra_option = source->extra_option;
        target->label_type = source->label_type;
        target->image_label_type = source->image_label_type;

        for (loop = 0;loop < SCL_SHIFT_STATE_MAX;loop++) {
            for (inner_loop = 0;inner_loop < MAX_SIZE_OF_LABEL_FOR_ONE;inner_loop++) {
                target->label[loop][inner_loop] =
                    source->label[loop][inner_loop];
            }
        }
        for (loop = 0;loop < SCL_SHIFT_STATE_MAX;loop++) {
            for (inner_loop = 0;inner_loop < SCL_BUTTON_STATE_MAX;inner_loop++) {
                target->image_label_path[loop][inner_loop] =
                    source->image_label_path[loop][inner_loop];
            }
        }
        for (loop = 0;loop < SCL_SHIFT_STATE_MAX;loop++) {
            for (inner_loop = 0;inner_loop < SCL_BUTTON_STATE_MAX;inner_loop++) {
                target->bg_image_path[loop][inner_loop] =
                    source->bg_image_path[loop][inner_loop];
            }
        }

        for (loop = 0;loop < SCL_SHIFT_STATE_MAX;loop++) {
            for (inner_loop = 0;inner_loop < MAX_SIZE_OF_MULTITAP_CHAR;inner_loop++) {
                target->key_value[loop][inner_loop] =
                    source->key_value[loop][inner_loop];
            }
        }
        for (loop = 0;loop < SCL_SHIFT_STATE_MAX;loop++) {
            for (inner_loop = 0;inner_loop < MAX_SIZE_OF_MULTITAP_CHAR;inner_loop++) {
                target->key_event[loop][inner_loop] =
                    source->key_event[loop][inner_loop];
            }
        }

        for (loop = 0;loop < SCL_SHIFT_STATE_MAX;loop++) {
            for (inner_loop = 0;inner_loop < MAX_SIZE_OF_AUTOPOPUP_STRING;inner_loop++) {
                target->autopopup_key_labels[loop][inner_loop] =
                    source->autopopup_key_labels[loop][inner_loop];
            }
        }
        for (loop = 0;loop < SCL_SHIFT_STATE_MAX;loop++) {
            for (inner_loop = 0;inner_loop < MAX_SIZE_OF_AUTOPOPUP_STRING;inner_loop++) {
                target->autopopup_key_events[loop][inner_loop] =
                    source->autopopup_key_events[loop][inner_loop];
            }
        }
        for (loop = 0;loop < SCL_SHIFT_STATE_MAX;loop++) {
            for (inner_loop = 0;inner_loop < MAX_SIZE_OF_AUTOPOPUP_STRING;inner_loop++) {
                target->autopopup_key_values[loop][inner_loop] =
                    source->autopopup_key_values[loop][inner_loop];
            }
        }
        for (loop = 0;loop < SCL_SHIFT_STATE_MAX;loop++) {
            for (inner_loop = 0;inner_loop < MAX_SIZE_OF_MULTITAP_CHAR;inner_loop++) {
                target->hint_string[loop][inner_loop] =
                    source->hint_string[loop][inner_loop];
            }
        }
    }

    return TRUE;
}

/**
 * Clears the given private properties data
 */
sclboolean
CSCLResourceCache::clear_privatekeyproperties(SclPrivateKeyProperties* privProperties)
{
    SCL_DEBUG();
    scl_assert_return_false(privProperties);

    /* sets the current key Properties to private key privProperties */

    sclint loop;
    sclint inner_loop;
    if (privProperties) {
        /* Configure */
        privProperties->valid = FALSE;
        privProperties->input_mode_index = NOT_USED;
        privProperties->layout_index = NOT_USED;
        privProperties->key_index = NOT_USED;

        privProperties->custom_id.clear();
        privProperties->button_type = BUTTON_TYPE_NORMAL;
        privProperties->key_type = KEY_TYPE_NONE;
        privProperties->popup_type = POPUP_TYPE_NONE;
        privProperties->use_magnifier = FALSE;
        privProperties->use_long_key_magnifier = FALSE;
        privProperties->sound_style.clear();
        privProperties->vibe_style.clear();

        for (loop = 0;loop < SCL_DRAG_STATE_MAX;loop++) {
            privProperties->popup_input_mode[loop].clear();
        }

        /* Properties */
        privProperties->label_count = 0;
        privProperties->key_value_count = 0;
        privProperties->long_key_type = KEY_TYPE_NONE;
        privProperties->long_key_value.clear();
        privProperties->long_key_event = 0;
        privProperties->use_repeat_key = FALSE;
        privProperties->dont_close_popup = FALSE;
        privProperties->extra_option = 0;
        privProperties->label_type.clear();
        privProperties->image_label_type.clear();

        for (loop = 0;loop < SCL_SHIFT_STATE_MAX;loop++) {
            for (inner_loop = 0;inner_loop < MAX_SIZE_OF_LABEL_FOR_ONE;inner_loop++) {
                privProperties->label[loop][inner_loop].clear();
            }
        }
        for (loop = 0;loop < SCL_SHIFT_STATE_MAX;loop++) {
            for (inner_loop = 0;inner_loop < SCL_BUTTON_STATE_MAX;inner_loop++) {
                privProperties->image_label_path[loop][inner_loop].clear();
            }
        }
        for (loop = 0;loop < SCL_SHIFT_STATE_MAX;loop++) {
            for (inner_loop = 0;inner_loop < SCL_BUTTON_STATE_MAX;inner_loop++) {
                privProperties->bg_image_path[loop][inner_loop].clear();
            }
        }

        for (loop = 0;loop < SCL_SHIFT_STATE_MAX;loop++) {
            for (inner_loop = 0;inner_loop < MAX_SIZE_OF_MULTITAP_CHAR;inner_loop++) {
                privProperties->key_value[loop][inner_loop].clear();
            }
        }
        for (loop = 0;loop < SCL_SHIFT_STATE_MAX;loop++) {
            for (inner_loop = 0;inner_loop < MAX_SIZE_OF_MULTITAP_CHAR;inner_loop++) {
                privProperties->key_event[loop][inner_loop] = 0;
            }
        }

        for (loop = 0;loop < SCL_SHIFT_STATE_MAX;loop++) {
            for (inner_loop = 0;inner_loop < MAX_SIZE_OF_AUTOPOPUP_STRING;inner_loop++) {
                privProperties->autopopup_key_labels[loop][inner_loop].clear();
            }
        }
        for (loop = 0;loop < SCL_SHIFT_STATE_MAX;loop++) {
            for (inner_loop = 0;inner_loop < MAX_SIZE_OF_AUTOPOPUP_STRING;inner_loop++) {
                privProperties->autopopup_key_events[loop][inner_loop] = 0;
            }
        }
        for (loop = 0;loop < SCL_SHIFT_STATE_MAX;loop++) {
            for (inner_loop = 0;inner_loop < MAX_SIZE_OF_AUTOPOPUP_STRING;inner_loop++) {
                privProperties->autopopup_key_values[loop][inner_loop].clear();
            }
        }
        for (loop = 0;loop < SCL_SHIFT_STATE_MAX;loop++) {
            for (inner_loop = 0;inner_loop < MAX_SIZE_OF_MULTITAP_CHAR;inner_loop++) {
                privProperties->hint_string[loop][inner_loop].clear();
            }
        }
    }

    return TRUE;
}

/**
 * Adds a new private key
 * It will update it to the current cache properties context because the application can call it anytime
 * For adapting it in realtime, you should explictly call the draw_button function.
 *
 * @param[out] fNeedInvalid It will return true if the current private can be adapt into the current display.
 * @return id an array index of the private key
 */
sclint
CSCLResourceCache::add_private_key(SclPrivateKeyProperties* privProperties, sclboolean *fNeedInvaild)
{
    SCL_DEBUG();
    scl_assert_return_false(privProperties);
    sclint ret = NOT_USED;
    *fNeedInvaild = FALSE; /* don't need to update now */

    if (privProperties->custom_id.empty())
        return ret;

    /* Finds an index to be set */
    sclint loop = 0;
    for (loop = 0;loop < MAX_PRIVATE_KEY; loop++) {
        if (mPrivateKeyProperties[loop].custom_id.compare(privProperties->custom_id) == 0) {
            break;
        }
    }

    if (loop == MAX_PRIVATE_KEY) {
        for (loop = 0;loop < MAX_PRIVATE_KEY; loop++) {
            if (mPrivateKeyProperties[loop].valid == FALSE) break;
        }
        if (loop == MAX_PRIVATE_KEY) {
            printf("Out of buffer!! could not insert new private data into buffer \n");
            return ret;
        }
    }

    copy_privatekeyproperties(privProperties, &mPrivateKeyProperties[loop]);
    mPrivateKeyProperties[loop].valid = TRUE;
    ret = loop;

    sclboolean found = FALSE;
    for (loop = 0;loop < MAX_KEY; loop++) {
        if ((!(privProperties->custom_id.empty())) && mCurBaseLayoutKeyCoordinates[loop].custom_id) {
            if (privProperties->custom_id.compare(mCurBaseLayoutKeyCoordinates[loop].custom_id) == 0) {
                /* sets the current properties to private key properties */
                copy_from_privatekeyproperties(privProperties, &mCurBaseLayoutKeyCoordinates[loop]);
                found = TRUE;
            }
        }
    }
    if (found) {
        *fNeedInvaild = TRUE;
        return ret;
    }

#if 0
    /* For popup layout */
    CSCLWindows *windows = CSCLWindows::get_instance();
    for (int ploop = 0; ploop < MAX_POPUP_WINDOW; ploop++) {
        layout = context->get_popup_layout(windows->get_nth_popup_window(ploop));
        if (privProperties->input_mode_index == inputmode && privProperties->layout_index == layout) {
            /* sets the current properties to private key properties */
            copy_privatekeyproperties_to_keyproperties(privProperties, &mCurPopupLayoutKeyProperties[ploop][privProperties->key_index]);
            return ret;
            *fNeedInvaild = TRUE;
        }
    }
#endif

    if (ret == NOT_USED) {
        printf("Failed!. Out of private data buffer\n");
    }
    return ret;
}

/**
 * Removes the private data of the given id from SclPrivateKeyProperties buffer
 */
sclboolean
CSCLResourceCache::remove_private_key(sclint id)
{
    SCL_DEBUG();
    sclint loop;
    CSCLContext *context = CSCLContext::get_instance();

    /* resets the current properties to predefined properties */
    sclshort layout =  context->get_base_layout();

    SclResParserManager *sclres_manager = SclResParserManager::get_instance();
    PSclLayoutKeyCoordinatePointerTable sclres_layout_key_coordinate_pointer_frame =
        sclres_manager->get_key_coordinate_pointer_frame();

    if (sclres_layout_key_coordinate_pointer_frame == NULL) {
        return FALSE;
    }
    if (scl_check_arrindex(layout, MAX_SCL_LAYOUT)) {
        for (loop = 0;loop < MAX_KEY; loop++) {
            if ((!(mPrivateKeyProperties[id].custom_id.empty())) && mCurBaseLayoutKeyCoordinates[loop].custom_id) {
                if (mPrivateKeyProperties[id].custom_id.compare(mCurBaseLayoutKeyCoordinates[loop].custom_id) == 0) {
                    SclLayoutKeyCoordinatePointer p = sclres_layout_key_coordinate_pointer_frame[layout][loop];
                    if (p == NULL) {
                        continue;
                    }
                    SclLayoutKeyCoordinatePointer the_key = mCurBaseLayoutKeyCoordinates + loop;
                    assert(the_key != NULL);
                    memcpy(the_key, p, sizeof(SclLayoutKeyCoordinate));

                }
            }
        }
    }

    /* Shift all the privatekey properties to the left by 1, starting from the item next to the id th element */
    for (loop = id;loop < MAX_PRIVATE_KEY - 1; loop++) {
        copy_privatekeyproperties(&mPrivateKeyProperties[loop + 1], &mPrivateKeyProperties[loop]);
    }
    /* Clear the last element */
    clear_privatekeyproperties(&mPrivateKeyProperties[MAX_PRIVATE_KEY - 1]);
    return TRUE;
}

/**
 * Clears all private keys
 */
sclboolean
CSCLResourceCache::clear_private_keys()
{
    SCL_DEBUG();
    for (sclint loop = 0; loop < MAX_PRIVATE_KEY;loop++) {
        clear_privatekeyproperties(&mPrivateKeyProperties[loop]);
    }
    return TRUE;
}

/**
 * Re-computes the cache data of the given window. The cache data has been including the current key properties, button context, layout etc,,
 * Another role of this func is to adjust the current coordinate according to the current resolution.
 * This func will be called when a newly window is created
 */
sclboolean
CSCLResourceCache::recompute_layout(sclwindow window)
{
    SCL_DEBUG();

    sclint loop;

    CSCLUtils *utils = CSCLUtils::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLContext *context = CSCLContext::get_instance();
    SclResParserManager *sclres_manager = SclResParserManager::get_instance();
    const PSclInputModeConfigure sclres_input_mode_configure = sclres_manager->get_input_mode_configure_table();
    const PSclLayout sclres_layout = sclres_manager->get_layout_table();
    const PSclLayoutKeyCoordinatePointerTable sclres_layout_key_coordinate_pointer_frame =
        sclres_manager->get_key_coordinate_pointer_frame();
    const PSclModifierDecoration sclres_modifier_decoration = sclres_manager->get_modifier_decoration_table();
    const PSclLabelPropertiesTable sclres_label_properties = sclres_manager->get_label_properties_frame();
    assert(sclres_input_mode_configure != NULL);
    assert(sclres_layout != NULL);
    assert(sclres_layout_key_coordinate_pointer_frame != NULL);
    assert(sclres_modifier_decoration != NULL);
    assert(sclres_label_properties != NULL);

    /* FIXME */
    scl8 popupindex = NOT_USED;

    SclLayout *pCurLayout = NULL;
    SclLayoutKeyCoordinate (*pCurLayoutKeyCoordinate)[MAX_KEY] = NULL;
    SclButtonContext (*pCurButtonContext)[MAX_KEY] = NULL;

    sclshort layout =  NOT_USED;
    if (windows && context && utils) {
        sclbyte inputmode = context->get_input_mode();

        if (windows->is_base_window(window)) {
            SCLDisplayMode display_mode = context->get_display_mode();
            layout = sclres_manager->get_layout_id(
                sclres_input_mode_configure[inputmode].layouts[display_mode]);
            if (layout != context->get_base_layout()) {
                sclres_manager->unload();
            }
            if (!(sclres_manager->loaded(layout))) {
                sclres_manager->load(layout);
                resize_layout_by_resolution(layout, TRUE);
            }
            context->set_base_layout(layout);

            pCurLayout = &mCurBaseLayout;
            pCurLayoutKeyCoordinate = &mCurBaseLayoutKeyCoordinates;
            pCurButtonContext = &mCurBaseButtonContext;
        } else {
            popupindex = windows->find_popup_window_index(window);
            /* Check if the popup index is in valid range */
            scl_assert_return_false(popupindex >= 0 && popupindex < MAX_POPUP_WINDOW);

            layout = context->get_popup_layout(window);

            if (!(sclres_manager->loaded(layout))) {
                sclres_manager->load(layout);
                resize_layout_by_resolution(layout, TRUE);
            }
            context->set_popup_layout(window, layout);

            if (popupindex >= 0 && popupindex < MAX_POPUP_WINDOW) {
                if (!(windows->is_base_window(window))) {
                    SclWindowContext *window_context = windows->get_window_context(window);
                    if (window_context) {
                        if (window_context->inputmode != NOT_USED) {
                            inputmode = window_context->inputmode;
                        }
                        if (window_context->layout != NOT_USED) {
                            layout = window_context->layout;
                        }
                    }
                }

                pCurLayout = &mCurPopupLayout[popupindex];
                pCurLayoutKeyCoordinate = &mCurPopupLayoutKeyCoordinates[popupindex];

                pCurButtonContext = &mCurPopupButtonContext[popupindex];
            }
        }

        if (pCurLayout && pCurLayoutKeyCoordinate && pCurButtonContext) {
            memset(pCurButtonContext, 0x00, sizeof(SclButtonContext) * MAX_KEY);
            /* If the layout index represents system-defined autopopup, generate layout and key properties data */
            if (layout == SCL_LAYOUT_AUTOPOPUP) {
                const SclLayoutKeyCoordinate *coordinate =
                    get_cur_layout_key_coordinate(context->get_cur_pressed_window(context->get_last_touch_device_id()),
                        context->get_cur_pressed_key(context->get_last_touch_device_id()));

                generate_autopopup_layout(coordinate, pCurLayout, pCurLayoutKeyCoordinate, pCurButtonContext);
            } else {
                if (scl_check_arrindex(layout, MAX_SCL_LAYOUT)) {
                    memcpy(pCurLayout, &sclres_layout[layout], sizeof(SclLayout));

                    memset(pCurLayoutKeyCoordinate, 0x00, sizeof(SclLayoutKeyCoordinate) * (MAX_KEY));
                    for (int i = 0; i < MAX_KEY; ++i) {
                        SclLayoutKeyCoordinatePointer p = sclres_layout_key_coordinate_pointer_frame[layout][i];
                        if (!p) break;
                        memcpy((SclLayoutKeyCoordinatePointer)pCurLayoutKeyCoordinate + i, p, sizeof(SclLayoutKeyCoordinate));
                    }

                    CSCLKeyFocusHandler* focus_handler = CSCLKeyFocusHandler::get_instance();
                    //reset navigation info
                    focus_handler->reset_key_navigation_info(window);
                    for (loop = 0;loop < MAX_KEY;loop++) {
                        SclLayoutKeyCoordinatePointer p = sclres_layout_key_coordinate_pointer_frame[layout][loop];
                        if (p && p->valid) {
                            //BUILDING KEY NAVIGATION INFO
                            focus_handler->update_key_navigation_info(window, loop, p);
                            //BUILDING KEY NAVIGATION INFO COMPLETED
                            (*pCurButtonContext)[loop].used = TRUE;
                            if (popupindex != NOT_USED) {
                                change_by_privatekey(inputmode, layout, loop, &(mCurPopupLayoutKeyCoordinates[popupindex][loop]));
                            } else {
                                change_by_privatekey(inputmode, layout, loop, &(mCurBaseLayoutKeyCoordinates[loop]));
                            }
                            /* if this button is UIITEM type, set the state of this button disabled */
                            if ((*pCurLayoutKeyCoordinate)[loop].button_type == BUTTON_TYPE_UIITEM) {
                                (*pCurButtonContext)[loop].state = BUTTON_STATE_DISABLED;
                            }

                            /* If this button's custom id is in the disabled key list, make it disabled */
                            sclboolean found = false;
                            if ((*pCurLayoutKeyCoordinate)[loop].custom_id) {
                                for (sclint inner_loop = 0;inner_loop < MAX_DISABLED_KEY;inner_loop++) {
                                    if (!(mDisabledKeyList[inner_loop].empty())) {
                                        if (mDisabledKeyList[inner_loop].compare(
                                            (*pCurLayoutKeyCoordinate)[loop].custom_id) == 0) {
                                                (*pCurButtonContext)[loop].state = BUTTON_STATE_DISABLED;
                                                found = true;
                                        }
                                    }
                                }
                            }
                            if (!found) {
                                (*pCurButtonContext)[loop].state = BUTTON_STATE_NORMAL;
                            }
                            (*pCurButtonContext)[loop].toggled = FALSE;

                            /* Apply the custom scale rate value */
                            (*pCurLayoutKeyCoordinate)[loop].x *= utils->get_custom_scale_rate_x();
                            (*pCurLayoutKeyCoordinate)[loop].y *= utils->get_custom_scale_rate_y();
                            (*pCurLayoutKeyCoordinate)[loop].width *= utils->get_custom_scale_rate_x();
                            (*pCurLayoutKeyCoordinate)[loop].height *= utils->get_custom_scale_rate_y();
                            (*pCurLayoutKeyCoordinate)[loop].add_hit_left *= utils->get_custom_scale_rate_x();
                            (*pCurLayoutKeyCoordinate)[loop].add_hit_right *= utils->get_custom_scale_rate_x();
                            (*pCurLayoutKeyCoordinate)[loop].add_hit_top *= utils->get_custom_scale_rate_y();
                            (*pCurLayoutKeyCoordinate)[loop].add_hit_bottom *= utils->get_custom_scale_rate_y();
                            (*pCurLayoutKeyCoordinate)[loop].popup_relative_x *= utils->get_custom_scale_rate_x();
                            (*pCurLayoutKeyCoordinate)[loop].popup_relative_y *= utils->get_custom_scale_rate_y();
                            (*pCurLayoutKeyCoordinate)[loop].extract_offset_x *= utils->get_custom_scale_rate_x();
                            (*pCurLayoutKeyCoordinate)[loop].extract_offset_y *= utils->get_custom_scale_rate_y();
                            (*pCurLayoutKeyCoordinate)[loop].magnifier_offset_x *= utils->get_custom_scale_rate_x();
                            (*pCurLayoutKeyCoordinate)[loop].magnifier_offset_y *= utils->get_custom_scale_rate_y();

                            if (windows->is_base_window(window)) {
                                /* Apply the custom starting coordinates */
                                (*pCurLayoutKeyCoordinate)[loop].x += mCurStartingCoordinates.x;
                                (*pCurLayoutKeyCoordinate)[loop].y += mCurStartingCoordinates.y;
                            }
                        }
                    }
                    //finalize navigation info
                    focus_handler->finalize_key_navigation_info(window);
                }
            }

            /* Apply the custom scale rate value */
            (*pCurLayout).width *= utils->get_custom_scale_rate_x();
            (*pCurLayout).height *= utils->get_custom_scale_rate_y();

            (*pCurLayout).add_grab_left *= utils->get_custom_scale_rate_x();
            (*pCurLayout).add_grab_right *= utils->get_custom_scale_rate_x();
            (*pCurLayout).add_grab_top *= utils->get_custom_scale_rate_y();
            (*pCurLayout).add_grab_bottom *= utils->get_custom_scale_rate_y();

            (*pCurLayout).mouse_manipulate_x *= utils->get_custom_scale_rate_x();
            (*pCurLayout).mouse_manipulate_y *= utils->get_custom_scale_rate_y();

            /* Resize window */
            /*if (windows->is_base_window(window)) {
                windows->resize_window(window, mCurBaseLayout.width, mCurBaseLayout.height);
                windows->resize_window(windows->get_dim_window(), mCurBaseLayout.width, mCurBaseLayout.height);
            }*/

            /* EFL testing */
            if (window) {
                windows->update_window(window);
            }
        }
    }

    return TRUE;
}

/**
 * Returns the current key_coordinate data
 */
SclLayoutKeyCoordinate*
CSCLResourceCache::get_cur_layout_key_coordinate(sclwindow window, sclbyte key_index)
{
    SCL_DEBUG();
    scl_assert_return_null(key_index < MAX_KEY);

    CSCLWindows *windows = CSCLWindows::get_instance();
    if (windows->get_base_window() == window) {
        if (key_index < MAX_KEY) {
            return &mCurBaseLayoutKeyCoordinates[key_index];
        }
    } else {
        sclbyte popupindex = windows->find_popup_window_index(window);
        scl_assert_return_false(popupindex < MAX_POPUP_WINDOW);
        if (key_index < MAX_KEY && popupindex < MAX_POPUP_WINDOW) {
            return &mCurPopupLayoutKeyCoordinates[popupindex][key_index];
        }
    }

    return NULL;
}

/**
 * FIXME : This must be very SLOW - let's refine this function ASAP
 * Returns the current label_properties data
 */
const SclLabelProperties*
CSCLResourceCache::get_label_properties(sclchar *label_type, sclbyte index) const
{
    SCL_DEBUG();

    SclResParserManager *sclres_manager = SclResParserManager::get_instance();
    PSclLabelPropertiesTable sclres_label_properties = sclres_manager->get_label_properties_frame();
    assert(sclres_label_properties != NULL);
    if (sclres_label_properties && label_type) {
        /* FIXME */
        //if (scl_check_arrindex(labeltype, MAX_LABEL_PROPERTIES) && scl_check_arrindex(index, MAX_SIZE_OF_LABEL_FOR_ONE)) {
        for(sclshort labeltype = 0;
            labeltype < MAX_SCL_LABEL_PROPERTIES && labeltype < sclres_manager->get_labelproperty_size();
            labeltype++) {
                if (sclres_label_properties[labeltype][0].label_type) {
                    if (strcmp(sclres_label_properties[labeltype][0].label_type, label_type) == 0) {
                    if (scl_check_arrindex_unsigned(index, MAX_SIZE_OF_LABEL_FOR_ONE)) {
                        return &sclres_label_properties[labeltype][index];
                    }
                }
            }
        }
    }

    CSCLUtils *utils = CSCLUtils::get_instance();
    if (utils && label_type) {
        utils->log("WARNINNG!!!!!!!!!!!!!!!!!! LABEL NAME %s COULD NOT BE FOUND!!!!!\n", label_type);
    }

    return NULL;
}

/**
 * Returns the current button_context data
 */
SclButtonContext*
CSCLResourceCache::get_cur_button_context(sclwindow window, sclbyte key_index)
{
    SCL_DEBUG();
    scl_assert_return_null(key_index < MAX_KEY);

    CSCLWindows *windows = CSCLWindows::get_instance();
    if (windows->get_base_window() == window) {
        if (key_index < MAX_KEY) {
            return &mCurBaseButtonContext[key_index];
        }
    } else {
        sclbyte popupindex = windows->find_popup_window_index(window);
        scl_assert_return_null(popupindex < MAX_POPUP_WINDOW);
        if (key_index < MAX_KEY && popupindex < MAX_POPUP_WINDOW) {
            return &mCurPopupButtonContext[popupindex][key_index];
        }
    }

    return NULL;
}

/* Generate and fill autopopup layout data */
void CSCLResourceCache::generate_autopopup_layout( const SclLayoutKeyCoordinate *coordinate,
        SclLayout *pCurLayout, SclLayoutKeyCoordinate (*pCurLayoutKeyCoordinates)[MAX_KEY],
        SclButtonContext (*pCurButtonContext)[MAX_KEY] )
{
    SCL_DEBUG();

    CSCLUtils *utils = CSCLUtils::get_instance();
    CSCLContext *context = CSCLContext::get_instance();

    SclResParserManager *sclres_manager = SclResParserManager::get_instance();
    PSclAutoPopupConfigure autopopup_configure = NULL;
    if (sclres_manager) {
        autopopup_configure = sclres_manager->get_autopopup_configure();
    }

    int loop;
    if (utils && context && coordinate && autopopup_configure) {
        sclbyte num_keys, num_columns, num_rows;
        sclint x, y, width, height;
        SCLShiftState shift_index = context->get_shift_state();
        if (shift_index < 0 || shift_index >= SCL_SHIFT_STATE_MAX) shift_index = SCL_SHIFT_STATE_OFF;
        if (context->get_caps_lock_mode()) {
            shift_index = (shift_index == SCL_SHIFT_STATE_OFF) ? SCL_SHIFT_STATE_ON : SCL_SHIFT_STATE_OFF;
        }
        if (utils->get_autopopup_window_variables(coordinate->autopopup_key_labels[shift_index], &num_keys, &num_columns, &num_rows, &width, &height)) {
            int row = 0, column = 0;

            pCurLayout->use_magnifier_window = FALSE;
            pCurLayout->valid = TRUE;
            pCurLayout->style = LAYOUT_STYLE_POPUP_GRAB;
            pCurLayout->name = NULL;
            pCurLayout->width = width;
            pCurLayout->height = height;
            memset(pCurLayout->image_path, 0x00, sizeof(pCurLayout->image_path));
            pCurLayout->use_sw_background = TRUE;
            if (autopopup_configure->bg_image_path != NULL) {
                pCurLayout->image_path[0] = autopopup_configure->bg_image_path;
                pCurLayout->use_sw_background = FALSE;
            }
            pCurLayout->use_sw_button = TRUE;
            /* If button image path is set, use images instead of SW buttons */
            if (autopopup_configure->button_image_path[0] != NULL) {
                pCurLayout->use_sw_button = FALSE;
            }
            pCurLayout->use_magnifier_window = FALSE;
            pCurLayout->extract_background = FALSE;
            pCurLayout->bg_color = autopopup_configure->bg_color;
            pCurLayout->bg_line_width = autopopup_configure->bg_line_width * utils->get_smallest_custom_scale_rate();
            pCurLayout->bg_line_color = autopopup_configure->bg_line_color;
            pCurLayout->add_grab_left = autopopup_configure->add_grab_left * utils->get_custom_scale_rate_x();
            pCurLayout->add_grab_right = autopopup_configure->add_grab_right * utils->get_custom_scale_rate_x();
            pCurLayout->add_grab_top = autopopup_configure->add_grab_top * utils->get_custom_scale_rate_y();
            pCurLayout->add_grab_bottom = autopopup_configure->add_grab_bottom * utils->get_custom_scale_rate_y();
            pCurLayout->mouse_manipulate_x = 0;
            pCurLayout->mouse_manipulate_y = (autopopup_configure->button_height * utils->get_custom_scale_rate_y() * -1);

            memset((*pCurLayoutKeyCoordinates), 0x00, sizeof(SclLayoutKeyCoordinate) * MAX_KEY);

            memset((*pCurButtonContext), 0x00, sizeof(SclButtonContext) * MAX_KEY);

            for (loop = 0;loop < (num_columns * num_rows) && loop < MAX_KEY && loop < MAX_SIZE_OF_AUTOPOPUP_STRING;loop++) {
                column = (loop % num_columns);
                row = loop / num_columns;
                x = autopopup_configure->bg_padding * utils->get_smallest_custom_scale_rate() +
                    (autopopup_configure->button_width * utils->get_custom_scale_rate_x() * column) +
                    (autopopup_configure->button_spacing * utils->get_smallest_custom_scale_rate() * column);
                y = autopopup_configure->bg_padding * utils->get_smallest_custom_scale_rate() +
                    (autopopup_configure->button_height * utils->get_custom_scale_rate_y() * (num_rows - row - 1)) +
                    (autopopup_configure->button_spacing * utils->get_smallest_custom_scale_rate() * row);

                (*pCurLayoutKeyCoordinates)[loop].valid = TRUE;
                (*pCurLayoutKeyCoordinates)[loop].x = x +
                    autopopup_configure->decoration_size * utils->get_smallest_custom_scale_rate();
                (*pCurLayoutKeyCoordinates)[loop].y = y +
                    autopopup_configure->decoration_size * utils->get_smallest_custom_scale_rate();
                (*pCurLayoutKeyCoordinates)[loop].width =
                    autopopup_configure->button_width * utils->get_custom_scale_rate_x();
                (*pCurLayoutKeyCoordinates)[loop].height =
                    autopopup_configure->button_height * utils->get_custom_scale_rate_y();
                (*pCurLayoutKeyCoordinates)[loop].popup_relative_x = 0;
                (*pCurLayoutKeyCoordinates)[loop].popup_relative_y = 0;
                (*pCurLayoutKeyCoordinates)[loop].extract_offset_x = 0;
                (*pCurLayoutKeyCoordinates)[loop].extract_offset_y = 0;
                (*pCurLayoutKeyCoordinates)[loop].sub_layout = NULL;

                (*pCurLayoutKeyCoordinates)[loop].valid = TRUE;
                if (loop < num_keys) {
                    (*pCurLayoutKeyCoordinates)[loop].button_type = BUTTON_TYPE_NORMAL;
                } else {
                    (*pCurLayoutKeyCoordinates)[loop].button_type = BUTTON_TYPE_UIITEM;
                }
                (*pCurLayoutKeyCoordinates)[loop].key_type = KEY_TYPE_STRING;
                (*pCurLayoutKeyCoordinates)[loop].popup_type = POPUP_TYPE_NONE;
                (*pCurLayoutKeyCoordinates)[loop].use_magnifier = FALSE;
                (*pCurLayoutKeyCoordinates)[loop].use_long_key_magnifier = TRUE;
                memset((*pCurLayoutKeyCoordinates)[loop].popup_input_mode, NOT_USED, sizeof((*pCurLayoutKeyCoordinates)[loop].popup_input_mode));

                (*pCurLayoutKeyCoordinates)[loop].valid = TRUE;
                (*pCurLayoutKeyCoordinates)[loop].label_count = 1;
                if (context->get_caps_lock_mode()) {
                    (*pCurLayoutKeyCoordinates)[loop].label[SCL_SHIFT_STATE_OFF][0] =
                        coordinate->autopopup_key_labels[SCL_SHIFT_STATE_ON][loop];
                    (*pCurLayoutKeyCoordinates)[loop].label[SCL_SHIFT_STATE_ON][0] =
                        coordinate->autopopup_key_labels[SCL_SHIFT_STATE_OFF][loop];
                    (*pCurLayoutKeyCoordinates)[loop].label[SCL_SHIFT_STATE_LOCK][0] =
                        coordinate->autopopup_key_labels[SCL_SHIFT_STATE_OFF][loop];
                } else {
                    (*pCurLayoutKeyCoordinates)[loop].label[SCL_SHIFT_STATE_OFF][0] =
                        coordinate->autopopup_key_labels[SCL_SHIFT_STATE_OFF][loop];
                    (*pCurLayoutKeyCoordinates)[loop].label[SCL_SHIFT_STATE_ON][0] =
                        coordinate->autopopup_key_labels[SCL_SHIFT_STATE_ON][loop];
                    (*pCurLayoutKeyCoordinates)[loop].label[SCL_SHIFT_STATE_LOCK][0] =
                        coordinate->autopopup_key_labels[SCL_SHIFT_STATE_LOCK][loop];
                }
                //(*pCurLayoutKeyProperties)[loop].labelPropId = SCL_LABEL_PROPERTY_AUTOPOPUP;
                (*pCurLayoutKeyCoordinates)[loop].label_type = autopopup_configure->label_type;
                memset((*pCurLayoutKeyCoordinates)[loop].image_label_path, 0x00, sizeof((*pCurLayoutKeyCoordinates)[loop].image_label_path));
                memset((*pCurLayoutKeyCoordinates)[loop].bg_image_path, 0x00, sizeof((*pCurLayoutKeyCoordinates)[loop].bg_image_path));
                for (int innerLoop = 0;innerLoop < SCL_BUTTON_STATE_MAX;innerLoop++) {
                    (*pCurLayoutKeyCoordinates)[loop].bg_image_path[SCL_SHIFT_STATE_OFF][innerLoop] =
                        (*pCurLayoutKeyCoordinates)[loop].bg_image_path[SCL_SHIFT_STATE_ON][innerLoop] =
                        (*pCurLayoutKeyCoordinates)[loop].bg_image_path[SCL_SHIFT_STATE_LOCK][innerLoop] =
                            autopopup_configure->button_image_path[innerLoop];
                }

                (*pCurLayoutKeyCoordinates)[loop].key_value_count = 1;

                if (coordinate->autopopup_key_values[0][loop] == NULL) {
                    (*pCurLayoutKeyCoordinates)[loop].key_value[0][0] = coordinate->autopopup_key_labels[0][loop];
                } else {
                    (*pCurLayoutKeyCoordinates)[loop].key_value[0][0] = coordinate->autopopup_key_values[0][loop];
                }
                if (coordinate->autopopup_key_values[1][loop] == NULL) {
                    (*pCurLayoutKeyCoordinates)[loop].key_value[1][0] = coordinate->autopopup_key_labels[1][loop];
                } else {
                    (*pCurLayoutKeyCoordinates)[loop].key_value[1][0] = coordinate->autopopup_key_values[1][loop];
                }
                if (coordinate->autopopup_key_values[2][loop] == NULL) {
                    (*pCurLayoutKeyCoordinates)[loop].key_value[2][0] = coordinate->autopopup_key_labels[2][loop];
                } else {
                    (*pCurLayoutKeyCoordinates)[loop].key_value[2][0] = coordinate->autopopup_key_values[2][loop];
                }
                (*pCurLayoutKeyCoordinates)[loop].key_event[0][0] = coordinate->autopopup_key_events[0][loop];
                (*pCurLayoutKeyCoordinates)[loop].key_event[1][0] = coordinate->autopopup_key_events[1][loop];
                (*pCurLayoutKeyCoordinates)[loop].key_event[2][0] = coordinate->autopopup_key_events[2][loop];
                (*pCurLayoutKeyCoordinates)[loop].long_key_type = KEY_TYPE_NONE;
                (*pCurLayoutKeyCoordinates)[loop].long_key_value = NULL;
                (*pCurLayoutKeyCoordinates)[loop].long_key_event = 0;

                (*pCurLayoutKeyCoordinates)[loop].autopopup_key_labels[0][0] = NULL;
                (*pCurLayoutKeyCoordinates)[loop].autopopup_key_labels[1][0] = NULL;
                (*pCurLayoutKeyCoordinates)[loop].autopopup_key_labels[2][0] = NULL;
                (*pCurLayoutKeyCoordinates)[loop].autopopup_key_events[0][0] = 0;
                (*pCurLayoutKeyCoordinates)[loop].autopopup_key_events[1][0] = 0;
                (*pCurLayoutKeyCoordinates)[loop].autopopup_key_events[2][0] = 0;
                (*pCurLayoutKeyCoordinates)[loop].autopopup_key_values[0][0] = NULL;
                (*pCurLayoutKeyCoordinates)[loop].autopopup_key_values[1][0] = NULL;
                (*pCurLayoutKeyCoordinates)[loop].autopopup_key_values[2][0] = NULL;
                (*pCurLayoutKeyCoordinates)[loop].extra_option = 0;

                /* If the keyvalue is in between the SCL_ISCHAR range, consider this to be a KEY_TYPE_CHAR */
                if ((*pCurLayoutKeyCoordinates)[loop].key_value[0][0]) {
                    if (strlen((*pCurLayoutKeyCoordinates)[loop].key_value[0][0]) == 1) {
                        if (SCL_ISCHAR(*(*pCurLayoutKeyCoordinates)[loop].key_value[0][0])) {
                            (*pCurLayoutKeyCoordinates)[loop].key_type = KEY_TYPE_CHAR;
                        }
                    }
                }

                (*pCurButtonContext)[loop].used = TRUE;
            }
            if (autopopup_configure->decoration_size > 0) {
                sclbyte decoidx;
                for (decoidx = 0;decoidx < MAX_WND_DECORATOR;decoidx++) {
                    if (loop + decoidx < MAX_KEY) {
                        (*pCurLayoutKeyCoordinates)[loop + decoidx].valid = TRUE;
                        switch (decoidx) {
                            case WND_DECORATOR_TOP_LEFT:
                                (*pCurLayoutKeyCoordinates)[loop + decoidx].x = 0;
                                (*pCurLayoutKeyCoordinates)[loop + decoidx].y = 0;
                                (*pCurLayoutKeyCoordinates)[loop + decoidx].width =
                                    autopopup_configure->decoration_size * utils->get_smallest_custom_scale_rate();
                                (*pCurLayoutKeyCoordinates)[loop + decoidx].height =
                                    autopopup_configure->decoration_size * utils->get_smallest_custom_scale_rate();
                            break;
                            case WND_DECORATOR_TOP_CENTER:
                                (*pCurLayoutKeyCoordinates)[loop + decoidx].x =
                                    autopopup_configure->decoration_size * utils->get_smallest_custom_scale_rate();
                                (*pCurLayoutKeyCoordinates)[loop + decoidx].y = 0;
                                (*pCurLayoutKeyCoordinates)[loop + decoidx].width = width -
                                    (2 * autopopup_configure->decoration_size * utils->get_smallest_custom_scale_rate());
                                (*pCurLayoutKeyCoordinates)[loop + decoidx].height =
                                    autopopup_configure->decoration_size * utils->get_smallest_custom_scale_rate();
                                break;
                            case WND_DECORATOR_TOP_RIGHT:
                                (*pCurLayoutKeyCoordinates)[loop + decoidx].x =
                                    width - autopopup_configure->decoration_size * utils->get_smallest_custom_scale_rate();
                                (*pCurLayoutKeyCoordinates)[loop + decoidx].y = 0;
                                (*pCurLayoutKeyCoordinates)[loop + decoidx].width =
                                    autopopup_configure->decoration_size * utils->get_smallest_custom_scale_rate();
                                (*pCurLayoutKeyCoordinates)[loop + decoidx].height =
                                    autopopup_configure->decoration_size * utils->get_smallest_custom_scale_rate();
                                break;
                            case WND_DECORATOR_MIDDLE_LEFT:
                                (*pCurLayoutKeyCoordinates)[loop + decoidx].x = 0;
                                (*pCurLayoutKeyCoordinates)[loop + decoidx].y =
                                    autopopup_configure->decoration_size * utils->get_smallest_custom_scale_rate();
                                (*pCurLayoutKeyCoordinates)[loop + decoidx].width =
                                    autopopup_configure->decoration_size * utils->get_smallest_custom_scale_rate();
                                (*pCurLayoutKeyCoordinates)[loop + decoidx].height = height -
                                    (2 * autopopup_configure->decoration_size * utils->get_smallest_custom_scale_rate());
                                break;
                            case WND_DECORATOR_MIDDLE_RIGHT:
                                (*pCurLayoutKeyCoordinates)[loop + decoidx].x =
                                    width - autopopup_configure->decoration_size * utils->get_smallest_custom_scale_rate();
                                (*pCurLayoutKeyCoordinates)[loop + decoidx].y =
                                    autopopup_configure->decoration_size * utils->get_smallest_custom_scale_rate();
                                (*pCurLayoutKeyCoordinates)[loop + decoidx].width =
                                    autopopup_configure->decoration_size * utils->get_smallest_custom_scale_rate();
                                (*pCurLayoutKeyCoordinates)[loop + decoidx].height = height -
                                    (2 * autopopup_configure->decoration_size * utils->get_smallest_custom_scale_rate());
                                break;
                            case WND_DECORATOR_BOTTOM_LEFT:
                                (*pCurLayoutKeyCoordinates)[loop + decoidx].x = 0;
                                (*pCurLayoutKeyCoordinates)[loop + decoidx].y =
                                    height - autopopup_configure->decoration_size * utils->get_smallest_custom_scale_rate();
                                (*pCurLayoutKeyCoordinates)[loop + decoidx].width =
                                    autopopup_configure->decoration_size * utils->get_smallest_custom_scale_rate();
                                (*pCurLayoutKeyCoordinates)[loop + decoidx].height =
                                    autopopup_configure->decoration_size * utils->get_smallest_custom_scale_rate();
                                break;
                            case WND_DECORATOR_BOTTOM_CENTER:
                                (*pCurLayoutKeyCoordinates)[loop + decoidx].x =
                                    autopopup_configure->decoration_size * utils->get_smallest_custom_scale_rate();
                                (*pCurLayoutKeyCoordinates)[loop + decoidx].y =
                                    height - autopopup_configure->decoration_size * utils->get_smallest_custom_scale_rate();
                                (*pCurLayoutKeyCoordinates)[loop + decoidx].width = width -
                                    (2 * autopopup_configure->decoration_size * utils->get_smallest_custom_scale_rate());
                                (*pCurLayoutKeyCoordinates)[loop + decoidx].height =
                                    autopopup_configure->decoration_size * utils->get_smallest_custom_scale_rate();
                                break;
                            case WND_DECORATOR_BOTTOM_RIGHT:
                                (*pCurLayoutKeyCoordinates)[loop + decoidx].x =
                                    width - autopopup_configure->decoration_size * utils->get_smallest_custom_scale_rate();
                                (*pCurLayoutKeyCoordinates)[loop + decoidx].y =
                                    height - autopopup_configure->decoration_size * utils->get_smallest_custom_scale_rate();
                                (*pCurLayoutKeyCoordinates)[loop + decoidx].width =
                                    autopopup_configure->decoration_size * utils->get_smallest_custom_scale_rate();
                                (*pCurLayoutKeyCoordinates)[loop + decoidx].height =
                                    autopopup_configure->decoration_size * utils->get_smallest_custom_scale_rate();
                                break;
                            default:
                                break;
                        }

                        (*pCurButtonContext)[loop + decoidx].used = TRUE;
                        (*pCurLayoutKeyCoordinates)[loop + decoidx].valid = TRUE;
                        (*pCurLayoutKeyCoordinates)[loop + decoidx].button_type = BUTTON_TYPE_UIITEM;
                        (*pCurLayoutKeyCoordinates)[loop + decoidx].valid = TRUE;
                        (*pCurLayoutKeyCoordinates)[loop + decoidx].bg_image_path[0][0] =
                            (*pCurLayoutKeyCoordinates)[loop + decoidx].bg_image_path[0][1] =
                            (*pCurLayoutKeyCoordinates)[loop + decoidx].bg_image_path[1][0] =
                            (*pCurLayoutKeyCoordinates)[loop + decoidx].bg_image_path[1][1] =
                            autopopup_configure->decoration_image_path[decoidx];
                    }
                }
            }
        }
    }
}

/**
* Sets the current theme name
*/
sclboolean
CSCLResourceCache::set_cur_themename( const sclchar *themename )
{
    if (themename) {
        strncpy(mCurThemename, themename, _POSIX_PATH_MAX - 1);
        mCurThemename[_POSIX_PATH_MAX - 1] = '\0';
    }

    return TRUE;
}

const sclchar*
CSCLResourceCache::get_cur_themename()
{
    return mCurThemename;
}

/**
 * Returns a template private key properties using key properties of the given context
 */
void
CSCLResourceCache::clone_keyproperties(SclPrivateKeyProperties* priv, sclbyte input_mode_index, sclbyte layout_index, sclbyte key_index)
{
    SclResParserManager *sclres_manager = SclResParserManager::get_instance();
    const PSclLayoutKeyCoordinatePointerTable sclres_layout_key_coordinate_pointer_frame = sclres_manager->get_key_coordinate_pointer_frame();

    SCL_DEBUG();
    if (priv && sclres_layout_key_coordinate_pointer_frame) {
        CSCLResourceCache *cache = CSCLResourceCache::get_instance();
        clear_privatekeyproperties(priv);

        /* gets the value of the previous key properties */
        SclLayoutKeyCoordinate keyCoordinate;
        memset(&keyCoordinate, 0x00, sizeof(keyCoordinate));
        if (scl_check_arrindex_unsigned(layout_index, MAX_SCL_LAYOUT) &&
            scl_check_arrindex_unsigned(key_index, MAX_KEY)) {
            SclLayoutKeyCoordinatePointer p = sclres_layout_key_coordinate_pointer_frame[layout_index][key_index];
            if (p) {
                memcpy(&keyCoordinate, p, sizeof(SclLayoutKeyCoordinate));
            }
        }

        scl_assert_return(keyCoordinate.valid == TRUE);

        /* Sets the default properties base on the properties values of the given context */
        cache->copy_to_privatekeyproperties(&keyCoordinate, priv);
    }
}

/**
 * Sets a private key to the current context
 *
 * @Usage
 *       SclPrivateKeyProperties privProperties;
 *       gCore->clone_keyproperties(&privProperties, INPUT_MODE_NUMBER, LYT_PORTRAIT_NOW_3x4, 0);
 *       // change
 *       gCore->set_private_key(&privProperties, TRUE);
 */
sclint
CSCLResourceCache::set_private_key(SclPrivateKeyProperties* properties, sclboolean fRedraw, sclboolean fPendingUpdate)
{
    SCL_DEBUG();
    CSCLContext *context = CSCLContext::get_instance();
    CSCLResourceCache *cache = CSCLResourceCache::get_instance();
    sclint privateId = NOT_USED;
    sclboolean isNeedUpdate = FALSE;
    if (cache && context && properties) {
        privateId = cache->add_private_key(properties, &isNeedUpdate);
        if (fRedraw && isNeedUpdate && !fPendingUpdate && privateId != NOT_USED) {
            CSCLWindows *windows = CSCLWindows::get_instance();
            if (windows) {
                /* Fix me (we should consider popupwindow later)*/
                windows->update_window(windows->get_base_window());
            }
        }
    }
    return privateId;
}

/**
 * Sets a private key to the current context
 * The other properties except given parameters will keep to the orginal value.
 * @Usage
 * gCore->set_private_key(INPUT_MODE_NUMBER, LYT_PORTRAIT_NOW_3x4, 0, "private", 999, TRUE);
 *
 * @param fRedraw If true, it will redraw the current key
 */
sclint
CSCLResourceCache::set_private_key(const sclchar* custom_id, sclchar* label, sclchar* imagelabel[SCL_BUTTON_STATE_MAX], sclchar* imagebg[SCL_BUTTON_STATE_MAX], sclulong key_event, sclchar *key_value, sclboolean fRedraw, sclboolean fPendingUpdate)
{
    SCL_DEBUG();

    SclPrivateKeyProperties privProperties;
    clear_privatekeyproperties(&privProperties);

    SclResParserManager *sclres_manager = SclResParserManager::get_instance();
    PSclLayoutKeyCoordinatePointerTable sclres_layout_key_coordinate_pointer_frame = sclres_manager->get_key_coordinate_pointer_frame();
    assert(sclres_layout_key_coordinate_pointer_frame != NULL);

    sclint loop;

    privProperties.valid = TRUE;
    privProperties.custom_id = custom_id;

    privProperties.key_event[SCL_SHIFT_STATE_LOCK][0] = privProperties.key_event[SCL_SHIFT_STATE_ON][0] =
        privProperties.key_event[SCL_SHIFT_STATE_OFF][0] = key_event;
    if (label) {
        privProperties.label[SCL_SHIFT_STATE_LOCK][0] = label;
        privProperties.label[SCL_SHIFT_STATE_ON][0] = label;
        privProperties.label[SCL_SHIFT_STATE_OFF][0] = label;
        privProperties.label_count = 1;
    }
    if (key_value) {
        privProperties.key_value[SCL_SHIFT_STATE_LOCK][0] = privProperties.key_value[SCL_SHIFT_STATE_ON][0] =
            privProperties.key_value[SCL_SHIFT_STATE_OFF][0] = key_value;
    }
    if (imagelabel) {
        for (loop =0;loop < SCL_BUTTON_STATE_MAX;loop++) {
            if (imagelabel[loop])
                privProperties.image_label_path[SCL_SHIFT_STATE_LOCK][loop] =
                    privProperties.image_label_path[SCL_SHIFT_STATE_ON][loop] =
                    privProperties.image_label_path[SCL_SHIFT_STATE_OFF][loop] = imagelabel[loop];
            }
        }
    if (imagebg) {
        for (loop =0;loop < SCL_BUTTON_STATE_MAX;loop++) {
            if (imagebg[loop]) {
                privProperties.bg_image_path[SCL_SHIFT_STATE_LOCK][loop] =
                    privProperties.bg_image_path[SCL_SHIFT_STATE_ON][loop] =
                    privProperties.bg_image_path[SCL_SHIFT_STATE_OFF][loop] = imagebg[loop];
            }
        }
    }
    return set_private_key(&privProperties, fRedraw, fPendingUpdate);
}

/**
* Unset private key for specific key
* @Usage
* gCore->unset_private_key(INPUT_MODE_NUMBER, LYT_PORTRAIT_NOW_3x4, 0);
*/
void
CSCLResourceCache::unset_private_key(sclshort input_mode_index, sclbyte layout_index, sclbyte key_index)
{
    sclint loop = 0;
    for (loop = 0;loop < MAX_PRIVATE_KEY; loop++) {
        if ((mPrivateKeyProperties[loop].input_mode_index == input_mode_index) &&
            mPrivateKeyProperties[loop].layout_index == layout_index &&
            mPrivateKeyProperties[loop].key_index == key_index) {
                remove_private_key(loop);
        }
    }
}

/**
* Unset private by custom_id, effective when removing all private keys with same custom_id
* @Usage
* gCore->unset_private_key(3);
*/
void
CSCLResourceCache::unset_private_key(const sclchar* custom_id)
{
    int loop;
    if (custom_id) {
        for(loop = 0;loop < MAX_PRIVATE_KEY;loop++) {
            if (mPrivateKeyProperties[loop].valid &&
                mPrivateKeyProperties[loop].custom_id.compare(custom_id) == 0) {
                remove_private_key(loop);
            }
        }
    }
}

/**
* Enable/disable button for handling mouse events
*/
void CSCLResourceCache::enable_button(const sclchar *custom_id, sclboolean enabled)
{
    SCL_DEBUG();

    sclint loop;
    if (custom_id) {
        sclbyte layout_index = NOT_USED;
        CSCLContext *context = CSCLContext::get_instance();
        SclResParserManager *sclres_manager = SclResParserManager::get_instance();
        PSclLayoutKeyCoordinatePointerTable sclres_layout_key_coordinate_pointer_frame =
            sclres_manager->get_key_coordinate_pointer_frame();
        assert(sclres_layout_key_coordinate_pointer_frame != NULL);

        if (context) {
            layout_index = context->get_base_layout();
        }
        if (scl_check_arrindex_unsigned(layout_index, MAX_SCL_LAYOUT)) {
            for (loop = 0;loop < MAX_KEY;loop++) {
                SclLayoutKeyCoordinatePointer p =
                    sclres_layout_key_coordinate_pointer_frame[layout_index][loop];
                if (!p || !(p->valid)) break;
                if (p->custom_id) {
                    if (strcmp(p->custom_id, custom_id) == 0) {
                        if (enabled) {
                            mCurBaseButtonContext[loop].state = BUTTON_STATE_NORMAL;
                        } else {
                            mCurBaseButtonContext[loop].state = BUTTON_STATE_DISABLED;
                        }
                    }
                }
            }
        }

        CSCLWindows *windows = CSCLWindows::get_instance();
        /* Fix me (we should decide by which way we would redraw the button's region - direct or indirect?)*/
        windows->update_window(windows->get_base_window());

        sclboolean found = FALSE;
        sclint empty_index = -1;

        for (loop = 0;loop < MAX_DISABLED_KEY;loop++) {
            if (!(mDisabledKeyList[loop].empty())) {
                if (mDisabledKeyList[loop].compare(custom_id) == 0) {
                    if (enabled) {
                        /* Erase this item from the disabled key list */
                        mDisabledKeyList[loop].clear();
                    }
                    found = TRUE;
                }
            } else {
                empty_index = loop;
            }
        }

        if (!found) {
            if (!enabled) {
                if (scl_check_arrindex(empty_index, MAX_DISABLED_KEY)) {
                    /* Add to the disabled key */
                    mDisabledKeyList[empty_index] = custom_id;
                }
            }
        }
    }
}

void
CSCLResourceCache::set_string_substitution(const sclchar *original, const sclchar *substitute)
{
    if (original && substitute) {
        mStringSubstitutor[std::string(original)] = std::string(substitute);
    }
}

void
CSCLResourceCache::unset_string_substitution(const sclchar *original)
{
    if (original) {
        mStringSubstitutor.erase(std::string(original));
    }
}

const sclchar*
CSCLResourceCache::find_substituted_string(const sclchar *original)
{
    const sclchar* ret = original;

    if (original) {
        std::map<std::string, std::string>::iterator iter = mStringSubstitutor.find(std::string(original));
        if (iter != mStringSubstitutor.end()) {
            ret = iter->second.c_str();
        }
    }

    return ret;
}

void
CSCLResourceCache::set_custom_starting_coordinates(sclint x, sclint y)
{
    mCurStartingCoordinates.x = x;
    mCurStartingCoordinates.y = y;
}

SclPoint
CSCLResourceCache::get_custom_starting_coordinates()
{
    return mCurStartingCoordinates;
}

void
CSCLResourceCache::set_custom_starting_coordinates_option(SCLStartingCoordinatesOption option)
{
    mCurStartingCoordinatesOption = option;
}

SCLStartingCoordinatesOption
CSCLResourceCache::get_custom_starting_coordinates_option()
{
    return mCurStartingCoordinatesOption;
}
