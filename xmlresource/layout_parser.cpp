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

#include <algorithm>
#include <vector>
#include <malloc.h>
#include <string.h>
#include <libxml/parser.h>

#include "layout_parser.h"
#include "default_configure_parser.h" /* use data in default_configure.xml */
#include "xml_parser_utils.h"
#include "layout_parser_helper.h"
#include "put_record.h"
#include "simple_debug.h"
#include <dlog.h>

using namespace std;

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
#define LAYOUT_BACKGROUND_TOGGLED_TAG "button_toggled"

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

#define LAYOUT_ROW_KEY_HINT_STRING_TAG "hint_string"

class LayoutParserImpl {
    public:
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
        LayoutParserImpl();
        ~LayoutParserImpl();

        void load(int layout_id);
        void unload();
        bool loaded(int layout_id);
        void set_directory(const string &dir);
        int get_layout_index(const char *name);
        int get_layout_size();
        PSclLayout get_layout_table();
        PSclLayoutKeyCoordinatePointerTable get_key_coordinate_pointer_frame();
        int get_drag_state_prop(const xmlNodePtr cur_node);
        int get_shift_state_prop(const xmlNodePtr cur_node);
        int get_button_state_prop(const xmlNodePtr cur_node);
        int get_multitouch_type_prop(const xmlNodePtr cur_node);
        int get_extra_option_prop(const xmlNodePtr cur_node);

        int parsing_layout_table(const vector<string> &layout_file_name);
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
        void parsing_hint_string_record_node(const xmlNodePtr cur_node, const PSclLayoutKeyCoordinate cur_rec);

        void add_layout_string(xmlChar*);
        void release_layout_strings();

        void add_key_string(xmlChar*);
        void release_key_strings();

    private:
        int m_layout_size;
        SclLayout m_layout_table[MAX_SCL_LAYOUT];
        vector<string> m_file_names;
        SclLayoutKeyCoordinate* m_key_coordinate_pointer_frame[MAX_SCL_LAYOUT][MAX_KEY];

        std::vector<xmlChar*> m_vec_layout_strings;
        std::vector<xmlChar*> m_vec_key_strings;
        std::string m_dir;
};


LayoutParserImpl::LayoutParserImpl() {
    m_layout_size = 0;
    memset(m_layout_table, 0x00, sizeof(SclLayout) * MAX_SCL_LAYOUT);
    memset(m_key_coordinate_pointer_frame, 0x00, sizeof(SclLayoutKeyCoordinatePointer) * MAX_SCL_LAYOUT * MAX_KEY);
}

LayoutParserImpl::~LayoutParserImpl() {
    for (int i = 0; i < MAX_SCL_LAYOUT; ++i) {
        for (int j = 0; j < MAX_KEY; ++j) {
            free(m_key_coordinate_pointer_frame[i][j]);
            m_key_coordinate_pointer_frame[i][j] = NULL;
        }
    }

    release_layout_strings();
    release_key_strings();
}

void
LayoutParserImpl::load(int layout_id) {
    if (layout_id >= 0 && layout_id < MAX_SCL_LAYOUT) {
        xmlDocPtr doc;
        xmlNodePtr cur_node;

        string input_file = m_dir + "/" + m_file_names[layout_id];
        doc = xmlReadFile(input_file.c_str(), NULL, 0);
        if (doc == NULL) {
            SCLLOG(SclLog::ERROR, "Could not load file: %s.", input_file.c_str());
            LOGE("Could not load file: %s.", input_file.c_str());
            return;
        }

        cur_node = xmlDocGetRootElement(doc);
        if (cur_node == NULL) {
            SCLLOG(SclLog::ERROR, "LayoutParserImpl: empty document.\n");
            LOGE("LayoutParserImpl: empty document.");
            xmlFreeDoc(doc);
            return;
        }
        if (0 != xmlStrcmp(cur_node->name, (const xmlChar*)LAYOUT_TAG))
        {
            SCLLOG(SclLog::ERROR, "LayoutParserImpl: root name error: %s\n!", (char *)cur_node->name);
            LOGE("LayoutParserImpl: root name error: %s!", (char *)cur_node->name);
            xmlFreeDoc(doc);
            return;
        }

        PSclLayout cur_rec_layout = m_layout_table + layout_id;
        loading_coordinate_resources(cur_node, cur_rec_layout, layout_id);

        xmlFreeDoc(doc);
    }
}

void LayoutParserImpl::unload() {
    for (int i = 0; i < MAX_SCL_LAYOUT; ++i) {
        for (int j = 0; j < MAX_KEY; ++j) {
            free(m_key_coordinate_pointer_frame[i][j]);
            m_key_coordinate_pointer_frame[i][j] = NULL;
        }
    }

    release_key_strings();
}

bool
LayoutParserImpl::loaded(int layout_id) {
    bool ret = TRUE;

    if (layout_id >= 0 && layout_id < MAX_SCL_LAYOUT) {
        if (m_key_coordinate_pointer_frame[layout_id][0] == NULL) {
            ret = FALSE;
        }
    }

    return ret;
}

void
LayoutParserImpl::set_directory(const string& dir) {
    m_dir = dir;
}

void
LayoutParserImpl::add_layout_string(xmlChar* newstr) {
    if (newstr) {
        m_vec_layout_strings.push_back(newstr);
    }
}

void
LayoutParserImpl::release_layout_strings() {
    for(size_t loop = 0; loop < m_vec_layout_strings.size(); loop++) {
        if (m_vec_layout_strings[loop]) {
            xmlFree(m_vec_layout_strings[loop]);
        }
    }
    m_vec_layout_strings.clear();
}

void
LayoutParserImpl::add_key_string(xmlChar* newstr) {
    if (newstr) {
        m_vec_key_strings.push_back(newstr);
    }
}

void
LayoutParserImpl::release_key_strings() {
    for(size_t loop = 0; loop < m_vec_key_strings.size(); loop++) {
        if (m_vec_key_strings[loop]) {
            xmlFree(m_vec_key_strings[loop]);
        }
    }
    m_vec_key_strings.clear();
}

int
LayoutParserImpl::get_layout_index(const char *name) {
    string strName = (string)name;
    vector<string>::iterator it;
    // make sure that the m_file_names are sorted.
    it = lower_bound(m_file_names.begin(), m_file_names.end(), strName);
    if (it != m_file_names.end() && *it == strName) {
        return it-m_file_names.begin();
    }
    return NOT_USED;
}

PSclLayout
LayoutParserImpl::get_layout_table() {
    return m_layout_table;
}

int
LayoutParserImpl::get_layout_size() {
    return m_layout_size;
}

PSclLayoutKeyCoordinatePointerTable
LayoutParserImpl::get_key_coordinate_pointer_frame() {
    return m_key_coordinate_pointer_frame;
}

int
LayoutParserImpl::parsing_layout_table(const vector<string> &vec_file) {
    m_file_names = vec_file;
    m_layout_size = vec_file.size();
    vector<string>::const_iterator it;
    for (it = vec_file.begin(); it != vec_file.end(); it++) {
        xmlDocPtr doc;
        xmlNodePtr cur_node;

        string input_file = m_dir + "/" + *it;
        doc = xmlReadFile(input_file.c_str(), NULL, 0);
        if (doc == NULL) {
            SCLLOG(SclLog::DEBUG, "Could not load file: %s.", input_file.c_str());
            return -1;
        }

        cur_node = xmlDocGetRootElement(doc);
        if (cur_node == NULL) {
            SCLLOG(SclLog::DEBUG, "LayoutParserImpl: empty document.\n");
            xmlFreeDoc(doc);
            return -1;
        }
        if (0 != xmlStrcmp(cur_node->name, (const xmlChar*)LAYOUT_TAG))
        {
            SCLLOG(SclLog::DEBUG, "LayoutParserImpl: root name error: %s\n!", (char *)cur_node->name);
            xmlFreeDoc(doc);
            return -1;
        }

        int layout_id = it - vec_file.begin();
        PSclLayout cur_rec_layout = &(m_layout_table[layout_id]);
        parsing_layout_node(cur_node, cur_rec_layout, layout_id);
        cur_rec_layout->name = (sclchar*)strdup(it->c_str());

        xmlFreeDoc(doc);
    }
    return 0;
}

