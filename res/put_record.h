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

#ifndef __PUT_RECORD_H__
#define __PUT_RECORD_H__
/*This file is just for debug*/
/*Use this file to output scl resources in txt format*/

// Open this flag to output the scl txt
// #define __SCL_TXT_DEBUG

/////////////////////////////////////////////////////
#ifdef __SCL_TXT_DEBUG

#include <stdio.h>
#include <string>
#include <unistd.h>
using namespace std;
#include "sclresource.h"

#define ENCODE 0
#define DECODE 1
#define PARSER 2
static const string get_folder() {
    return string(getenv("HOME")) + "/";
}

static inline void
put_str(FILE* fp, const char* str) {
    assert(fp);
    if (NULL != str) {
        fprintf(fp, "%s\n", str);
    } else {
        fprintf(fp, "String:NULL\n");
    }
}

static inline void
put_int(FILE* fp, int data) {
    assert(fp);
    fprintf(fp, "%d\n", data);
}
static inline void
put_flt(FILE* fp, double data) {
    assert(fp);
    fprintf(fp, "%lf\n", data);
}
static inline void
put_color(FILE* fp, const SclColor& c) {
    assert(fp);

    fprintf(fp, "Color:\n");
    put_int(fp, c.r);
    put_int(fp, c.g);
    put_int(fp, c.b);
    put_int(fp, c.a);
}

static inline void
put_point(FILE* fp, const SclPoint& p) {
    assert(fp);

    fprintf(fp, "Point:\n");
    put_int(fp, p.x);
    put_int(fp, p.y);
}
static inline void
put_rect(FILE* fp, const SclRect& r) {
    assert(fp);

    fprintf(fp, "Rect:\n");
    put_int(fp, r.left);
    put_int(fp, r.top);
    put_int(fp, r.right);
    put_int(fp, r.bottom);
}

static inline void
put_note(FILE*fp, int no) {
    fprintf(fp, "--------------------------");
    fprintf(fp, "%d", no);
    fprintf(fp, "--------------------------\n\n");
}

static inline void
put_note(FILE*fp, int i, int j) {
    fprintf(fp, "--------------------------");
    fprintf(fp, "i is %d, j is %d", i, j);
    fprintf(fp, "--------------------------\n\n");
}
static inline void
put_seperator(FILE *fp) {
    fprintf(fp, "\n##########\n\n");
}

static void
put_magnifier_wnd_configure_record(FILE* fp, const SclMagnifierWndConfigure& record) {
    assert(fp);

    put_int(fp, record.style);
    put_int(fp, record.width);
    put_int(fp, record.height);
    put_rect(fp, record.label_area_rect);
    put_str(fp, record.bg_image_path);
    put_seperator(fp);

    put_str(fp, record.bg_shift_image_path);
    put_str(fp, record.bg_shift_lock_image_path);
    put_str(fp, record.bg_long_key_image_path);
    put_int(fp, record.use_actual_window);
    put_str(fp, record.label_type);
    put_seperator(fp);

    put_int(fp, record.padding_x);
    put_int(fp, record.padding_y);
    put_int(fp, record.show_shift_label);
}

static void
put_autopopup_configure_record(FILE*fp, const SclAutoPopupConfigure& record) {
    assert(fp);

    put_str(fp, record.bg_image_path);
    put_color(fp, record.bg_color);
    put_flt(fp, record.bg_line_width);
    put_color(fp, record.bg_line_color);
    put_int(fp, record.bg_padding);
    put_seperator(fp);

    for (int i = 0; i < SCL_BUTTON_STATE_MAX; ++i) {
        put_str(fp, record.button_image_path[i]);
    }
    put_seperator(fp);

    put_int(fp, record.sw_button_style);
    put_int(fp, record.button_width);
    put_int(fp, record.button_height);
    put_int(fp, record.button_spacing);

    put_str(fp, record.label_type);
    put_seperator(fp);
    for (int i = 0; i < MAX_WND_DECORATOR; ++i) {
        put_str(fp, record.decoration_image_path[i]);
    }

    put_seperator(fp);
    put_int(fp, record.decoration_size);
    put_int(fp, record.max_column);
    put_int(fp, record.add_grab_left);
    put_int(fp, record.add_grab_right);
    put_int(fp, record.add_grab_top);
    put_seperator(fp);
    put_int(fp, record.add_grab_bottom);
}

