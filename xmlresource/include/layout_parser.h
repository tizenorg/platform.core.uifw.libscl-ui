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

#ifndef __LAYOUT_PARSER__H__
#define __LAYOUT_PARSER__H__

#include <vector>
#include <libxml/parser.h>
#include "sclres_type.h"

#define LAYOUT_TAG "layout"
#define LAYOUT_NAME_ATTRIBUTE "name"
#define LAYOUT_DIRECTION_ATTRIBUTE "direction"
#define LAYOUT_DIRECTION_ATTRIBUTE_PORTRAIT_VALUE "portrait"
#define LAYOUT_DIRECTION_ATTRIBUTE_LANDSCAPE_VALUE "landscape"
#define LAYOUT_STYLE_ATTRIBUTE "style"
#define LAYOUT_STYLE_ATTRIBUTE_BASE_VALUE "base"
#define LAYOUT_STYLE_ATTRIBUTE_POPUP_VALUE "popup"
#define LAYOUT_STYLE_ATTRIBUTE_POPUP_GRAB_VALUE "popup_grab"
#define LAYOUT_WIDTH_ATTRIBUTE "width"
#define LAYOUT_HEIGHT_ATTRIBUTE "height"
#define LAYOUT_MAGNIFIER_ATTRIBUTE "magnifier"
#define LAYOUT_PART_BACKGROUND_ATTRIBUTE "part_background"
#define LAYOUT_SW_BUTTON_ATTRIBUTE "sw_button"
#define LAYOUT_SW_BACKGROUND_ATTRIBUTE "sw_background"
#define LAYOUT_KEY_WIDTH_ATTRIBUTE "key_width"
#define LAYOUT_KEY_HEIGHT_ATTRIBUTE "key_height"
#define LAYOUT_KEY_SPACING_ATTRIBUTE "key_spacing"
#define LAYOUT_ROW_SPACING_ATTRIBUTE "row_spacing"
#define LAYOUT_KEY_ADD_HIT_LEFT_ATTRIBUTE "hit_left"
#define LAYOUT_KEY_ADD_HIT_RIGHT_ATTRIBUTE "hit_right"
#define LAYOUT_KEY_ADD_HIT_TOP_ATTRIBUTE "hit_top"
#define LAYOUT_KEY_ADD_HIT_BOTTOM_ATTRIBUTE "hit_bottom"
#define LAYOUT_VIBE_STYLE_ATTRIBUTE "vibe_style"
#define LAYOUT_SOUND_STYLE_ATTRIBUTE "sound_style"
#define LAYOUT_LABEL_TYPE_ATTRIBUTE "label_type"
#define LAYOUT_MODIFIER_DECORATION_ATTRIBUTE "modifier_decoration"

#define LAYOUT_ADD_GRAB_TAG "grab_area"
#define LAYOUT_ADD_GRAB_LEFT_TAG "left"
#define LAYOUT_ADD_GRAB_RIGHT_TAG "right"
#define LAYOUT_ADD_GRAB_TOP_TAG "top"
#define LAYOUT_ADD_GRAB_BOTTOM_TAG "bottom"

#define LAYOUT_KEY_BACKGROUND_REC_TAG "rec"
#define LAYOUT_KEY_BACKGROUND_REC_BUTTON_ATTRIBUTE "button"
#define LAYOUT_KEY_BACKGROUND_REC_BUTTON_ATTRIBUTE_NORMAL_VALUE "normal"
#define LAYOUT_KEY_BACKGROUND_REC_BUTTON_ATTRIBUTE_PRESSED_VALUE "pressed"
#define LAYOUT_KEY_BACKGROUND_REC_BUTTON_ATTRIBUTE_DISABLED_VALUE "disabled"

#define LAYOUT_KEY_BACKGROUND_TAG "background_image"
#define LAYOUT_KEY_BACKGROUND_REC_TAG "rec"
#define LAYOUT_KEY_BACKGROUND_REC_BUTTON_ATTRIBUTE "button"
#define LAYOUT_KEY_BACKGROUND_REC_BUTTON_ATTRIBUTE_NORMAL_VALUE "normal"
#define LAYOUT_KEY_BACKGROUND_REC_BUTTON_ATTRIBUTE_PRESSED_VALUE "pressed"
#define LAYOUT_KEY_BACKGROUND_REC_BUTTON_ATTRIBUTE_DISABLED_VALUE "disabled"

#define LAYOUT_BACKGROUND_TAG "image_path"
#define LAYOUT_BACKGROUND_NORMAL_TAG "button_normal"
#define LAYOUT_BACKGROUND_PRESSED_TAG "button_pressed"
#define LAYOUT_BACKGROUND_DISABLED_TAG "button_disabled"