void
LayoutParserImpl::parsing_background(
        const xmlNodePtr cur_node,
        PSclLayout cur_layout) {
    assert(cur_node != NULL);
    assert(cur_layout != NULL);

    xmlNodePtr child_node = cur_node->xmlChildrenNode;
    while ( child_node != NULL) {
        if ( 0 == xmlStrcmp(child_node->name, (const xmlChar* )LAYOUT_BACKGROUND_NORMAL_TAG) ) {
            xmlChar *key = xmlNodeGetContent(child_node);
            cur_layout->image_path[BUTTON_STATE_NORMAL] = (char *)key;
            add_layout_string(key);
        } else if ( 0 == xmlStrcmp(child_node->name, (const xmlChar* )LAYOUT_BACKGROUND_PRESSED_TAG) ) {
            xmlChar *key = xmlNodeGetContent(child_node);
            cur_layout->image_path[BUTTON_STATE_PRESSED] = (char *)key;
            add_layout_string(key);
        } else if ( 0 == xmlStrcmp(child_node->name, (const xmlChar* )LAYOUT_BACKGROUND_DISABLED_TAG ) ) {
            xmlChar *key = xmlNodeGetContent(child_node);
            cur_layout->image_path[BUTTON_STATE_DISABLED] = (char *)key;
            add_layout_string(key);
        } else if ( 0 == xmlStrcmp(child_node->name, (const xmlChar* )LAYOUT_BACKGROUND_TOGGLED_TAG ) ) {
            xmlChar *key = xmlNodeGetContent(child_node);
            cur_layout->image_path[BUTTON_STATE_TOGGLED] = (char *)key;
            add_layout_string(key);
        }

        child_node = child_node->next;
    }
}

void
LayoutParserImpl::parsing_key_background(
        const xmlNodePtr cur_node,
        PSclLayout cur_layout) {
    assert(cur_node != NULL);
    assert(cur_layout != NULL);

    xmlNodePtr child_node = cur_node->xmlChildrenNode;
    while ( child_node != NULL) {
        if ( 0 == xmlStrcmp(child_node->name, (const xmlChar* )"rec") ) {
            int shift_state = get_shift_state_prop(child_node);
            int button_state = get_button_state_prop(child_node);
            for(int shift_loop = 0;shift_loop < SCL_SHIFT_STATE_MAX;shift_loop++) {
                for(int button_loop = 0;button_loop < SCL_BUTTON_STATE_MAX;button_loop++) {
                    if ((shift_state == shift_loop || shift_state == -1) &&
                        (button_state == button_loop || button_state == -1)) {
                            xmlChar* key = xmlNodeGetContent(child_node);
                            cur_layout->key_background_image[shift_loop][button_loop] = (sclchar*)key;
                            add_layout_string(key);
                    }
                }
            }
        }

        child_node = child_node->next;
    }
}


void
LayoutParserImpl::set_default_layout_value(const PSclLayout cur_layout) {
    DefaultConfigParser *default_configure_parser = DefaultConfigParser::get_instance();
    assert(default_configure_parser);
    const PSclDefaultConfigure sclres_default_configure = default_configure_parser->get_default_configure();

    assert(cur_layout != NULL);
    cur_layout->valid = 1;
    cur_layout->style = LAYOUT_STYLE_BASE;
    cur_layout->name = NULL;

    if (sclres_default_configure) {
        cur_layout->width = sclres_default_configure->target_screen_width;
        cur_layout->height = sclres_default_configure->target_screen_height;
    }

    cur_layout->key_width = 0;
    cur_layout->key_height = 0;
    cur_layout->key_spacing = 0;
    cur_layout->row_spacing = 0;

    cur_layout->image_path[BUTTON_STATE_NORMAL]      = NULL;
    cur_layout->image_path[BUTTON_STATE_PRESSED]     = NULL;
    cur_layout->image_path[BUTTON_STATE_DISABLED]    = NULL;
    cur_layout->image_path[BUTTON_STATE_TOGGLED]     = NULL;

    cur_layout->use_sw_button = false;
    cur_layout->use_magnifier_window = false;

    cur_layout->display_mode = DISPLAYMODE_PORTRAIT;
    cur_layout->use_sw_background = false;
    memset (&(cur_layout->bg_color), 0, sizeof(SclColor));
    cur_layout->bg_line_width = 0.0;
    memset (&(cur_layout->bg_line_color), 0, sizeof(SclColor));

    memset (cur_layout->key_background_image, 0, sizeof(cur_layout->key_background_image));
    cur_layout->sound_style = NULL;
    cur_layout->vibe_style = NULL;
    cur_layout->label_type = NULL;
    cur_layout->modifier_decorator = NULL;

    cur_layout->add_grab_left = NOT_USED;
    cur_layout->add_grab_right = NOT_USED;
    cur_layout->add_grab_top = NOT_USED;
    cur_layout->add_grab_bottom = NOT_USED;
}

void
LayoutParserImpl::set_default_row_value(
        Row* row,
        const PSclLayout cur_rec_layout,
        int row_y) {
    if (row) {
        row->row_x = 0;
        row->row_y = row_y;
        row->key_width = cur_rec_layout->key_width;
        row->key_height = cur_rec_layout->key_height;
        row->key_spacing = cur_rec_layout->key_spacing;
        row->row_spacing = cur_rec_layout->row_spacing;
        row->add_hit_left = cur_rec_layout->add_hit_left;
        row->add_hit_right = cur_rec_layout->add_hit_right;
        row->add_hit_top = cur_rec_layout->add_hit_top;
        row->add_hit_bottom = cur_rec_layout->add_hit_bottom;
        row->sub_layout = NULL;
        row->label_type = cur_rec_layout->label_type;
        row->vibe_style = cur_rec_layout->vibe_style;
        row->sound_style = cur_rec_layout->sound_style;
        for(int shift_state = 0; shift_state < SCL_SHIFT_STATE_MAX; shift_state++) {
            for( int button_state = 0; button_state < SCL_BUTTON_STATE_MAX; ++button_state) {
                row->bg_image_path[shift_state][button_state] = cur_rec_layout->key_background_image[shift_state][button_state];
            }
        }
    }
}

