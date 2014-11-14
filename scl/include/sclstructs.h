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

#include <string>
#include "scltypes.h"
#include "sclconfig.h"

#ifndef __SCL_STRUCTS_H__
#define __SCL_STRUCTS_H__

//SCL_BEGIN_DECLS

/**
  *@brief SCL input mode configuration structure, which indicates a type of SW keyboard
  */
typedef struct _SclInputModeConfigure {
    sclchar* name;						/* Name of this inputmode */
    sclchar* layouts[DISPLAYMODE_MAX];	/* This contains the filename of each layouts */
    sclboolean use_virtual_window;		/* Whether to use a new window or make a popup window as a layer of existing base window */
    sclboolean use_dim_window;			/* Whether to use a dim window */
    sclint timeout;						/* fTimeout seconds of idle state will close this popup window. Set 0 to disable */
}SclInputModeConfigure;

/**
  *@brief SCL Layout structure, which is a physical keyboard representation defined by a group of SclLayoutKeyCoordinates
  */
typedef struct _SclLayout {
    sclboolean valid;					/* Indicates whether this information is in use or not */

    sclchar* name;						/* Name of this layout */
    SCLDisplayMode display_mode;		/* Indicates whether this layout is for portrait mode or landscape display */
    SCLLayoutStyle style;				/* Desribes the style of this layout */

    sclshort width;						/* Width of this layout */
    sclshort height;					/* Height of this layout*/

    sclboolean use_sw_button;			/* Whether to draw buttons by SW or not */
    sclboolean use_magnifier_window;	/* Whether to use magnifier window or not */
    sclboolean extract_background;		/* Whether to display only some part of background image */

    sclshort key_width;					/* Default width of key */
    sclshort key_height;				/* Default height of keys */
    sclshort key_spacing;				/* Default spacing amount between keys */
    sclshort row_spacing;				/* Default spacing amount between rows */

    sclshort add_hit_left;				/* Left value for expanded hit area */
    sclshort add_hit_right;				/* Right value for expanded hit area */
    sclshort add_hit_top;				/* Left value for expanded hit area */
    sclshort add_hit_bottom;			/* Left value for expanded hit area */

    sclboolean use_sw_background;		/* Whether to draw background by SW or not */
    SclColor bg_color;					/* If drawn by SW, describes the background color */
    scldouble bg_line_width;			/* If drawn by SW, describes the background border width */
    SclColor bg_line_color;				/* If drawn by SW, describes the background border color */

    sclshort add_grab_left;				/* If this layout's style is POPUP_GRAB, desribes the left pixel for grab area */
    sclshort add_grab_right;			/* If this layout's style is POPUP_GRAB, desribes the right pixel for grab area */
    sclshort add_grab_top;				/* If this layout's style is POPUP_GRAB, desribes the top pixel for grab area */
    sclshort add_grab_bottom;			/* If this layout's style is POPUP_GRAB, desribes the bottom pixel for grab area */

    sclshort mouse_manipulate_x;		/* The X-point that this layout needs to manipulate the position of mouse event */
    sclshort mouse_manipulate_y;		/* The Y-point that this layout needs to manipulate the position of mouse event */

    /* Background image of each button state */
    sclchar* image_path[SCL_BUTTON_STATE_MAX];

    /* Default background images for keys */
    sclchar* key_background_image[SCL_SHIFT_STATE_MAX][SCL_BUTTON_STATE_MAX];

    sclchar* sound_style;				/* A default sound feedback style when buttons are clicked */
    sclchar* vibe_style;				/* A default vibration feedback style when this buttons are clicked */
    sclchar* label_type;				/* Default label property type for specifying the appearance of label strings */

    /* If the button should be displayed differently depending on the modifier, describe a decorator and provide its ID here */
    sclchar* modifier_decorator;
}SclLayout;

/**
  *@brief SCL Layout key coordinate structure, which represents a key's position-related information
  */