static void
put_nine_patch_info_record(FILE* fp, const SclNinePatchInfo& record) {
    assert(fp);

    put_str(fp, record.image_path);
    put_int(fp, record.left);
    put_int(fp, record.right);
    put_int(fp, record.top);
    put_int(fp, record.bottom);
}

static void
put_modifier_decoration_record(FILE* fp, const SclModifierDecoration& record) {
    assert(fp);

    put_int(fp, record.extract_background);
    put_str(fp, record.name);
    put_seperator(fp);

    for (int i = 0; i < DISPLAYMODE_MAX; ++i) {
        for (int j = 0; j < KEY_MODIFIER_MAX; ++j) {
            put_str(fp, record.bg_image_path[i][j]);
        }
    }
}

static void
put_default_configure_record(FILE* fp, const SclDefaultConfigure& record) {
    put_int(fp, record.display_mode);
    put_str(fp, record.input_mode);
    put_str(fp, record.image_file_base_path);
    put_int(fp, record.target_screen_width);
    put_int(fp, record.target_screen_height);
    put_seperator(fp);

    put_int(fp, record.auto_detect_landscape);
    put_int(fp, record.use_magnifier_window);
    put_int(fp, record.use_auto_popup);
    put_int(fp, record.use_zoom_window);
    put_int(fp, record.on_error_noti_send);
    put_seperator(fp);
    put_int(fp, record.use_word_deletion);
    put_int(fp, record.sw_button_style);
    put_seperator(fp);
    for (int i = 0; i < DISPLAYMODE_MAX; ++i) {
        put_int(fp, record.touch_offset_level[i]);
    }

    put_seperator(fp);
    for (int i = 0; i < DISPLAYMODE_MAX; ++i) {
        put_point(fp, record.touch_offset[i]);
    }

    put_seperator(fp);
    put_str(fp, record.default_sub_layout);
    put_int(fp, record.use_actual_dim_window);
    put_color(fp, record.dim_color);
}

static void put_label_properties_record(FILE* fp, const SclLabelProperties& record) {
    put_str(fp, record.label_type);
    put_str(fp, record.font_name);
    put_int(fp, record.font_size);
    put_seperator(fp);
    for (int i = 0; i < SCL_SHIFT_STATE_MAX; ++i) {
        for (int j = 0; j < SCL_BUTTON_STATE_MAX; ++j) {
            put_color(fp, record.font_color[i][j]);
        }
    }
    put_seperator(fp);
    put_int(fp, record.alignment);
    put_int(fp, record.padding_x);
    put_int(fp, record.padding_y);
    put_int(fp, record.inner_width);
    put_int(fp, record.inner_height);
    put_int(fp, record.shadow_distance);
    put_int(fp, record.shadow_direction);
    put_seperator(fp);

    for (int i = 0; i < SCL_SHIFT_STATE_MAX; ++i) {
        for (int j = 0; j < SCL_BUTTON_STATE_MAX; ++j) {
            put_color(fp, record.shadow_color[i][j]);
        }
    }
}