void
LayoutParserImpl::set_default_key_coordinate_value(
        const PSclLayoutKeyCoordinate cur_rec_coordinate,
        const Row* row) {
    assert(row != NULL);
    assert(cur_rec_coordinate != NULL);

    int loop;

    if (row && cur_rec_coordinate) {
        cur_rec_coordinate->valid = FALSE;
        cur_rec_coordinate->custom_id = NULL;
        cur_rec_coordinate->button_type = BUTTON_TYPE_NORMAL;
        cur_rec_coordinate->key_type = KEY_TYPE_CHAR;
        cur_rec_coordinate->popup_type = POPUP_TYPE_NONE;
        cur_rec_coordinate->use_magnifier = (sclboolean)true;
        cur_rec_coordinate->use_long_key_magnifier = (sclboolean)false;
        //cur_rec_coordinate->enabled =(sclboolean) TRUE;
        for(loop = 0;loop < SCL_DRAG_STATE_MAX;loop++) {
            cur_rec_coordinate->popup_input_mode[loop] = NULL;
        }
        cur_rec_coordinate->sound_style = row->sound_style;
        cur_rec_coordinate->vibe_style = row->vibe_style;
        cur_rec_coordinate->is_side_button = false;

        cur_rec_coordinate->x = row->row_x;
        cur_rec_coordinate->y = row->row_y;
        cur_rec_coordinate->width = row->key_width;
        cur_rec_coordinate->height = row->key_height;

        cur_rec_coordinate->add_hit_left = row->add_hit_left;
        cur_rec_coordinate->add_hit_right = row->add_hit_right;
        cur_rec_coordinate->add_hit_top = row->add_hit_top;
        cur_rec_coordinate->add_hit_bottom = row->add_hit_bottom;

        cur_rec_coordinate->label_type = row->label_type;
        cur_rec_coordinate->image_label_type = NULL;

        cur_rec_coordinate->label_count = 0;
        for(int shift_state = 0; shift_state < SCL_SHIFT_STATE_MAX; shift_state++) {
            for( int label_for_one = 0; label_for_one < MAX_SIZE_OF_LABEL_FOR_ONE; ++ label_for_one) {
                cur_rec_coordinate->label[shift_state][label_for_one] = NULL;
            }
        }
        for(int shift_state = 0; shift_state < SCL_SHIFT_STATE_MAX; shift_state++) {
            for( int button_state = 0; button_state < SCL_BUTTON_STATE_MAX; ++button_state) {
                cur_rec_coordinate->image_label_path[shift_state][button_state] = NULL;
            }
        }
        for(int shift_state = 0; shift_state < SCL_SHIFT_STATE_MAX; shift_state++) {
            for( int button_state = 0; button_state < SCL_BUTTON_STATE_MAX; ++button_state) {
                //cur_rec_coordinate->bg_image_path[shift_state][button_state] = NULL;
                cur_rec_coordinate->bg_image_path[shift_state][button_state] = row->bg_image_path[shift_state][button_state];
            }
        }
        cur_rec_coordinate->key_value_count = 0;
        for(int shift_state = 0; shift_state < SCL_SHIFT_STATE_MAX; shift_state++) {
            for( int multitap_state = 0; multitap_state < MAX_SIZE_OF_MULTITAP_CHAR; ++multitap_state) {
                cur_rec_coordinate->key_value[shift_state][multitap_state] = NULL;
            }
        }
        for(int shift_state = 0; shift_state < SCL_SHIFT_STATE_MAX; shift_state++) {
            for( int multitap_state = 0; multitap_state < MAX_SIZE_OF_MULTITAP_CHAR; ++multitap_state) {
                cur_rec_coordinate->key_event[shift_state][multitap_state] = 0;
            }
        }
        /* assume the long_key_type is the same with key_type, by default */
        cur_rec_coordinate->long_key_type = cur_rec_coordinate->key_type;
        cur_rec_coordinate->long_key_value = NULL;
        cur_rec_coordinate->long_key_event = 0;

        cur_rec_coordinate->use_repeat_key = false;
        for(int shift_state = 0; shift_state < SCL_SHIFT_STATE_MAX; shift_state++) {
            for( int autopopup_state = 0; autopopup_state < MAX_SIZE_OF_AUTOPOPUP_STRING; ++autopopup_state) {
                cur_rec_coordinate->autopopup_key_labels[shift_state][autopopup_state] = NULL;
                cur_rec_coordinate->autopopup_key_events[shift_state][autopopup_state] = 0;
                cur_rec_coordinate->autopopup_key_values[shift_state][autopopup_state] = NULL;
            }
        }
        cur_rec_coordinate->dont_close_popup = false;
        cur_rec_coordinate->extra_option = NOT_USED;
        cur_rec_coordinate->multitouch_type = SCL_MULTI_TOUCH_TYPE_EXCLUSIVE;
        cur_rec_coordinate->modifier_decorator = NULL;
        for(int shift_state = 0; shift_state < SCL_SHIFT_STATE_MAX; shift_state++) {
            for( int multitap_state = 0; multitap_state < MAX_SIZE_OF_MULTITAP_CHAR; ++multitap_state) {
                cur_rec_coordinate->hint_string[shift_state][multitap_state] = NULL;
            }
        }
        cur_rec_coordinate->sub_layout = row->sub_layout;
    }
}

void
LayoutParserImpl::parsing_grab_area(
        const xmlNodePtr cur_node,
        const PSclLayout cur_rec_layout) {
    assert(cur_node != NULL);

    xmlNodePtr child_node = cur_node->xmlChildrenNode;
    while (child_node!=NULL) {
        if (0 == xmlStrcmp(child_node->name, (const xmlChar*)LAYOUT_ADD_GRAB_LEFT_TAG) ) {
            cur_rec_layout->add_grab_left = get_content_int(child_node);
        }
        else if (0 == xmlStrcmp(child_node->name, (const xmlChar*)LAYOUT_ADD_GRAB_RIGHT_TAG) ) {
            cur_rec_layout->add_grab_right = get_content_int(child_node);
        }
        else if (0 == xmlStrcmp(child_node->name, (const xmlChar*)LAYOUT_ADD_GRAB_TOP_TAG) ) {
            cur_rec_layout->add_grab_top = get_content_int(child_node);
        }
        else if (0 == xmlStrcmp(child_node->name, (const xmlChar*)LAYOUT_ADD_GRAB_BOTTOM_TAG) ) {
            cur_rec_layout->add_grab_bottom= get_content_int(child_node);
        }
        child_node = child_node->next;
    }
}

