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

#ifndef __SCL_UI_H__
#define __SCL_UI_H__

#include "scltypes.h"
#include "sclstructs.h"
#include "scleventcallback.h"

//SCL_BEGIN_DECLS

namespace scl
{

/* To use pimpl idiom */
class CSCLUIImpl;

/**
 * @brief The base class to work as a soft-based keyboard
 *
 * This class implements all functions for working as a soft-based keyboard
 * In side of ISE developer, they can modify it by their requirements.
 */
class CSCLUI
{
public:
    CSCLUI();
    ~CSCLUI();

    /*
     * @brief This API initializes SCL library and creates new SCL context
     * @param[in] main_window a pointer to main window object
     * @param[in] parser_type decides which parser should be selected for parsing SCL UI resource file
     * @param[in] entry_filepath a file path to SCL's UI resource entry file
     * @return non-zero value is returned when successful
     */
    sclboolean init(sclwindow main_window, SCLParserType parser_type, const sclchar *entry_filepath);

    /**
     * @brief This API requests SCL library to show the S/W keyboard on the screen
     * @param[in] auto_relocate Indicates whether the SCL library should relocate the keyboard window automatically
     *            When set to FALSE, the developer should move the window to its expected position.
     */
    //void show(sclboolean auto_relocate = TRUE);
    void show();

    /**
     * @brief This API requests SCL library to hide the S/W keyboard from the screen
     * @param[in] ctx target SCL context
     */
    void hide();


    /**
     * @brief This API registers a callback interface, to handle the events generated by SCL library
     * @param[in] callback a pointer to ISCLEventCallback interface implementation class
     * @param[in] input_mode when given not NULL, this callback processes event on that input_mode only.
     */
    void set_ui_event_callback(ISCLUIEventCallback *callback, const sclchar *input_mode = NULL);

    /**
     * @brief This API requests SCL library to rotate the window to given degree
     * @param[in] degree an SCLRotation value for rotating the keyboard
     * @return non-zero value is returned when successful
     */
    sclboolean set_rotation(SCLRotation degree);

    /**
     * @brief This API returns the current rotation degree
     * @return current rotation value
     */
    SCLRotation get_rotation();

    /**
     * @brief This API checks if the current display mode is portrait or landscape
     * @return current display mode
     */
    SCLDisplayMode get_display_mode();

    /**
     * @brief This API request SCL library to change to given input mode
     * @param[in] input_mode the name of the desired input mode
     * @return non-zero value is returned when successful
     */
    sclboolean set_input_mode(const sclchar *input_mode);

    /**
     * @brief This API retrieves the current input mode
     * @return a string pointer that indicates the name of current input mode
     */
    const sclchar* get_input_mode();

    /**
     * @brief This API request SCL library to suspend screen updates
     * @param[in] pend whether to suspend screen updates
     */
    void set_update_pending(sclboolean pend);

    /**
     * @brief This API request SCL library to change a specific key with user customized values, with different parameters
     * @param[in] custom_id the name of the key to be reset
     * @param[in] label the string to be displayed on the button
     * @param[in] label the file path to button's label image
     * @param[in] imagebg the file path to button's background image
     * @param[in] key_event a numeric value to be emitted when this button is clicked
     * @param[in] key_value a string value to be emitted when this button is clicked
     * @param[in] redraw whether to redraw the key right after updating detailed information
     * @return internal private key index
     */
    sclint set_private_key(const sclchar* custom_id, sclchar* label, sclchar* imagelabel[SCL_BUTTON_STATE_MAX], sclchar* imagebg[SCL_BUTTON_STATE_MAX], sclulong key_event, sclchar *key_value, sclboolean redraw);

    /**
     * @brief This API request SCL library to reset a key to its original configuration
     * @param[in] custom_id the name of the key to be reset
     */
    void unset_private_key(const sclchar* custom_id);

    /**
     * @brief This API request SCL library to enable a button with given ID
     * @param[in] custom_id the name of the key to be enabled
     * @param[in] enabled whether this button should be enabled or not
     */
    void enable_button(const sclchar* custom_id, sclboolean enabled);