typedef struct _SclLayoutKeyCoordinate {
    sclboolean valid;					/* Indicates whether this information is in use or not */
    sclshort x;							/* X coordinate of this button */
    sclshort y;							/* Y coordinate of this button */
    sclshort width;						/* Width of this button */
    sclshort height;					/* Height of this button */
    sclshort add_hit_left;				/* Left value for expanded hit area */
    sclshort add_hit_right;				/* Right value for expanded hit area */
    sclshort add_hit_top;				/* Left value for expanded hit area */
    sclshort add_hit_bottom;			/* Left value for expanded hit area */
    sclshort popup_relative_x;			/* Relative X offset for popup window */
    sclshort popup_relative_y;			/* Relative Y offset for popup window */
    sclshort extract_offset_x;			/* If the popup layout has ExtractBG, this value provides the X offset inside the BG image */
    sclshort extract_offset_y;			/* If the popup layout has ExtractBG, this value provides the Y offset inside the BG image */
    sclchar* sub_layout;				/* Indicates which sublayout this button belongs to */
    sclshort magnifier_offset_x;		/* Relative X offset for magnifier window */
    sclshort magnifier_offset_y;		/* Relative Y offset for magnifier window */

    sclchar* custom_id;					/* A value for indentifying a button when handling private keys or disable key. */
    SCLButtonType button_type;			/* Indicates the behavioral type of this button */
    SCLKeyType key_type;				/* Delivered key type when the button event is emitted */
    SCLPopupType popup_type;			/* Popup type when this button is activated */
    sclboolean use_magnifier;			/* Indicates whether this button needs to display magnifier window when clicked */
    sclboolean use_long_key_magnifier;	/* Indicates whether this button needs to display magnifier window when long-pressed */
    /* Popup inputmode IDs depending on the direction of the drag action */
    sclchar* popup_input_mode[SCL_DRAG_STATE_MAX];
    sclchar* sound_style;				/* A sound feedback style when this button is clicked */
    sclchar* vibe_style;				/* A vibration feedback style when this button is clicked */
    sclboolean is_side_button;			/* Indicates whether this button is located on the very left or right of the layout */

    sclbyte label_count;				/* Number of labels for this button (Max value :MAX_SIZE_OF_LABEL_FOR_ONE )*/
    /* Label strings depending on the shift state */
    sclchar* label[SCL_SHIFT_STATE_MAX][MAX_SIZE_OF_LABEL_FOR_ONE];
    sclchar* label_type;				/* Label propery name for specifying the appearance of label strings */
    /* image_path for the case of image label */
    sclchar* image_label_path[SCL_SHIFT_STATE_MAX][SCL_BUTTON_STATE_MAX];
    sclchar* image_label_type;			/* Label propery name for specifying the appearance of image labels */
    /* If bg_image_path is NULL, background will not be drawn. If "", layout image will be used instead */
    /* image_path for the button's background */
    sclchar* bg_image_path[SCL_SHIFT_STATE_MAX][SCL_BUTTON_STATE_MAX];
    sclbyte key_value_count;			/* Number of key events in case of multitap or rotation button */
    /* A string value delivered to ISE when clicked */
    sclchar* key_value[SCL_SHIFT_STATE_MAX][MAX_SIZE_OF_MULTITAP_CHAR];
    /* A long int value delivered to ISE when clicked */
    sclulong key_event[SCL_SHIFT_STATE_MAX][MAX_SIZE_OF_MULTITAP_CHAR];
    SCLKeyType long_key_type;				/* A key type delivered to ISE when long key clicked */
    sclchar* long_key_value;			/* A string value delivered to ISE when long key clicked */
    sclulong long_key_event;			/* A long int value delivered to ISE when long key clicked */
    sclboolean use_repeat_key;			/* Indicates whether this button allows repeated key events when pressed for a long time */
    /* optional */
    /* A string of candidate characters that will be shown in an autopopup window */
    sclchar* autopopup_key_labels[SCL_SHIFT_STATE_MAX][MAX_SIZE_OF_AUTOPOPUP_STRING];
    sclulong autopopup_key_events[SCL_SHIFT_STATE_MAX][MAX_SIZE_OF_AUTOPOPUP_STRING];
    sclchar* autopopup_key_values[SCL_SHIFT_STATE_MAX][MAX_SIZE_OF_AUTOPOPUP_STRING];
    /* When set, any events raised from this button will not close opened popup window */
    sclboolean dont_close_popup;
    /* Extra options for each types of buttons :
        BUTTON_TYPE_ROTATION - Set to 1 if the button should return to initial state when other key is pressed
        BUTTON_TYPE_DIRECTION -
            use definitions below, that are declared in sclconfig.h file

            #define DIRECTION_EXTRA_OPTION_4_DIRECTIONS 0
            #define DIRECTION_EXTRA_OPTION_8_DIRECTIONS 1
            #define DIRECTION_EXTRA_OPTION_4_DIRECTIONS_WITH_LONG 2
            #define DIRECTION_EXTRA_OPTION_8_DIRECTIONS_WITH_LONG 3
            #define DIRECTION_EXTRA_OPTION_4_DIRECTIONS_WITH_RETURN 4
            #define DIRECTION_EXTRA_OPTION_8_DIRECTIONS_WITH_RETURN 5
            #define DIRECTION_EXTRA_OPTION_4_DIRECTIONS_WITH_RETURN_AND_CURVE 6
    */
    sclbyte extra_option;
    sclbyte multitouch_type;			/* Indicates what kind of multitouch type this button supports */
    /* If the button should be displayed differently depending on the modifier, describe a decorator and provide its ID here */
    sclchar* modifier_decorator;
    /* For the case if this button needs to display multiple labels or different string from the label string */
    sclchar* magnifier_label[SCL_SHIFT_STATE_MAX][MAX_SIZE_OF_LABEL_FOR_ONE];

    sclchar* hint_string[SCL_SHIFT_STATE_MAX][MAX_SIZE_OF_MULTITAP_CHAR];
}SclLayoutKeyCoordinate;