void
LayoutParserImpl::parsing_layout_node(
        const xmlNodePtr cur_node,
        const PSclLayout cur_rec_layout,
        int layout_no) {
    assert(cur_node != NULL);
    assert(cur_rec_layout != NULL);

    set_default_layout_value(cur_rec_layout);

    xmlChar* key;

    if (equal_prop(cur_node, LAYOUT_DIRECTION_ATTRIBUTE,
        LAYOUT_DIRECTION_ATTRIBUTE_LANDSCAPE_VALUE)) {
        cur_rec_layout->display_mode = DISPLAYMODE_LANDSCAPE;
    }

    if (equal_prop(cur_node, LAYOUT_STYLE_ATTRIBUTE,
        LAYOUT_STYLE_ATTRIBUTE_POPUP_VALUE)) {
            cur_rec_layout->style = LAYOUT_STYLE_POPUP;
    } else if (equal_prop(cur_node, LAYOUT_STYLE_ATTRIBUTE,
        LAYOUT_STYLE_ATTRIBUTE_POPUP_GRAB_VALUE)) {
            cur_rec_layout->style = LAYOUT_STYLE_POPUP_GRAB;
    }

    get_prop_number(cur_node, LAYOUT_WIDTH_ATTRIBUTE, &(cur_rec_layout->width));
    get_prop_number(cur_node, LAYOUT_HEIGHT_ATTRIBUTE, &(cur_rec_layout->height));
    get_prop_bool(cur_node, LAYOUT_MAGNIFIER_ATTRIBUTE, &(cur_rec_layout->use_magnifier_window));

    get_prop_bool(cur_node, LAYOUT_PART_BACKGROUND_ATTRIBUTE, &(cur_rec_layout->extract_background));
    get_prop_bool(cur_node, LAYOUT_SW_BUTTON_ATTRIBUTE, &(cur_rec_layout->use_sw_button));
    get_prop_bool(cur_node, LAYOUT_SW_BACKGROUND_ATTRIBUTE, &(cur_rec_layout->use_sw_background));

    get_prop_number(cur_node, LAYOUT_KEY_WIDTH_ATTRIBUTE, &(cur_rec_layout->key_width));
    get_prop_number(cur_node, LAYOUT_KEY_HEIGHT_ATTRIBUTE, &(cur_rec_layout->key_height));
    get_prop_number(cur_node, LAYOUT_KEY_SPACING_ATTRIBUTE, &(cur_rec_layout->key_spacing));
    get_prop_number(cur_node, LAYOUT_ROW_SPACING_ATTRIBUTE, &(cur_rec_layout->row_spacing));

    get_prop_number(cur_node, LAYOUT_KEY_ADD_HIT_LEFT_ATTRIBUTE, &(cur_rec_layout->add_hit_left));
    get_prop_number(cur_node, LAYOUT_KEY_ADD_HIT_RIGHT_ATTRIBUTE, &(cur_rec_layout->add_hit_right));
    get_prop_number(cur_node, LAYOUT_KEY_ADD_HIT_TOP_ATTRIBUTE, &(cur_rec_layout->add_hit_top));
    get_prop_number(cur_node, LAYOUT_KEY_ADD_HIT_BOTTOM_ATTRIBUTE, &(cur_rec_layout->add_hit_bottom));

    key = xmlGetProp(cur_node, (const xmlChar*)LAYOUT_VIBE_STYLE_ATTRIBUTE);
    if (key) {
        cur_rec_layout->vibe_style = (sclchar*)key;
        add_layout_string(key);
    }
    key = xmlGetProp(cur_node, (const xmlChar*)LAYOUT_SOUND_STYLE_ATTRIBUTE);
    if (key) {
        cur_rec_layout->sound_style = (sclchar*)key;
        add_layout_string(key);
    }
    key = xmlGetProp(cur_node, (const xmlChar*)LAYOUT_LABEL_TYPE_ATTRIBUTE);
    if (key) {
        cur_rec_layout->label_type = (sclchar*)key;
        add_layout_string(key);
    }
    key = xmlGetProp(cur_node, (const xmlChar*)LAYOUT_MODIFIER_DECORATION_ATTRIBUTE);
    if (key) {
        cur_rec_layout->modifier_decorator = (sclchar*)key;
        add_layout_string(key);
    }

    xmlNodePtr child_node = cur_node->xmlChildrenNode;
    while (child_node != NULL) {

        /* row node: layout coordinate resources is no need to parsing at this time */

        if ( 0 == xmlStrcmp(child_node->name, (const xmlChar*)LAYOUT_BACKGROUND_TAG)) {
            parsing_background(child_node, cur_rec_layout);
        }
        else if ( 0 == xmlStrcmp(child_node->name, (const xmlChar*)LAYOUT_KEY_BACKGROUND_TAG)) {
            parsing_key_background(child_node, cur_rec_layout);
        }
        else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)LAYOUT_ADD_GRAB_TAG)) {
            parsing_grab_area(child_node, cur_rec_layout);
        }

        child_node = child_node->next;
    }
}

void
LayoutParserImpl::loading_coordinate_resources(
        const xmlNodePtr cur_node,
        const PSclLayout cur_rec_layout,
        int layout_no) {
    assert(cur_node != NULL);

    int row_y = 0;

    SclLayoutKeyCoordinatePointer *cur_key = &m_key_coordinate_pointer_frame[layout_no][0];

    if (*cur_key == NULL) {
        xmlNodePtr child_node = cur_node->xmlChildrenNode;
        while (child_node != NULL) {
            if ( 0 == xmlStrcmp(child_node->name, (const xmlChar*)LAYOUT_ROW_TAG)) {
                parsing_layout_row_node(child_node, cur_rec_layout, &row_y, &cur_key);
            }

            child_node = child_node->next;
        }
    }
}

void
LayoutParserImpl::parsing_layout_row_node(
        const xmlNodePtr cur_node,
        const PSclLayout cur_rec_layout,
        int *row_y,
        SclLayoutKeyCoordinatePointer **cur_key) {
    assert(cur_node != NULL);

    Row row;
    if (row_y) {
        set_default_row_value(&row, cur_rec_layout, *row_y);
    }

    get_prop_number(cur_node, LAYOUT_ROW_X_ATTRIBUTE, &(row.row_x));
    get_prop_number(cur_node, LAYOUT_ROW_Y_ATTRIBUTE, &(row.row_y));
    get_prop_number(cur_node, LAYOUT_ROW_KEY_WIDTH_ATTRIBUTE, &(row.key_width));
    get_prop_number(cur_node, LAYOUT_ROW_KEY_HEIGHT_ATTRIBUTE, &(row.key_height));
    get_prop_number(cur_node, LAYOUT_ROW_KEY_SPACING_ATTRIBUTE, &(row.key_spacing));

    xmlChar *key = xmlGetProp(cur_node, (const xmlChar*)LAYOUT_ROW_SUBLAYOUT_ID_ATTRIBUTE);
    if (key) {
        row.sub_layout = (sclchar*)key;
        add_key_string(key);
    }

    xmlNodePtr child_node = cur_node->xmlChildrenNode;
    while (child_node != NULL) {
        if ( 0 == xmlStrcmp(child_node->name, (const xmlChar*)LAYOUT_ROW_KEY_TAG)) {
            parsing_key_coordinate_record_node(child_node, &row, *cur_key);
            (*cur_key)++;
        }
        child_node = child_node->next;
    }
    /* Do not increase row_y position when this row belongs to specific sublayout */
    if (row.sub_layout == NULL) {
        if (row_y) {
            *row_y = row.row_y + row.key_height + row.row_spacing;
        }
    }
}

int
LayoutParserImpl::get_drag_state_prop(const xmlNodePtr cur_node) {
    assert(cur_node != NULL);
    typedef struct _Match_Struct{
        int value;
        const char* key;
    }Match_Struct;
    static Match_Struct table[] = {
        {SCL_DRAG_STATE_NONE,       "drag_state_none"       },
        {SCL_DRAG_STATE_LEFT,       "drag_state_left"       },
        {SCL_DRAG_STATE_RIGHT,      "drag_state_right"      },
        {SCL_DRAG_STATE_UP,         "drag_state_up"         },
        {SCL_DRAG_STATE_DOWN,       "drag_state_down"       },
        {SCL_DRAG_STATE_INVALID,    "drag_state_invalid"    },
        {SCL_DRAG_STATE_RETURN,     "drag_state_return"     }
    };

    xmlChar* key = xmlGetProp(cur_node, (const xmlChar*)"drag_state");
    if (key == NULL) return SCL_DRAG_STATE_NONE;

    int drag_state = SCL_DRAG_STATE_NONE;

    for(size_t i = 0; i < sizeof(table)/sizeof(Match_Struct); ++i) {
        if (0 == strcmp((const char*)key, table[i].key))
        {
            drag_state = table[i].value;
            break;
        }
    }

    xmlFree(key);
    return drag_state;
}