#define LAYOUT_ROW_TAG "row"
#define LAYOUT_ROW_SUBLAYOUT_ID_ATTRIBUTE "sub_layout"
#define LAYOUT_ROW_X_ATTRIBUTE "x"
#define LAYOUT_ROW_Y_ATTRIBUTE "y"
#define LAYOUT_ROW_KEY_WIDTH_ATTRIBUTE LAYOUT_KEY_WIDTH_ATTRIBUTE
#define LAYOUT_ROW_KEY_HEIGHT_ATTRIBUTE LAYOUT_KEY_HEIGHT_ATTRIBUTE
#define LAYOUT_ROW_KEY_SPACING_ATTRIBUTE LAYOUT_KEY_SPACING_ATTRIBUTE

#define LAYOUT_ROW_KEY_TAG "key"
#define LAYOUT_ROW_KEY_CUSTOMID_ATTRIBUTE "custom_id"
#define LAYOUT_ROW_KEY_BUTTON_TYPE_ATTRIBUTE "button_type"
#define LAYOUT_ROW_KEY_KEY_TYPE_ATTRIBUTE "key_type"
#define LAYOUT_ROW_KEY_POPUP_TYPE_ATTRIBUTE "popup_type"
#define LAYOUT_ROW_KEY_MAGNIFIER_ATTRIBUTE "use_magnifier"
#define LAYOUT_ROW_KEY_LONGKEY_MAGNIFIER_ATTRIBUTE "longkey_magnifier"
#define LAYOUT_ROW_KEY_VIBE_STYLE_ATTRIBUTE LAYOUT_VIBE_STYLE_ATTRIBUTE
#define LAYOUT_ROW_KEY_SOUND_STYLE_ATTRIBUTE LAYOUT_SOUND_STYLE_ATTRIBUTE
#define LAYOUT_ROW_KEY_SIDE_BUTTON_ATTRIBUTE "is_side_button"

#define LAYOUT_ROW_KEY_X_ATTRIBUTE "x"
#define LAYOUT_ROW_KEY_Y_ATTRIBUTE "y"
#define LAYOUT_ROW_KEY_KEY_WIDTH_ATTRIBUTE "width"
#define LAYOUT_ROW_KEY_KEY_HEIGHT_ATTRIBUTE "height"
#define LAYOUT_ROW_KEY_ADD_HIT_LEFT_ATTRIBUTE "hit_left"
#define LAYOUT_ROW_KEY_ADD_HIT_RIGHT_ATTRIBUTE "hit_right"
#define LAYOUT_ROW_KEY_ADD_HIT_TOP_ATTRIBUTE "hit_top"
#define LAYOUT_ROW_KEY_ADD_HIT_BOTTOM_ATTRIBUTE "hit_bottom"
#define LAYOUT_ROW_KEY_POPUP_POS_X_ATTRIBUTE "popup_offset_x"
#define LAYOUT_ROW_KEY_POPUP_POS_Y_ATTRIBUTE "popup_offset_y"
#define LAYOUT_ROW_KEY_POPUP_IMAGE_X_ATTRIBUTE "popup_image_x"
#define LAYOUT_ROW_KEY_POPUP_IMAGE_Y_ATTRIBUTE "popup_image_y"
#define LAYOUT_ROW_KEY_SUBLAYOUT_ID_ATTRIBUTE "sub_layout"

#define LAYOUT_ROW_KEY_LABEL_TYPE_ATTRIBUTE "label_type"
#define LAYOUT_ROW_KEY_IMAGE_LABEL_TYPE_ATTRIBUTE "image_label_type"
#define LAYOUT_ROW_KEY_LONGKEY_TYPE_ATTRIBUTE "long_key_type"
#define LAYOUT_ROW_KEY_LONGKEY_VALUE_ATTRIBUTE "long_key_value"
#define LAYOUT_ROW_KEY_LONGKEY_EVENT_ATTRIBUTE "long_key_event"
#define LAYOUT_ROW_KEY_USE_REPEAT_KEY_ATTRIBUTE "use_repeat_key"
#define LAYOUT_ROW_KEY_DONOT_CLOSE_POPUP_ATTRIBUTE "donot_close_popup"
#define LAYOUT_ROW_KEY_EXTRA_OPTION_ATTRIBUTE "extra_option"
#define LAYOUT_ROW_KEY_MULTITOUCH_TYPE_ATTRIBUTE "multitouch_type"
#define LAYOUT_ROW_KEY_MODIFIER_DECORATION_ID_ATTRIBUTE "modifier_decoration_id"

#define LAYOUT_ROW_KEY_KEY_SPACING_ATTRIBUTE "key_spacing"

#define LAYOUT_ROW_KEY_LABEL_TAG "label"
#define LAYOUT_ROW_KEY_IMAGE_LABEL_TAG "image_label"
#define LAYOUT_ROW_KEY_BACKGROUND_IMAGE_TAG "background_image"
#define LAYOUT_ROW_KEY_KEY_VALUE_TAG "key_value"
#define LAYOUT_ROW_KEY_KEY_EVENT_TAG "key_event"

#define LAYOUT_ROW_KEY_POPUP_INPUTMODE_RECORD_TAG "popup_input_mode_record"
#define LAYOUT_ROW_KEY_POPUP_INPUTMODE_RECORD_INPUTMODE_TAG "popup_input_mode"