///**
//*@brief SCL Layout keyset structure, indicates which keyset should be used for specific inputmode and layout combination
//*/
//typedef struct _SclLayoutKeyset {
//    sclint inputmodeID;		/* Target inputmode ID */
//    sclint layoutID;		/* Target layout ID */
//    sclint keysetID;		/* Keyset ID that should be selected when the above inputmodeID and layoutID are given */
//    sclboolean popupEntry;	/* Whether this entry is for popup layout */
//} SclLayoutKeyset;

///**
//  *@brief SCL Layout key configure structure
//  */
//typedef struct _SclLayoutKeyConfigure {
//    sclboolean valid;			/* Indicates whether this information is in use or not */
//    sclchar* custom_id;			/* A value for indentifying a button when handling private keys or disable key. */
//    SCLButtonType button_type;	/* Indicates the behavioral type of this button */
//    SCLKeyType key_type;			/* Delivered key type when the button event is emitted */
//    SCLPopupType popup_type;		/* Popup type when this button is activated */
//    sclboolean use_magnifier;	/* Indicates whether this button needs to display magnifier window when clicked */
//    sclboolean use_long_key_magnifier;	/* Indicates whether this button needs to display magnifier window when long-pressed */
//    sclboolean enabled;			/* Indicates whether this button needs to be automatically enabled */ /* SEEMS TO BE NEEDLESS */
//    sclbyte popup_input_mode[SCL_DRAG_STATE_MAX];	/* Popup inputmode IDs depending on the direction of the drag action */
//    sclchar* sound_style;	/* A sound feedback style when this button is clicked */
//    sclchar* vibe_style;		/* A vibration feedback style when this button is clicked */
//    sclboolean is_side_button;	/* Indicates whether this button is located on the very left or right of the layout */
//}SclLayoutKeyConfigure;

/**
  *@brief SCL Label properties structure
  */
typedef struct _SclLabelProperties {
    sclboolean valid;						/* Indicates whether this information is in use or not */
    sclchar* label_type;					/* Identifies the current label property */
    sclchar* font_name;						/* Font name of this label property */
    sclbyte font_size;						/* Font size of this label property */
    /* Font color depending on the button and shift state */
    SclColor font_color[SCL_SHIFT_STATE_MAX][SCL_BUTTON_STATE_MAX];
    SCLLabelAlignment alignment;			/* Alignment option for this label property */
    sclshort padding_x;						/* X padding value for alignment option above */
    sclshort padding_y;						/* Y padding value for alignment option above */
    sclbyte inner_width;					/* Inner width value for making a inner rect padded in above values */
    sclbyte inner_height;					/* Inner height value for making a inner rect padded in above values */
    sclbyte shadow_distance;				/* A distance value for drawing shadow */
    SCLShadowDirection shadow_direction;	/* A direction option for drawing shadown */
    /* Shadow color depeding on the button and shift state */
	SclColor shadow_color[SCL_SHIFT_STATE_MAX][SCL_BUTTON_STATE_MAX];
}SclLabelProperties;