int
LayoutParserImpl::get_shift_state_prop(const xmlNodePtr cur_node) {
    assert(cur_node != NULL);

    int shift_state = -1;

    if (equal_prop(cur_node, "shift", "on")) {
        shift_state = SCL_SHIFT_STATE_ON;
    } else if (equal_prop(cur_node, "shift", "off")) {
        shift_state = SCL_SHIFT_STATE_OFF;
    } else if (equal_prop(cur_node, "shift", "loc")) {
        shift_state = SCL_SHIFT_STATE_LOCK;
    }
    return shift_state;
}
int
LayoutParserImpl::get_button_state_prop(const xmlNodePtr cur_node) {
    assert(cur_node != NULL);
    int button_state = -1;

    if (equal_prop(cur_node, "button", "pressed")) {
        button_state = BUTTON_STATE_PRESSED;
    } else if (equal_prop(cur_node, "button", "normal")) {
        button_state = BUTTON_STATE_NORMAL;
    } else if (equal_prop(cur_node, "button", "disabled")) {
        button_state = BUTTON_STATE_DISABLED;
    } else if (equal_prop(cur_node, "button", "toggled")) {
        button_state = BUTTON_STATE_TOGGLED;
    }
    return button_state;
}

int
LayoutParserImpl::get_multitouch_type_prop(const xmlNodePtr cur_node) {
    assert(cur_node != NULL);
    typedef struct _Match_Struct{
        int value;
        const char* key;
    }Match_Struct;
    static Match_Struct table[] = {
        {SCL_MULTI_TOUCH_TYPE_EXCLUSIVE,        "exclusive"         },
        {SCL_MULTI_TOUCH_TYPE_SETTLE_PREVIOUS,  "settle_previous"   },
        {SCL_MULTI_TOUCH_TYPE_COOPERATIVE,      "cooperative"       },
        {SCL_MULTI_TOUCH_TYPE_GRAB_SUB_EVENTS,  "grab_sub_events"   },
    };

    xmlChar* key = xmlGetProp(cur_node, (const xmlChar*)LAYOUT_ROW_KEY_MULTITOUCH_TYPE_ATTRIBUTE);
    if (key == NULL) return SCL_MULTI_TOUCH_TYPE_EXCLUSIVE;

    int type = SCL_MULTI_TOUCH_TYPE_EXCLUSIVE;

    for(size_t i = 0; i < sizeof(table)/sizeof(Match_Struct); ++i) {
        if (0 == strcmp((const char*)key, table[i].key))
        {
            type = table[i].value;
            break;
        }
    }

    xmlFree(key);
    return type;
}

int
LayoutParserImpl::get_extra_option_prop(
        const xmlNodePtr cur_node) {
    assert(cur_node != NULL);
    typedef struct _Match_Struct{
        int value;
        const char* key;
    }Match_Struct;
    static Match_Struct table[] = {
        {DIRECTION_EXTRA_OPTION_4_DIRECTIONS,                        "4-directions"                 },
        {DIRECTION_EXTRA_OPTION_8_DIRECTIONS,                        "8-directions"                 },
        {DIRECTION_EXTRA_OPTION_4_DIRECTIONS_WITH_LONG,              "4-directions-long"            },
        {DIRECTION_EXTRA_OPTION_8_DIRECTIONS_WITH_LONG,              "8-directions-long"            },
        {DIRECTION_EXTRA_OPTION_4_DIRECTIONS_WITH_RETURN,            "4-directions-return"          },
        {DIRECTION_EXTRA_OPTION_8_DIRECTIONS_WITH_RETURN,            "8-directions-return"          },
        {DIRECTION_EXTRA_OPTION_4_DIRECTIONS_WITH_RETURN_AND_CURVE,  "4-directions-return-curve"    },
    };

    xmlChar* key = xmlGetProp(cur_node, (const xmlChar*)LAYOUT_ROW_KEY_EXTRA_OPTION_ATTRIBUTE);
    if (key == NULL) return 0;

    int type = DIRECTION_EXTRA_OPTION_4_DIRECTIONS;

    for(size_t i = 0; i < sizeof(table)/sizeof(Match_Struct); ++i) {
        if (0 == strcmp((const char*)key, table[i].key))
        {
            type = table[i].value;
            break;
        }
    }

    xmlFree(key);
    return type;
}


void
LayoutParserImpl::parsing_label_record_node(
        const xmlNodePtr cur_node,
        const PSclLayoutKeyCoordinate cur_rec) {
    assert(cur_node != NULL);
    assert(cur_rec != NULL);
    assert(0 == xmlStrcmp(cur_node->name, (const xmlChar*)"label"));
    xmlNodePtr child_node = cur_node->xmlChildrenNode;

    while (child_node != NULL) {
        if (0 == xmlStrcmp(child_node->name, (const xmlChar*)"rec") ) {
            int shift_state = get_shift_state_prop(child_node);

            int label_for_one_state = 0;
            get_prop_number(child_node, "multi", &label_for_one_state);
            if (cur_rec->label_count < (label_for_one_state + 1)) {
                cur_rec->label_count = (label_for_one_state + 1);
            }

            sclboolean auto_upper = FALSE;
            get_prop_bool(child_node, "auto_upper", &auto_upper);

            if (label_for_one_state >= 0 && label_for_one_state < MAX_SIZE_OF_LABEL_FOR_ONE) {
                for(int shift_loop = 0;shift_loop < SCL_SHIFT_STATE_MAX;shift_loop++) {
                    if ((shift_state == shift_loop || shift_state == -1)) {
                        xmlChar* key = xmlNodeGetContent(child_node);
                        cur_rec->label[shift_loop][label_for_one_state] = (sclchar*)key;
                        if (auto_upper) {
                            if (xmlStrlen(key) == 1 && shift_loop != SCL_SHIFT_STATE_OFF) {
                                /* Let's manipulate the string for auto_upper */
                                *key = toupper(*(cur_rec->label[SCL_SHIFT_STATE_OFF][label_for_one_state]));
                            }
                        }
                        /* If current key_value is NULL, let's just consider this label is the default key_value */
                        if (label_for_one_state == 0) {
                            if (cur_rec->key_value[shift_loop][label_for_one_state] == NULL) {
                                cur_rec->key_value[shift_loop][label_for_one_state] = (sclchar*)key;
                            }
                        }
                        add_key_string(key);
                    }
                }
            }
        }
        child_node = child_node->next;
    }
}

void
LayoutParserImpl::parsing_magnifier_label_record_node(
        const xmlNodePtr cur_node,
        const PSclLayoutKeyCoordinate cur_rec) {
    assert(cur_node != NULL);
    assert(cur_rec != NULL);
    assert(0 == xmlStrcmp(cur_node->name, (const xmlChar*)"magnifier_label"));
    xmlNodePtr child_node = cur_node->xmlChildrenNode;

    while (child_node != NULL) {
        if (0 == xmlStrcmp(child_node->name, (const xmlChar*)"rec") ) {
            int shift_state = get_shift_state_prop(child_node);

            int label_for_one_state = 0;
            get_prop_number(child_node, "multi", &label_for_one_state);

            if (label_for_one_state >= 0 && label_for_one_state < MAX_SIZE_OF_LABEL_FOR_ONE) {
                for(int shift_loop = 0;shift_loop < SCL_SHIFT_STATE_MAX;shift_loop++) {
                    if ((shift_state == shift_loop || shift_state == -1)) {
                        xmlChar* key = xmlNodeGetContent(child_node);
                        cur_rec->magnifier_label[shift_loop][label_for_one_state] = (sclchar*)key;
                        add_key_string(key);
                    }
                }
            }
        }
        child_node = child_node->next;
    }
}

