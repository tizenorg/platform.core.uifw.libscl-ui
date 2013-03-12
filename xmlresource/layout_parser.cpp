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

#include "xmlresource.h"
#include "layout_parser.h"
#include "main_entry_parser.h"
#include "xml_parser_utils.h"
#include "input_mode_configure_parser.h"
#include "layout_parser_helper.h"
#include "put_record.h"
#include <algorithm>
#include <malloc.h>
using namespace std;
using namespace xmlresource;

Layout_Parser* Layout_Parser::m_instance = NULL;

Layout_Parser::Layout_Parser() {
    m_layout_size = 0;
    memset(m_layout_files, 0x00, sizeof(char*) * MAX_SCL_LAYOUT);
    memset(m_layout_table, 0x00, sizeof(SclLayout) * MAX_SCL_LAYOUT);
    memset(m_key_coordinate_pointer_frame, 0x00, sizeof(SclLayoutKeyCoordinatePointer) * MAX_SCL_LAYOUT * MAX_KEY);
}

Layout_Parser::~Layout_Parser() {
    for (int i = 0; i < MAX_SCL_LAYOUT; ++i) {
        for (int j = 0; j < MAX_KEY; ++j) {
            free(m_key_coordinate_pointer_frame[i][j]);
            m_key_coordinate_pointer_frame[i][j] = NULL;
        }
    }

    release_layout_strings();
    release_key_strings();
}
Layout_Parser* Layout_Parser::get_instance() {
    if (m_instance == NULL) {
        m_instance = new Layout_Parser();
    }
    return m_instance;
}
void Layout_Parser::init() {
    parsing_layout_table();
}

void Layout_Parser::load(int layout_id) {
    /* Check if the lazy_loading feature is enabled, and if so, load key resources now */
    sclboolean use_lazy_loading = FALSE;
    Default_Configure_Parser *defalut_configure_parser = Default_Configure_Parser::get_instance();
    if (defalut_configure_parser) {
        PSclDefaultConfigure default_configure = defalut_configure_parser->get_default_configure();
        if (default_configure) {
            use_lazy_loading = default_configure->use_lazy_loading;
        }
    }
    if (use_lazy_loading) {
        if (layout_id >= 0 && layout_id < MAX_SCL_LAYOUT) {
            xmlDocPtr doc;
            xmlNodePtr cur_node;

            char input_file[_POSIX_PATH_MAX] = {0};
            XMLResource *xml_resource = XMLResource::get_instance();
            snprintf(input_file, _POSIX_PATH_MAX, "%s/%s",
                xml_resource->get_resource_directory(), m_layout_files[layout_id]);

            doc = xmlReadFile(input_file, NULL, 0);
            if (doc == NULL) {
                printf("Could not load file.\n");
                exit(1);
            }

            cur_node = xmlDocGetRootElement(doc);
            if (cur_node == NULL) {
                printf("empty document.\n");
                xmlFreeDoc(doc);
                exit(1);
            }
            if (0 != xmlStrcmp(cur_node->name, (const xmlChar*)LAYOUT_TAG))
            {
                printf("root name %s error!\n", cur_node->name);
                xmlFreeDoc(doc);
                exit(1);
            }

            PSclLayout cur_rec_layout = m_layout_table + layout_id;
            parsing_layout_node_lazy(cur_node, cur_rec_layout, layout_id);

            xmlFreeDoc(doc);
        }
    }
}
void Layout_Parser::unload() {
    for (int i = 0; i < MAX_SCL_LAYOUT; ++i) {
        for (int j = 0; j < MAX_KEY; ++j) {
            free(m_key_coordinate_pointer_frame[i][j]);
            m_key_coordinate_pointer_frame[i][j] = NULL;
        }
    }

    release_key_strings();
}
bool Layout_Parser::loaded(int layout_id) {
    bool ret = TRUE;

    sclboolean use_lazy_loading = FALSE;
    Default_Configure_Parser *defalut_configure_parser = Default_Configure_Parser::get_instance();
    if (defalut_configure_parser) {
        PSclDefaultConfigure default_configure = defalut_configure_parser->get_default_configure();
        if (default_configure) {
            use_lazy_loading = default_configure->use_lazy_loading;
        }
    }

    if (use_lazy_loading) {
        if (layout_id >= 0 && layout_id < MAX_SCL_LAYOUT) {
            if (m_key_coordinate_pointer_frame[layout_id][0] == NULL) {
                ret = FALSE;
            }
        }
    }

    return ret;
}