#define LAYOUT_ROW_KEY_AUTOPOPUP_KEYS_TAG "auto_popup_keys"
#define LAYOUT_ROW_KEY_AUTOPOPUP_KEYS_SHIFTMODE_ATTRIBUTE "shift_state"
#define LAYOUT_ROW_KEY_MAGNIFIER_LABEL_TAG "magnifier_label"

class Layout_Parser {
    public:
        ~Layout_Parser();
        static Layout_Parser *get_instance();
    int init(const char *dir, char **layout_files, int size);

    void load(int layout_id);
    void unload();
    bool loaded(int layout_id);

    int get_layout_index(const char *name);
    int get_layout_size();
    PSclLayout get_layout_table();
    PSclLayoutKeyCoordinatePointerTable get_key_coordinate_pointer_frame();

    private:
        typedef struct {
            int row_x;
            int row_y;

            sclshort key_width;
            sclshort key_height;
            sclshort key_spacing;
            sclshort row_spacing;

            sclshort add_hit_left;
            sclshort add_hit_right;
            sclshort add_hit_top;
            sclshort add_hit_bottom;

            sclchar *sub_layout;
            sclchar *vibe_style;
            sclchar *sound_style;
            sclchar *label_type;
            sclchar *bg_image_path[SCL_SHIFT_STATE_MAX][SCL_BUTTON_STATE_MAX];
        } Row;
    Layout_Parser();

    int get_drag_state_prop(const xmlNodePtr cur_node);
    int get_shift_state_prop(const xmlNodePtr cur_node);
    int get_button_state_prop(const xmlNodePtr cur_node);
    int get_multitouch_type_prop(const xmlNodePtr cur_node);
    int get_extra_option_prop(const xmlNodePtr cur_node);

    int parsing_layout_table(const char* dir, char **layout_files, int size);
    void parsing_layout_node(const xmlNodePtr cur_node, const PSclLayout cur_rec_layout, int layout_no);
    void loading_coordinate_resources(const xmlNodePtr cur_node, const PSclLayout cur_rec_layout, int layout_no);
    void parsing_background(const xmlNodePtr cur_node, const PSclLayout);
    void parsing_key_background(const xmlNodePtr cur_node, const PSclLayout);
    void parsing_grab_area(const xmlNodePtr cur_node, const PSclLayout cur_rec_layout);

    void set_default_layout_value(const PSclLayout);
    void set_default_row_value(Row*, const PSclLayout cur_rec_layout, const int row_y);
    void set_default_key_coordinate_value(const PSclLayoutKeyCoordinate cur_rec_coordinate, const Row*);

    void free_key_coordinate_table(const PSclLayoutKeyCoordinateTable curTable);

    void parsing_layout_row_node(const xmlNodePtr cur_node, const PSclLayout cur_rec_layout, int *row_y,
        SclLayoutKeyCoordinatePointer **cur_key);
    void parsing_key_coordinate_record_node(const xmlNodePtr cur_node, Row* row, SclLayoutKeyCoordinatePointer *cur_key);

    void parsing_label_record_node(const xmlNodePtr cur_node, const PSclLayoutKeyCoordinate cur_rec);
    void parsing_label_image_record_node(const xmlNodePtr cur_node, const PSclLayoutKeyCoordinate cur_rec);
    void parsing_background_image_record_node(const xmlNodePtr cur_node, const PSclLayoutKeyCoordinate cur_rec);
    void parsing_popup_input_mode_record_node(const xmlNodePtr cur_node, const PSclLayoutKeyCoordinate cur_rec);
    void parsing_key_value_record_node(const xmlNodePtr cur_node, const PSclLayoutKeyCoordinate cur_rec);
    void parsing_key_event_record_node(const xmlNodePtr cur_node, const PSclLayoutKeyCoordinate cur_rec);
    void parsing_auto_popup_keys_record_node(const xmlNodePtr cur_node, const PSclLayoutKeyCoordinate cur_rec);
    void parsing_magnifier_label_record_node(const xmlNodePtr cur_node, const PSclLayoutKeyCoordinate cur_rec);

    void add_layout_string(xmlChar*);
    void release_layout_strings();

    void add_key_string(xmlChar*);
    void release_key_strings();

    private:
        static Layout_Parser *m_instance;
        int m_layout_size;
        SclLayout m_layout_table[MAX_SCL_LAYOUT];
        sclchar *m_layout_files[MAX_SCL_LAYOUT];
        SclLayoutKeyCoordinate* m_key_coordinate_pointer_frame[MAX_SCL_LAYOUT][MAX_KEY];

        std::vector<xmlChar*> m_vec_layout_strings;
        std::vector<xmlChar*> m_vec_key_strings;
        std::string m_dir;
    private:
    class DestructHelper {
        public:
        ~DestructHelper() {
            if (Layout_Parser::m_instance != NULL)
                delete m_instance;
        }
    };
    static DestructHelper des;
};


#endif