    /**
     * @brief This API sets the name of the current theme to change the look and feel of S/W keyboard
     * @param[in] themename the name of the theme to be applied
     * @return non-zero value is returned when successful
     */
    sclboolean set_cur_themename(const sclchar *themename);

    /**
     * @brief This API sets the name of the desired predefined sub-layout, which changes only some part of current layout
     * @param[in] themename the name of the theme to be applied
     * @return non-zero value is returned when successful
     */
    sclboolean set_cur_sublayout(const sclchar *sub_layout_name);

    /**
     * @brief This API retrieves the name of current selected sub-layout
     * @return the string pointer that indicates the name of current sub-layout
     */
    const sclchar* get_cur_sublayout();

    /**
     * @brief This API sets the current shift state
     * @param[in] state the SCLShiftState value for desired shift state
     */
    void set_shift_state(SCLShiftState state);

    /**
     * @brief This API retrieves the current shift state
     * @return current shift state
     */
    SCLShiftState get_shift_state();

    /**
     * @brief This API notifies the SCL library that the application's focus was moved to another input context
     */
    void notify_app_focus_changed();

    /**
     * @brief This API request SCL library to reset the timer, which closes all popup windows when expired
     */
    void reset_popup_timeout();

    /**
     * @brief This API request SCL library to close all opened popup windows
     */
    void close_all_popups();

    /*
     * @brief This API acquires the size of main window
     * @param[out] rect the geometry information of main window
     */
    SclRectangle get_main_window_rect();

    /**
     * @brief This API retrieves the current input mode
     * @param[in] input_mode the name of the desired input mode
     * @param[in] display_mode desired display mode value
     * @return a size struct contains the input_mode's size in given display_mode
     */
    SclSize get_input_mode_size(const sclchar *input_mode, SCLDisplayMode display_mode);

    /**
     * @brief This API acquires the screen size of current device
     * @param[out] width the x-axis size of screen
     * @param[out] height the y-axis size of screem
     */
    void get_screen_resolution(sclint *width, sclint *height);

    /**
     * @brief This API request SCL library to scale the input value,
              by calculating the rate of target screen size and current screen size
     * @param[in] x the value that needs to be scaled
     * @return scaled value
     */
    scl16 get_scaled_x(scl16 x);

    /**
     * @brief This API request SCL library to scale the input value,
              by calculating the rate of target screen size and current screen size
     * @param[in] y the value that needs to be scaled
     * @return scaled value
     */
    scl16 get_scaled_y(scl16 y);

    /**
     * @brief This API returns the smaller scale rate, between x and y
     * @return scale rate
     */
    sclfloat get_scale_rate();

    /**
     * @brief This API retrieves the current custom scale rate x, for resizing the keyboard's content
     * @return current custom x scale value
     */
    sclfloat get_custom_scale_rate_x();

    /**
     * @brief This API retrieves the current custom scale rate y, for resizing the keyboard's content
     * @return current custom y scale value
     */
    sclfloat get_custom_scale_rate_y();

    /**
     * @brief This API sets the current custom scale rate values, for resizing the keyboard's content
     * @param[in] x the scale rate value
     */
    void set_custom_scale_rate(sclfloat x, sclfloat y);

    /**
     * @brief This API returns the number of current multi touch contexts,
     *        which means how many fingers are currently pressing the touch device
     * @return number of touch contexts
     */
    sclint get_multi_touch_context_num();

    /**
     * @brief This API retrieves the descriptor for the nth multi touch context, for processing multi touch events
     * @param[in] seqorder the sequence order of currently requesting multi touch context
     * @param[out] desc a pointer to SclKeyEventDesc struct, to be filled with given multi touch context's detailed information
     * @return non-zero value is returned when successful
     */
    sclboolean get_multi_touch_event(sclint seqorder, SclUIEventDesc *desc);

    /**
     * @brief This API requests SCL library to start working in debug mode
     */
    void set_debug_mode(SCLDebugMode mode);

    /**
     * @brief This API retrieves the current debug mode of SCL library
     */
    SCLDebugMode get_debug_mode();