bool xmlStringCompare (xmlChar* i, xmlChar* j) { return (xmlStrcmp(i, j) < 0); }

void Layout_Parser::add_layout_string(xmlChar* newstr) {
    if (newstr) {
        m_vec_layout_strings.push_back(newstr);
    }
}

void Layout_Parser::release_layout_strings() {
    for(int loop = 0;loop < m_vec_layout_strings.size();loop++) {
        if (m_vec_layout_strings[loop]) {
            xmlFree(m_vec_layout_strings[loop]);
        }
    }
    m_vec_layout_strings.clear();
}

void Layout_Parser::add_key_string(xmlChar* newstr) {
    if (newstr) {
        m_vec_key_strings.push_back(newstr);
    }
}

void Layout_Parser::release_key_strings() {
    for(int loop = 0;loop < m_vec_key_strings.size();loop++) {
        if (m_vec_key_strings[loop]) {
            xmlFree(m_vec_key_strings[loop]);
        }
    }
    m_vec_key_strings.clear();
}

int Layout_Parser::get_layout_index(const char *name) {
    int ret = NOT_USED;
    if (name) {
        for(int loop = 0;loop < MAX_SCL_LAYOUT && ret == NOT_USED;loop++) {
            if (m_layout_table[loop].name) {
                if (strcmp(m_layout_table[loop].name, name) == 0) {
                    ret = loop;
                    break;
                }
            }
        }
    }
    return ret;
}

PSclLayout Layout_Parser::get_layout_table() {
    return m_layout_table;
}
int Layout_Parser::get_layout_size() {
    return m_layout_size;
}
PSclLayoutKeyCoordinatePointerTable Layout_Parser::get_key_coordinate_pointer_frame() {
    return m_key_coordinate_pointer_frame;
}

void Layout_Parser::parsing_layout_table() {
    int index = 0;

    m_layout_size = 0;

    Input_Mode_Configure_Parser *input_mode_configure_parser = Input_Mode_Configure_Parser::get_instance();
    for(int inputmode = 0;inputmode < MAX_SCL_INPUT_MODE;inputmode++) {
        int empty_index;
        const sclchar* layout_filepath;
        PSclInputModeConfigure configure_table = input_mode_configure_parser->get_input_mode_configure_table();
        SclInputModeConfigure& cur_rec = configure_table[inputmode];

        for(int display = 0;display < DISPLAYMODE_MAX;display++) {
            empty_index = NOT_USED;
            layout_filepath = cur_rec.layouts[display];
            if (layout_filepath) {
                for(int checkidx = 0;checkidx < MAX_SCL_LAYOUT && empty_index == NOT_USED;checkidx++) {
                    if (m_layout_files[checkidx] == NULL) empty_index = checkidx;
                    else if (strncmp(m_layout_files[checkidx], layout_filepath, _POSIX_PATH_MAX) == 0) break;
                }
                if (empty_index != NOT_USED) {
                    m_layout_size++;
                    m_layout_files[empty_index] = layout_filepath;

                    xmlDocPtr doc;
                    xmlNodePtr cur_node;

                    char input_file[_POSIX_PATH_MAX] = {0};
                    XMLResource *xml_resource = XMLResource::get_instance();
                    snprintf(input_file, _POSIX_PATH_MAX, "%s/%s",
                        xml_resource->get_resource_directory(), layout_filepath);

                    doc = xmlReadFile(input_file, NULL, 0);
                    if (doc == NULL) {
                        printf("Could not load file.\n");
                        exit(1);
                    }

                    cur_node = xmlDocGetRootElement(doc);
                    if (cur_node == NULL) {
                        printf("empty document.\n");
                        xmlFreeDoc(doc);
                        exit(1);
                    }
                    if (0 != xmlStrcmp(cur_node->name, (const xmlChar*)LAYOUT_TAG))
                    {
                       printf("root name %s error!\n", cur_node->name);
                       xmlFreeDoc(doc);
                       exit(1);
                    }

                    PSclLayout cur_rec_layout = m_layout_table + empty_index;
                    parsing_layout_node(cur_node, cur_rec_layout, empty_index);
                    cur_rec_layout->name = (sclchar*)layout_filepath;

                    xmlFreeDoc(doc);
                }
            }
        }
    }
}