static void put_key_coordinate_record(FILE* fp, const SclLayoutKeyCoordinate& record) {
    put_int(fp, record.x);
    put_int(fp, record.y);
    put_int(fp, record.width);
    put_int(fp, record.height);
    put_seperator(fp);
    put_int(fp, record.add_hit_left);
    put_int(fp, record.add_hit_right);
    put_int(fp, record.add_hit_top);
    put_int(fp, record.add_hit_bottom);
    put_seperator(fp);

    put_int(fp, record.popup_relative_x);
    put_int(fp, record.popup_relative_y);
    put_int(fp, record.extract_offset_x);
    put_int(fp, record.extract_offset_y);
    put_seperator(fp);
    put_str(fp, record.sub_layout);
    put_seperator(fp);

    put_int(fp, record.magnifier_offset_x);
    put_int(fp, record.magnifier_offset_y);
    put_seperator(fp);
    put_str(fp, record.custom_id);
    put_int(fp, record.button_type);
    put_int(fp, record.key_type);
    put_int(fp, record.popup_type);
    put_seperator(fp);

    put_int(fp, record.use_magnifier);
    put_int(fp, record.use_long_key_magnifier);

    put_seperator(fp);
    for (int i = 0; i < SCL_DRAG_STATE_MAX; ++i) {
        put_str(fp, record.popup_input_mode[i]);
    }
    put_seperator(fp);
    put_str(fp, record.sound_style);
    put_str(fp, record.vibe_style);
    put_int(fp, record.is_side_button);
    put_int(fp, record.label_count);
    put_seperator(fp);
    for (int i = 0; i < SCL_SHIFT_STATE_MAX; ++i) {
        for (int j = 0; j < MAX_SIZE_OF_LABEL_FOR_ONE; ++j) {
            put_str(fp, record.label[i][j]);
        }
    }
    put_seperator(fp);
    put_str(fp, record.label_type);
    put_seperator(fp);
    for (int i = 0; i < SCL_SHIFT_STATE_MAX; ++i) {
        for (int j = 0; j < SCL_BUTTON_STATE_MAX; ++j) {
            put_str(fp, record.image_label_path[i][j]);
        }
    }
    put_str(fp, record.image_label_type);
    put_seperator(fp);
    for (int i = 0; i < SCL_SHIFT_STATE_MAX; ++i) {
        for (int j = 0; j < SCL_BUTTON_STATE_MAX; ++j) {
            put_str(fp, record.bg_image_path[i][j]);
        }
    }
    put_seperator(fp);
    put_int(fp, record.key_value_count);
    put_seperator(fp);
    for (int i = 0; i < SCL_SHIFT_STATE_MAX; ++i) {
        for (int j = 0; j < MAX_SIZE_OF_MULTITAP_CHAR; ++j) {
            put_str(fp, record.key_value[i][j]);
        }
    }
    put_seperator(fp);
    for (int i = 0; i < SCL_SHIFT_STATE_MAX; ++i) {
        for (int j = 0; j < MAX_SIZE_OF_LABEL_FOR_ONE; ++j) {
            put_int(fp, record.key_event[i][j]);

        }
    }
    put_seperator(fp);
    put_str(fp, record.long_key_value);
    put_int(fp, record.use_repeat_key);
    put_seperator(fp);
    for (int i = 0; i < SCL_SHIFT_STATE_MAX; ++i) {
        for (int j = 0; j < MAX_SIZE_OF_AUTOPOPUP_STRING; ++j) {
            put_str(fp, record.autopopup_key_labels[i][j]);
            put_str(fp, record.autopopup_key_values[i][j]);
            put_int(fp, record.autopopup_key_events[i][j]);
        }
    }
    put_seperator(fp);
    put_int(fp, record.dont_close_popup);
    put_int(fp, record.extra_option);
    put_int(fp, record.multitouch_type);
    put_seperator(fp);
    for (int i = 0; i < SCL_SHIFT_STATE_MAX; ++i) {
        for (int j = 0; j < MAX_SIZE_OF_LABEL_FOR_ONE; ++j) {
            put_str(fp, record.magnifier_label[i][j]);
        }
    }
    put_seperator(fp);
    for (int i = 0; i < SCL_SHIFT_STATE_MAX; ++i) {
        for (int j = 0; j < MAX_SIZE_OF_MULTITAP_CHAR; ++j) {
            put_str(fp, record.hint_string[i][j]);
        }
    }
}
static void put_input_mode_configure_record(FILE* fp, const SclInputModeConfigure& record) {
    put_str(fp, record.name);
    put_str(fp, record.layouts[0]);
    put_str(fp, record.layouts[1]);
    put_int(fp, record.use_virtual_window);
    put_int(fp, record.use_dim_window);
    put_flt(fp, record.timeout);
}
static void put_layout_record(FILE* fp, const SclLayout& record ) {
    put_str(fp, record.name);
    put_int(fp, record.display_mode);
    put_int(fp, record.style);
    put_int(fp, record.width);
    put_int(fp, record.height);
    put_seperator(fp);
    put_int(fp, record.use_sw_button);
    put_int(fp, record.extract_background);
    put_int(fp, record.use_magnifier_window);
    put_int(fp, record.key_width);
    put_int(fp, record.key_height);
    put_seperator(fp);
    put_int(fp, record.use_sw_background);
    put_flt(fp, record.bg_line_width);
    put_int(fp, record.add_grab_left);
    put_int(fp, record.add_grab_right);
    put_int(fp, record.add_grab_top);
    put_int(fp, record.add_grab_bottom);
    put_seperator(fp);
    for(int i = 0; i < SCL_BUTTON_STATE_MAX; ++i) {
        put_str(fp, record.image_path[i]);
    }
    put_seperator(fp);
    //key_background_image
    for (int i = 0; i < SCL_SHIFT_STATE_MAX; ++i) {
        for (int j = 0; j < SCL_BUTTON_STATE_MAX; ++j) {
            put_str(fp, record.key_background_image[i][j]);
        }
    }
}

