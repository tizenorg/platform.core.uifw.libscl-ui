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
#include "scltypes.h"
#include "sclres_type.h"
#ifndef __SCL_KEY_FOCUS_HANDLER_H__
#define __SCL_KEY_FOCUS_HANDLER_H__

//#define TARGET_EMULATOR
//SCL_BEGIN_DECLS

namespace scl
{
typedef enum _SclKeyFocusNavigationDirection {
	NAVIGATE_LEFT,
	NAVIGATE_RIGHT,
	NAVIGATE_UP,
	NAVIGATE_DOWN
}SclKeyFocusNavigationDirection;

typedef struct _SclKeyboardRowInfo {
	sclbyte start_index;
	sclbyte size;
	sclchar *sub_layout;
	sclshort col_coord[20];
}SclKeyboardRowInfo;

typedef struct _SclKeyFocusNavigationInfo {
	sclbyte total_rows;
	sclbyte current_row;
	sclbyte current_column;
	sclshort row_coord;
	SclKeyboardRowInfo* rows[20];
}SclKeyFocusNavigationInfo;

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

	/*Focus grab/ungrab API*/
	bool grab_keyboard(const sclwindow parent);
	void ungrab_keyboard(const sclwindow parent);

	/*Focus navigation info buildig API*/
    void reset_key_navigation_info(void);
    void update_key_navigation_info(SclLayoutKeyCoordinatePointer p_next_key, sclbyte index);
    void finalize_key_navigation_info(void);

	/*Focus navigation API*/
	void	init_key_index(void);
    sclbyte get_current_key_index(void);
    sclbyte get_next_key_index(SclKeyFocusNavigationDirection direction);

private:
    SclKeyFocusNavigationInfo m_key_navi_info;
	bool m_keyboard_grabbed;
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
