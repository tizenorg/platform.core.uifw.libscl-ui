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

#ifndef __KEYSET_ATTR_HELPER__H__
#define __KEYSET_ATTR_HELPER__H__
#include <string>

class RotationHelper{
    public:
        static const int PORTRAIT = 0;
        static const int LANDSCAPE = 1;
        static const int MAX = 2;
        static const int DEFAULT = 0;

        static const std::string pcPortrait;
        static const std::string pcLandscape;
        static const std::string toString(const int val);

        static const int Int(const std::string str);
};
class PopStyleHelper{
    public:
        static const int BASE = 0;
        static const int POPUP = 1;
        static const int POPUP_GRAB = 2;
        static const int MAX = 3;
        static const int DEFAULT = 0;

        static const std::string pcBase;
        static const std::string pcPopup;
        static const std::string pcPopup_Grab;
        static const std::string toString(const int val);
        static const int Int(const std::string str);
};
class ButtonTypeHelper{
    public:
        //button type
        static const int NORMAL = 0;
        static const int GRAB = 1;
        static const int SELFISH = 2;
        static const int DRAG = 3;
        static const int MULTITAP = 4;
        static const int ROTATION = 5;
        static const int DIRECTION = 6;
        static const int RELATIVE_DIRECTION = 7;
        static const int UIITEM = 8;
        static const int MAX = 9;
        static const int DEFAULT = 0;

        static const std::string pcNormal;
        static const std::string pcGrab;
        static const std::string pcSelfish;
        static const std::string pcDrag;
        static const std::string pcMultitap;
        static const std::string pcRotation;
        static const std::string pcDirection;
        static const std::string pcRelative_Direction;
        static const std::string pcUiitem;

        static const std::string toString(const int val);

        static const int Int(const std::string str);
};
class ShiftStateHelper{
    public:
        static const int ON = 0;
        static const int OFF = 1;
        static const int LOC = 2;
        static const int MAX = 3;
        static const int DEFAULT = 0;

        static const std::string pcOn;
        static const std::string pcOff;
        static const std::string pcLoc;

        static const std::string toString(const int val);

        static const int Int(const std::string str);
};
class ButtonStateHelper{
    public:
        static const int NORMAL = 0;
        static const int PRESSED = 1;
        static const int DISABLED = 2;
        static const int MAX = 3;
        static const int DEFAULT = 0;

        static const std::string pcNormal;
        static const std::string pcPressed;
        static const std::string pcDisabled ;
        static const std::string toString(const int val);
        static const int Int(const std::string str);
};
class KeyTypeHelper{
    public:
        //key type
        static const int NONE = 0;
        static const int CHAR = 1;
        static const int CONTROL = 2;
        static const int MODECHANGE = 3;
        static const int STRING = 4;
        static const int USER = 5;
        static const int MAX = 6;
        static const int DEFAULT = 0;

        static const std::string pcNone;
        static const std::string pcChar;
        static const std::string pcControl;
        static const std::string pcModechange;
        static const std::string pcString;
        static const std::string pcUser;
        static const std::string toString(const int val);

        static const int Int(const std::string str);
};
class PopupTypeHelper {
    public:
        //popup type
        static const int NONE = 0;
        static const int BTN_RELEASE_POPUP = 1;
        static const int BTN_RELEASE_POPUP_ONCE = 2;
        static const int BTN_PRESS_POPUP_DRAG = 3;
        static const int LONGPRESS_POPUP = 4;
        static const int LONGPRESS_POPUP_ONCE = 5;
        static const int AUTO_POPUP = 6;
        static const int MAX = 7;
        static const int DEFAULT = 0;

        static const std::string pcNone;
        static const std::string pcBtn_Release_Popup;
        static const std::string pcBtn_Release_Popup_Once;
        static const std::string pcBtn_Press_Popup_Drag;
        static const std::string pcLongpress_Popup;
        static const std::string pcLongpress_Popup_Once;
        static const std::string pcAuto_Popup;

        static const std::string toString(const int val);
        static const int Int(const std::string str);
};
class DragStateHelper{
    public:
        //drag state
        static const int NONE = 0;
        static const int LEFT = 1;
        static const int RIGHT = 2;
        static const int UP = 3;
        static const int DOWN = 4;
        static const int INVALID = 5;
        static const int RETURN = 6;
        static const int MAX = 7;
        static const int DEFAULT = 0;

        static const std::string pcNone;
        static const std::string pcLeft;
        static const std::string pcRight;
        static const std::string pcUp;
        static const std::string pcDown;
        static const std::string pcInvalid;
        static const std::string pcReturn;

        static const std::string toString(const int val);

        static const int Int(const std::string str);
};
class ExtraOptionHelper {
    public:
        //extra option
        static const int OPTION_4_DIRECTIONS = 0;
        static const int OPTION_8_DIRECTIONS = 1;
        static const int OPTION_4_DIRECTIONS_LONG = 2;
        static const int OPTION_8_DIRECTIONS_LONG = 3;
        static const int OPTION_4_DIRECTIONS_RETURN = 4;
        static const int OPTION_8_DIRECTIONS_RETURN = 5;
        static const int OPTION_4_DIRECTIONS_RETURN_CURVE = 6;
        static const int MAX = 7;
        static const int DEFAULT = 0;

        static const std::string pcOption_4_directions;
        static const std::string pcOption_8_directions;
        static const std::string pcOption_4_directions_long;
        static const std::string pcOption_8_directions_long;
        static const std::string pcOption_4_directions_return;
        static const std::string pcOption_8_directions_return;
        static const std::string pcOption_4_directions_return_curve;

        static const std::string toString(const int val);
        static const int Int(const std::string str);
};

class MultitouchTypeHelper{
    public:
        //multitouch type
        static const int EXCLUSIVE = 0;
        static const int COOPERATIVE = 1;
        static const int GRAB_SUB_EVENTS = 2;
        static const int MAX = 3;
        static const int DEFAULT = 0;

        static const std::string pcExclusive;
        static const std::string pcCooperative;
        static const std::string pcGrab_Sub_Events;

        static const std::string toString(const int val);
        static const int Int(const std::string str);
};

class BOOLTypeHelper{
    public:
        static const bool DEFAULT = false;
        static const std::string pcTrue;
        static const std::string pcFalse;

        static const std::string toString(const bool val);
        static const int Bool(const std::string str);
};
class INTTypeHelper{
    public:
        static const int DEFAULT = 0;
        static const std::string dextoString(const int val);
        static const std::string toString(const int val);
        static const int Int(const std::string str);
        static const int dexInt(const std::string str);
};

#endif