/**
  *@brief SCL Layout key properties structure
  */
/* SCL Supports drawing button's background using SclLayout's image_path - use below macro to bg_image_path for this feature */
#define SCL_BACKGROUND_IMAGE_STRING "BACKGROUND_IMAGE"
#define _Label_
#define _LabelImg_
#define _BGImg_
#define _KeyVal_
#define _KeyEvt_
//typedef struct _SclLayoutKeyProperties {
//    sclboolean valid;			/* Indicates whether this information is in use or not */
//    sclbyte label_count;			/* Number of labels for this button (Max value :MAX_SIZE_OF_LABEL_FOR_ONE )*/
//    sclchar* label[SCL_SHIFT_STATE_MAX][MAX_SIZE_OF_LABEL_FOR_ONE]; /* Label strings depending on the shift state */
//    sclchar* label_name;			/* Label propery name for specifying the appearance of label strings */
//    sclchar* label_image_path[SCL_SHIFT_STATE_MAX][SCL_BUTTON_STATE_MAX];	/* image_path for the case of image label */
//    /** If bg_image_path is NULL, background will not be drawn. If "", layout image will be used instead */
//    sclchar* bg_image_path[SCL_SHIFT_STATE_MAX][SCL_BUTTON_STATE_MAX];	/* image_path for the button's background */
//    sclbyte key_value_count;		/* Number of key events in case of multitap or rotation button */
//    sclchar* key_value[SCL_SHIFT_STATE_MAX][MAX_SIZE_OF_MULTITAP_CHAR];	/* A string value delivered to ISE when clicked */
//    sclulong key_event[SCL_SHIFT_STATE_MAX][MAX_SIZE_OF_MULTITAP_CHAR];	/* A long int value delivered to ISE when clicked */
//    sclchar* long_key_value;		/* A string value delivered to ISE when long key clicked */
//    sclulong long_key_event;		/* A long int value delivered to ISE when long key clicked */
//    sclboolean use_repeat_key;	/* Indicates whether this button allows repeated key events when pressed for a long time */
//    /* optional */
//    /* A string of candidate characters that will be shown in an autopopup window */
//    sclchar* autopopup_keys[SCL_SHIFT_STATE_MAX][MAX_SIZE_OF_AUTOPOPUP_STRING];
//    /* When set, any events raised from this button will not close opened popup window */
//    sclboolean dont_close_popup;
//    /* Extra options for each types of buttons :
//        BUTTON_TYPE_ROTATION - Set to 1 if the button should return to initial state when other key is pressed
//        BUTTON_TYPE_DIRECTION -
//            use definitions below, that are declared in sclconfig.h file
//
//            #define DIRECTION_EXTRA_OPTION_4_DIRECTIONS 0
//            #define DIRECTION_EXTRA_OPTION_8_DIRECTIONS 1
//            #define DIRECTION_EXTRA_OPTION_4_DIRECTIONS_WITH_LONG 2
//            #define DIRECTION_EXTRA_OPTION_8_DIRECTIONS_WITH_LONG 3
//            #define DIRECTION_EXTRA_OPTION_4_DIRECTIONS_WITH_RETURN 4
//            #define DIRECTION_EXTRA_OPTION_8_DIRECTIONS_WITH_RETURN 5
//            #define DIRECTION_EXTRA_OPTION_4_DIRECTIONS_WITH_RETURN_AND_CURVE 6
//    */
//    sclbyte extra_option;
//    sclbyte multitouch_type;		/* Indicates what kind of multitouch type this button supports */
//    /* If the button should be displayed differently depending on the modifier, describe a decorator and provide its ID here */
//    sclbyte modifier_decorator;
//    /* For the case if this button needs to display multiple labels or different string from the label string */
//    sclchar* magnifier_label[SCL_SHIFT_STATE_MAX][MAX_SIZE_OF_LABEL_FOR_ONE];
//}SclLayoutKeyProperties;

/**
  *@brief SCL private key structure for application, to change the configuration and property of a specific key
  */
