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
#include "sclkeyfocushandler.h"
#include <Elementary.h>
#include <Evas.h>
#include <dlog.h>
#include <Ecore_X.h>
#include <utilX.h>
#include "sclres_type.h"
#include "scldebug.h"
#include "sclcontext.h"
using namespace scl;

/**
 * Constructor
 */
CSCLKeyFocusHandler::CSCLKeyFocusHandler()
{
    SCL_DEBUG();
    m_keyboard_grabbed = FALSE;
    m_key_navi_info.total_rows = 0;
    m_key_navi_info.current_row = 0;
    m_key_navi_info.current_column = 0;
    m_key_navi_info.row_coord = 0;
    int i = 0;
    for(i = 0;i < 20; i++) {
        m_key_navi_info.rows[i] = NULL;
    }
#ifdef TARGET_EMULATOR
    create_sniffer_window();
#endif
}

/**
 * Destructor
 */
CSCLKeyFocusHandler::~CSCLKeyFocusHandler()
{
    SCL_DEBUG();
    for(int counter = 0; counter < m_key_navi_info.total_rows; counter++) {
        if (m_key_navi_info.rows[counter]) {
            if (m_key_navi_info.rows[counter]->sub_layout) {
                delete m_key_navi_info.rows[counter]->sub_layout;
                m_key_navi_info.rows[counter]->sub_layout = NULL;
            }
            delete m_key_navi_info.rows[counter];
            m_key_navi_info.rows[counter] = NULL;
        }
    }
}

CSCLKeyFocusHandler*
CSCLKeyFocusHandler::get_instance()
{
    static CSCLKeyFocusHandler instance;
    return &instance;
}

/**
 * Grabs the navigation and Return keys
 */
bool
CSCLKeyFocusHandler::grab_keyboard(const sclwindow parent)
{
    Evas_Object *window = (Evas_Object *)parent;
    Ecore_X_Window x_window = elm_win_xwindow_get(window);

    Display *x_display = (Display *)ecore_x_display_get();
    int grab_result;
    grab_result = utilx_grab_key(x_display, x_window, "Right", EXCLUSIVE_GRAB);
    if (0 == grab_result) {
        LOGD("Right Key Grabbed successfully\n");
    } else if (EXCLUSIVE_GRABBED_ALREADY == grab_result) {
        LOGD("Right Key already grabbed in Exclusiv mode\n");
    } else {
        LOGD("Failed to Grab Right key\n");
    }

    grab_result = utilx_grab_key(x_display, x_window, "Left", EXCLUSIVE_GRAB);
    if(0 == grab_result) {
        LOGD("Left Key Grabbed successfully\n");
    } else if (EXCLUSIVE_GRABBED_ALREADY == grab_result) {
        LOGD("Left Key already grabbed in Exclusiv mode\n");
    } else {
        LOGD("Failed to Grab Left key\n");
    }

    grab_result = utilx_grab_key(x_display, x_window, "Up", EXCLUSIVE_GRAB);
    if (0 == grab_result) {
        LOGD("Up Key Grabbed successfully\n");
    } else if (EXCLUSIVE_GRABBED_ALREADY == grab_result) {
        LOGD("Up Key already grabbed in Exclusiv mode\n");
    } else {
        LOGD("Failed to Grab Up key\n");
    }

    grab_result = utilx_grab_key(x_display, x_window, "Down", EXCLUSIVE_GRAB);
    if (0 == grab_result) {
        LOGD("Down Key Grabbed successfully\n");
    } else if (EXCLUSIVE_GRABBED_ALREADY == grab_result) {
        LOGD("Down Key already grabbed in Exclusiv mode\n");
    } else {
        LOGD("Failed to Grab Down key\n");
    }



    grab_result = utilx_grab_key(x_display, x_window, "Return", EXCLUSIVE_GRAB);
    if (0 == grab_result) {
        LOGD("Return Key Grabbed successfully\n");
    } else if (EXCLUSIVE_GRABBED_ALREADY == grab_result) {
        LOGD("Return Key already grabbed in Exclusiv mode\n");
    } else {
        LOGD("Failed to Grab Return key\n");
    }
/*
	grab_result = utilx_grab_key(x_display, x_window, KEY_VOLUMEUP, EXCLUSIVE_GRAB);
	if( 0 == grab_result ) {
		LOGD("KEY_VOLUMEUP Grabbed successfully\n");
	} else if(EXCLUSIVE_GRABBED_ALREADY == grab_result) {
		LOGD("KEY_VOLUMEUP already grabbed in Exclusiv mode\n");
	} else {
		LOGD("Failed to Grab KEY_VOLUMEUP\n");
	}


	grab_result = utilx_grab_key(x_display, x_window, KEY_VOLUMEDOWN, EXCLUSIVE_GRAB);
	if( 0 == grab_result ) {
		LOGD("KEY_VOLUMEDOWN Grabbed successfully\n");
	} else if(EXCLUSIVE_GRABBED_ALREADY == grab_result) {
		LOGD("KEY_VOLUMEDOWN already grabbed in Exclusiv mode\n");
	} else {
		LOGD("Failed to Grab KEY_VOLUMEDOWN\n");
	}
*/
    m_keyboard_grabbed = TRUE;
    return TRUE;
}