void
LayoutParserImpl::parsing_hint_string_record_node(
    const xmlNodePtr cur_node,
    const PSclLayoutKeyCoordinate cur_rec) {
        assert(cur_node != NULL);
        assert(cur_rec != NULL);
        assert(0 == xmlStrcmp(cur_node->name, (const xmlChar*)"hint_string"));
        xmlNodePtr child_node = cur_node->xmlChildrenNode;

        while (child_node != NULL) {
            if (0 == xmlStrcmp(child_node->name, (const xmlChar*)"rec") ) {
                int shift_state = get_shift_state_prop(child_node);
                int multichar_state = 0;
                get_prop_number(child_node, "multichar_state", &multichar_state);

                if (multichar_state >= 0 && multichar_state < MAX_SIZE_OF_MULTITAP_CHAR) {
                    for(int shift_loop = 0;shift_loop < SCL_SHIFT_STATE_MAX;shift_loop++) {
                        if ((shift_state == shift_loop || shift_state == -1)) {
                            xmlChar* key = xmlNodeGetContent(child_node);
                            cur_rec->hint_string[shift_loop][multichar_state] = (sclchar*)key;
                            add_key_string(key);
                        }
                    }
                }
            }
            child_node = child_node->next;
        }
}

void
LayoutParserImpl::parsing_label_image_record_node(
        const xmlNodePtr cur_node,
        const PSclLayoutKeyCoordinate cur_rec) {
    assert(cur_node != NULL);
    assert(cur_rec != NULL);
    assert(0 == xmlStrcmp(cur_node->name, (const xmlChar*)"image_label"));
    xmlNodePtr child_node = cur_node->xmlChildrenNode;

    while (child_node != NULL) {
        if (0 == xmlStrcmp(child_node->name, (const xmlChar*)"rec") ) {
            int shift_state = get_shift_state_prop(child_node);
            int button_state = get_button_state_prop(child_node);
            for(int shift_loop = 0;shift_loop < SCL_SHIFT_STATE_MAX;shift_loop++) {
                for(int button_loop = 0;button_loop < SCL_BUTTON_STATE_MAX;button_loop++) {
                    if ((shift_state == shift_loop || shift_state == -1) &&
                        (button_state == button_loop || button_state == -1)) {
                            xmlChar* key = xmlNodeGetContent(child_node);
                            cur_rec->image_label_path[shift_loop][button_loop] = (sclchar*)key;
                            add_key_string(key);
                    }
                }
            }
        }
        child_node = child_node->next;
    }
}

void
LayoutParserImpl::parsing_background_image_record_node(
        const xmlNodePtr cur_node,
        const PSclLayoutKeyCoordinate cur_rec) {
    assert(cur_node != NULL);
    assert(cur_rec != NULL);
    assert(0 == xmlStrcmp(cur_node->name, (const xmlChar*)"background_image"));
    xmlNodePtr child_node = cur_node->xmlChildrenNode;

    while (child_node != NULL) {
        if (0 == xmlStrcmp(child_node->name, (const xmlChar*)"rec") ) {
            int shift_state = get_shift_state_prop(child_node);
            int button_state = get_button_state_prop(child_node);
            for(int shift_loop = 0;shift_loop < SCL_SHIFT_STATE_MAX;shift_loop++) {
                for(int button_loop = 0;button_loop < SCL_BUTTON_STATE_MAX;button_loop++) {
                    if ((shift_state == shift_loop || shift_state == -1) &&
                        (button_state == button_loop || button_state == -1)) {
                            xmlChar* key = xmlNodeGetContent(child_node);
                            cur_rec->bg_image_path[shift_loop][button_loop] = (sclchar*)key;
                            add_key_string(key);
                    }
                }
            }
        }
        child_node = child_node->next;
    }
}

void
LayoutParserImpl::parsing_key_value_record_node(
        const xmlNodePtr cur_node,
        const PSclLayoutKeyCoordinate cur_rec) {
    assert(cur_node != NULL);
    assert(cur_rec != NULL);
    assert(0 == xmlStrcmp(cur_node->name, (const xmlChar*)"key_value"));
    xmlNodePtr child_node = cur_node->xmlChildrenNode;

    while (child_node != NULL) {
        if (0 == xmlStrcmp(child_node->name, (const xmlChar*)"rec") ) {
            int shift_state = get_shift_state_prop(child_node);
            int multichar_state = 0;
            get_prop_number(child_node, "multichar_state", &multichar_state);

            sclboolean auto_upper = FALSE;
            get_prop_bool(child_node, "auto_upper", &auto_upper);

            if (multichar_state >= 0 && multichar_state < MAX_SIZE_OF_MULTITAP_CHAR) {
                for(int shift_loop = 0;shift_loop < SCL_SHIFT_STATE_MAX;shift_loop++) {
                    if ((shift_state == shift_loop || shift_state == -1)) {
                        xmlChar* key = xmlNodeGetContent(child_node);
                        cur_rec->key_value[shift_loop][multichar_state] = (sclchar*)key;
                        if (auto_upper) {
                            if (xmlStrlen(key) == 1 && shift_loop != SCL_SHIFT_STATE_OFF) {
                                /* Let's manipulate the string for auto_upper */
                                *key = toupper(*(cur_rec->key_value[SCL_SHIFT_STATE_OFF][multichar_state]));
                            }
                        }
                        add_key_string(key);
                    }
                }
            }
        }
        child_node = child_node->next;
    }
}

void
LayoutParserImpl::parsing_key_event_record_node(
        const xmlNodePtr cur_node,
        const PSclLayoutKeyCoordinate cur_rec) {
    assert(cur_node != NULL);
    assert(cur_rec != NULL);
    assert(0 == xmlStrcmp(cur_node->name, (const xmlChar*)"key_event"));
    xmlNodePtr child_node = cur_node->xmlChildrenNode;

    while (child_node != NULL) {
        if (0 == xmlStrcmp(child_node->name, (const xmlChar*)"rec") ) {
            int shift_state = get_shift_state_prop(child_node);
            int multichar_state = 0;
            get_prop_number(child_node, "multichar_state", &multichar_state);

            if (multichar_state >= 0 && multichar_state < MAX_SIZE_OF_MULTITAP_CHAR) {
                for(int shift_loop = 0;shift_loop < SCL_SHIFT_STATE_MAX;shift_loop++) {
                    if ((shift_state == shift_loop || shift_state == -1)) {
                        cur_rec->key_event[shift_loop][multichar_state] = (sclulong)get_content_dex_string_int(child_node);
                    }
                }
            }
        }
        child_node = child_node->next;
    }
}