    /**
     * @brief This API sets the timeout value for recognizing long press event
     * @param[in] msc the time before the system waits until it determines the press event is long press event
     * @return non-zero value is returned when successful
     */
    sclboolean set_longkey_duration(scllong msc);

    /**
     * @brief This API sets the threshold value for recognizing long press event
     * @param[in] dist maximum distance, in pixels.
                  the move event should not go beyond this distance from its initial pressed ponit
     * @return non-zero value is returned when successful
     */
    sclboolean set_longkey_cancel_dist(sclshort dist);

    /**
     * @brief This API sets the timeout value for emitting repeat key event
     * @param[in] msc the time that SCL library has to wait before emitting next repeat key even
     * @return non-zero value is returned when successful
     */
    sclboolean set_repeatkey_duration(scllong msc);

    /**
     * @brief This API sets the timeout value for recognizing long press event, if current button supports hidden text popup
     * @param[in] msc the time before the system waits until it determines the press event is long press event
     * @return non-zero value is returned when successful
     */
    sclboolean set_autopoup_key_duration(scllong msc);

    /**
     * @brief This API sets the timeout value before it reverts the ui feedback of button press event
     * @param[in] msc the time before the system waits until it resets the pressed key to its normal state
     * @return non-zero value is returned when successful
     */
    sclboolean set_button_delay_duration(scllong msc);

    /**
     * @brief This API requests SCL library to enable/disable magnifier window
     * @param[in] enabled indicates whether mangifier window should be displayed or not
     */
    void enable_magnifier(sclboolean enabled);

    /**
     * @brief This API requests SCL library to enable/disable sound feedback
     * @param[in] enabled indicates whether sound feedback should be played or not, when a key is pressed
     */
    void enable_sound(sclboolean enabled);

    /**
     * @brief This API requests SCL library to enable/disable vibration feedback
     * @param[in] enabled indicates whether vibration feedback should be played or not, when a key is pressed
     */
    void enable_vibration(sclboolean enabled);

    /**
     * @brief This API requests SCL library to enable/disable tts feedback
     * @param[in] enabled indicates whether tts feedback should be played or not, when a key is pressed
     */
    void enable_tts(sclboolean enabled);

    /**
     * @brief This API requests SCL library to enable/disable shift multi touch action
     * @param[in] enabled indicates whether shift button should provide multi touch action
     */
    void enable_shift_multi_touch(sclboolean enabled);

    /**
     * @brief This API requests SCL library to apply the touch offset adjustment
     * @param[in] enabled indicates whether the touch offset adjustment should applied or not
     */
    void enable_touch_offset(sclboolean enabled);

    /**
     * @brief This API requests SCL library to ignore any events raised from the keyboard window
     * @param[in] enabled indicates whether the keyboard window should ignore any touch events it receives
     */
    void disable_input_events(sclboolean disabled);

    /**
     * @brief This API requests SCL library to change the content of magnifier window that will be displayed
     * @param[in] touch_id indicates which multi touch context this customization should be applied to
     * @param[in] index indicates the index of magnifier window's label
     * @param[in] label the string to be displayed on the magnfier window
     */
    void set_custom_magnifier_label(scltouchdevice touch_id, sclint index, const sclchar* label);

    /**
     * @brief This API requests SCL library to substitute a string with given one when displaying it
     * @param[in] original indicates the string that should be substituted from
     * @param[in] substitute indicates the string that should be substituted to
     */
    void set_string_substitution(const sclchar *original, const sclchar *substitute);

    /**
     * @brief This API requests SCL library to reset substitution for the given string
     * @param[in] original indicates the string that was registered to be substituted from
     */
    void unset_string_substitution(const sclchar *original);

    /**
     * @brief This API sets the autocapital shift state flag
     * @param[in] flag: TRUE indicates the libscl-ui will auto handle the shift state
     *                  FALSE not, then the users will handle this outside the libscl-ui
     * default is TRUE
     */
    void set_autocapital_shift_state(sclboolean flag);

    /**
     * @brief This API gets the autocapital shift state flag
     */
    sclint get_autocapital_shift_state();

private:
    CSCLUIImpl *m_impl;
};

}

//SCL_END_DECLS

#endif //__SCL_CORE_H__