/**
 * UnGrabs the navigation and Return keys
 */
void
CSCLKeyFocusHandler::ungrab_keyboard(const sclwindow parent)
{
    Evas_Object *window = (Evas_Object *)parent;
    Ecore_X_Window x_window = elm_win_xwindow_get(window);
    Display *x_display = (Display *)ecore_x_display_get();
    int grab_result;
    grab_result = utilx_ungrab_key(x_display, x_window, "Right");
    if (0 == grab_result) {
        LOGD("Right Key UnGrabbed successfully\n");
    } else {
        LOGD("Failed to UnGrab Right key\n");
    }

    grab_result = utilx_ungrab_key(x_display, x_window, "Left");
    if (0 == grab_result) {
        LOGD("Left Key UnGrabbed successfully\n");
    } else {
        LOGD("Failed to UnGrab Left key\n");
    }

    grab_result = utilx_ungrab_key(x_display, x_window, "Up");
    if (0 == grab_result) {
        LOGD("Up Key UnGrabbed successfully\n");
    } else {
        LOGD("Failed to UnGrab Up key\n");
    }

    grab_result = utilx_ungrab_key(x_display, x_window, "Down");
    if (0 == grab_result) {
        LOGD("Down Key UnGrabbed successfully\n");
    } else {
        LOGD("Failed to UnGrab Down key\n");
    }

    grab_result = utilx_ungrab_key(x_display, x_window, "Return");
    if(0 == grab_result) {
        LOGD("Return Key UnGrabbed successfully\n");
    } else {
        LOGD("Failed to UnGrab Return key\n");
    }
/*
	grab_result = utilx_ungrab_key(x_display, x_window, "Enter");
	if( 0 == grab_result ) {
		LOGD("Enter Key UnGrabbed successfully\n");
	} else {
		LOGD("Failed to UnGrab Enter key\n");
	}

	grab_result = utilx_ungrab_key(x_display, x_window, KEY_VOLUMEUP);
	if( 0 == grab_result ) {
		LOGD("KEY_VOLUMEUP UnGrabbed successfully\n");
	} else {
		LOGD("Failed to UnGrab KEY_VOLUMEUP\n");
	}

	grab_result = utilx_ungrab_key(x_display, x_window, KEY_VOLUMEDOWN);
	if( 0 == grab_result ) {
		LOGD("KEY_VOLUMEDOWN UnGrabbed successfully\n");
	} else {
		LOGD("Failed to UnGrab KEY_VOLUMEDOWN\n");
	}
*/
    m_keyboard_grabbed = FALSE;
}

/**
 * Resets the navigation info
 */
void
CSCLKeyFocusHandler::reset_key_navigation_info(void)
{
    LOGD("Inside reset_key_navigation_info");
    LOGD("Total rows = %d \n",m_key_navi_info.total_rows);
    for(int counter = 0; counter < m_key_navi_info.total_rows; counter++) {
        if (m_key_navi_info.rows[counter]) {
            if (m_key_navi_info.rows[counter]->sub_layout) {
                delete m_key_navi_info.rows[counter]->sub_layout;
                m_key_navi_info.rows[counter]->sub_layout = NULL;
            }
            delete m_key_navi_info.rows[counter];
            m_key_navi_info.rows[counter] = NULL;
        }
    }

    m_key_navi_info.total_rows = 0;
    m_key_navi_info.current_row = 0;
    m_key_navi_info.current_column = 0;
    m_key_navi_info.row_coord = 0;

    LOGD("reset_key_navigation_info executed succesfully \n");
}

