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

#include "layout_parser_helper.h"
#include <stdio.h>
#include <string>
using namespace std;

const string RotationHelper::pcPortrait = "portrait";
const string RotationHelper::pcLandscape = "landscape";

const string RotationHelper::toString(const int val) {
    switch (val) {
        case PORTRAIT:
            return pcPortrait;
        case LANDSCAPE:
            return pcLandscape;
        default:
            return pcLandscape;
    }
}
const int RotationHelper::Int(const string str) {
    if (str ==  pcPortrait) {
        return PORTRAIT;
    } else if (str ==  pcLandscape) {
        return LANDSCAPE;
    } else {
        return PORTRAIT;
    }
}

////////////////////////////////////////////////////////////
const string PopStyleHelper::pcBase = "base";
const string PopStyleHelper::pcPopup = "popup";
const string PopStyleHelper::pcPopup_Grab = "popup_grab";
const string PopStyleHelper::toString(const int val) {
    switch (val) {
        case BASE:
            return pcBase;
        case POPUP:
            return pcPopup;
        case POPUP_GRAB:
            return pcPopup_Grab;
        default:
            return pcBase;
    }
}

const int PopStyleHelper::Int(const string str) {
    if (str ==  pcBase) {
        return BASE;
    } else if (str ==  pcPopup) {
        return POPUP;
    } else if (str ==  pcPopup_Grab) {
        return POPUP_GRAB;
    } else {
        return BASE;
    }
}

///////////////////////////////////////////////////////

const string ButtonTypeHelper::pcNormal = "normal";
const string ButtonTypeHelper::pcGrab = "grab";
const string ButtonTypeHelper::pcSelfish = "selfish";
const string ButtonTypeHelper::pcDrag = "drag";
const string ButtonTypeHelper::pcMultitap = "multitap";
const string ButtonTypeHelper::pcRotation = "rotation";
const string ButtonTypeHelper::pcDirection = "direction";
const string ButtonTypeHelper::pcRelative_Direction = "relative_direction";
const string ButtonTypeHelper::pcToggle = "toggle";
const string ButtonTypeHelper::pcUiitem = "uiitem";

const string ButtonTypeHelper::toString(const int val) {
    switch (val) {
        case NORMAL:
            return pcNormal;
        case GRAB:
            return pcGrab;
        case SELFISH:
            return pcSelfish;
        case DRAG:
            return pcDrag;
        case MULTITAP:
            return pcMultitap;
        case ROTATION:
            return pcRotation;
        case DIRECTION:
            return pcDirection;
        case RELATIVE_DIRECTION:
            return pcRelative_Direction;
        case UIITEM:
            return pcUiitem;
        default:
            return pcNormal;
    }
}

const int ButtonTypeHelper::Int(const string str) {
    if (str ==  pcNormal) {
        return NORMAL;
    } else if (str ==  pcGrab) {
        return GRAB;
    } else if (str ==  pcSelfish) {
        return SELFISH;
    } else if (str ==  pcDrag) {
        return DRAG;
    } else if (str ==  pcMultitap) {
        return MULTITAP;
    } else if (str ==  pcRotation) {
        return ROTATION;
    } else if (str ==  pcDirection) {
        return DIRECTION;
    } else if (str ==  pcRelative_Direction) {
        return RELATIVE_DIRECTION;
    } else if (str ==  pcToggle) {
        return TOGGLE;
    } else if (str ==  pcUiitem) {
        return UIITEM;
    } else {
        return NORMAL;
    }
}
////////////////////////////////////////////////////
const string ShiftStateHelper::pcOn = "on";
const string ShiftStateHelper::pcOff = "off";
const string ShiftStateHelper::pcLoc = "loc";

const string ShiftStateHelper::toString(const int val) {
    switch (val) {
        case ON:
            return pcOn;
        case OFF:
            return pcOff;
        case LOC:
            return pcLoc;
        default:
            return pcOn;
    }
}

