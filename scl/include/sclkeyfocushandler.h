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
#include "scltypes.h"
#include "sclres_type.h"
#ifndef __SCL_KEY_FOCUS_HANDLER_H__
#define __SCL_KEY_FOCUS_HANDLER_H__

//#define TARGET_EMULATOR
//SCL_BEGIN_DECLS

namespace scl
{

#define NAVI_INFO_MAX_ROWS  20
#define NAVI_INFO_MAX_COLS  20

typedef struct {
    scl8 candidate;
    scl8 candidate_otherside;
} NEXT_CANDIDATE_INFO;

/*
typedef struct _SclKeyboardRowInfo {
    sclbyte start_index;
    sclbyte size;
    sclchar *sub_layout;
    sclshort col_coord[NAVI_INFO_MAX_COLS];
}SclKeyboardRowInfo;

typedef struct _SclKeyFocusNavigationInfo {
    sclbyte total_rows;
    sclshort row_coord;
    SclKeyboardRowInfo* rows[NAVI_INFO_MAX_ROWS];
}SclKeyFocusNavigationInfo;

typedef struct _SclPopupRelativeKeyInfo {
    sclbyte index;
    sclwindow window;
}SclPopupRelativeKeyInfo;

typedef struct _SclPopupRowInfo {
    sclbyte start_index;
    sclbyte size;
    sclshort col_coord[NAVI_INFO_MAX_COLS];
    SclPopupRelativeKeyInfo nexts[NAVI_INFO_MAX_COLS][NAVIGATE_MAX];
}SclPopupRowInfo;

typedef struct _SclPopupNavigationInfo {
    sclbyte total_rows;
    sclshort row_coord;
    SclPopupRowInfo* rows[NAVI_INFO_MAX_ROWS];
}SclPopupNavigationInfo;
*/

/**
* @brief The class to store key focus navigation information
*
* This class implements functions to support key focus navigation on the software keyboard
* using remote controller input. It generates navigation information for the currently loaded 
* keyboard layout and provides the index of next key depending upon the navigation direction.
* The key information can be retrieved from the array of key coordinates from Resource Cache.
* 
* Apart from key navigation information, this class also provides functions to grab and ungrab
* the remote controller keys.
*
* The public interface can be divided in three categories :
* 1. Focus grab/ungrab API : Invoked while showing and hiding the software keyboard
* - Client class: CSCLWindowsImplEfl
* 2. Focus navigation info buildig API : Invoked while computing layout of the current keyboard
* - Client class/function: CSCLResourceCache::recompute_layout
* 3. Focus navigation API: Invoked while handling remote controller key press events
* - Client class: CSCLEventsImplEfl
*/
class CSCLKeyFocusHandler
{
public:
    ~CSCLKeyFocusHandler();

    static CSCLKeyFocusHandler* get_instance();

#ifdef USING_KEY_GRAB
    /*Focus grab/ungrab API*/
    bool grab_keyboard(const sclwindow parent);
    void ungrab_keyboard(const sclwindow parent);
#endif

    /*Focus navigation info buildig API*/
    void reset_key_navigation_info(sclwindow window);
    void update_key_navigation_info(sclwindow window, scl8 index, SclLayoutKeyCoordinatePointer p_next_key);
    void finalize_key_navigation_info(sclwindow window);

    void popup_opened(sclwindow window);
    void popup_closed(sclwindow window);

    /*Focus navigation API*/
    void init_key_index();
    scl8 get_current_focus_key(void);
    sclwindow get_current_focus_window(void);
    void process_navigation(SCLHighlightNavigationDirection direction);

private:
    NEXT_CANDIDATE_INFO get_next_candidate_key(SCLHighlightNavigationDirection direction, SclRectangle cur, sclwindow window);

    sclwindow m_focus_window;
    scl8 m_focus_key;

#ifdef USING_KEY_GRAB
    bool m_keyboard_grabbed;
#endif
    bool sub_layout_match(sclchar *layout1,sclchar *layout2);

    CSCLKeyFocusHandler();
#ifdef TARGET_EMULATOR
    sclwindow m_sniffer;
    void create_sniffer_window(void);
    void set_window_accepts_focus(const sclwindow window, sclboolean acceptable);
#endif

};

}

//SCL_END_DECLS

#endif