/**
 * Compares the sub-layout values
 */
inline bool
CSCLKeyFocusHandler::sub_layout_match(sclchar *layout1,sclchar *layout2)
{
    if (layout1) {
        if (layout2) {
            if (strcmp(layout1, layout2) == 0) {
                return TRUE;
            }
        }
    } else if (layout2) {
        return FALSE;
    } else {
        return TRUE;
    }
    return FALSE;
}

/**
 * Builds the key navigation info
 */
void
CSCLKeyFocusHandler::update_key_navigation_info(SclLayoutKeyCoordinatePointer p_next_key, sclbyte index)
{
    //LOGD("Inside update_key_navigation_info for index = %d \n",index);
    //check for new row..
    //LOGD("y coordinate = %d \n",p_next_key->y);
    bool allocate_row = FALSE;
    if (!m_key_navi_info.total_rows || m_key_navi_info.row_coord != p_next_key->y) {
        allocate_row = TRUE;
    } else {
        if (!sub_layout_match(m_key_navi_info.rows[m_key_navi_info.current_row]->sub_layout,p_next_key->sub_layout)) {
            allocate_row = TRUE;
        }
    }

    if (allocate_row) {
        //allocate new row..
        //LOGD("allocate new row.. = \n");
        m_key_navi_info.rows[m_key_navi_info.total_rows++] = new SclKeyboardRowInfo;
        //preserving the y-coordinate temprorily in row_coord for comparision in next call..
        m_key_navi_info.row_coord = p_next_key->y;
        //update the current_row to keep updating the size in next calls
        m_key_navi_info.current_row = m_key_navi_info.total_rows - 1;
        //set up the start index of row
        m_key_navi_info.rows[m_key_navi_info.current_row]->start_index = index;
        //update sub_layout info if any..
        if (p_next_key->sub_layout) {
            int size = strlen(p_next_key->sub_layout);
            m_key_navi_info.rows[m_key_navi_info.current_row]->sub_layout = new sclchar[size + 1];
            strcpy(m_key_navi_info.rows[m_key_navi_info.current_row]->sub_layout,p_next_key->sub_layout);
        } else {
            m_key_navi_info.rows[m_key_navi_info.current_row]->sub_layout = NULL;
        }
        //reset size, will be increamented afterwards..
        m_key_navi_info.rows[m_key_navi_info.current_row]->size = 0;
    }
    //update the start of this column
    m_key_navi_info.rows[m_key_navi_info.current_row]->col_coord[m_key_navi_info.rows[m_key_navi_info.current_row]->size] = p_next_key->x;
    //update the row size..
    m_key_navi_info.rows[m_key_navi_info.current_row]->size++;
    //LOGD("update_key_navigation_info executed succesfully for index = %d \n",index);
}

/**
 * Finalize the navigation info
 */
void
CSCLKeyFocusHandler::finalize_key_navigation_info(void)
{
    LOGD("Inside finalize_key_navigation_info");

    m_key_navi_info.current_row = 0;
    m_key_navi_info.current_column = 0;
    LOGD("Total rows = %d \n",m_key_navi_info.total_rows);
    LOGD("finalize_key_navigation_info executed succesfully \n");
}

/**
 * Initializes the key index to first key of first row
 */
void
CSCLKeyFocusHandler::init_key_index(void)
{
    CSCLContext *context = CSCLContext::get_instance();
    sclchar* current_sub_layout = context->get_cur_sublayout();
    int row = 0;
    for(row = 0; row <= m_key_navi_info.total_rows - 1; row++) {
        if (m_key_navi_info.rows[row]->sub_layout) {
            if (strcmp(m_key_navi_info.rows[row]->sub_layout, current_sub_layout) == 0) {
                break;
            }
        } else {
            break;
        }
    }
    if (row > m_key_navi_info.total_rows - 1) {
        row = 0;
    }
    m_key_navi_info.current_row = row;
    m_key_navi_info.current_column = 0;
    LOGD("Inside init_key_index m_key_navi_info.current_row = %d \n",m_key_navi_info.current_row);
}