void
LayoutParserImpl::parsing_auto_popup_keys_record_node(
        const xmlNodePtr cur_node,
        const PSclLayoutKeyCoordinate cur_rec) {
    assert(cur_node != NULL);
    assert(cur_rec != NULL);
    assert(0 == xmlStrcmp(cur_node->name, (const xmlChar*)"auto_popup_keys"));
    xmlNodePtr child_node = cur_node->xmlChildrenNode;

    while (child_node != NULL) {
        int shift_state = get_shift_state_prop(child_node);
        int autopopup_state = 0;
        get_prop_number(child_node, "autopopup", &autopopup_state);

        sclboolean auto_upper = FALSE;
        get_prop_bool(child_node, "auto_upper", &auto_upper);

        if (autopopup_state >= 0 && autopopup_state < MAX_SIZE_OF_AUTOPOPUP_STRING) {
            for(int shift_loop = 0;shift_loop < SCL_SHIFT_STATE_MAX;shift_loop++) {
                if ((shift_state == shift_loop || shift_state == -1)) {
                    xmlChar* key = xmlNodeGetContent(child_node);
                    if (0 == xmlStrcmp(child_node->name, (const xmlChar*)"label") ) {
                        if (auto_upper) {
                            if (xmlStrlen(key) == 1 && shift_loop != SCL_SHIFT_STATE_OFF) {
                                /* Let's manipulate the string for auto_upper */
                                *key = toupper(*(cur_rec->autopopup_key_labels[SCL_SHIFT_STATE_OFF][autopopup_state]));
                            }
                        }
                        cur_rec->autopopup_key_labels[shift_loop][autopopup_state] = (sclchar*)key;
                        /* If current key_value is NULL, let's just consider this label is the default key_value */
                        if (cur_rec->autopopup_key_values[shift_loop][autopopup_state] == NULL) {
                            cur_rec->autopopup_key_values[shift_loop][autopopup_state] = (sclchar*)key;
                        }
                    } else if (0 == xmlStrcmp(child_node->name, (const xmlChar*)"value") ) {
                        cur_rec->autopopup_key_values[shift_loop][autopopup_state] = (sclchar*)key;
                    } else if (0 == xmlStrcmp(child_node->name, (const xmlChar*)"event") ) {
                        cur_rec->autopopup_key_events[shift_loop][autopopup_state] = atoi((sclchar*)key);
                    }
                    add_key_string(key);
                }
            }
        }
        child_node = child_node->next;
    }
}

void
LayoutParserImpl::parsing_key_coordinate_record_node(
        const xmlNodePtr cur_node, Row *row,
        SclLayoutKeyCoordinatePointer *cur_rec_coordinate) {
    assert(cur_node != NULL);
    assert(cur_rec_coordinate != NULL);

    *cur_rec_coordinate = (SclLayoutKeyCoordinatePointer)malloc(sizeof(SclLayoutKeyCoordinate));
    if (*cur_rec_coordinate == NULL) {
        SCLLOG(SclLog::ERROR, "LayoutParserImpl: memory malloc eror.\n");
        return;
    }
    memset(*cur_rec_coordinate, 0x00, sizeof(SclLayoutKeyCoordinate));

    set_default_key_coordinate_value(*cur_rec_coordinate, row);

    xmlChar *key;

    int key_spacing = 0;

    if (row) {
        key_spacing = row->key_spacing;
    }

    (*cur_rec_coordinate)->valid = TRUE;

    key = xmlGetProp(cur_node, (const xmlChar*)LAYOUT_ROW_KEY_CUSTOMID_ATTRIBUTE);
    if (key) {
        (*cur_rec_coordinate)->custom_id = (sclchar*)key;
        add_key_string(key);
    }

    key = xmlGetProp(cur_node, (const xmlChar*)LAYOUT_ROW_KEY_BUTTON_TYPE_ATTRIBUTE);
    if (key) {
        (*cur_rec_coordinate)->button_type = (SCLButtonType)ButtonTypeHelper::Int((const char*)key);
        xmlFree(key);
    }

    key = xmlGetProp(cur_node, (const xmlChar*)LAYOUT_ROW_KEY_KEY_TYPE_ATTRIBUTE);
    if (key) {
        (*cur_rec_coordinate)->key_type = (SCLKeyType)KeyTypeHelper::Int((const char*)key);
        xmlFree(key);
    }
    key = xmlGetProp(cur_node, (const xmlChar*)LAYOUT_ROW_KEY_POPUP_TYPE_ATTRIBUTE);
    if (key) {
        (*cur_rec_coordinate)->popup_type = (SCLPopupType)PopupTypeHelper::Int((const char*)key);
        xmlFree(key);
    }

    get_prop_bool(cur_node, LAYOUT_ROW_KEY_MAGNIFIER_ATTRIBUTE, &((*cur_rec_coordinate)->use_magnifier));
    get_prop_bool(cur_node, LAYOUT_ROW_KEY_LONGKEY_MAGNIFIER_ATTRIBUTE,
        &((*cur_rec_coordinate)->use_long_key_magnifier));

    key = xmlGetProp(cur_node, (const xmlChar*)LAYOUT_VIBE_STYLE_ATTRIBUTE);
    if (key) {
        (*cur_rec_coordinate)->vibe_style = (sclchar*)key;
        add_key_string(key);
    }
    key = xmlGetProp(cur_node, (const xmlChar*)LAYOUT_SOUND_STYLE_ATTRIBUTE);
    if (key) {
        (*cur_rec_coordinate)->sound_style = (sclchar*)key;
        add_key_string(key);
    }

    get_prop_bool(cur_node, LAYOUT_ROW_KEY_SIDE_BUTTON_ATTRIBUTE, &((*cur_rec_coordinate)->is_side_button));

    get_prop_number(cur_node, LAYOUT_ROW_KEY_X_ATTRIBUTE, &((*cur_rec_coordinate)->x));
    get_prop_number(cur_node, LAYOUT_ROW_KEY_Y_ATTRIBUTE, &((*cur_rec_coordinate)->y));
    get_prop_number(cur_node, LAYOUT_ROW_KEY_KEY_WIDTH_ATTRIBUTE, &((*cur_rec_coordinate)->width));
    get_prop_number(cur_node, LAYOUT_ROW_KEY_KEY_HEIGHT_ATTRIBUTE, &((*cur_rec_coordinate)->height));

    get_prop_number(cur_node, LAYOUT_ROW_KEY_ADD_HIT_LEFT_ATTRIBUTE, &((*cur_rec_coordinate)->add_hit_left));
    get_prop_number(cur_node, LAYOUT_ROW_KEY_ADD_HIT_RIGHT_ATTRIBUTE, &((*cur_rec_coordinate)->add_hit_right));
    get_prop_number(cur_node, LAYOUT_ROW_KEY_ADD_HIT_TOP_ATTRIBUTE, &((*cur_rec_coordinate)->add_hit_top));
    get_prop_number(cur_node, LAYOUT_ROW_KEY_ADD_HIT_BOTTOM_ATTRIBUTE, &((*cur_rec_coordinate)->add_hit_bottom));

    get_prop_number(cur_node, LAYOUT_ROW_KEY_POPUP_POS_X_ATTRIBUTE, &((*cur_rec_coordinate)->popup_relative_x));
    get_prop_number(cur_node, LAYOUT_ROW_KEY_POPUP_POS_Y_ATTRIBUTE, &((*cur_rec_coordinate)->popup_relative_y));

    get_prop_number(cur_node, LAYOUT_ROW_KEY_POPUP_IMAGE_X_ATTRIBUTE, &((*cur_rec_coordinate)->extract_offset_x));
    get_prop_number(cur_node, LAYOUT_ROW_KEY_POPUP_IMAGE_Y_ATTRIBUTE, &((*cur_rec_coordinate)->extract_offset_y));

    key = xmlGetProp(cur_node, (const xmlChar*)LAYOUT_ROW_KEY_LONGKEY_TYPE_ATTRIBUTE);
    if (key) {
        (*cur_rec_coordinate)->long_key_type = (SCLKeyType)KeyTypeHelper::Int((const char*)key);
        xmlFree(key);
    } else {
        /* assume the long_key_type is the same with key_type, by default */
        (*cur_rec_coordinate)->long_key_type = (*cur_rec_coordinate)->key_type;
    }

    key = xmlGetProp(cur_node, (const xmlChar*)LAYOUT_ROW_KEY_LONGKEY_VALUE_ATTRIBUTE);
    if (key) {
        (*cur_rec_coordinate)->long_key_value = (sclchar*)key;
        add_key_string(key);
    }

    get_prop_number(cur_node, LAYOUT_ROW_KEY_LONGKEY_EVENT_ATTRIBUTE, &((*cur_rec_coordinate)->long_key_event));

    get_prop_bool(cur_node, LAYOUT_ROW_KEY_USE_REPEAT_KEY_ATTRIBUTE, &((*cur_rec_coordinate)->use_repeat_key));
    get_prop_bool(cur_node, LAYOUT_ROW_KEY_DONOT_CLOSE_POPUP_ATTRIBUTE, &((*cur_rec_coordinate)->dont_close_popup));

    (*cur_rec_coordinate)->extra_option = get_extra_option_prop(cur_node);
    (*cur_rec_coordinate)->multitouch_type = get_multitouch_type_prop(cur_node);

    key = xmlGetProp(cur_node, (const xmlChar*)LAYOUT_ROW_KEY_SUBLAYOUT_ID_ATTRIBUTE);
    if (key) {
        (*cur_rec_coordinate)->sub_layout = (sclchar*)key;
        add_key_string(key);
    }

    key = xmlGetProp(cur_node, (const xmlChar*)LAYOUT_ROW_KEY_LABEL_TYPE_ATTRIBUTE);
    if (key) {
        (*cur_rec_coordinate)->label_type = (sclchar*)key;
        add_key_string(key);
    }

    key = xmlGetProp(cur_node, (const xmlChar*)LAYOUT_ROW_KEY_IMAGE_LABEL_TYPE_ATTRIBUTE);
    if (key) {
        (*cur_rec_coordinate)->image_label_type = (sclchar*)key;
        add_key_string(key);
    }

    key = xmlGetProp(cur_node, (const xmlChar*)LAYOUT_ROW_KEY_MODIFIER_DECORATION_ID_ATTRIBUTE);
    if (key) {
        (*cur_rec_coordinate)->modifier_decorator = (sclchar*)key;
        add_key_string(key);
    }

    get_prop_number(cur_node, LAYOUT_ROW_KEY_KEY_SPACING_ATTRIBUTE, &key_spacing);

    xmlNodePtr child_node = cur_node->xmlChildrenNode;
    while (child_node != NULL) {
        if ( 0 == xmlStrcmp(child_node->name, (const xmlChar*)LAYOUT_ROW_KEY_LABEL_TAG)) {
            parsing_label_record_node(child_node, (*cur_rec_coordinate));
            (*cur_rec_coordinate)->valid = TRUE;
        }
        else if ( 0 == xmlStrcmp(child_node->name, (const xmlChar*)LAYOUT_ROW_KEY_IMAGE_LABEL_TAG)) {
            parsing_label_image_record_node(child_node, (*cur_rec_coordinate));
            (*cur_rec_coordinate)->valid = TRUE;
        }
        else if ( 0 == xmlStrcmp(child_node->name, (const xmlChar*)LAYOUT_ROW_KEY_BACKGROUND_IMAGE_TAG)) {
            parsing_background_image_record_node(child_node, (*cur_rec_coordinate));
            (*cur_rec_coordinate)->valid = TRUE;
        }
        else if ( 0 == xmlStrcmp(child_node->name, (const xmlChar*)LAYOUT_ROW_KEY_KEY_VALUE_TAG)) {
            parsing_key_value_record_node(child_node, (*cur_rec_coordinate));
            (*cur_rec_coordinate)->valid = TRUE;
        }
        else if ( 0 == xmlStrcmp(child_node->name, (const xmlChar*)LAYOUT_ROW_KEY_KEY_EVENT_TAG)) {
            parsing_key_event_record_node(child_node, (*cur_rec_coordinate));
            (*cur_rec_coordinate)->valid = TRUE;
        }
        else if ( 0 == xmlStrcmp(child_node->name, (const xmlChar*)LAYOUT_ROW_KEY_AUTOPOPUP_KEYS_TAG)) {
            parsing_auto_popup_keys_record_node(child_node, (*cur_rec_coordinate));
            (*cur_rec_coordinate)->valid = TRUE;
        }
        else if ( 0 == xmlStrcmp(child_node->name, (const xmlChar*)LAYOUT_ROW_KEY_POPUP_INPUTMODE_RECORD_TAG)) {
            parsing_popup_input_mode_record_node(child_node, (*cur_rec_coordinate));
            (*cur_rec_coordinate)->valid = TRUE;
        }
        else if ( 0 == xmlStrcmp(child_node->name, (const xmlChar*)LAYOUT_ROW_KEY_MAGNIFIER_LABEL_TAG)) {
            parsing_magnifier_label_record_node(child_node, (*cur_rec_coordinate));
            (*cur_rec_coordinate)->valid = TRUE;
        }
        else if ( 0 == xmlStrcmp(child_node->name, (const xmlChar*)LAYOUT_ROW_KEY_HINT_STRING_TAG)) {
            parsing_hint_string_record_node(child_node, (*cur_rec_coordinate));
            (*cur_rec_coordinate)->valid = TRUE;
        }

        child_node = child_node->next;
    }

    if (row) {
        row->row_x = (*cur_rec_coordinate)->x + (*cur_rec_coordinate)->width + key_spacing;
    }
}