const int ShiftStateHelper::Int(const string str) {
    if (str ==  pcOn) {
        return ON;
    } else if (str ==  pcOff) {
        return OFF;
    } else if (str ==  pcLoc) {
        return LOC;
    } else {
        return ON;
    }
}
/////////////////////////////////////////////////////////
const string ButtonStateHelper::pcNormal = "normal";
const string ButtonStateHelper::pcPressed = "pressed";
const string ButtonStateHelper::pcDisabled = "disabled";
const string ButtonStateHelper::toString(const int val) {
    switch (val) {
        case NORMAL:
            return pcNormal;
        case PRESSED:
            return pcPressed;
        case DISABLED:
            return pcDisabled;
        default:
            return pcNormal;
    }
}
const int ButtonStateHelper::Int(const string str) {
    if (str ==  pcNormal) {
        return NORMAL;
    } else if (str ==  pcPressed) {
        return PRESSED;
    } else if (str ==  pcDisabled) {
        return DISABLED;
    } else {
        return NORMAL;
    }
}
///////////////////////////////////////////////////
const string KeyTypeHelper::pcNone = "none";
const string KeyTypeHelper::pcChar = "char";
const string KeyTypeHelper::pcControl = "control";
const string KeyTypeHelper::pcModechange = "modechange";
const string KeyTypeHelper::pcString = "string";
const string KeyTypeHelper::pcUser = "user";
const string KeyTypeHelper::toString(const int val) {
    switch (val) {
        case NONE:
            return pcNone;
        case CHAR:
            return pcChar;
        case CONTROL:
            return pcControl;
        case MODECHANGE:
            return pcModechange;
        case STRING:
            return pcString;
        case USER:
            return pcUser;
        default:
            return pcNone;
    }
}

const int KeyTypeHelper::Int(const string str) {
    if (str ==  pcNone) {
        return NONE;
    } else if (str ==  pcChar) {
        return CHAR;
    } else if (str ==  pcControl) {
        return CONTROL;
    } else if (str ==  pcModechange) {
        return MODECHANGE;
    } else if (str ==  pcString) {
        return STRING;
    } else if (str ==  pcUser) {
        return USER;
    } else {
        return NONE;
    }
}
/////////////////////////////////////////////////////////
const string PopupTypeHelper::pcNone = "none";
const string PopupTypeHelper::pcBtn_Release_Popup = "release_popup";
const string PopupTypeHelper::pcBtn_Release_Popup_Once = "release_popup_once";
const string PopupTypeHelper::pcBtn_Press_Popup_Drag = "press_popup_drag";
const string PopupTypeHelper::pcLongpress_Popup = "longpress_popup";
const string PopupTypeHelper::pcLongpress_Popup_Once = "longpress_popup_once";
const string PopupTypeHelper::pcAuto_Popup = "auto_popup";

const string PopupTypeHelper::toString(const int val) {
    switch (val) {
        case NONE:
            return pcNone;
        case BTN_RELEASE_POPUP:
            return pcBtn_Release_Popup;
        case BTN_RELEASE_POPUP_ONCE:
            return pcBtn_Release_Popup_Once;
        case BTN_PRESS_POPUP_DRAG:
            return pcBtn_Press_Popup_Drag;
        case LONGPRESS_POPUP:
            return pcLongpress_Popup;
        case LONGPRESS_POPUP_ONCE:
            return pcLongpress_Popup_Once;
        case AUTO_POPUP:
            return pcAuto_Popup;
        default:
            return pcNone;
    }
}
const int PopupTypeHelper::Int(const string str) {
    if (str ==  pcNone) {
        return NONE;
    } else if (str ==  pcBtn_Release_Popup) {
        return BTN_RELEASE_POPUP;
    } else if (str ==  pcBtn_Release_Popup_Once) {
        return BTN_RELEASE_POPUP_ONCE;
    } else if (str ==  pcBtn_Press_Popup_Drag) {
        return BTN_PRESS_POPUP_DRAG;
    } else if (str ==  pcLongpress_Popup) {
        return LONGPRESS_POPUP;
    } else if (str ==  pcLongpress_Popup_Once) {
        return LONGPRESS_POPUP_ONCE;
    } else if (str ==  pcAuto_Popup) {
        return AUTO_POPUP;
    } else {
        return NONE;
    }
}
////////////////////////////////////////////////////////
const string DragStateHelper::pcNone = "none";
const string DragStateHelper::pcLeft = "left";
const string DragStateHelper::pcRight = "right";
const string DragStateHelper::pcUp = "up";
const string DragStateHelper::pcDown = "down";
const string DragStateHelper::pcInvalid = "invalid";
const string DragStateHelper::pcReturn = "return";

const string DragStateHelper::toString(const int val) {
    switch (val) {
        case NONE:
            return pcNone;
        case LEFT:
            return pcLeft;
        case RIGHT:
            return pcRight;
        case UP:
            return pcUp;
        case DOWN:
            return pcDown;
        case INVALID:
            return pcInvalid;
        case RETURN:
            return pcReturn;
        default:
            return pcNone;
    }
}