static void put_input_mode_configure_table(int flag, const PSclInputModeConfigure table) {
    string file_name;
    if (flag == ENCODE)
        file_name = "encode_input_mode_configure.txt";
    else if (flag == DECODE)
        file_name = "decode_input_mode_configure.txt";
    else
        file_name = "sclres_input_mode_configure.txt";

    string path = get_folder() + file_name;
    FILE *fp = fopen(path.c_str(), "w+");
    if (!fp) {
        perror("Warning: path.c_str()");
        return;
    }
    for (int i = 0; i < MAX_SCL_INPUT_MODE; ++i) {
        put_note(fp, i);
        put_input_mode_configure_record(fp, table[i]);
    }

    fclose(fp);
}

static void put_layout_table(int flag, const PSclLayout table) {
    string file_name;
    if (flag == ENCODE)
        file_name = "encode_layout.txt";
    else if (flag == DECODE)
        file_name = "decode_layout.txt";
    else
        file_name = "sclres_layout.txt";

    string path = get_folder() + file_name;
    FILE *fp = fopen(path.c_str(), "w+");
    if (!fp) {
        perror("Warning: path.c_str()");
        return;
    }
    for (int i = 0; i < MAX_SCL_LAYOUT; ++i) {
        put_note(fp, i);
        put_layout_record(fp, table[i]);
    }
    fclose(fp);
}

static void
put_key_coordinate_frame(int flag, const PSclLayoutKeyCoordinatePointerTable frame) {
    string file_name;
    if (flag == ENCODE) {
        file_name = "encode_key_coordinate_frame.txt";
    } else if (flag == DECODE){
        file_name = "decode_key_coordinate_frame.txt";
    } else
        file_name = "sclres_key_coordinate_frame.txt";

    string path = get_folder() + file_name;
    FILE *fp = fopen(path.c_str(), "w+");
    if (!fp) {
        perror("Warning: path.c_str()");
        return;
    }
    for (int i = 0; i < MAX_SCL_LAYOUT; ++i) {
        if (frame[i][0] == NULL) break;

        for (int j = 0; j < MAX_KEY; ++j) {
            put_note(fp, i, j);
            const SclLayoutKeyCoordinatePointer p = frame[i][j];
            if (!p) break;
            const SclLayoutKeyCoordinate& cur = *p;
            put_key_coordinate_record(fp, cur);
        }
    }
    fclose(fp);
}

