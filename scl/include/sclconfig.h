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

#ifndef __SCL_CONFIG_H__
#define __SCL_CONFIG_H__

namespace scl
{

#ifndef IMG_PATH_PREFIX
#define IMG_PATH_PREFIX "image"
#endif

/* Display direction definition */
typedef enum _SCLDisplayMode {
    DISPLAYMODE_PORTRAIT = 0,	/* portrait display mode */
    DISPLAYMODE_LANDSCAPE,		/* landscape display mode */
    DISPLAYMODE_MAX,			/* maximum count of display mode */
}SCLDisplayMode;

/* Rotation definition */
typedef enum _SCLRotation {
    ROTATION_0 = 0,		/* No rotation */
    ROTATION_90_CW,		/* 90 degree, clockwise */
    ROTATION_180,		/* 180 degree */
    ROTATION_90_CCW,	/* 90 degree, counter-clockwise (Same as 270 degree, clockwise) */
    ROTATION_MAX,
}SCLRotation;

/**@brief  Layout and Input mode Style definition */
typedef enum _SCLLayoutStyle {
    LAYOUT_STYLE_BASE = 0,
    LAYOUT_STYLE_POPUP,
    LAYOUT_STYLE_POPUP_GRAB,
    MAX_LAYOUT_STYLE
}SCLLayoutStyle;

/* Key Define */

/**@brief  Button Type definition */
typedef enum _SCLButtonType {
    BUTTON_TYPE_NORMAL	 = 0,		/**< Normal key */
    BUTTON_TYPE_GRAB,				/**< Focus grab */
    /* No button gets focus when cursor moves out of this button's area */
    BUTTON_TYPE_SELFISH,
    /* A button that fires click event when dragged into the button's area */
    BUTTON_TYPE_DRAG,				/**< Drag button */
    /* A button with variant value - ex> [a] -> [b] */
    BUTTON_TYPE_MULTITAP,			/**< Multi-tap key */
    /* A multitap with automatic commitment - ex> ([a] -> a[b]) , ([p] -> p[i] -> pi[n] -> pin[g])*/
    BUTTON_TYPE_ROTATION,			/**< Rotation key */
    BUTTON_TYPE_DIRECTION,			/**< Direction decided by whole mouse movement from inital point */
    BUTTON_TYPE_RELATIVE_DIRECTION,	/**< Direction decided by recent mouse movement */
    BUTTON_TYPE_TOGGLE,				/** A button that has toggled state */
    BUTTON_TYPE_UIITEM,				/** Not a button, just for displaying image */
    MAX_BUTTON_TYPE					/* maximum number of Key type */
}SCLButtonType;

/* Values used for extra_option field of SclLayoutKeyProperties structure, in case of BUTTON_TYPE_DIRECTION type button */
#define DIRECTION_EXTRA_OPTION_4_DIRECTIONS 0
#define DIRECTION_EXTRA_OPTION_8_DIRECTIONS 1
#define DIRECTION_EXTRA_OPTION_4_DIRECTIONS_WITH_LONG 2
#define DIRECTION_EXTRA_OPTION_8_DIRECTIONS_WITH_LONG 3
#define DIRECTION_EXTRA_OPTION_4_DIRECTIONS_WITH_RETURN 4
#define DIRECTION_EXTRA_OPTION_8_DIRECTIONS_WITH_RETURN 5
#define DIRECTION_EXTRA_OPTION_4_DIRECTIONS_WITH_RETURN_AND_CURVE 6

/**@brief Key Type definition */
typedef enum _SCLKeyType {
    KEY_TYPE_NONE = 0,		/**< none */
    KEY_TYPE_CHAR,			/**< key that can be displayed independently */
    KEY_TYPE_CONTROL,		/**< key to use as control */
    KEY_TYPE_MODECHANGE,	/**< key to change mode*/
    KEY_TYPE_STRING,		/**< key to be displayed as string */
    KEY_TYPE_USER,			/**< key to be processed by user custom function */
    MAX_KEY_TYPE			/* maximum number of Key type */
}SCLKeyType;

/**@brief Key Event Type definition */
typedef enum _SCLEventType{
    EVENT_TYPE_NONE = 0,	/**< none */
    EVENT_TYPE_PRESS,		/**< button press event */
    EVENT_TYPE_MOVE,		/**< button move event */
    EVENT_TYPE_RELEASE,		/**< button release event */
    EVENT_TYPE_LONGPRESS,	/**< button longpress event */
    EVENT_TYPE_REPEAT,		/**< button repeat event */
    MAX_EVENT_TYPE			/* maximum number of Key type */
}SCLEventType;


/**@brief Key Modifier definition */
typedef enum _SCLKeyModifier {
    KEY_MODIFIER_NONE = 0,						/**< none */
    KEY_MODIFIER_LONGKEY,						/**< longkey event */
    KEY_MODIFIER_MULTITAP_START,				/**< multitap started */
    KEY_MODIFIER_MULTITAP_REPEAT,				/**< multitap repeated */

    KEY_MODIFIER_DIRECTION_LEFT,				/**< drag left */
    KEY_MODIFIER_DIRECTION_RIGHT,				/**< drag right */
    KEY_MODIFIER_DIRECTION_UP,					/**< drag up */
    KEY_MODIFIER_DIRECTION_DOWN,				/**< drage down */
    KEY_MODIFIER_DIRECTION_UP_LEFT,				/**< drag up left */
    KEY_MODIFIER_DIRECTION_UP_RIGHT,			/**< drag up right */
    KEY_MODIFIER_DIRECTION_DOWN_LEFT,			/**< drag down left */
    KEY_MODIFIER_DIRECTION_DOWN_RIGHT,			/**< drage down right */

    KEY_MODIFIER_DIRECTION_LEFT_LONG,			/**< drag left */
    KEY_MODIFIER_DIRECTION_RIGHT_LONG,			/**< drag right */
    KEY_MODIFIER_DIRECTION_UP_LONG,				/**< drag up */
    KEY_MODIFIER_DIRECTION_DOWN_LONG,			/**< drage down */
    KEY_MODIFIER_DIRECTION_UP_LEFT_LONG,		/**< drag up left */
    KEY_MODIFIER_DIRECTION_UP_RIGHT_LONG,		/**< drag up right */
    KEY_MODIFIER_DIRECTION_DOWN_LEFT_LONG,		/**< drag down left */
    KEY_MODIFIER_DIRECTION_DOWN_RIGHT_LONG,		/**< drage down right */

    KEY_MODIFIER_DIRECTION_LEFT_RETURN,			/**< drag left */
    KEY_MODIFIER_DIRECTION_RIGHT_RETURN,		/**< drag right */
    KEY_MODIFIER_DIRECTION_UP_RETURN,			/**< drag up */
    KEY_MODIFIER_DIRECTION_DOWN_RETURN,			/**< drage down */
    KEY_MODIFIER_DIRECTION_UP_LEFT_RETURN,		/**< drag up left */
    KEY_MODIFIER_DIRECTION_UP_RIGHT_RETURN,		/**< drag up right */
    KEY_MODIFIER_DIRECTION_DOWN_LEFT_RETURN,	/**< drag down left */
    KEY_MODIFIER_DIRECTION_DOWN_RIGHT_RETURN,	/**< drage down right */

    KEY_MODIFIER_DIRECTION_CURVE_UP_LEFT,		/**< drag up left */
    KEY_MODIFIER_DIRECTION_CURVE_UP_RIGHT,		/**< drag up right */
    KEY_MODIFIER_DIRECTION_CURVE_DOWN_LEFT,		/**< drag down left */
    KEY_MODIFIER_DIRECTION_CURVE_DOWN_RIGHT,	/**< drage down right */
    KEY_MODIFIER_DIRECTION_CURVE_LEFT_UP,		/**< drag left up */
    KEY_MODIFIER_DIRECTION_CURVE_LEFT_DOWN,		/**< drag left down */
    KEY_MODIFIER_DIRECTION_CURVE_RIGHT_UP,		/**< drag right up */
    KEY_MODIFIER_DIRECTION_CURVE_RIGHT_DOWN,	/**< drage right down */

    KEY_MODIFIER_TOGGLED,						/**< toggled state */

    KEY_MODIFIER_MAX			/* maximum number of Key type */
}SCLKeyModifier;


/**@brief  Popup Type definition */
typedef enum _SCLPopupType {
    /**< doesn't use popup */
    POPUP_TYPE_NONE = 0,
    /**< to use popup (It is used in extention character sign. After to be selected, this action is continued until selecting another area) */
    POPUP_TYPE_BTN_RELEASE_POPUP,
    /**< to use popup (It is used in extention character sign. After to be selected, this action disappears instantly) */
    POPUP_TYPE_BTN_RELEASE_POPUP_ONCE,
    /**< to use popup (it is used in extention character sign. This action disappears at Drag-Up */
    POPUP_TYPE_BTN_PRESS_POPUP_DRAG,
    /**< to use popup (it is used in extention character sign. This action disappears at Drag-Up */
    POPUP_TYPE_BTN_LONGPRESS_POPUP,
    /**< to use popup (it is used in extention character sign. This action disappears at Drag-Up */
    POPUP_TYPE_BTN_LONGPRESS_POPUP_ONCE,
    /**< to use popup (for multi-tap char) */
    POPUP_TYPE_AUTO_POPUP,
    /* maximum number of popup type */
    MAX_POPUP_TYPE
}SCLPopupType;

/**@brief Candidate position style definition */
typedef enum _SCLCandidatePosition {
    CAND_POS_ABOVE_KEYPAD = 0,
    CAND_POS_BELOW_TEXT,
    CAND_POS_MANUAL,
    MAX_CANDIDATE_POSITION
}SCLCandidatePosition;

/**@brief Candidate style definition */
typedef enum _SCLCandidateStyle {
    CAND_STYLE_DEFAULT = 0,
    CAND_STYLE_A,
    CAND_STYLE_B,
    CAND_STYLE_C,
    MAX_CANDIDATE_STYLE
}SCLCandidateStyle;

/**@brief Magnifier display style definition */
typedef enum _SCLMagnifierStyle {
    MAGNIFIER_STYLE_LABEL_ONLY = 0,
    MAGNIFIER_STYLE_SEL_AREA_CAPTURE,
    MAX_MAGNIFIER_STYLE
}SCLMagnifierStyle;

/**@brief Preview window position type definition */
typedef enum _SCLPreviewPosition {
    PREVIEW_POS_CENTER_OF_APPL_AREA = 0,
    PREVIEW_POS_ABOVE_SELECTED_BUTTON,
    PREVIEW_POS_MANUAL,
    MAX_PREVIEW_POSITION
}SCLPreviewPosition;

/**@brief Action state definition */
typedef enum _SCLActionState {
    ACTION_STATE_BASE_INIT = 0,
    ACTION_STATE_BASE_PRESS,
    ACTION_STATE_BASE_MOVING,
    ACTION_STATE_BASE_RELEASE,
    ACTION_STATE_BASE_LONGKEY,
    ACTION_STATE_BASE_REPEATKEY,
    ACTION_STATE_POPUP_INIT,
    ACTION_STATE_POPUP_PRESS,
    ACTION_STATE_POPUP_MOVING,
    ACTION_STATE_POPUP_RELEASE,
    ACTION_STATE_POPUP_LONGKEY,
    ACTION_STATE_POPUP_REPEATKEY,
    MAX_ACTION_STATE
}SCLActionState;

/**@brief  Button state definition */
typedef enum _SCLButtonState {
    BUTTON_STATE_NORMAL = 0,	/**< normal state */
    BUTTON_STATE_PRESSED,		/**< pressed state */
    BUTTON_STATE_DISABLED,		/**< disabled state */
    BUTTON_STATE_TOGGLED,		/**< toggled state */
    //BUTTON_STATE_HIGHLIGHT,	/**< highlighed state */
    //BUTTON_STATE_LONGKEY,		/**< londkey state */
    SCL_BUTTON_STATE_MAX		/* maximum number of button state */
}SCLButtonState;


/**@brief  Drag Direction definition */
typedef enum _SCLDragType {
    DRAG_NONE = 0,		/* doesn't use Drag */
    DRAG_LEFT,			/* drag to left side */
    DRAG_RIGHT,			/* drag to right side */
    DRAG_UP,			/* drag to upper side */
    DRAG_DOWN,			/* drage to down side */
    DRAG_LEFT_TOP,		/* drag to left top side */
    DRAG_RIGHT_TOP,		/* drag to right top side */
    DRAG_LEFT_BOTTOM,	/* drag to left bottm side */
    DRAG_RIGHT_BOTTOM,	/* drage to right bottom side */
    MAX_DRAG,			/* maximum number of drag type */
}SCLDragType;

/**@brief  Label alignment definition */
typedef enum _SCLLabelAlignment {
    LABEL_ALIGN_LEFT_TOP = 0,
    LABEL_ALIGN_CENTER_TOP,
    LABEL_ALIGN_RIGHT_TOP,
    LABEL_ALIGN_LEFT_MIDDLE,
    LABEL_ALIGN_CENTER_MIDDLE,
    LABEL_ALIGN_RIGHT_MIDDLE,
    LABEL_ALIGN_LEFT_BOTTOM,
    LABEL_ALIGN_CENTER_BOTTOM,
    LABEL_ALIGN_RIGHT_BOTTOM,
    MAX_LABEL_ALIGNMENT
}SCLLabelAlignment;

/**@brief  Shadow direction definition */
typedef enum _SCLShadowDirection {
    SHADOW_DIRECTION_NONE = 0,
    SHADOW_DIRECTION_LEFT_TOP ,
    SHADOW_DIRECTION_CENTER_TOP,
    SHADOW_DIRECTION_RIGHT_TOP,
    SHADOW_DIRECTION_LEFT_MIDDLE,
    SHADOW_DIRECTION_CENTER_MIDDLE,
    SHADOW_DIRECTION_RIGHT_MIDDLE,
    SHADOW_DIRECTION_LEFT_BOTTOM,
    SHADOW_DIRECTION_CENTER_BOTTOM,
    SHADOW_DIRECTION_RIGHT_BOTTOM,
    MAX_SHADOW_DIRECTION
}SCLShadowDirection;

/**@brief  timer id */
typedef enum _SCLTimer {
    SCL_TIMER_AUTOPOPUP = 100,
    SCL_TIMER_SHORT_LONGKEY,
    SCL_TIMER_LONGKEY,
    SCL_TIMER_REPEATKEY,
    SCL_TIMER_USERAREA,
    SCL_TIMER_BUTTON_DELAY,
    SCL_TIMER_POPUP_TIMEOUT,
    SCL_TIMER_ANIMATION,
    SCL_TIMER_AUTOTEST,
}SCLTimer;

/**@brief  Feedback style */
typedef enum _SCLFeedbackStyle {
    FEEDBACK_STYLE_SOUND,
    FEEDBACK_STYLE_VIBRATION,
    MAX_FEEDBACK_STYLE
}SCLFeedbackStyle;

/**@brief  Touch Offset */
typedef enum _SCLTouchOffsetLevel {
    TOUCH_OFFSET_LEVEL_1,
    TOUCH_OFFSET_LEVEL_2,
    TOUCH_OFFSET_LEVEL_3,
    MAX_TOUCH_OFFSET_LEVEL
}SCLTouchOffsetLevel;

/* Type Limitation definition */
#define MAX_SIZE_OF_LAYOUT_NAME 255
#define MAX_SIZE_OF_MULTITAP_CHAR 10
#define MAX_SIZE_OF_LABEL_FOR_ONE 9
#define MAX_SIZE_OF_LAYOUT 10
#define MAX_SIZE_OF_CANDIDATE 50
#define MAX_PRIVATE_KEY 100
#define MAX_DISABLED_KEY 100
#define MAX_SIZE_OF_AUTOPOPUP_STRING 20
#define MAX_SIZE_OF_MAGNIFIER_STRING 4
#define MAX_SIZE_OF_SUBLAYOUT_STRING 32

/**@brief  Window Decorator Images */
typedef enum _SCLWindowDecorator {
    WND_DECORATOR_TOP_LEFT,
    WND_DECORATOR_TOP_CENTER,
    WND_DECORATOR_TOP_RIGHT,
    WND_DECORATOR_MIDDLE_LEFT,
    WND_DECORATOR_MIDDLE_RIGHT,
    WND_DECORATOR_BOTTOM_LEFT,
    WND_DECORATOR_BOTTOM_CENTER,
    WND_DECORATOR_BOTTOM_RIGHT,
    MAX_WND_DECORATOR,
}SCLWindowDecorator;

/**@brief  Highligh moving direction */
typedef enum _SCLHighlightNavigationDirection {
    HIGHLIGHT_NAVIGATE_NONE,
    HIGHLIGHT_NAVIGATE_LEFT,
    HIGHLIGHT_NAVIGATE_RIGHT,
    HIGHLIGHT_NAVIGATE_UP,
    HIGHLIGHT_NAVIGATE_DOWN,
    HIGHLIGHT_NAVIGATE_MAX
}SCLHighlightNavigationDirection;

/**@brief  SCL Notification to ISEs */
typedef enum _SCLUINotiType {
    SCL_UINOTITYPE_POPUP_OPENING,
    SCL_UINOTITYPE_POPUP_OPENED,
    SCL_UINOTITYPE_POPUP_CLOSING,
    SCL_UINOTITYPE_POPUP_CLOSED,
    SCL_UINOTITYPE_GESTURE_FLICK,
    SCL_UINOTITYPE_SHIFT_STATE_CHANGE,
    SCL_UINOTITYPE_INPUT_MODE_CHANGE,
    SCL_UINOTITYPE_HIGHLIGHT_NAVIGATE,
    MAX_NOTITYPE,
}SCLUINotiType;

/**@brief  Current dragging state */
typedef enum _SCLDragState {
    SCL_DRAG_STATE_NONE,
    SCL_DRAG_STATE_LEFT,
    SCL_DRAG_STATE_RIGHT,
    SCL_DRAG_STATE_UP,
    SCL_DRAG_STATE_DOWN,
    SCL_DRAG_STATE_INVALID,
    SCL_DRAG_STATE_RETURN,
    SCL_DRAG_STATE_MAX,
} SCLDragState;

typedef enum _SCLMultitouchType {
    SCL_MULTI_TOUCH_TYPE_EXCLUSIVE,			/* On a new PRESS event, previous touch events will be forcefully released */
    SCL_MULTI_TOUCH_TYPE_SETTLE_PREVIOUS,	/* On a new PRESS event, previous touch events will be forcefully settled */
    SCL_MULTI_TOUCH_TYPE_COOPERATIVE,		/* New touch events do not affect previous touch events */
    SCL_MULTI_TOUCH_TYPE_GRAB_SUB_EVENTS,	/* New touch events are considered to be sub events, not generating button events */
} SCLMultiTouchType;

/* Shift flag */
typedef enum _SCLShiftState {
    SCL_SHIFT_STATE_OFF,
    SCL_SHIFT_STATE_ON,
    SCL_SHIFT_STATE_LOCK,
    SCL_SHIFT_STATE_MAX,
} SCLShiftState;

/* Shift Multi-touch state */
typedef enum _SCLShiftMultitouchState {
    SCL_SHIFT_MULTITOUCH_OFF,				/* Shift button is set to OFF state */
    SCL_SHIFT_MULTITOUCH_ON_PRESSED,		/* When OFF state, shift button is just pressed  */
    SCL_SHIFT_MULTITOUCH_ON_KEY_ENTERED,	/* While the shift button is in pressed state, another key was entered  */
    SCL_SHIFT_MULTITOUCH_ON_RELEASED,		/* Shift button was released without any other key stroke */
    SCL_SHIFT_MULTITOUCH_LOCK,				/* Shift button is set to LOCK state */
    SCL_SHIFT_MULTITOUCH_MAX,
} SCLShiftMultitouchState;

/* UI Parser types */
typedef enum _SCLParserType {
    SCL_PARSER_TYPE_XML,
    SCL_PARSER_TYPE_BINARY_XML,
} SCLParserType;

/* Starting Coordinates Options */
typedef enum _SCLStartingCoordinatesOption {
    SCL_STARTING_COORDINATES_OPTION_ALL,			/* Draw everything relative to the starting coordinate */
    SCL_STARTING_COORDINATES_OPTION_BUTTONS_ONLY,	/* Affect starting coordinates only to the buttons */
} SCLStartingCoordinatesOption;

/* SCL predefined Identifiers */
#define SCL_SHIFT_STATE_OFF_HINT_STRING "Shift Off"
#define SCL_SHIFT_STATE_ON_HINT_STRING "Shift On"
#define SCL_SHIFT_STATE_LOCK_HINT_STRING "Shift Lock"

#define SCL_INPUTMODE_AUTOPOPUP 10000
#define SCL_LAYOUT_AUTOPOPUP 10000
#define SCL_LAYOUT_AUTOPOPUP_NAME "Hidden Text Popup"

#define SCL_LABEL_PROPERTY_AUTOPOPUP 10000
#define SCL_LABEL_PROPERTY_CANDIDATE 10001
#define SCL_LABEL_PROPERTY_ZOOMING 10002
#define SCL_LABEL_PROPERTY_PREVIEW 10003

#define SCL_LABEL_OVERLENGTH_TEXT_RESIZE_RATE 0.9f

#define SCL_LONGKEY_DURATION 800
#define SCL_LONGKEY_CANCEL_DIST -1
#define SCL_AUTOPOPUP_KEY_DURATION 500
#define SCL_REPEATKEY_DURATION 150
#define SCL_REPEATKEY_WORD_DELETION_START_DURATION 100
#define SCL_REPEATKEY_ACCELERATION 10
#define SCL_REPEATKEY_MIN_DURATION 50

#define SCL_BUTTON_MIN_DURATION 100

#define SCL_DIRECTION_RECOG_DIST 60
#define SCL_DIRECTION_RECOG_DIST_SIDE 20
#define SCL_DIRECTION_LONG_RECOG_DIST 50
#define SCL_DIRECTION_RELATIVE_RECOG_DIST 10
#define SCL_DIRECTION_RELATIVE_DIFFDIR_RECOG_DIST 30

#define SCL_DRAG_CURVE_RECOG_DIST 70
#define SCL_DRAG_CURVE_4_DIRECTION_ANGLE_VALUE 1.0f // ratio between x and y for determining 4 direction, 3 means atan(1/3) degree
#define SCL_DRAG_CURVE_FINAL_ANGLE_VALUE 3.0f // ratio between x and y for determining final drag curve, 3 means atan(1/3) degree

#define SCL_DRAG_RETURN_RECOG_THRESHOLD_RETURN 1.1f // In RETURN drag state, apply this value for checking drag return action
#define SCL_DRAG_RETURN_RECOG_THRESHOLD_OTHER 0.9f // In non-NONE drag state, apply this value for checking drag return action
#define SCL_DRAG_RETURN_RECOG_DIST_BACKWARD 20
#define SCL_DRAG_RETURN_RECOG_ANGLE_BACKWARD (M_PI * 3) / 4

#define SCL_AUTOTEST_TIMER_INTERVAL 20
#define SCL_AUTOTEST_TIMER_INITIAL_INTERVAL 3000
#define SCL_AUTOTEST_NUM 500

#define SCL_MOUSE_BUTTON_CHANGE_THRESHOLD_X 15
#define SCL_MOUSE_BUTTON_CHANGE_THRESHOLD_Y 20

#define SCL_FLICK_GESTURE_RECOG_THRESHOLD 200
#define SCL_FLICK_GESTURE_RECOG_TIME 500

#define SCL_AUTO_DETECT_PORTRAIT_LANDSCAPE TRUE

/* FIXME : This should be configurable also */
#define SCL_HIGHLIGHT_UI_IMAGE "B09_Qwerty_btn_highlight.png"

#define SCL_ANIMATION_TIMER_INTERVAL (1000 / 30) // 30 frames per second
#define SCL_ANIMATION_TIME 300 // Animation for 300 ms

typedef enum _SCLDebugMode {
    DEBUGMODE_DISABLED,
    DEBUGMODE_DISPLAY_INTERNAL,
    DEBUGMODE_AUTOTEST,
}SCLDebugMode;

}

#endif //__SCL_CONFIG_H__