void
LayoutParserImpl::parsing_popup_input_mode_record_node(
        const xmlNodePtr cur_node,
        const PSclLayoutKeyCoordinate cur_rec) {
    assert(cur_node != NULL);
    assert(cur_rec != NULL);
    assert(0 == xmlStrcmp(cur_node->name, (const xmlChar*)LAYOUT_ROW_KEY_POPUP_INPUTMODE_RECORD_TAG));
    xmlNodePtr child_node = cur_node->xmlChildrenNode;

    while (child_node != NULL) {
        if (0 == xmlStrcmp(child_node->name, (const xmlChar*)LAYOUT_ROW_KEY_POPUP_INPUTMODE_RECORD_INPUTMODE_TAG) ) {
            int drag_state = get_drag_state_prop(child_node);
            assert(drag_state >=0);
            assert(drag_state < SCL_DRAG_STATE_MAX);
            xmlChar* key = xmlNodeGetContent(child_node);

            if (key) {
                cur_rec->popup_input_mode[drag_state] = (sclchar*)key;
                add_key_string(key);
            }
        }
        child_node = child_node->next;
    }
}

LayoutParser::LayoutParser() {
    m_impl = new LayoutParserImpl;
}
LayoutParser::~LayoutParser() {
    SCLLOG(SclLog::MESSAGE, "~LayoutParser() has called");
    delete m_impl;
    m_impl = NULL;
}

LayoutParser*
LayoutParser::get_instance() {
    static LayoutParser instance;
    return &instance;
}

int
LayoutParser::init(const char* dir, const vector<string> &vec_file) {
    int ret = -1;
    if (dir) {
        m_impl->set_directory(dir);
        ret = m_impl->parsing_layout_table(vec_file);
    }

    return ret;
}

void
LayoutParser::load(int layout_id) {
    m_impl->load(layout_id);
}

bool
LayoutParser::loaded(int layout_id) {
    return m_impl->loaded(layout_id);
}

void
LayoutParser::unload() {
    m_impl->unload();
}

int
LayoutParser::get_layout_index(const char *name) {
    return m_impl->get_layout_index(name);
}

int
LayoutParser::get_layout_size() {
    return m_impl->get_layout_size();
}

PSclLayout
LayoutParser::get_layout_table() {
    return m_impl->get_layout_table();
}

PSclLayoutKeyCoordinatePointerTable
LayoutParser::get_key_coordinate_pointer_frame() {
    return m_impl->get_key_coordinate_pointer_frame();
}