static void
put_label_properties_frame(int flag, const PSclLabelPropertiesTable frame) {
    string file_name;
    if (flag == ENCODE) {
        file_name = "encode_label_properties_frame.txt";
    } else if (flag == DECODE){
        file_name = "decode_label_properties_frame.txt";
    } else
        file_name = "sclres_label_properties_frame.txt";

    string path = get_folder() + file_name;
    FILE *fp = fopen(path.c_str(), "w+");
    if (!fp) {
        perror("Warning: path.c_str()");
        return;
    }
    for (int i = 0; i < MAX_SCL_LABEL_PROPERTIES; ++i) {
        for(int j = 0; j < MAX_SIZE_OF_LABEL_FOR_ONE; ++j) {
            put_note(fp, i, j);
            put_label_properties_record(fp, frame[i][j]);
        }
    }
    fclose(fp);
}

static void
put_default_configure(int flag, const SclDefaultConfigure& record) {

    string file_name;
    if (flag == ENCODE) {
        file_name = "encode_default_configure.txt";
    } else if (flag == DECODE) {
        file_name = "decode_default_configure.txt";
    } else
        file_name = "sclres_default_configure.txt";

    string path = get_folder() + file_name;
    FILE *fp = fopen(path.c_str(), "w+");
    if (!fp) {
        perror("Warning: path.c_str()");
        return;
    }
    put_default_configure_record(fp, record);
    fclose(fp);
}

static void
put_autopopup_configure(int flag, const SclAutoPopupConfigure& record) {

    string file_name;
    if (flag == ENCODE) {
        file_name = "encode_autopopup_configure.txt";
    } else if (flag == DECODE){
        file_name = "decode_autopopup_configure.txt";
    } else
        file_name = "sclres_autopopup_configure.txt";

    string path = get_folder() + file_name;
    FILE *fp = fopen(path.c_str(), "w+");
    if (!fp) {
        perror("Warning: path.c_str()");
        return;
    }
    put_autopopup_configure_record(fp, record);
    fclose(fp);
}

static void
put_nine_patch_info(int flag, const SclNinePatchInfo* table) {
    static const int MAX_NINE_PATCH_FILE_LIST = 128;

    string file_name;
    if (flag == ENCODE) {
        file_name = "encode_nine_patch_info.txt";
    } else if (flag == DECODE) {
        file_name = "decode_nine_patch_info.txt";
    } else
        file_name = "sclres_nine_patch_info.txt";

    string path = get_folder() + file_name;
    FILE *fp = fopen(path.c_str(), "w+");
    if (!fp) {
        perror("Warning: path.c_str()");
        return;
    }

    for (int i = 0; i < MAX_NINE_PATCH_FILE_LIST; ++i) {
        put_note(fp, i);
        put_nine_patch_info_record(fp, table[i]);
    }
    fclose(fp);
}


static void
put_magnifier_wnd_configure(int flag, const SclMagnifierWndConfigure& record) {
    string file_name;
    if (flag == ENCODE) {
        file_name = "encode_magnifier_wnd_configure.txt";
    } else if (flag == DECODE) {
        file_name = "decode_magnifier_wnd_configure.txt";
    } else
        file_name = "sclres_magnifier_wnd_configure.txt";

    string path = get_folder() + file_name;
    FILE *fp = fopen(path.c_str(), "w+");
    if (!fp) {
        perror("Warning: path.c_str()");
        return;
    }
    put_magnifier_wnd_configure_record(fp, record);
    fclose(fp);
}

static void
put_modifier_decoration(int flag, const PSclModifierDecoration table) {
    string file_name;
    if (flag == ENCODE) {
        file_name = "encode_modifier_decoration.txt";
    } else if (flag == DECODE) {
        file_name = "decode_modifier_decoration.txt";
    } else
        file_name = "sclres_modifier_decoration.txt";

    string path = get_folder() + file_name;
    FILE *fp = fopen(path.c_str(), "w+");
    if (!fp) {
        perror("Warning: path.c_str()");
        return;
    }

    for (int i = 0; i < MAX_SCL_MODIFIER_DECORATION_NUM; ++i) {
        put_note(fp, i);
        put_modifier_decoration_record(fp, table[i]);
    }
    fclose(fp);
}
#endif
#endif