typedef struct _SclPrivateKeyProperties {
    sclboolean valid;				/* Indicates whether this information is in use or not */
    sclshort input_mode_index;		/* Target inputmode index that this private key wants to affect */
    sclshort layout_index;			/* Target layout index that this private key wants to affect */
    sclbyte key_index;				/* Target key index that this private key wants to affect */

    /* Configurations */ /* Has same purpose with the SclLayoutKeyConfiguration's member variable */
    std::string custom_id;
    SCLButtonType button_type;
    SCLKeyType key_type;
    SCLPopupType popup_type;
    sclboolean use_magnifier;
    sclboolean use_long_key_magnifier;
    sclboolean enabled;
    std::string popup_input_mode[SCL_DRAG_STATE_MAX];
    std::string sound_style;
    std::string vibe_style;

    /* Properties */ /* Has same purpose with the SclLayoutKeyProperties' member variable */
    sclbyte label_count;
    std::string label[SCL_SHIFT_STATE_MAX][MAX_SIZE_OF_LABEL_FOR_ONE];
    std::string label_type;
    std::string image_label_path[SCL_SHIFT_STATE_MAX][SCL_BUTTON_STATE_MAX];
    std::string image_label_type;
    std::string bg_image_path[SCL_SHIFT_STATE_MAX][SCL_BUTTON_STATE_MAX];
    sclbyte key_value_count;
    std::string key_value[SCL_SHIFT_STATE_MAX][MAX_SIZE_OF_MULTITAP_CHAR];
    sclulong key_event[SCL_SHIFT_STATE_MAX][MAX_SIZE_OF_MULTITAP_CHAR];
    SCLKeyType long_key_type;
    std::string long_key_value;
    sclulong long_key_event;
    sclboolean use_repeat_key;
    std::string autopopup_key_labels[SCL_SHIFT_STATE_MAX][MAX_SIZE_OF_AUTOPOPUP_STRING];
    sclulong autopopup_key_events[SCL_SHIFT_STATE_MAX][MAX_SIZE_OF_AUTOPOPUP_STRING];
    std::string autopopup_key_values[SCL_SHIFT_STATE_MAX][MAX_SIZE_OF_AUTOPOPUP_STRING];
    sclboolean dont_close_popup;
    sclbyte extra_option;
    sclbyte multitouch_type;
    sclbyte modifier_decorator;

    std::string hint_string[SCL_SHIFT_STATE_MAX][MAX_SIZE_OF_MULTITAP_CHAR];
}SclPrivateKeyProperties;

/**
  *@brief SCL S/W button style structure
  */
typedef struct _SclSWButtonStyle {
    scldouble line_width[SCL_BUTTON_STATE_MAX];	/* A border line width when drawing a SW button */
    SclColor line_color[SCL_BUTTON_STATE_MAX];	/* A border line color when drawing a SW buttton */
    SclColor bg_color[SCL_BUTTON_STATE_MAX];	/* A background color when drawing a SW button */
    scldouble line_curve;						/* A line curve value for drawing round rectangle. This can be ignored in some platforms */
    sclfloat bg_alpha;							/* A alpha value used when painting background. This can be ignored in some platforms */
}SclSWButtonStyle;

/**
  *@brief SCL input mode configuration structure
  */
typedef struct _SclDefaultConfigure {
    SCLDisplayMode display_mode;			/* Initial display mode */
    sclchar *input_mode;					/* Initial input mode */
    sclchar *image_file_base_path;			/* Default image files directory */
    sclint target_screen_width;				/* Targeting screen width */
    sclint target_screen_height;			/* Targeting screen height */
    sclboolean auto_detect_landscape;		/* A flag for switching portrait-landscape if screen width is bigger than height */
    sclboolean use_magnifier_window;		/* A flag for enabling magnifer window */
    sclboolean use_auto_popup;				/* A flag for enabling autopopup window */
    sclboolean use_zoom_window;				/* A flag for enabling zoom window */
    sclboolean on_error_noti_send;			/* A flag for playing error sound */
    sclboolean use_word_deletion;			/* A flag for using word deletion mode when repeating delete key */
    sclbyte sw_button_style;				/* Describes which SW button style should be used */
    /* Touch offset adjustment level description */
    SCLTouchOffsetLevel touch_offset_level[DISPLAYMODE_MAX];
    SclPoint touch_offset[DISPLAYMODE_MAX];	/* Touch offset adjustment value description */
    sclchar *default_sub_layout;			/* Initial sub_layout_name */
    sclboolean use_actual_dim_window;		/* Whether to use a new window or make a dim window as a layer of existing base window */
    SclColor dim_color;						/* Color setting for dim window */
}SclDefaultConfigure;