/**
 * Returns the currently focussed key index
 */
sclbyte
CSCLKeyFocusHandler::get_current_key_index(void)
{
    LOGD("Inside get_current_key_index m_key_navi_info.current_row = %d \n",m_key_navi_info.current_row);
    sclbyte index = m_key_navi_info.rows[m_key_navi_info.current_row]->start_index + m_key_navi_info.current_column;
    LOGD("Inside get_current_key_index index = %d \n",index);
    return index;
}

/**
 * Computes and Returns the key index for next focussed key depending upon the navigation direction
 */
sclbyte
CSCLKeyFocusHandler::get_next_key_index(SclKeyFocusNavigationDirection direction)
{
    LOGD("Inside get_next_key_index m_key_navi_info.current_row = %d \n",m_key_navi_info.current_row);
    sclbyte index = 0;
    bool row_found = FALSE;
    int row = 0;
    int col = 0;	
    switch(direction) {
        case NAVIGATE_LEFT:
            if (m_key_navi_info.current_column) {
                m_key_navi_info.current_column--;
            } else {
                m_key_navi_info.current_column = m_key_navi_info.rows[m_key_navi_info.current_row]->size - 1;
            }
        break;
        case NAVIGATE_RIGHT:
            if (m_key_navi_info.current_column < m_key_navi_info.rows[m_key_navi_info.current_row]->size - 1) {
                m_key_navi_info.current_column++;
            } else {
                m_key_navi_info.current_column = 0;
            }
            LOGD("Inside get_next_key_index m_key_navi_info.current_column = %d \n",m_key_navi_info.current_column);
        break;
        case NAVIGATE_UP:
            if (m_key_navi_info.current_row) {
                CSCLContext *context = CSCLContext::get_instance();
                sclchar* current_sub_layout = context->get_cur_sublayout();
                for(row = m_key_navi_info.current_row - 1; row >= 0; row--) {
                    if (m_key_navi_info.rows[row]->sub_layout) {
                        if (strcmp(m_key_navi_info.rows[row]->sub_layout, current_sub_layout) == 0) {
                            row_found = TRUE;
                            break;
                        }
                    } else {
                        row_found = TRUE;
                        break;
                    }
                }
            }
        break;
        case NAVIGATE_DOWN:
            if (m_key_navi_info.current_row < m_key_navi_info.total_rows - 1) {
                CSCLContext *context = CSCLContext::get_instance();
                sclchar* current_sub_layout = context->get_cur_sublayout();
                for(row = m_key_navi_info.current_row + 1; row <= m_key_navi_info.total_rows - 1; row++) {
                    if (m_key_navi_info.rows[row]->sub_layout) {
                        if (strcmp(m_key_navi_info.rows[row]->sub_layout, current_sub_layout) == 0) {
                            row_found = TRUE;
                            break;
                        }
                    } else {
                        row_found = TRUE;
                        break;
                    }
                }
            }
        break;
        default:
        break;
    }

    if (row_found) {
        //get x xoordinate of the currently focused key
        int cur_row = m_key_navi_info.current_row;
        int cur_col = m_key_navi_info.current_column;
        sclshort cur_col_coodinate = m_key_navi_info.rows[cur_row]->col_coord[cur_col];
        //check if the current colum is within the size range of target row
        if (cur_col <= m_key_navi_info.rows[row]->size - 1) {
            //check if the x coordinate of current column is greater than the same column in the target row
            if (m_key_navi_info.rows[row]->col_coord[cur_col] <= cur_col_coodinate) {
                //check for a match from current column to last column
                for(col = cur_col; col <= m_key_navi_info.rows[row]->size - 1; col++) {
                    if (m_key_navi_info.rows[row]->col_coord[col] == cur_col_coodinate) {
                        break;
                    } else if (m_key_navi_info.rows[row]->col_coord[col] > cur_col_coodinate) {
                        col--;
                        break;
                    }
                }
            } else {
                //check for a match from current column to first column
                for(col = cur_col - 1; col >= 0; col--) {
                    if (m_key_navi_info.rows[row]->col_coord[col] <= cur_col_coodinate) {
                        break;
                    }
                }
            }
        } else {
            for(col = m_key_navi_info.rows[row]->size - 1; col >= 0; col--) {
                if (m_key_navi_info.rows[row]->col_coord[col] <= cur_col_coodinate) {
                    break;
                }
            }
        }
        if (col < 0) {
            col = 0;
        } else if (col > m_key_navi_info.rows[row]->size - 1) {
            col = m_key_navi_info.rows[row]->size - 1;
        } else {
        }
        m_key_navi_info.current_row = row;		
        m_key_navi_info.current_column = col;
    }
    index = m_key_navi_info.rows[m_key_navi_info.current_row]->start_index + m_key_navi_info.current_column;
    LOGD("Inside get_next_key_index index = %d \n",index);
    return index;
}