void
Layout_Parser::parsing_background(
        const xmlNodePtr cur_node,
        PSclLayout cur_layout) {
    assert(cur_node != NULL);
    assert(cur_layout != NULL);

    xmlNodePtr child_node = cur_node->xmlChildrenNode;
    while ( child_node != NULL) {
        if ( 0 == xmlStrcmp(child_node->name, (const xmlChar* )"button_normal") ) {
            xmlChar *key = xmlNodeGetContent(child_node);
            cur_layout->image_path[BUTTON_STATE_NORMAL] = (char *)key;
            add_layout_string(key);
        }
        else if ( 0 == xmlStrcmp(child_node->name, (const xmlChar* )"button_pressed") ) {
            xmlChar *key = xmlNodeGetContent(child_node);
            cur_layout->image_path[BUTTON_STATE_PRESSED] = (char *)key;
            add_layout_string(key);
        }
        else if ( 0 == xmlStrcmp(child_node->name, (const xmlChar* )"button_disabled") ) {
            xmlChar *key = xmlNodeGetContent(child_node);
            cur_layout->image_path[BUTTON_STATE_DISABLED] = (char *)key;
            add_layout_string(key);
        }

        child_node = child_node->next;
    }
}

void Layout_Parser::parsing_key_background(
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


void Layout_Parser::set_default_layout_value(const PSclLayout cur_layout) {
    XMLResource *xml_resource = XMLResource::get_instance();
    const PSclDefaultConfigure sclres_default_configure = xml_resource->get_default_configure();

    assert(cur_layout != NULL && sclres_default_configure != NULL);
    //default value
    cur_layout->valid = 1;
    cur_layout->style = LAYOUT_STYLE_BASE;
    cur_layout->name = NULL;

    cur_layout->width = sclres_default_configure->target_screen_width;
    cur_layout->height = sclres_default_configure->target_screen_height;

    cur_layout->key_width = 0;
    cur_layout->key_height = 0;
    cur_layout->key_spacing = 0;
    cur_layout->row_spacing = 0;

    cur_layout->image_path[BUTTON_STATE_NORMAL]      = NULL;
    cur_layout->image_path[BUTTON_STATE_PRESSED]     = NULL;
    cur_layout->image_path[BUTTON_STATE_DISABLED]    = NULL;

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

void Layout_Parser::set_default_row_value(
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

void Layout_Parser::set_default_key_coordinate_value(
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
        cur_rec_coordinate->sub_layout = row->sub_layout;
    }
}

void Layout_Parser::parsing_grab_area(
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

void Layout_Parser::parsing_layout_node(
        const xmlNodePtr cur_node,
        const PSclLayout cur_rec_layout,
        int layout_no) {
    assert(cur_node != NULL);
    assert(cur_rec_layout != NULL);

    set_default_layout_value(cur_rec_layout);

    int row_y = 0;
    xmlChar* key;

    SclLayoutKeyCoordinatePointer *cur_key = &m_key_coordinate_pointer_frame[layout_no][0];

    /*key = xmlGetProp(cur_node, (const xmlChar*)LAYOUT_NAME_ATTRIBUTE);
    if (key) {
        cur_rec_layout->name = (sclchar*)key;
    }*/

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
        if ( 0 == xmlStrcmp(child_node->name, (const xmlChar*)LAYOUT_ROW_TAG)) {
            /* Check if the lazy_loading feature is enabled, and if so, do not load any layout resources now */
            sclboolean use_lazy_loading = FALSE;
            Default_Configure_Parser *defalut_configure_parser = Default_Configure_Parser::get_instance();
            if (defalut_configure_parser) {
                PSclDefaultConfigure default_configure = defalut_configure_parser->get_default_configure();
                if (default_configure) {
                    use_lazy_loading = default_configure->use_lazy_loading;
                }
            }
            if (!use_lazy_loading) {
                parsing_layout_row_node(child_node, cur_rec_layout, &row_y, &cur_key);
            }
        }
        else if ( 0 == xmlStrcmp(child_node->name, (const xmlChar*)LAYOUT_BACKGROUND_TAG)) {
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

void Layout_Parser::parsing_layout_node_lazy(
        const xmlNodePtr cur_node,
        const PSclLayout cur_rec_layout,
        int layout_no) {
    assert(cur_node != NULL);

    int row_y = 0;
    xmlChar* key;

    SclLayoutKeyCoordinatePointer *cur_key = &m_key_coordinate_pointer_frame[layout_no][0];

    /* Do a lazy loading, only when the key_coordinate_pointer_frame does not have any keys */
    if (*cur_key == NULL) {
        xmlNodePtr child_node = cur_node->xmlChildrenNode;
        while (child_node != NULL) {
            if ( 0 == xmlStrcmp(child_node->name, (const xmlChar*)LAYOUT_ROW_TAG)) {
                /* Check if the lazy_loading feature is enabled, and if so, do not load any layout resources now */
                sclboolean use_lazy_loading = FALSE;
                Default_Configure_Parser *defalut_configure_parser = Default_Configure_Parser::get_instance();
                if (defalut_configure_parser) {
                    PSclDefaultConfigure default_configure = defalut_configure_parser->get_default_configure();
                    if (default_configure) {
                        use_lazy_loading = default_configure->use_lazy_loading;
                    }
                }
                if (use_lazy_loading) {
                    parsing_layout_row_node(child_node, cur_rec_layout, &row_y, &cur_key);
                }
            }

            child_node = child_node->next;
        }
    }
}

void
Layout_Parser::parsing_layout_row_node(
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
Layout_Parser::get_drag_state_prop(const xmlNodePtr cur_node) {
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

    for(int i = 0; i < sizeof(table)/sizeof(Match_Struct); ++i) {
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
Layout_Parser::get_shift_state_prop(const xmlNodePtr cur_node) {
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
Layout_Parser::get_button_state_prop(const xmlNodePtr cur_node) {
    assert(cur_node != NULL);
    int button_state = -1;

    if (equal_prop(cur_node, "button", "pressed")) {
        button_state = BUTTON_STATE_PRESSED;
    } else if (equal_prop(cur_node, "button", "normal")) {
        button_state = BUTTON_STATE_NORMAL;
    }
    else if (equal_prop(cur_node, "button", "disabled")) {
        button_state = BUTTON_STATE_DISABLED;
    }
    return button_state;
}

int
Layout_Parser::get_multitouch_type_prop(const xmlNodePtr cur_node) {
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

    for(int i = 0; i < sizeof(table)/sizeof(Match_Struct); ++i) {
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
Layout_Parser::get_extra_option_prop(
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

    for(int i = 0; i < sizeof(table)/sizeof(Match_Struct); ++i) {
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
Layout_Parser::parsing_label_record_node(
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
Layout_Parser::parsing_magnifier_label_record_node(
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
Layout_Parser::parsing_label_image_record_node(
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
Layout_Parser::parsing_background_image_record_node(
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
Layout_Parser::parsing_key_value_record_node(
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
Layout_Parser::parsing_key_event_record_node(
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
Layout_Parser::parsing_auto_popup_keys_record_node(
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

void Layout_Parser::parsing_key_coordinate_record_node(
        const xmlNodePtr cur_node, Row *row,
        SclLayoutKeyCoordinatePointer *cur_rec_coordinate) {
    assert(cur_node != NULL);
    assert(cur_rec_coordinate != NULL);

    *cur_rec_coordinate = (SclLayoutKeyCoordinatePointer)malloc(sizeof(SclLayoutKeyCoordinate));
    if (*cur_rec_coordinate == NULL) {
        printf("memory malloc eror.\n");
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

        child_node = child_node->next;
    }

    if (row) {
        row->row_x = (*cur_rec_coordinate)->x + (*cur_rec_coordinate)->width + key_spacing;
    }
}

void
Layout_Parser::parsing_popup_input_mode_record_node(
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