const int DragStateHelper::Int(const string str) {
    if (str ==  pcNone) {
        return NONE;
    } else if (str ==  pcLeft) {
        return LEFT;
    } else if (str ==  pcRight) {
        return RIGHT;
    } else if (str ==  pcUp) {
        return UP;
    } else if (str ==  pcDown) {
        return DOWN;
    } else if (str ==  pcInvalid) {
        return INVALID;
    } else if (str ==  pcReturn) {
        return RETURN;
    } else {
        return NONE;
    }
}
/////////////////////////////////////////////////////////////
const string ExtraOptionHelper::pcOption_4_directions = "4-directions";
const string ExtraOptionHelper::pcOption_8_directions = "8-directions";
const string ExtraOptionHelper::pcOption_4_directions_long = "4-directions-long";
const string ExtraOptionHelper::pcOption_8_directions_long = "8-directions-long";
const string ExtraOptionHelper::pcOption_4_directions_return = "4-directions-return";
const string ExtraOptionHelper::pcOption_8_directions_return = "8-directions-return";
const string ExtraOptionHelper::pcOption_4_directions_return_curve = "4-directions-return-curve";

const string ExtraOptionHelper::toString(const int val) {
    switch (val) {
        case OPTION_4_DIRECTIONS:
            return pcOption_4_directions;
        case OPTION_8_DIRECTIONS:
            return pcOption_8_directions;
        case OPTION_4_DIRECTIONS_LONG:
            return pcOption_4_directions_long;
        case OPTION_8_DIRECTIONS_LONG:
            return pcOption_8_directions_long;
        case OPTION_4_DIRECTIONS_RETURN:
            return pcOption_4_directions_return;
        case OPTION_8_DIRECTIONS_RETURN:
            return pcOption_8_directions_return;
        case OPTION_4_DIRECTIONS_RETURN_CURVE:
            return pcOption_4_directions_return_curve;
        default:
            return pcOption_4_directions;
    }
}
const int ExtraOptionHelper::Int(const string str) {
    if (str ==  pcOption_4_directions) {
        return OPTION_4_DIRECTIONS;
    } else if (str ==  pcOption_8_directions) {
        return OPTION_8_DIRECTIONS;
    } else if (str ==  pcOption_4_directions_long) {
        return OPTION_4_DIRECTIONS_LONG;
    } else if (str ==  pcOption_8_directions_long) {
        return OPTION_8_DIRECTIONS_LONG;
    } else if (str ==  pcOption_4_directions_return) {
        return OPTION_4_DIRECTIONS_RETURN;
    } else if (str ==  pcOption_8_directions_return) {
        return OPTION_8_DIRECTIONS_RETURN;
    } else if (str ==  pcOption_4_directions_return_curve) {
        return OPTION_4_DIRECTIONS_RETURN_CURVE;
    } else {
        return OPTION_4_DIRECTIONS;
    }
}
/////////////////////////////////////////////////////////////////////////////
const string MultitouchTypeHelper::pcExclusive = "exclusive";
const string MultitouchTypeHelper::pcCooperative = "cooperative";
const string MultitouchTypeHelper::pcGrab_Sub_Events = "grab_sub_events";

const string MultitouchTypeHelper::toString(const int val) {
    switch (val) {
        case EXCLUSIVE:
            return pcExclusive;
        case COOPERATIVE:
            return pcCooperative;
        case GRAB_SUB_EVENTS:
            return pcGrab_Sub_Events;
        default:
            return pcExclusive;
    }
}
const int MultitouchTypeHelper::Int(const string str) {
    if (str ==  pcExclusive) {
        return EXCLUSIVE;
    } else if (str ==  pcCooperative) {
        return COOPERATIVE;
    } else if (str ==  pcGrab_Sub_Events) {
        return GRAB_SUB_EVENTS;
    } else {
        return EXCLUSIVE;
    }
}
////////////////////////////////////////////////////
const string BOOLTypeHelper::pcTrue = "true";
const string BOOLTypeHelper::pcFalse = "false";

const string BOOLTypeHelper::toString(const bool val) {
    if (val) return pcTrue;
    return pcFalse;
}
const int BOOLTypeHelper::Bool(const string str) {
    if (str ==  pcFalse) {
        return false;
    } else if (str ==  pcTrue) {
        return true;
    } else {
        return false;
    }
}
/////////////////////////////////////////////////
const string INTTypeHelper::dextoString(const int val) {
    char temp[8] = {0};
    sprintf(temp, "0x%x", val);
    return string(temp);
}
const string INTTypeHelper::toString(const int val) {
    char temp[8] = {0};
    sprintf(temp, "%d", val);
    return string(temp);
}
const int INTTypeHelper::Int(const string str) {
    int val = 0;
    sscanf(str.c_str(), "%d", &val);
    return val;
}
const int INTTypeHelper::dexInt(const string str) {
    int val = 0;
    sscanf(str.c_str(), "%x", &val);
    return val;
}