#ifdef TARGET_EMULATOR

static Ecore_Event_Handler *_sniffer_win_show_handler         = NULL;

/**
 * ecore event handler deletion
 */
static void delete_sniffer_win_show_handler (void)
{
    if (_sniffer_win_show_handler) {
        ecore_event_handler_del (_sniffer_win_show_handler);
        _sniffer_win_show_handler = NULL;
    }
}

/**
 * callback for window show event (sniffer window)
 */
static Eina_Bool x_event_sniffer_window_show_cb (void *data, int ev_type, void *event)
{

    Evas_Object *evas_window = (Evas_Object *)data;
    Ecore_X_Window x_window = elm_win_xwindow_get(evas_window);
    Ecore_X_Event_Window_Show *e = (Ecore_X_Event_Window_Show*)event;

    if (e->win == x_window) {

        LOGD("INSIDE =-=-=-=- x_event_sniffer_window_show_cb, Trying to Grab Key Board : \n");
        Eina_Bool ret = ecore_x_keyboard_grab(x_window);

        if (EINA_TRUE == ret) {
            LOGD("Keyboard Grabbed successfully by sniffer\n");
        } else {
            LOGD("Failed to Grab keyboard by sniffer\n");
        }
        ecore_event_handler_del(_sniffer_win_show_handler);
        return ECORE_CALLBACK_CANCEL;
    }
    LOGD("Wrong window .. renewing callback\n");
    return ECORE_CALLBACK_RENEW;
}

/**
 * sniffer window creation function, the keyboard would be grabbed by this window in case of Tizen Emulator
 */
void
CSCLKeyFocusHandler::create_sniffer_window(void)
{
    LOGD("CSCLKeyFocusHandler : INSIDE =-=-=-=- create_sniffer_window : \n");	
    Evas_Object *win = NULL;

    win = elm_win_add(NULL, "KEY_SNIFFER", ELM_WIN_UTILITY);

    elm_win_borderless_set(win, EINA_TRUE);
    elm_win_alpha_set(win, EINA_FALSE);
    elm_win_title_set(win, "KEY_SNIFFER");
    elm_win_fullscreen_set(win,EINA_FALSE);
    set_window_accepts_focus(win, FALSE);
    evas_object_show(win);
    evas_object_resize(win, 100, 100);
    m_sniffer = win;
    _sniffer_win_show_handler = ecore_event_handler_add (ECORE_X_EVENT_WINDOW_SHOW, x_event_sniffer_window_show_cb, m_sniffer);
}

void
CSCLKeyFocusHandler::set_window_accepts_focus(const sclwindow window, sclboolean acceptable)
{
    Eina_Bool accepts_focus;
    Ecore_X_Window_State_Hint initial_state;
    Ecore_X_Pixmap icon_pixmap;
    Ecore_X_Pixmap icon_mask;
    Ecore_X_Window icon_window;
    Ecore_X_Window window_group;
    Eina_Bool is_urgent;

    if (window) {
        ecore_x_icccm_hints_get(elm_win_xwindow_get(static_cast<Evas_Object*>(window)),
            &accepts_focus, &initial_state, &icon_pixmap, &icon_mask, &icon_window, &window_group, &is_urgent);
        ecore_x_icccm_hints_set(elm_win_xwindow_get(static_cast<Evas_Object*>(window)),
            acceptable, initial_state, icon_pixmap, icon_mask, icon_window, window_group, is_urgent);
    }
}


#endif