/**
  *@brief SCL magnifier(preview) window configuration structure. It will show the selected key label on the magnifier window
  */
typedef struct _SclMagnifierWndConfigure {
    SCLMagnifierStyle style;			/* Magnifier style description */
    sclshort width;						/* Width of magnifier window */
    sclshort height;					/* Height of magnifier window */
    SclRect label_area_rect;			/* Area description for displaying label string */
    sclchar* bg_image_path;				/* File path for background image */
    sclchar* bg_shift_image_path;		/* File path for background image in shift key state */
    sclchar* bg_shift_lock_image_path;	/* File path for background image in shift lock key state */
    sclchar* bg_long_key_image_path;	/* File path for background image in longkey state */
    sclboolean use_actual_window;		/* Whether to use a new window or make a popup window as a layer of existing base window */
    sclchar* label_type;				/* Label property name for each strings */
    sclshort padding_x;					/* Padding value X for displaying label string */
    sclshort padding_y;					/* Padding value Y for displaying label string */
    sclboolean show_shift_label;		/* Whether a shift label or a default label string should be displayed in shift mode */
}SclMagnifierWndConfigure;

/**
*@brief SCL auto-generated popup configuration structure. Mostly used for umlaut alternate characters in european languages
*/
typedef struct _SclAutoPopupConfigure {
    sclchar* bg_image_path;				/* File path for background image */
    SclColor bg_color;					/* Background color for SW background */
    scldouble bg_line_width;			/* Background SW border width */
    SclColor bg_line_color;				/* Background SW border color */
    sclbyte bg_padding;					/* Padding amount between the bg border and buttons */
    /* A filepath for each button's background image */
    sclchar* button_image_path[SCL_BUTTON_STATE_MAX];
    sclbyte sw_button_style;			/* Describes which SW button style should be used */
    sclshort button_width;				/* Each button's width */
    sclshort button_height;				/* Each button's height */
    sclbyte button_spacing;				/* Spacing amount between buttons */
    sclchar* label_type;				/* Label property name for each strings */
    /* A 8-way decotation images for autopopup window */
    sclchar* decoration_image_path[MAX_WND_DECORATOR];
    sclbyte decoration_size;			/* Size of window decoration images */
    sclbyte max_column;					/* Maximum number of items in a row */

    sclshort add_grab_left;				/* If this layout's style is POPUP_GRAB, desribes the left pixel for grab area */
    sclshort add_grab_right;			/* If this layout's style is POPUP_GRAB, desribes the right pixel for grab area */
    sclshort add_grab_top;				/* If this layout's style is POPUP_GRAB, desribes the top pixel for grab area */
    sclshort add_grab_bottom;			/* If this layout's style is POPUP_GRAB, desribes the bottom pixel for grab area */
}SclAutoPopupConfigure;

/**
*@brief 9 patch image information struct - image files registered here would be displayed in 9 patch style
*/
typedef struct _SclNinePatchInfo {
    const sclchar *image_path;			/* File path for identifying nine patch image file */
    sclshort left;						/* Amount of left pixels for describing area that should not be scaled */
    sclshort right;						/* Amount of right pixels for describing area that should not be scaled */
    sclshort top;						/* Amount of top pixels for describing area that should not be scaled */
    sclshort bottom;					/* Amount of bottom pixels for describing area that should not be scaled */
}SclNinePatchInfo;

/**
*@brief A struct for decorating a button depending on the current modifier state
*/
typedef struct _SclModifierDecoration {
    sclboolean valid;					/* Indicates whether this information is in use or not */
    sclboolean extract_background;		/* Whether to display only some part of background image */
    sclchar *name;
    /* Background image file for decoration */
    sclchar *bg_image_path[DISPLAYMODE_MAX][KEY_MODIFIER_MAX];
}SclModifierDecoration;

//SCL_END_DECLS

#endif //__SCL_STRUCTS_H__
