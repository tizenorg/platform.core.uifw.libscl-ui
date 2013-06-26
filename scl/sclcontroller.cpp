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

#include <time.h>
#include <math.h>
#include <assert.h>
#include <vector>

#include "sclcontroller.h"
#include "scldebug.h"
#include "sclresourcecache.h"
#include "sclactionstate.h"
#include "scluibuilder.h"
#include "sclkeydefines.h"
#include "sclfeedback.h"
#include "sclerroradjustment.h"
#include "sclimageproxy.h"
#include "sclres_manager.h"
#include "scleventhandler.h"

//#define DIRECTLY_DRAW_ON_EVENTS

using namespace scl;

static sclboolean
_play_tts_for_input_mode_name(int mode) {
    SCL_DEBUG();

    CSCLContext *context = CSCLContext::get_instance();
    if (context->get_tts_enabled() == FALSE) {
        return FALSE;
    }

    SclResParserManager *sclres_manager = SclResParserManager::get_instance();
    const SclInputModeConfigure *pinput_mode_table = sclres_manager->get_input_mode_configure_table();
    if (NULL == pinput_mode_table) {
        return FALSE;
    }

    const char* name = pinput_mode_table[mode].name;
    if (NULL == name) {
        return FALSE;
    }

    CSCLUtils *utils = CSCLUtils::get_instance();
    utils->play_tts(name);
    return TRUE;
}

static sclboolean
_play_tts_for_layout_autopopup_name() {
    SCL_DEBUG();

    CSCLContext *context = CSCLContext::get_instance();
    if (context->get_tts_enabled() == FALSE) {
        return FALSE;
    }

    CSCLUtils *utils = CSCLUtils::get_instance();
    utils->play_tts(SCL_LAYOUT_AUTOPOPUP_NAME);
    return TRUE;
}

CSCLController* CSCLController::m_instance = NULL; /* For singleton */


CSCLController::CSCLController()
{
    SCL_DEBUG();

    m_long_key_duration = SCL_LONGKEY_DURATION;
    m_long_key_cancel_distance = SCL_LONGKEY_CANCEL_DIST;
    m_repeat_key_duration = SCL_REPEATKEY_DURATION;
    m_autopopup_key_duration = SCL_AUTOPOPUP_KEY_DURATION;

    m_button_delay_duration = SCL_BUTTON_MIN_DURATION;

    m_key_repeated_num = 0;

    m_debug_mode = DEBUGMODE_DISABLED;
    m_debug_variable = 0;

    m_input_events_disabled = FALSE;
}

CSCLController::~CSCLController()
{
    SCL_DEBUG();
}

CSCLController*
CSCLController::get_instance()
{
    if (!m_instance) {
        m_instance = new CSCLController();
    }
    return (CSCLController*)m_instance;
}

void
CSCLController::init()
{
    SCL_DEBUG();
}

//#define TEST_NEWBACKEND
#ifdef TEST_NEWBACKEND
#include <Ecore_Evas.h>
#include <Ecore.h>
#include <vector>
typedef enum {
    EFLOBJECT_NONE,
    EFLOBJECT_IMAGE,
    EFLOBJECT_CLIPOBJECT,
    EFLOBJECT_TEXTBLOCK,
} EFLOBJECT_TYPE;

    typedef struct {
        EFLOBJECT_TYPE type;
        SclRectangle position;
        Evas_Object *object;
        char *etc_info;
        sclboolean extracted;
        void *data;
    } EFLObject;
#include <Ecore_Evas.h>
#include <Ecore.h>
    typedef struct {
        sclboolean used;

        Evas_Object *image;
        Evas_Object *clipper;

        sclwindow window;
        sclchar image_path[_POSIX_PATH_MAX];
        sclint imgPathHash;
        sclint dest_x;
        sclint dest_y;
        sclint dest_width;
        sclint dest_height;
        sclint src_x;
        sclint src_y;
        sclint src_width;
        sclint src_height;
        sclboolean extrace_image;
    } ImageCache;

    typedef struct {
        sclboolean used;

        Evas_Object *text;

        sclwindow window;
        scl::SclFontInfo font_info;
        SclColor color;
        sclchar str[_POSIX_PATH_MAX];;
        sclint strHash;
        sclint pos_x;
        sclint pos_y;
        sclint width;
        sclint height;
        SCLLabelAlignment align;
        sclshort padding_x;
        sclshort padding_y;
        sclbyte inner_width;
        sclbyte inner_height;
    } TextCache;

extern std::vector<ImageCache> g_ImageCache;
extern std::vector<TextCache> g_TextCache;
#else
#endif

/**
 * Sets the current input mode to the given mode
 */
sclboolean
CSCLController::process_input_mode_change(const sclbyte mode)
{
    SCL_DEBUG();

    sclboolean ret = FALSE;

    CSCLUtils *utils = CSCLUtils::get_instance();
    CSCLContext *context = CSCLContext::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLResourceCache *cache = CSCLResourceCache::get_instance();
    SclResParserManager *sclres_manager = SclResParserManager::get_instance();
    PSclInputModeConfigure sclres_input_mode_configure = sclres_manager->get_input_mode_configure_table();
    assert(sclres_input_mode_configure != NULL);

    if (context && windows && cache && utils) {
        if (mode == context->get_input_mode() || mode == (sclbyte)NOT_USED) {
            /* BtnContext does not get initialized if we don't call here */
            cache->recompute_layout(windows->get_base_window());
            return FALSE;
        }

        context->set_input_mode(mode);
        /* FIXME : NEWXML temporary commenting out */
        //context->set_base_layout(sclres_input_mode_configure[mode].layouts[context->get_display()]);

        _play_tts_for_input_mode_name(mode);

        sclwindow window = windows->get_base_window();
        handle_engine_signal(SCL_SIG_INPMODE_CHANGE, window);

#ifdef TEST_NEWBACKEND
        SclWindowContext *winctx = windows->get_window_context(window, FALSE);
        if (winctx) {
            if (winctx->etc_info) {

                Eina_List *list = (Eina_List*)(winctx->etc_info);
                Eina_List *iter = NULL;
                Eina_List *iter_next = NULL;
                void *data = NULL;
                int iIndex = 0;

                EINA_LIST_FOREACH_SAFE(list, iter, iter_next, data) {
                    if (data) {
                        EFLObject *object = (EFLObject*)(data);
                        if (object) {
                            Evas_Object* eo = object->object;
                            if (object->extracted) {
                                void *data = evas_object_image_data_get(eo, 1);
                                if (data) {
                                    free(data);
                                }
                            }

                            sclint loop;
                            for(loop = 0;loop < g_ImageCache.size();loop++) {
                                if (g_ImageCache[loop].image == object->object) {
                                    g_ImageCache[loop].used = FALSE;
                                }
                            }
                            for(loop = 0;loop < g_TextCache.size();loop++) {
                                if (g_TextCache[loop].text == object->object) {
                                    g_TextCache[loop].used = FALSE;
                                }
                            }

                            if (eo) {
                                evas_object_del(eo);
                                object->object = NULL;
                            }
                            delete object;
                        }
                        list = eina_list_remove_list(list, iter);
                    }
                    iIndex++;
                }
                winctx->etc_info= list;
            }
        }
#endif
    }

    return ret;
}

/**
 * Sets the current display mode to the given mode
 */
sclboolean
CSCLController::process_rotation_change(const SCLRotation rotation)
{
    SCL_DEBUG();
    CSCLContext *context = CSCLContext::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();
    SclResParserManager *sclres_manager = SclResParserManager::get_instance();
    PSclInputModeConfigure sclres_input_mode_configure = sclres_manager->get_input_mode_configure_table();
    assert(sclres_input_mode_configure != NULL);

    if (context && windows) {
        SCLDisplayMode mode;
        if (rotation == ROTATION_90_CW || rotation == ROTATION_90_CCW) {
            mode = DISPLAYMODE_LANDSCAPE;
        } else {
            mode = DISPLAYMODE_PORTRAIT;
        }

        //if (mode == context->get_display()) return FALSE;
        context->set_display_mode(mode);
        context->set_rotation(rotation);
        /* FIXME : NEWXML temporary commenting out */
        //context->set_base_layout(sclres_input_mode_configure[context->get_input_mode()].layouts[context->get_display()]);

        sclwindow window = windows->get_base_window();
        handle_engine_signal(SCL_SIG_DISP_CHANGE, window);
        windows->update_window(window);

        /* Moved to here since the new WMSync requires the rotation call be invoked as the
           last step of display change process */
        /* Make sure to set window's rotation degree before sending engine signal, which adjusts the size of main window */
        windows->set_window_rotation(NULL, rotation);
    }
    return TRUE;
}

/**
 * Checks if the given button with given touch_id needs magnifier window
 */
sclboolean
CSCLController::check_magnifier_available(sclwindow window, sclbyte key_index, scltouchdevice touch_id)
{
    sclboolean ret = FALSE;

    CSCLContext *context = CSCLContext::get_instance();
    CSCLResourceCache *cache = CSCLResourceCache::get_instance();
    const SclLayout *layout = NULL;
    SclButtonContext *btncontext = NULL;
    const SclLayoutKeyCoordinate *coordinate = NULL;

    if (context && cache) {
        layout = cache->get_cur_layout(window);
        btncontext = cache->get_cur_button_context(window, key_index);
        coordinate = cache->get_cur_layout_key_coordinate(window, key_index);

        SCLShiftState shiftidx = context->get_shift_state();
        if (shiftidx < 0 || shiftidx >= SCL_SHIFT_STATE_MAX) shiftidx = SCL_SHIFT_STATE_OFF;

        if (layout && coordinate && btncontext && context->get_magnifier_enabled() ) {
            if (coordinate->key_type != KEY_TYPE_CONTROL &&
                coordinate->key_type != KEY_TYPE_MODECHANGE &&
                coordinate->key_type != KEY_TYPE_NONE) {

                    ret = TRUE;

                    /* Do not show if current layout does not allow magnifier */
                    if (!(layout->use_magnifier_window)) {
                        //utils->log("show_magnifier !(layout->use_magnifier_window \n");
                        ret = FALSE;
                    }

                    /* Do not show if there's nothing to show */
                    const sclchar* custom_label = NULL;
                    for(sclint label_index = 0;label_index < MAX_SIZE_OF_LABEL_FOR_ONE && !custom_label;label_index++) {
                        const sclchar *temp_label = context->get_custom_magnifier_label(touch_id, label_index);
                        if (temp_label) {
                            custom_label = temp_label;
                        }
                    }
                    if (!custom_label) {
                        //if (coordinate->key_value[shiftidx][btncontext->multikeyIdx] == NULL) {
                        if (coordinate->label[shiftidx][btncontext->multikeyIdx] == NULL) {
                            //utils->log("show_magnifier coordinate->key_value[shift][btncontext->multikeyIdx] == NULL \n");
                            ret = FALSE;
                            //} else if (strlen(coordinate->key_value[shiftidx][btncontext->multikeyIdx]) == 0) {
                        } else if (strlen(coordinate->label[shiftidx][btncontext->multikeyIdx]) == 0) {
                            //utils->log("show_magnifier coordinate->key_value[shift][btncontext->multikeyIdx]) == 0 \n");
                            ret = FALSE;
                        }
                    }

                    if (touch_id != context->get_last_touch_device_id()) {
                        ret = FALSE;
                    }
                }
        }
    }

    return ret;
}

sclboolean
CSCLController::process_button_pressed_event(sclwindow window, sclint x, sclint y, sclbyte key_index,
                                             scltouchdevice touch_id, sclboolean actual_event)
{
    SCL_DEBUG();

    sclboolean ret = FALSE;
    sclboolean redraw = FALSE;

    CSCLContext *context = CSCLContext::get_instance();
    CSCLResourceCache *cache = CSCLResourceCache::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLEvents *events = CSCLEvents::get_instance();
    CSCLUtils *utils = CSCLUtils::get_instance();
    CSCLFeedback *feedback = CSCLFeedback::get_instance();
    CSCLEventHandler *handler = CSCLEventHandler::get_instance();
    SclResParserManager *sclres_manager = SclResParserManager::get_instance();
    PSclInputModeConfigure sclres_input_mode_configure = sclres_manager->get_input_mode_configure_table();
    PSclLayout sclres_layout = sclres_manager->get_layout_table();
    assert(sclres_input_mode_configure != NULL);
    assert(sclres_layout != NULL);

    SclButtonContext *btncontext = NULL;
    const SclLayoutKeyCoordinate *coordinate = NULL;

    if (context && cache) {
        btncontext = cache->get_cur_button_context(window, key_index);
        coordinate = cache->get_cur_layout_key_coordinate(window, key_index);
    }

    static sclwindow prevwin = SCLWINDOW_INVALID;
    static sclbyte prevkey = NOT_USED;

    if (context && cache && windows && events && utils && feedback && handler && btncontext && coordinate) {
        /* First check if this button is enabled in current active sublayout */
        sclboolean sub_layout_match = TRUE;
        if (coordinate->sub_layout && context->get_cur_sublayout()) {
            if (strncmp(coordinate->sub_layout, context->get_cur_sublayout(), MAX_SIZE_OF_SUBLAYOUT_STRING) != 0) {
                sub_layout_match = FALSE;
            }
        }
        /* If this button is pressed */
        if ( x >= coordinate->x - coordinate->add_hit_left &&
                x < coordinate->x + coordinate->width + coordinate->add_hit_right &&
                y >= coordinate->y - coordinate->add_hit_top &&
                y < coordinate->y + coordinate->height + coordinate->add_hit_bottom &&
                /* Process the event only if the this item's sublayout id is active one */
                sub_layout_match ) {
            //utils->log("process_button_pressed_event___TRUE\n");

            /* If newly pressed key has type MULTI_TOUCH_TYPE_EXCLUSIVE, release all existing pressed events */
            if (actual_event) {
                if (coordinate->multitouch_type == SCL_MULTI_TOUCH_TYPE_EXCLUSIVE) {
                    /* When calling mouse_release, the seq order of current multitouch events will be changed,
                    so we put all the multitouch events into a vector and use them afterwards forreleasing */
                    sclint loop = 0;
                    sclint multi_touch_context_num = context->get_multi_touch_context_num();
                    std::vector<SclUIEventDesc> multi_touch_events;
                    for(loop = 0;loop < multi_touch_context_num;loop++) {
                        SclUIEventDesc desc;
                        context->get_multi_touch_event(loop, &desc);
                        multi_touch_events.push_back(desc);
                    }
                    for(loop = 0;loop < multi_touch_context_num;loop++) {
                        SclUIEventDesc desc = multi_touch_events[loop];
                        if (desc.touch_id != touch_id) {
                            mouse_release(context->get_cur_move_window(desc.touch_id),
                                context->get_cur_move_point(desc.touch_id).x, context->get_cur_move_point(desc.touch_id).y,
                                desc.touch_id, FALSE);
                        }
                    }
                }
            }

            /* Make an unique ID for timer */
            const scl16 uniqId = utils->get_unique_id();

            context->set_cur_pressed_event_id(touch_id, uniqId);
            context->set_cur_pressed_key(touch_id, key_index);
            context->set_cur_pressed_window(touch_id, window);

            btncontext->state = BUTTON_STATE_PRESSED;

            redraw = TRUE;
            ret = TRUE;

#ifndef DIRECTLY_DRAW_ON_EVENTS
            /* If the window doesn't get exposed before corresponding release event,
             * the inverted state of a button will never be drawn onto screen.
             * To prevent such a case, we draw the inverted state of button forcefully and directly,
             * without waiting for expose event */
            /*CSCLGraphics *grps = CSCLGraphics::get_instance();
            CSCLUIBuilder *builder = CSCLUIBuilder::get_instance();
            scldrawctx draw_ctx = grps->begin_paint(window, TRUE);
            builder->draw_button(window, draw_ctx, key_index, btncontext->state, TRUE);
            grps->end_paint(window, draw_ctx);*/
#endif

            /* for feedback */
            feedback->button_pressed(window, key_index);

            /* Special routine for autopopup */
            if (coordinate->popup_type == POPUP_TYPE_AUTO_POPUP) {
                events->create_timer(SCL_TIMER_AUTOPOPUP, m_autopopup_key_duration, uniqId);
            } else {
                /* for long key & repeat key */
                events->create_timer(SCL_TIMER_LONGKEY, m_long_key_duration, uniqId);
            }

            SCLShiftState shiftidx = context->get_shift_state();
            if (shiftidx < 0 || shiftidx >= SCL_SHIFT_STATE_MAX) shiftidx = SCL_SHIFT_STATE_OFF;

            SclUIEventDesc key_event_desc = {0};
            key_event_desc.key_value = coordinate->key_value[shiftidx][0];
            key_event_desc.key_event = coordinate->key_event[shiftidx][0];
            key_event_desc.key_type = coordinate->key_type;
            key_event_desc.key_modifier = KEY_MODIFIER_NONE;
            key_event_desc.event_type = EVENT_TYPE_PRESS;

            SclPoint curpoint = {x, y};
            key_event_desc.touch_id = touch_id;
            key_event_desc.mouse_pressed_point = curpoint;
            key_event_desc.mouse_current_point = curpoint;
            key_event_desc.mouse_farthest_point = curpoint;

            key_event_desc.touch_event_order = context->get_multi_touch_event_order(touch_id);

            handler->on_event_drag_state_changed(key_event_desc);

            /* Now process normal behaviours of each button type */
            switch (coordinate->button_type) {
            case BUTTON_TYPE_NORMAL:
            case BUTTON_TYPE_GRAB:
            case BUTTON_TYPE_SELFISH:
            case BUTTON_TYPE_DIRECTION:
            case BUTTON_TYPE_RELATIVE_DIRECTION: {
                /* Send click event right away if this button uses repeat key */
                if (coordinate->use_repeat_key) {
                    handler->on_event_key_clicked(key_event_desc);
                }
            }
            break;
            case BUTTON_TYPE_MULTITAP: {
            }
            break;
            case BUTTON_TYPE_ROTATION: {
            }
            break;
            case BUTTON_TYPE_DRAG: {
                /* Drag buttons fires click event immediately when they are pressed */
                handler->on_event_key_clicked(key_event_desc);
            }
            break;
            }


            switch (coordinate->popup_type) {
            case POPUP_TYPE_BTN_PRESS_POPUP_DRAG: {
                sclint popup_input_mode = sclres_manager->get_inputmode_id(coordinate->popup_input_mode[SCL_DRAG_STATE_NONE]);
                SCLDisplayMode display_mode = context->get_display_mode();
                /* FIXME */
                //if (scl_check_arrindex(popup_input_mode, MAX_INPUT_MODE_POPUP) &&
                if (scl_check_arrindex(popup_input_mode, MAX_SCL_INPUT_MODE) &&
                    scl_check_arrindex(display_mode, DISPLAYMODE_MAX)) {
                    sclshort popupLayoutId =
                        sclres_manager->get_layout_id(sclres_input_mode_configure[popup_input_mode].layouts[display_mode]);
                    SclRectangle popupRect;
                    SclRectangle baseWndRect;
                    SclLayout *layout = NULL;
                    /* FIXME */
                    //if (scl_check_arrindex(popupLayoutId, MAX_LAYOUT)) {
                    if (scl_check_arrindex(popupLayoutId, MAX_SCL_LAYOUT)) {
                        layout = &sclres_layout[popupLayoutId];
                    }
                    if (layout) {
                        windows->get_window_rect(windows->get_base_window(), &baseWndRect);
                        popupRect.x = coordinate->x + coordinate->popup_relative_x + baseWndRect.x;
                        popupRect.y = coordinate->y + coordinate->popup_relative_y + baseWndRect.y;
                        //popupRect.width = utils->get_scale_x(layout->width);
                        //popupRect.height= utils->get_scale_y(layout->height);
                        popupRect.width = layout->width;
                        popupRect.height= layout->height;
                        windows->close_all_popups();
                        SclWindowOpener opener;
                        opener.window = window;
                        opener.key = key_index;
                        windows->open_popup(opener,
                                            popupRect,
                                            popup_input_mode,
                                            popupLayoutId,
                                            coordinate->popup_type,
                                            sclres_input_mode_configure[popup_input_mode].use_virtual_window,
                                            sclres_input_mode_configure[popup_input_mode].use_dim_window,
                                            coordinate->extract_offset_x,
                                            coordinate->extract_offset_y,
                                            sclres_input_mode_configure[popup_input_mode].timeout
                                           );
                        windows->hide_window(windows->get_magnifier_window());
                        /* FIXME : The parent key should be turned back to NORMAL state when RELEASED,
                            in case of POPUP_TYPE_BTN_PRESS_POPUP_DRAG type. Temporariliy setting NORMAL here. */
                        btncontext->state = BUTTON_STATE_NORMAL;
                        _play_tts_for_input_mode_name(popup_input_mode);
                    }
                }
            }
            break;
            case POPUP_TYPE_BTN_RELEASE_POPUP:
            case POPUP_TYPE_BTN_RELEASE_POPUP_ONCE:
            case POPUP_TYPE_BTN_LONGPRESS_POPUP:
            case POPUP_TYPE_BTN_LONGPRESS_POPUP_ONCE:
            case POPUP_TYPE_AUTO_POPUP:
            case POPUP_TYPE_NONE:
                /* Nothing to do in here */
                break;
            }

            /* Shows the magnifier window(the magnifier window will display when a kind of button type is character) */
            //if (windows->get_nth_window_in_Z_order_list(SCL_WINDOW_Z_TOP) == windows->get_base_window()) {
            if (coordinate->use_magnifier) {
                sclboolean showMagnifier = check_magnifier_available(window, key_index, touch_id);

                SclResParserManager *sclres_manager = SclResParserManager::get_instance();
                PSclMagnifierWndConfigure magnifier_configure = NULL;
                if (sclres_manager) {
                    magnifier_configure = sclres_manager->get_magnifier_configure();
                }
                if (showMagnifier && magnifier_configure) {
                    SclPoint pos = {0,};
                    /* calculates x position to be set */
                    pos.x = (coordinate->x + (coordinate->width / 2)) - (magnifier_configure->width / 2);

                    /* calculates y position to be set */
                    sclint scnWidth, scnHeight;
                    utils->get_screen_resolution(&scnWidth, &scnHeight);

                    pos.y = coordinate->y - magnifier_configure->height;

                    /* FIXME : Temporary way of clearing magnifier window */
                    /*SclWindowContext *winctx = windows->get_window_context(windows->get_magnifier_window(), FALSE);
                    sclboolean clearmagwin = FALSE;
                    if (winctx) {
                        clearmagwin = !(winctx->hidden);
                    }
                    static int clearnum = 0;
                    if (key_index == prevkey && window == prevwin) {
                        clearmagwin = FALSE;
                    }
                    if (clearmagwin) {
                        if (++clearnum > 1) {
                            clearmagwin = FALSE;
                        }
                    } else {
                        clearnum = 0;
                    }

                    if (clearmagwin) {
                        CSCLGraphics *graphics = CSCLGraphics::get_instance();
                        CSCLUtils *utils = CSCLUtils::get_instance();
                        sclchar composed_path[_POSIX_PATH_MAX] = {0,};
                        scldrawctx draw_ctx = graphics->begin_paint(windows->get_magnifier_window());
                        utils->get_composed_path(composed_path, scl_magnifier_configure.bg_image_path);
                        graphics->draw_image(windows->get_magnifier_window(), draw_ctx, composed_path, 0, 0);
                        graphics->end_paint(windows->get_magnifier_window(), draw_ctx);
                    }
                    */
                    windows->hide_window(windows->get_magnifier_window());

                    SclWindowContext *winctx = windows->get_window_context(window);
                    if (winctx) {
                        pos.x += winctx->geometry.x;
                        pos.y += winctx->geometry.y;
                    }

                    if (pos.x < 0 - magnifier_configure->padding_x) {
                        pos.x = 0 - magnifier_configure->padding_x;
                    }
                    if (pos.x > scnWidth + magnifier_configure->padding_x - magnifier_configure->width) {
                        pos.x = scnWidth + magnifier_configure->padding_x - magnifier_configure->width;
                    }
                    pos.y += magnifier_configure->padding_y;
                    pos.x += coordinate->magnifier_offset_x;
                    pos.y += coordinate->magnifier_offset_y;
                    windows->move_window(windows->get_magnifier_window(), pos.x, pos.y);
                    //windows->resize_window(windows->get_magnifier_window(), utils->get_scale_x(scl_magnifier_configure.width), utils->get_scale_y(scl_magnifier_configure.height));
                    /*If we use transient_for them the ISE will occure some crash. It needs to check X11 */
                    /*windows->set_parent(windows->get_base_window(), windows->get_magnifier_window());*/

                    windows->show_window(windows->get_magnifier_window(), TRUE);
                    //windows->update_window(windows->get_magnifier_window());
                }
#if 0
                static int fFirst = true;
                if (fFirst) {
                    windows->show_window(windows->get_magnifier_window());
                    fFirst = false;
                } else {
                    windows->update_window(windows->get_magnifier_window());
                }
#endif
                /* We cannot use move_resize_window. It had occured some wrong behavior */
                /*windows->move_resize_window(windows->get_magnifier_window(), pos.x, pos.y, scl_magnifier_configure.width, scl_magnifier_configure.height);*/
                if (!showMagnifier) {
                    windows->hide_window(windows->get_magnifier_window());
                }
            }

            prevwin = window;
            prevkey = key_index;
        } else {
            /* COMMENTED OUT FOR TESTING MULTITOUCH!! */
            ///* In case the current button is not the given key index */
            //if (btncontext->state == BUTTON_STATE_PRESSED) {
            //    /* Even if the press event occured outside of this button's physical area, reset its context */
            //    btncontext->state = BUTTON_STATE_NORMAL;
            //    redraw = TRUE;
            //}
            /* BUTTON_TYPE_MULTITAP type button should restore its multikey index when another button is clicked */
            if (coordinate->button_type & BUTTON_TYPE_MULTITAP) {
                btncontext->multikeyIdx = 0;
            }
        }

        /* If there is any need for redrawing */
        if (redraw) {
#ifdef DIRECTLY_DRAW_ON_EVENTS
            CSCLUIBuilder *builder = CSCLUIBuilder::get_instance();
            if (builder) {
                builder->draw_button(window, NULL, key_index, btncontext->state, TRUE);
            }
#else
            CSCLWindows *windows = CSCLWindows::get_instance();
            if (windows) {
                windows->update_window(window, coordinate->x, coordinate->y, coordinate->width, coordinate->height);
            }
#endif
        }
    }

    return ret;
}

sclboolean
CSCLController::process_button_long_pressed_event(sclwindow window, sclbyte key_index,
                                                  scltouchdevice touch_id, sclboolean actual_event)
{
    SCL_DEBUG();

    sclboolean ret = FALSE;

    CSCLContext *context = CSCLContext::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLActionState *state = CSCLActionState::get_instance();
    CSCLResourceCache *cache = CSCLResourceCache::get_instance();
    CSCLEventHandler *handler = CSCLEventHandler::get_instance();
    SclResParserManager *sclres_manager = SclResParserManager::get_instance();
    PSclInputModeConfigure sclres_input_mode_configure = sclres_manager->get_input_mode_configure_table();
    PSclLayout sclres_layout = sclres_manager->get_layout_table();

    assert(sclres_input_mode_configure != NULL);
    assert(sclres_layout != NULL);
    if (context && cache && handler && windows && state) {
        const SclLayoutKeyCoordinate* coordinate = cache->get_cur_layout_key_coordinate(window, key_index);

        /* Should return FALSE if this key does not have any longkey related property */
        if (coordinate) {
            if (actual_event) {
                if (coordinate->multitouch_type == SCL_MULTI_TOUCH_TYPE_SETTLE_PREVIOUS) {
                    /* When calling mouse_release, the seq order of current multitouch events will be ch anged,
                       so we put all the multitouch events into a vector and use them afterwards for rel easing */
                    sclboolean finished = FALSE;
                    sclint loop = 0;
                    sclint multi_touch_context_num = context->get_multi_touch_context_num();
                    std::vector<SclUIEventDesc> multitouch_events;
                    for (loop = 0;loop < multi_touch_context_num;loop++) {
                        SclUIEventDesc desc;
                        context->get_multi_touch_event(loop, &desc);
                        multitouch_events.push_back(desc);
                    }
                    for (loop = 0;loop < multi_touch_context_num && !finished;loop++) {
                        SclUIEventDesc desc = multitouch_events[loop];
                        if (desc.touch_id != touch_id) {
                            sclwindow cur_pressed_window = context->get_cur_pressed_window(desc.touch_id);
                            scl8 cur_pressed_key = context->get_cur_pressed_key(desc.touch_id);
                            const SclLayoutKeyCoordinate *cur_pressed_coordinate =
                                cache->get_cur_layout_key_coordinate(cur_pressed_window, cur_pressed_key);
                            if (cur_pressed_coordinate) {
                                if (cur_pressed_coordinate->multitouch_type == SCL_MULTI_TOUCH_TYPE_SETTLE_PREVIOUS) {
                                    mouse_release(context->get_cur_move_window(desc.touch_id),
                                        context->get_cur_move_point(desc.touch_id).x,
                                        context->get_cur_move_point(desc.touch_id).y,
                                        desc.touch_id, FALSE);
                                }
                            }
                        } else {
                            finished = TRUE;
                        }
                    }
                }
            }
        }

        /* Should return FALSE if this key does not have any longkey related property */
        if (coordinate) {
            if (coordinate->popup_type == POPUP_TYPE_BTN_LONGPRESS_POPUP ||
                coordinate->popup_type == POPUP_TYPE_BTN_LONGPRESS_POPUP_ONCE ) {
                    SclRectangle popupRect;
                    SclRectangle baseWndRect;
                    windows->get_window_rect(windows->get_base_window(), &baseWndRect);
                    popupRect.x = coordinate->x + coordinate->popup_relative_x + baseWndRect.x;
                    popupRect.y = coordinate->y + coordinate->popup_relative_y + baseWndRect.y;
                    sclint popup_input_mode = sclres_manager->get_inputmode_id(coordinate->popup_input_mode[SCL_DRAG_STATE_NONE]);
                    SCLDisplayMode display_mode = context->get_display_mode();
                    /* FIXME */
                    //if (scl_check_arrindex(popup_input_mode, MAX_INPUT_MODE_POPUP) &&
                    if (scl_check_arrindex(popup_input_mode, MAX_SCL_INPUT_MODE) &&
                        scl_check_arrindex(display_mode, DISPLAYMODE_MAX)) {
                        SclLayout *layout = NULL;
                        sclshort popupLayoutId =
                            sclres_manager->get_layout_id(sclres_input_mode_configure[popup_input_mode].layouts[display_mode]);
                        /* FIXME */
                        //if (scl_check_arrindex(popupLayoutId, MAX_LAYOUT)) {
                        if (scl_check_arrindex(popupLayoutId, MAX_SCL_LAYOUT)) {
                            layout = &sclres_layout[popupLayoutId];
                        }
                        if (layout) {
                            //popupRect.width = utils->get_scale_x(layout->width);
                            //popupRect.height= utils->get_scale_y(layout->height);
                            popupRect.width = layout->width;
                            popupRect.height= layout->height;

                            SclWindowOpener opener;
                            opener.window = window;
                            opener.key = key_index;
                            windows->open_popup(
                                opener,
                                popupRect,
                                popup_input_mode,
                                popupLayoutId,
                                coordinate->popup_type,
                                sclres_input_mode_configure[popup_input_mode].use_virtual_window,
                                sclres_input_mode_configure[popup_input_mode].use_dim_window,
                                coordinate->extract_offset_x,
                                coordinate->extract_offset_y,
                                sclres_input_mode_configure[popup_input_mode].timeout
                                );
                            windows->hide_window(windows->get_magnifier_window());
                            _play_tts_for_input_mode_name(popup_input_mode);
                            ret = TRUE;
                        }
                    }
            } else if (coordinate->long_key_value) {
                if (strlen(coordinate->long_key_value) > 0) {
                    SclPoint ptMoving = context->get_cur_move_point(touch_id);
                    /*if (ptMoving.x >= coordinate->x && ptMoving.x <= coordinate->x + coordinate->width &&
                        ptMoving.y >= coordinate->y && ptMoving.y <= coordinate->y + coordinate->height) {*/
                        if (windows->is_base_window(window)) {
                            state->set_cur_action_state(ACTION_STATE_BASE_LONGKEY);
                        } else {
                            state->set_cur_action_state(ACTION_STATE_POPUP_LONGKEY);
                        }
                        ret = TRUE;

                        SclResParserManager *sclres_manager = SclResParserManager::get_instance();
                        PSclMagnifierWndConfigure magnifier_configure = NULL;
                        if (sclres_manager) {
                            magnifier_configure = sclres_manager->get_magnifier_configure();
                        }
                        if (coordinate->use_long_key_magnifier && magnifier_configure) {
                            CSCLUtils *utils = CSCLUtils::get_instance();
                            SclPoint pos = {0,};

                            const SclLayout* layout = cache->get_cur_layout(window);
                            if (layout) {
                                sclfloat scale_rate_x, scale_rate_y;
                                if (layout->display_mode == DISPLAYMODE_PORTRAIT) {
                                    scale_rate_x = utils->get_scale_rate_x();
                                    scale_rate_y = utils->get_scale_rate_y();
                                } else {
                                    scale_rate_x = utils->get_scale_rate_y();
                                    scale_rate_y = utils->get_scale_rate_x();
                                }
                                sclint scnWidth, scnHeight;
                                utils->get_screen_resolution(&scnWidth, &scnHeight);

                                //SclWindowContext *winctx = windows->get_window_context(window, TRUE);
                                SclWindowContext *winctx = windows->get_window_context(window);
                                if (winctx) {
                                    pos.x = winctx->geometry.x + (coordinate->x + (coordinate->width / 2)) -
                                        (magnifier_configure->width / 2);
                                    pos.y = winctx->geometry.y + coordinate->y - magnifier_configure->height;
                                }
                                if (pos.x < 0 - magnifier_configure->padding_x) {
                                    pos.x = 0 - magnifier_configure->padding_x;
                                }
                                if (pos.x > scnWidth + magnifier_configure->padding_x - magnifier_configure->width) {
                                    pos.x = scnWidth + magnifier_configure->padding_x - magnifier_configure->width;
                                }
                                pos.y += magnifier_configure->padding_y;
                                pos.x += coordinate->magnifier_offset_x;
                                pos.y += coordinate->magnifier_offset_y;
                                windows->move_window(windows->get_magnifier_window(), pos.x, pos.y);
                                windows->update_window(windows->get_magnifier_window());
                                windows->show_window(windows->get_magnifier_window(),TRUE);
                            }
                        }

                        SclUIEventDesc key_event_desc = {0};
                        key_event_desc.key_type = coordinate->long_key_type;
                        key_event_desc.key_value = coordinate->long_key_value;
                        key_event_desc.key_event = coordinate->long_key_event;
                        key_event_desc.key_modifier = KEY_MODIFIER_LONGKEY;

                        key_event_desc.event_type = EVENT_TYPE_LONGPRESS;
                        key_event_desc.touch_id = touch_id;
                        key_event_desc.mouse_pressed_point = context->get_cur_pressed_point(touch_id);
                        key_event_desc.mouse_current_point = context->get_cur_move_point(touch_id);
                        key_event_desc.mouse_farthest_point = context->get_farthest_move_point(touch_id);

                        key_event_desc.touch_event_order = context->get_multi_touch_event_order(touch_id);

                        handler->on_event_key_clicked(key_event_desc);
                    //}
                }
            }
        }
        if (ret) {
            context->set_cur_key_modifier(touch_id, KEY_MODIFIER_LONGKEY);
        }
    }
    /* Longkey processing in here */
    return ret;
}

sclboolean
CSCLController::process_button_repeat_pressed_event(sclwindow window, sclbyte key_index,
                                                    scltouchdevice touch_id, sclboolean actual_event)
{
    SCL_DEBUG();
    CSCLContext *context = CSCLContext::get_instance();
    CSCLResourceCache *cache = CSCLResourceCache::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLEventHandler *handler = CSCLEventHandler::get_instance();

    if (context && cache && windows && handler) {
        const SclLayoutKeyCoordinate *coordinate = cache->get_cur_layout_key_coordinate(window, key_index);

        SCLShiftState shiftidx = context->get_shift_state();
        if (shiftidx < 0 || shiftidx >= SCL_SHIFT_STATE_MAX) shiftidx = SCL_SHIFT_STATE_OFF;

        if (coordinate) {
            switch (coordinate->button_type) {
                case BUTTON_TYPE_NORMAL:
                case BUTTON_TYPE_GRAB:
                case BUTTON_TYPE_SELFISH:
                case BUTTON_TYPE_DIRECTION:
                case BUTTON_TYPE_RELATIVE_DIRECTION: {
                    /* This is for enabling backspace key in search layout*/
                    //if (coordinate->key_type != KEY_TYPE_MODECHANGE && coordinate->key_type != KEY_TYPE_COMPOSITION) {
                    //if (coordinate->key_type != KEY_TYPE_MODECHANGE || coordinate->key_event[0][0] == MVK_BackSpace) {
                    if (coordinate->key_type != KEY_TYPE_MODECHANGE) {
                        sclulong repeatKeyEvent = coordinate->key_event[shiftidx][0];

                        /* In case of Delete key, Change from Char deletion to Word deletion
                           when the input accelation speed is reached to Max */
                        SclResParserManager *sclres_manager = SclResParserManager::get_instance();
                        PSclDefaultConfigure default_configure = NULL;
                        if (sclres_manager) {
                            default_configure = sclres_manager->get_default_configure();
                        }
                        if (default_configure) {
                            if (default_configure->use_word_deletion) {
                                scllong interval = m_repeat_key_duration - (m_key_repeated_num * SCL_REPEATKEY_ACCELERATION);
                                if (repeatKeyEvent == MVK_BackSpace &&
                                    interval <= SCL_REPEATKEY_WORD_DELETION_START_DURATION) {
                                    repeatKeyEvent = MVK_3270_DeleteWord;
                                }
                            }
                        }

                        SclUIEventDesc key_event_desc = {0};
                        key_event_desc.key_value = coordinate->key_value[shiftidx][0];
                        key_event_desc.key_event = repeatKeyEvent;
                        key_event_desc.key_type = coordinate->key_type;
                        key_event_desc.key_modifier = KEY_MODIFIER_NONE;

                        key_event_desc.event_type = EVENT_TYPE_REPEAT;
                        key_event_desc.touch_id = touch_id;
                        key_event_desc.mouse_pressed_point = context->get_cur_pressed_point(touch_id);
                        key_event_desc.mouse_current_point = context->get_cur_move_point(touch_id);
                        key_event_desc.mouse_farthest_point = context->get_farthest_move_point(touch_id);

                        key_event_desc.touch_event_order = context->get_multi_touch_event_order(touch_id);

                        handler->on_event_key_clicked(key_event_desc);
                    }
                }
                break;
            }
        }
    }

    /* Longkey processing in here */
    return TRUE;
}

sclboolean
CSCLController::process_button_move_event(sclwindow window, sclint x, sclint y, sclbyte key_index,
                                          scltouchdevice touch_id, sclboolean actual_event)
{
    SCL_DEBUG();

    sclboolean ret = FALSE;

    CSCLUtils *utils = CSCLUtils::get_instance();
    CSCLEvents *events = CSCLEvents::get_instance();
    CSCLContext *context = CSCLContext::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLFeedback *feedback = CSCLFeedback::get_instance();
    CSCLEventHandler *handler = CSCLEventHandler::get_instance();
    CSCLResourceCache *cache = CSCLResourceCache::get_instance();

    SclButtonContext *btncontext = NULL;

    const SclLayoutKeyCoordinate *coordinate = NULL;

    if (cache) {
        coordinate = cache->get_cur_layout_key_coordinate(window, key_index);
        btncontext = cache->get_cur_button_context(window, key_index);
    }

    if (btncontext && coordinate && feedback && utils && context && handler && cache && events && windows) {
        /* If this key is the key previously pressed, add threshold value for avoiding unintended moving */
        sclint thresholdX = 0;
        sclint thresholdY = 0;
        if (context->get_cur_pressed_window(touch_id) == window && context->get_cur_pressed_key(touch_id) == key_index) {
            thresholdX = utils->get_scale_x(SCL_MOUSE_BUTTON_CHANGE_THRESHOLD_X);
            thresholdY = utils->get_scale_y(SCL_MOUSE_BUTTON_CHANGE_THRESHOLD_Y);
        }

        /* First check if this button is enabled in current active sublayout */
        sclboolean subLayoutMatch = TRUE;
        if (coordinate->sub_layout && context->get_cur_sublayout()) {
            if (strncmp(coordinate->sub_layout, context->get_cur_sublayout(), MAX_SIZE_OF_SUBLAYOUT_STRING) != 0) {
                subLayoutMatch = FALSE;
            }
        }
        if ( x >= coordinate->x - coordinate->add_hit_left  - thresholdX &&
            x < coordinate->x + coordinate->width + coordinate->add_hit_right + thresholdX&&
                y >= coordinate->y - coordinate->add_hit_top - thresholdY &&
                y < coordinate->y + coordinate->height + coordinate->add_hit_bottom + thresholdY &&
                subLayoutMatch ) {
            ret = TRUE;

            SCLShiftState shiftidx = context->get_shift_state();
            if (shiftidx < 0 || shiftidx >= SCL_SHIFT_STATE_MAX) shiftidx = SCL_SHIFT_STATE_OFF;

            const SclLayout* layout = cache->get_cur_layout(windows->get_base_window());

            sclwindow pressed_window = context->get_cur_pressed_window(touch_id);
            scl8 pressed_key = context->get_cur_pressed_key(touch_id);
            SclButtonContext *pressed_context = cache->get_cur_button_context(pressed_window, pressed_key);
            const SclLayoutKeyCoordinate *pressed_coordinate =
                cache->get_cur_layout_key_coordinate(pressed_window, pressed_key);

            if (pressed_context == NULL || pressed_coordinate == NULL) {
                return FALSE;
            }

            if (key_index != pressed_key || window != pressed_window ) {
                /* When the focus has moved to another button, destroy all the timers */
                events->destroy_all_timer();

                if (check_event_transition_enabled(pressed_coordinate, coordinate)) {
                    if (layout) {
                        sclfloat scale_rate_x, scale_rate_y;
                        if (layout->display_mode == DISPLAYMODE_PORTRAIT) {
                            scale_rate_x = utils->get_scale_rate_x();
                            scale_rate_y = utils->get_scale_rate_y();
                        } else {
                            scale_rate_x = utils->get_scale_rate_y();
                            scale_rate_y = utils->get_scale_rate_x();
                        }

                        const scl16 uniqId = utils->get_unique_id();
                        context->set_cur_pressed_event_id(touch_id, uniqId);
                        /* Special routine for autopopup */
                        if (coordinate->popup_type == POPUP_TYPE_AUTO_POPUP) {
                            events->create_timer(SCL_TIMER_AUTOPOPUP, m_autopopup_key_duration, uniqId);
                        } else {
                            /* for long key & repeat key */
                            events->create_timer(SCL_TIMER_LONGKEY, m_long_key_duration, uniqId);
                        }

                        context->set_cur_pressed_window(touch_id, window);
                        context->set_cur_pressed_key(touch_id, key_index);

                        sclboolean showMagnifier = check_magnifier_available(window, key_index, touch_id);

                        SclResParserManager *sclres_manager = SclResParserManager::get_instance();
                        PSclMagnifierWndConfigure magnifier_configure = NULL;
                        if (sclres_manager) {
                            magnifier_configure = sclres_manager->get_magnifier_configure();
                        }
                        if (showMagnifier && magnifier_configure) {
                            SclPoint pos = {0,};
                            /* calculates x position to be set */
                            pos.x = (coordinate->x + (coordinate->width / 2)) - (magnifier_configure->width / 2);

                            /* calculates y position to be set */
                            sclint scnWidth, scnHeight;
                            utils->get_screen_resolution(&scnWidth, &scnHeight);

                            pos.y = (scnHeight - layout->height) + coordinate->y - magnifier_configure->height;

                            if (pos.x < 0 - magnifier_configure->padding_x) {
                                pos.x = 0 - magnifier_configure->padding_x;
                            }
                            if (pos.x > scnWidth + magnifier_configure->padding_x - magnifier_configure->width) {
                                pos.x = scnWidth + magnifier_configure->padding_x - magnifier_configure->width;
                            }
                            pos.y += magnifier_configure->padding_y;
                            if (windows->get_nth_window_in_Z_order_list(SCL_WINDOW_Z_TOP) == windows->get_base_window()) {
                                windows->move_window(windows->get_magnifier_window(), pos.x, pos.y);
                                windows->update_window(windows->get_magnifier_window());
                            }
                        }

                        /* for feedback */
                        feedback->button_moved(window, key_index);

                        btncontext->state = BUTTON_STATE_PRESSED;
                        if (pressed_context) {
                            /* But, if this button should be in pressed state in other multitouch id, do not initialize it */
                            sclboolean found = FALSE;
                            for(sclint loop = 0;loop < context->get_multi_touch_context_num() && !found;loop++) {
                                SclUIEventDesc desc;
                                context->get_multi_touch_event(loop, &desc);
                                if (desc.touch_id != touch_id) {
                                    MultiTouchContext *mulctx = context->find_multi_touch_context(desc.touch_id);
                                    if (mulctx) {
                                        if (mulctx->cur_pressed_window == pressed_window &&
                                            mulctx->cur_pressed_key == pressed_key) {
                                            found = TRUE;
                                        }
                                    }
                                }
                            }
                            if (!found) {
                                pressed_context->state = BUTTON_STATE_NORMAL;
                            }
                        }
                        /* If the window doesn't get exposed before corresponding release event,
                        * the inverted state of a button will never be drawn onto screen.
                        * To prevent such a case, we draw the inverted state of button forcefully and directly,
                        * without waiting for expose event */
                        /* Redrawing pressed button does not work properly, commented out */
                        /*
                        CSCLGraphics *grps = CSCLGraphics::get_instance();
                        CSCLUIBuilder *builder = CSCLUIBuilder::get_instance();
                        scldrawctx draw_ctx;
                        if (pressed_window != SCLWINDOW_INVALID && pressed_key != NOT_USED) {
                            draw_ctx = grps->begin_paint(pressed_window, TRUE);
                            builder->draw_button(pressed_window, draw_ctx, pressed_key, FALSE);
                            grps->end_paint(pressed_window, draw_ctx);
                        }
                        draw_ctx = grps->begin_paint(window, TRUE);
                        builder->draw_button(window, draw_ctx, key_index, TRUE);
                        grps->end_paint(window, draw_ctx);
                        */

                        switch (coordinate->button_type) {
                        case BUTTON_TYPE_DRAG: {
                            SclUIEventDesc key_event_desc = {0};
                            key_event_desc.key_value = coordinate->key_value[shiftidx][0];
                            key_event_desc.key_event = coordinate->key_event[shiftidx][0];
                            key_event_desc.key_type = coordinate->key_type;
                            key_event_desc.key_modifier = KEY_MODIFIER_NONE;

                            key_event_desc.event_type = EVENT_TYPE_MOVE;
                            key_event_desc.touch_id = touch_id;
                            key_event_desc.mouse_pressed_point = context->get_cur_pressed_point(touch_id);
                            key_event_desc.mouse_current_point = context->get_cur_move_point(touch_id);
                            key_event_desc.mouse_farthest_point = context->get_farthest_move_point(touch_id);

                            key_event_desc.touch_event_order = context->get_multi_touch_event_order(touch_id);

                            SclResParserManager *sclres_manager = SclResParserManager::get_instance();
                            PSclMagnifierWndConfigure magnifier_configure = NULL;
                            if (sclres_manager) {
                                magnifier_configure = sclres_manager->get_magnifier_configure();
                            }
                            sclboolean processed = handler->on_event_drag_state_changed(key_event_desc);
                            if (processed && context->get_magnifier_enabled() && magnifier_configure) {
                                SclPoint zoomwinpos = {0,};
                                const SclLayout *layout = cache->get_cur_layout(windows->get_base_window());
                                sclfloat scale_rate_x, scale_rate_y;
                                if (layout->display_mode == DISPLAYMODE_PORTRAIT) {
                                    scale_rate_x = utils->get_scale_rate_x();
                                    scale_rate_y = utils->get_scale_rate_y();
                                } else {
                                    scale_rate_x = utils->get_scale_rate_y();
                                    scale_rate_y = utils->get_scale_rate_x();
                                }
                                /* calculates x position to be set */
                                zoomwinpos.x = (coordinate->x + (coordinate->width / 2)) -
                                    (magnifier_configure->width / 2);

                                /* calculates y position to be set */
                                sclint scnWidth, scnHeight;
                                utils->get_screen_resolution(&scnWidth, &scnHeight);

                                zoomwinpos.y = coordinate->y - magnifier_configure->height;
                                SclWindowContext *winctx = windows->get_window_context(window);
                                if (winctx) {
                                    zoomwinpos.x += winctx->geometry.x;
                                    zoomwinpos.y += winctx->geometry.y;
                                }
                                if (zoomwinpos.x < 0 - magnifier_configure->padding_x) {
                                    zoomwinpos.x = 0 - magnifier_configure->padding_x;
                                }
                                if (zoomwinpos.x > scnWidth + magnifier_configure->padding_x -
                                    magnifier_configure->width) {
                                        zoomwinpos.x = scnWidth + magnifier_configure->padding_x -
                                            magnifier_configure->width;
                                }
                                zoomwinpos.y += magnifier_configure->padding_y;
                                zoomwinpos.x += coordinate->magnifier_offset_x;
                                zoomwinpos.y += coordinate->magnifier_offset_y;
                                windows->move_window(windows->get_magnifier_window(), zoomwinpos.x, zoomwinpos.y);
                                windows->show_window(windows->get_magnifier_window(), 0);
                            }

                            handler->on_event_key_clicked(key_event_desc);
                            if (!(windows->is_base_window(window))) {
                                /* When press event occured in popup window, reset POPUP_TIMEOUT timer */
                                //SclWindowContext *winctx = windows->get_window_context(window, FALSE);
                                SclWindowContext *winctx = windows->get_window_context(window);
                                if (winctx) {
                                    if (winctx->timeout > 0) {
                                        events->destroy_timer(SCL_TIMER_POPUP_TIMEOUT);
                                        events->create_timer(SCL_TIMER_POPUP_TIMEOUT, winctx->timeout, 0, TRUE);
                                    }
                                }
                            }
                        }
                        break;
                        }

#ifdef DIRECTLY_DRAW_ON_EVENTS
                        CSCLUIBuilder *builder = CSCLUIBuilder::get_instance();
                        if (builder) {
                            if (btncontext) {
                                builder->draw_button(window, NULL, key_index, btncontext->state);
                            }
                            if (pressedContext) {
                                builder->draw_button(pressed_window, NULL, pressed_key, pressedContext->state, TRUE);
                            }
                        }
#else
                        windows->update_window(window,
                                coordinate->x, coordinate->y, coordinate->width, coordinate->height);
                        const SclLayoutKeyCoordinate *pressed_coordinate =
                            cache->get_cur_layout_key_coordinate(pressed_window, pressed_key);
                        if (pressed_coordinate) {
                            windows->update_window(pressed_window, pressed_coordinate->x, pressed_coordinate->y,
                                    pressed_coordinate->width, pressed_coordinate->height);
                        }
#endif
                    }

                    //utils->log("Now Moving : %d %d\n", pos.x, pos.y);
                } else {
                    /* If the focus went out from our SELFISH button */
                    if (pressed_coordinate->button_type == BUTTON_TYPE_SELFISH) {
                        pressed_context->state = BUTTON_STATE_NORMAL;
                        windows->update_window(pressed_window, pressed_coordinate->x, pressed_coordinate->y,
                            pressed_coordinate->width, pressed_coordinate->height);
                        /* And if this SELFISH button was the last button pressed */
                        if (touch_id == context->get_last_touch_device_id()) {
                            windows->hide_window(windows->get_magnifier_window());
                        }
                    }
                }
            } else {
                /* If the focus came back into our SELFISH button */
                if (pressed_coordinate->button_type == BUTTON_TYPE_SELFISH && pressed_context->state != BUTTON_STATE_PRESSED) {
                    pressed_context->state = BUTTON_STATE_PRESSED;
                    windows->update_window(pressed_window, pressed_coordinate->x, pressed_coordinate->y,
                        pressed_coordinate->width, pressed_coordinate->height);
                    /* And if this SELFISH button was the last button pressed */
                    if (touch_id == context->get_last_touch_device_id()) {
                        sclboolean showMagnifier = check_magnifier_available(pressed_window, pressed_key, touch_id);

                        if (showMagnifier) {
                            windows->show_window(windows->get_magnifier_window());
                        }
                    }
                }
            }
        }
    }

    return ret;
}

sclboolean
CSCLController::process_button_over_event(sclwindow window, sclint x, sclint y, sclbyte keyindex)
{
    SCL_DEBUG();

    sclboolean ret = FALSE;

    CSCLUtils *utils = CSCLUtils::get_instance();
    CSCLEvents *events = CSCLEvents::get_instance();
    CSCLContext *context = CSCLContext::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLFeedback *feedback = CSCLFeedback::get_instance();
    CSCLResourceCache *cache = CSCLResourceCache::get_instance();

    SclButtonContext *btncontext = NULL;

    const SclLayoutKeyCoordinate *coordinate = NULL;
    if(cache) {
        coordinate = cache->get_cur_layout_key_coordinate(window, keyindex);
        btncontext = cache->get_cur_button_context(window, keyindex);
    }

    if(btncontext && coordinate && feedback && utils && context && cache && events && windows) {
       /* If this key is the key previously pressed, add threshold value for avoiding unintended moving */
        sclboolean subLayoutMatch = TRUE;
        if (coordinate->sub_layout && context->get_cur_sublayout()) {
            if (strncmp(coordinate->sub_layout, context->get_cur_sublayout(), MAX_SIZE_OF_SUBLAYOUT_STRING) != 0) {
                subLayoutMatch = FALSE;
            }
        }
        if ( x >= coordinate->x - coordinate->add_hit_left &&
            x < coordinate->x + coordinate->width + coordinate->add_hit_right &&
                y >= coordinate->y - coordinate->add_hit_top &&
                y < coordinate->y + coordinate->height + coordinate->add_hit_bottom &&
                subLayoutMatch ) {
            ret = TRUE;

            SCLShiftState shiftidx = context->get_shift_state();
            if(shiftidx < 0 || shiftidx >= SCL_SHIFT_STATE_MAX) shiftidx = SCL_SHIFT_STATE_OFF;

            const SclLayout* layout = cache->get_cur_layout(windows->get_base_window());

            sclwindow highlighted_window = context->get_cur_highlighted_window();
            scl8 highlighted_key = context->get_cur_highlighted_key();
            SclButtonContext *cur_context = cache->get_cur_button_context(window, keyindex);

            if(cur_context == NULL) {
                return FALSE;
            }
            if (keyindex != highlighted_key || window != highlighted_window ) {
                printf("%d != %d || %p != %p\n", keyindex, highlighted_key, window, highlighted_window);
                if(layout) {
                    SclPoint pos = {0,};
                    sclfloat scale_rate_x, scale_rate_y;
                    if(layout->display_mode == DISPLAYMODE_PORTRAIT) {
                        scale_rate_x = utils->get_scale_rate_x();
                        scale_rate_y = utils->get_scale_rate_y();
                    } else {
                        scale_rate_x = utils->get_scale_rate_y();
                        scale_rate_y = utils->get_scale_rate_x();
                    }

                    if (coordinate->key_type != KEY_TYPE_NONE) {
                        if (context->get_tts_enabled()) {
                            const sclchar *targetstr = coordinate->hint_string[shiftidx][btncontext->multikeyIdx];
                            if (targetstr == NULL) {
                                targetstr = coordinate->label[shiftidx][0];
                            }
                            if (targetstr == NULL) {
                                targetstr = coordinate->key_value[shiftidx][btncontext->multikeyIdx];
                            }
                            /*if(state->get_cur_action_state() == ACTION_STATE_BASE_LONGKEY ||
                                state->get_cur_action_state() == ACTION_STATE_POPUP_LONGKEY ) {
                                    targetstr = coordinate->long_key_value;
                            }*/
                            const sclchar *sayit = cache->find_substituted_string(targetstr);
                            utils->play_tts(sayit);
                        }
                    }
                }

                context->set_cur_highlighted_window(window);
                context->set_cur_highlighted_key(keyindex);
            }
        }
    }

    return ret;
}
SCLKeyModifier
CSCLController::get_drag_key_modifier(sclint deltax, sclint deltay, sclfloat dist, sclboolean check_farthest,
                                      scltouchdevice touch_id, sclbyte extra_option) {
    typedef struct {
        double lowerbound;
        double upperbound;
        SCLKeyModifier modifier;
    } DIRECTIONINFO;

    CSCLContext *context = CSCLContext::get_instance();
    SCLKeyModifier key_modifier = KEY_MODIFIER_NONE;

    if (context) {
        double theta = atan2(deltay , (deltax ? deltax : 1)); /* Avoid divide by 0 exception */
        sclfloat ratio = fabs((sclfloat)deltay / (deltax ? deltax : 1));
        SCLDragState cur_drag_state = context->get_cur_drag_state(touch_id);
        if (extra_option == DIRECTION_EXTRA_OPTION_8_DIRECTIONS ||
            extra_option == DIRECTION_EXTRA_OPTION_8_DIRECTIONS_WITH_LONG ||
            extra_option == DIRECTION_EXTRA_OPTION_8_DIRECTIONS_WITH_RETURN) { /* 8 directions */
                /* If the theta is below 0, the direction is upward since the y coordinate grows downward */
                /* The below angle values are customized for MoAKey, need to provide customizing API */
                DIRECTIONINFO info[] = {
                    {-8 * (M_PI / 8), -7 * (M_PI / 8), KEY_MODIFIER_DIRECTION_LEFT},
                    {-7 * (M_PI / 8), -5 * (M_PI / 8), KEY_MODIFIER_DIRECTION_UP_LEFT},
                    {-5 * (M_PI / 8), -2.7 * (M_PI / 8), KEY_MODIFIER_DIRECTION_UP},
                    {-2.7 * (M_PI / 8), -1.5 * (M_PI / 8), KEY_MODIFIER_DIRECTION_UP_RIGHT},
                    {-1.5 * (M_PI / 8),  1 * (M_PI / 8), KEY_MODIFIER_DIRECTION_RIGHT},
                    { 1 * (M_PI / 8),  3 * (M_PI / 8), KEY_MODIFIER_DIRECTION_DOWN_RIGHT},
                    { 3 * (M_PI / 8),  5 * (M_PI / 8), KEY_MODIFIER_DIRECTION_DOWN},
                    { 5 * (M_PI / 8),  7 * (M_PI / 8), KEY_MODIFIER_DIRECTION_DOWN_LEFT},
                    { 7 * (M_PI / 8),  8 * (M_PI / 8), KEY_MODIFIER_DIRECTION_LEFT},
                };
                for (sclint loop = 0;loop < sizeof(info) / sizeof(DIRECTIONINFO);loop++) {
                    if (theta >= info[loop].lowerbound && theta <= info[loop].upperbound) {
                        key_modifier = info[loop].modifier;
                    }
                }
        } else { /* 4 directions */
            /* If the state was dragging to one of 4 directions and the final release point is
            * far enough from inital press point, and the angle is in between out predefined angle value */
            if (extra_option == DIRECTION_EXTRA_OPTION_4_DIRECTIONS_WITH_RETURN_AND_CURVE &&
                cur_drag_state != SCL_DRAG_STATE_NONE && cur_drag_state != SCL_DRAG_STATE_INVALID &&
                dist > SCL_DRAG_CURVE_RECOG_DIST &&
                ratio > (1 / SCL_DRAG_CURVE_FINAL_ANGLE_VALUE) &&
                ratio < SCL_DRAG_CURVE_FINAL_ANGLE_VALUE) {
                    if (cur_drag_state == SCL_DRAG_STATE_DOWN) {
                        if (deltax > 0) key_modifier = KEY_MODIFIER_DIRECTION_CURVE_DOWN_RIGHT;
                        else key_modifier = KEY_MODIFIER_DIRECTION_CURVE_DOWN_LEFT;
                    }
                    if (cur_drag_state == SCL_DRAG_STATE_UP) {
                        if (deltax > 0) key_modifier = KEY_MODIFIER_DIRECTION_CURVE_UP_RIGHT;
                        else key_modifier = KEY_MODIFIER_DIRECTION_CURVE_UP_LEFT;
                    }
                    if (cur_drag_state == SCL_DRAG_STATE_LEFT) {
                        if (deltay > 0) key_modifier = KEY_MODIFIER_DIRECTION_CURVE_LEFT_DOWN;
                        else key_modifier = KEY_MODIFIER_DIRECTION_CURVE_LEFT_UP;
                    }
                    if (cur_drag_state == SCL_DRAG_STATE_RIGHT) {
                        if (deltay > 0) key_modifier = KEY_MODIFIER_DIRECTION_CURVE_RIGHT_DOWN;
                        else key_modifier = KEY_MODIFIER_DIRECTION_CURVE_RIGHT_UP;
                    }
            } else {
                DIRECTIONINFO info[] = {
                    {-4 * (M_PI / 4), -3 * (M_PI / 4), KEY_MODIFIER_DIRECTION_LEFT},
                    {-3 * (M_PI / 4), -1 * (M_PI / 4), KEY_MODIFIER_DIRECTION_UP},
                    {-1 * (M_PI / 4),  1 * (M_PI / 4), KEY_MODIFIER_DIRECTION_RIGHT},
                    { 1 * (M_PI / 4),  3 * (M_PI / 4), KEY_MODIFIER_DIRECTION_DOWN},
                    { 3 * (M_PI / 4),  4 * (M_PI / 4), KEY_MODIFIER_DIRECTION_LEFT},
                };
                for (sclint loop = 0;loop < sizeof(info) / sizeof(DIRECTIONINFO);loop++) {
                    if (theta >= info[loop].lowerbound && theta <= info[loop].upperbound) {
                        key_modifier = info[loop].modifier;
                    }
                }
            }
        }

        if (extra_option == DIRECTION_EXTRA_OPTION_8_DIRECTIONS_WITH_LONG ||
            extra_option == DIRECTION_EXTRA_OPTION_4_DIRECTIONS_WITH_LONG) {
                if (key_modifier >= KEY_MODIFIER_DIRECTION_LEFT &&
                    key_modifier <= KEY_MODIFIER_DIRECTION_DOWN_RIGHT) {
                        key_modifier = (SCLKeyModifier)(key_modifier + 8); // Add LONG attribute;
                }
        }
        if (check_farthest || context->get_cur_drag_state(touch_id) == SCL_DRAG_STATE_RETURN) {
            if (key_modifier >= KEY_MODIFIER_DIRECTION_LEFT &&
                key_modifier <= KEY_MODIFIER_DIRECTION_DOWN_RIGHT) {
                    key_modifier = (SCLKeyModifier)(key_modifier + 16); // Add RETURN attribute;
            }
        }
    }

    return key_modifier;
}

sclboolean
CSCLController::process_button_release_event(sclwindow window, sclint x, sclint y, sclbyte key_index,
                                             scltouchdevice touch_id, sclboolean actual_event)
{
    SCL_DEBUG();

    sclboolean ret = FALSE;
    sclboolean redraw = FALSE;
    sclboolean fireEvt = FALSE;
    SCLKeyModifier key_modifier = KEY_MODIFIER_NONE;

    static sclwindow lastFiredWin = SCLWINDOW_INVALID;
    static sclbyte lastFiredKey = NOT_USED;

    CSCLUtils *utils = CSCLUtils::get_instance();
    CSCLFeedback *feedback = CSCLFeedback::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLContext *context = CSCLContext::get_instance();
    CSCLActionState *state = CSCLActionState::get_instance();
    CSCLEventHandler *handler = CSCLEventHandler::get_instance();
    CSCLResourceCache *cache = CSCLResourceCache::get_instance();

    SclResParserManager *sclres_manager = SclResParserManager::get_instance();
    PSclLayout sclres_layout = sclres_manager->get_layout_table();
    PSclInputModeConfigure sclres_input_mode_configure = sclres_manager->get_input_mode_configure_table();
    assert(sclres_layout != NULL);
    assert(sclres_input_mode_configure != NULL);
    SclButtonContext *btncontext = NULL;
    const SclLayoutKeyCoordinate *coordinate = NULL;

    if (cache) {
        btncontext = cache->get_cur_button_context(window, key_index);
        coordinate = cache->get_cur_layout_key_coordinate(window, key_index);
    }

    const SclLayoutKeyCoordinate *targetCoordination = NULL;

    if (utils && feedback && windows && context && state && handler && cache && btncontext && coordinate) {
        scl8 savedInputMode = context->get_input_mode();

        sclwindow pressed_window = context->get_cur_pressed_window(touch_id);
        scl8 pressed_key = context->get_cur_pressed_key(touch_id);

        if (actual_event) {
             if (coordinate->multitouch_type == SCL_MULTI_TOUCH_TYPE_SETTLE_PREVIOUS) {
                 /* When calling mouse_release, the seq order of current multitouch events will be changed,
                    so we put all the multitouch events into a vector and use them afterwards for releasing */
                 sclboolean finished = FALSE;
                 sclint loop = 0;
                 sclint multi_touch_context_num = context->get_multi_touch_context_num();
                 std::vector<SclUIEventDesc> multi_touch_events;
                 for(loop = 0;loop < multi_touch_context_num;loop++) {
                     SclUIEventDesc desc;
                     context->get_multi_touch_event(loop, &desc);
                     multi_touch_events.push_back(desc);
                 }
                 for(loop = 0;loop < multi_touch_context_num && !finished;loop++) {
                     SclUIEventDesc desc = multi_touch_events[loop];
                     if (desc.touch_id != touch_id) {
                         sclwindow cur_pressed_window = context->get_cur_pressed_window(desc.touch_id);
                         scl8 cur_pressed_key = context->get_cur_pressed_key(desc.touch_id);
                         const SclLayoutKeyCoordinate *cur_pressed_coordinate =
                             cache->get_cur_layout_key_coordinate(cur_pressed_window, cur_pressed_key);
                         if (cur_pressed_coordinate) {
                             if (cur_pressed_coordinate->multitouch_type == SCL_MULTI_TOUCH_TYPE_SETTLE_PREVIOUS) {
                                 mouse_release(context->get_cur_move_window(desc.touch_id),
                                     context->get_cur_move_point(desc.touch_id).x, context->get_cur_move_point(desc.touch_id).y,
                                     desc.touch_id, FALSE);
                             }
                         }
                     } else {
                         finished = TRUE;
                     }
                 }
             }
         }

        /* If this key is the key previously pressed, add threshold value for avoiding unintended moving */
        sclint thresholdX = 0;
        sclint thresholdY = 0;
        if (context) {
            if (context->get_cur_pressed_window(touch_id) == window && context->get_cur_pressed_key(touch_id) == key_index) {
                thresholdX = utils->get_scale_x(SCL_MOUSE_BUTTON_CHANGE_THRESHOLD_X);
                thresholdY = utils->get_scale_y(SCL_MOUSE_BUTTON_CHANGE_THRESHOLD_Y);
            }
        }

        /* Check if the pressed button's type is directional button */
        if (coordinate->button_type == BUTTON_TYPE_DIRECTION || coordinate->button_type == BUTTON_TYPE_RELATIVE_DIRECTION) {
            if (context) {
                if (context->get_cur_pressed_window(touch_id) == window && context->get_cur_pressed_key(touch_id) == key_index) {
                    ret = TRUE;
                    sclboolean check_farthest = FALSE;

                    sclint startx = x;
                    sclint starty = y;

                    /* If the buttontype is RELATIVE_DIRECTION, get the distance from last move point */
                    if (coordinate->button_type == BUTTON_TYPE_RELATIVE_DIRECTION) {
                        startx = context->get_prev_move_point(touch_id).x;
                        starty = context->get_prev_move_point(touch_id).y;
                    } else {
                        startx = context->get_cur_pressed_point(touch_id).x;
                        starty = context->get_cur_pressed_point(touch_id).y;
                    }

                    sclint deltax = x - startx;
                    sclint deltay = y - starty;

                    sclfloat dist = utils->get_distance(x, y, startx, starty);
                    sclfloat direction_recog_dist = SCL_DIRECTION_RECOG_DIST * utils->get_smallest_scale_rate();
                    if (coordinate->is_side_button) {
                        direction_recog_dist = SCL_DIRECTION_RECOG_DIST_SIDE * utils->get_smallest_scale_rate();
                    };
                    if (coordinate->button_type == BUTTON_TYPE_RELATIVE_DIRECTION) {
                        direction_recog_dist = SCL_DIRECTION_RELATIVE_RECOG_DIST * utils->get_smallest_scale_rate();
                    }
                    if (context->get_cur_drag_state(touch_id) == SCL_DRAG_STATE_RETURN &&
                        coordinate->button_type != BUTTON_TYPE_RELATIVE_DIRECTION) {
                        if (coordinate->extra_option == DIRECTION_EXTRA_OPTION_8_DIRECTIONS_WITH_RETURN ||
                            coordinate->extra_option == DIRECTION_EXTRA_OPTION_4_DIRECTIONS_WITH_RETURN ||
                            coordinate->extra_option == DIRECTION_EXTRA_OPTION_4_DIRECTIONS_WITH_RETURN_AND_CURVE) {
                                deltax = context->get_farthest_move_point(touch_id).x - context->get_cur_pressed_point(touch_id).x;
                                deltay = context->get_farthest_move_point(touch_id).y - context->get_cur_pressed_point(touch_id).y;
                                dist = utils->get_distance(context->get_farthest_move_point(touch_id), context->get_cur_pressed_point(touch_id));
                                check_farthest = TRUE;
                        }
                    }
                    if (coordinate->button_type == BUTTON_TYPE_RELATIVE_DIRECTION) {
                        key_modifier = context->get_cur_key_modifier(touch_id);
                    } else if (dist > direction_recog_dist) {
                        key_modifier = get_drag_key_modifier(deltax, deltay, dist, check_farthest, touch_id, coordinate->extra_option);
                    }
                }
            }
        }

        /* First check if this button is enabled in current active sublayout */
        sclboolean subLayoutMatch = TRUE;
        if (coordinate->sub_layout && context->get_cur_sublayout()) {
            if (strncmp(coordinate->sub_layout, context->get_cur_sublayout(), MAX_SIZE_OF_SUBLAYOUT_STRING) != 0) {
                subLayoutMatch = FALSE;
            }
        }
        /* Check if the event occured inside this button's rectangle */
        if ( x >= coordinate->x - coordinate->add_hit_left  - thresholdX &&
                x < coordinate->x + coordinate->width + coordinate->add_hit_right + thresholdX &&
                y >= coordinate->y - coordinate->add_hit_top - thresholdY &&
                y < coordinate->y + coordinate->height + coordinate->add_hit_bottom + thresholdY &&
                subLayoutMatch ) {
            ret = TRUE;
        }

        if (ret) {
            /* for feedback */
            feedback->button_released(window, key_index);

            /* If this button's index is the same as the one initially pressed */
            if (pressed_window == window && pressed_key == key_index) {
                fireEvt = TRUE;
                targetCoordination = coordinate;
            } else {
                const SclLayoutKeyCoordinate *pressed_coordinate =
                    cache->get_cur_layout_key_coordinate(pressed_window, pressed_key);

                if (pressed_coordinate) {
                    if (check_event_transition_enabled(pressed_coordinate, coordinate)) {
                        fireEvt = TRUE;
                        targetCoordination = pressed_coordinate;
                    } else {
                        ret = FALSE;
                    }
                }
            }
        }

        /* In case of mode change buttons, event should be fired only when it was pressed lastly */
        if (fireEvt) {
            if (coordinate->key_type == KEY_TYPE_MODECHANGE) {
                if (touch_id != context->get_last_touch_device_id()) {
                    fireEvt = FALSE;
                }
            }
        }

        /* If this key's modifier is LONGKEY, this means the event is already fired so skip this one */
        if (context->get_cur_key_modifier(touch_id) == KEY_MODIFIER_LONGKEY) {
            fireEvt = FALSE;
        }

        /* Don't fire any events if we're in longkey state */
        if (state->get_cur_action_state() != ACTION_STATE_BASE_LONGKEY &&
                state->get_cur_action_state() != ACTION_STATE_BASE_REPEATKEY &&
                state->get_cur_action_state() != ACTION_STATE_POPUP_LONGKEY &&
                state->get_cur_action_state() != ACTION_STATE_POPUP_REPEATKEY) {
            /* An event occured? */
            if (fireEvt) {
                if (targetCoordination) {
                    SCLShiftState shiftidx = context->get_shift_state();
                    if (shiftidx < 0 || shiftidx >= SCL_SHIFT_STATE_MAX) shiftidx = SCL_SHIFT_STATE_OFF;

                    SclUIEventDesc key_event_desc = {0};
                    key_event_desc.key_type = targetCoordination->key_type;

                    key_event_desc.event_type = EVENT_TYPE_RELEASE;
                    key_event_desc.touch_id = touch_id;
                    key_event_desc.mouse_pressed_point = context->get_cur_pressed_point(touch_id);
                    key_event_desc.mouse_current_point = context->get_cur_move_point(touch_id);
                    key_event_desc.mouse_farthest_point = context->get_farthest_move_point(touch_id);

                    key_event_desc.touch_event_order = context->get_multi_touch_event_order(touch_id);

                    switch (targetCoordination->button_type) {
                    case BUTTON_TYPE_NORMAL:
                    case BUTTON_TYPE_GRAB :
                    case BUTTON_TYPE_SELFISH:
                    case BUTTON_TYPE_DIRECTION :
                    case BUTTON_TYPE_RELATIVE_DIRECTION: {
                        SclButtonContext *pressed_context = cache->get_cur_button_context(pressed_window, pressed_key);
                        if (pressed_context) {
                            if (!(targetCoordination->use_repeat_key) && pressed_context->state == BUTTON_STATE_PRESSED) {
                                key_event_desc.key_value = targetCoordination->key_value[shiftidx][0];
                                key_event_desc.key_event = targetCoordination->key_event[shiftidx][0];
                                key_event_desc.key_modifier = key_modifier;
                                handler->on_event_key_clicked(key_event_desc);
                            }
                        }
                    }
                    break;
                    case BUTTON_TYPE_MULTITAP:
                    case BUTTON_TYPE_ROTATION: {
                        if (targetCoordination->button_type == BUTTON_TYPE_MULTITAP) {
                            if (window == lastFiredWin && key_index == lastFiredKey) {
                                key_modifier = KEY_MODIFIER_MULTITAP_REPEAT;
                            } else {
                                key_modifier = KEY_MODIFIER_MULTITAP_START;
                            }
                        } else {
                            key_modifier = KEY_MODIFIER_NONE;
                        }
                        if (btncontext->multikeyIdx < MAX_SIZE_OF_MULTITAP_CHAR) {
                            key_event_desc.key_value = coordinate->key_value[shiftidx][btncontext->multikeyIdx];
                            key_event_desc.key_event = coordinate->key_event[shiftidx][btncontext->multikeyIdx];
                            key_event_desc.key_modifier = key_modifier;
                            handler->on_event_key_clicked(key_event_desc);
                        }
                        /* Check if the multikey index is in valid range, and increase by one */
                        if (btncontext->multikeyIdx >= MAX_SIZE_OF_MULTITAP_CHAR - 1) {
                            btncontext->multikeyIdx = 0;
                        } else {
                            sclbyte orgindex = btncontext->multikeyIdx;
                            btncontext->multikeyIdx = 0;
                            if (targetCoordination->key_value[shiftidx][orgindex + 1]) {
                                if (strlen(targetCoordination->key_value[shiftidx][orgindex + 1]) > 0) {
                                    btncontext->multikeyIdx = orgindex + 1;
                                }
                            }
                        }
                    }
                    break;
                    case BUTTON_TYPE_DRAG : {
                    }
                    break;
                    }

                    switch (coordinate->popup_type) {
                    case POPUP_TYPE_BTN_RELEASE_POPUP:
                    case POPUP_TYPE_BTN_RELEASE_POPUP_ONCE: {
                        SCLDragState dragstate = context->get_cur_drag_state(touch_id);
                        sclint popup_input_mode = NOT_USED;
                        if (scl_check_arrindex(dragstate, SCL_DRAG_STATE_MAX)) {
                            popup_input_mode = sclres_manager->get_inputmode_id(coordinate->popup_input_mode[dragstate]);
                            /* FIXME */
                            //if (!scl_check_arrindex(popup_input_mode, MAX_INPUT_MODE_POPUP)) {
                            if (!scl_check_arrindex(popup_input_mode, MAX_SCL_INPUT_MODE)) {
                                popup_input_mode =
                                    sclres_manager->get_inputmode_id(coordinate->popup_input_mode[SCL_DRAG_STATE_NONE]);
                            }
                        }
                        SCLDisplayMode display_mode = context->get_display_mode();
                        /* FIXME */
                        //if (scl_check_arrindex(popup_input_mode, MAX_INPUT_MODE_POPUP) &&
                        if (scl_check_arrindex(popup_input_mode, MAX_SCL_INPUT_MODE) &&
                            scl_check_arrindex(display_mode, DISPLAYMODE_MAX)) {
                            sclshort popupLayoutId =
                                sclres_manager->get_layout_id(sclres_input_mode_configure[popup_input_mode].layouts[display_mode]);
                            if (popupLayoutId == NOT_USED){
                                // deal with NOT_USED
                                printf("popupLayoutID is not used.\n");
                            }
                            SclLayout *layout = NULL;
                            /* FIXME */
                            //if (scl_check_arrindex(popupLayoutId, MAX_LAYOUT)) {
                            if (scl_check_arrindex(popupLayoutId, MAX_SCL_LAYOUT)) {
                                layout = &sclres_layout[popupLayoutId];
                            }
                            if (layout) {
                                SclRectangle popupRect;
                                SclRectangle baseWndRect;
                                windows->get_window_rect(windows->get_base_window(), &baseWndRect);
                                popupRect.x = coordinate->x + coordinate->popup_relative_x + baseWndRect.x;
                                popupRect.y = coordinate->y + coordinate->popup_relative_y + baseWndRect.y;

                                //popupRect.width = utils->get_scale_x(layout->width);
                                //popupRect.height= utils->get_scale_y(layout->height);
                                popupRect.width = layout->width;
                                popupRect.height= layout->height;

                                /* Let's make sure this popup window does not go beyond the screen area */
                                sclint scr_w, scr_h;
                                utils->get_screen_resolution(&scr_w, &scr_h);

                                if (popupRect.x + popupRect.width > scr_w) {
                                    popupRect.x = scr_w - popupRect.width;
                                }
                                if (popupRect.y + popupRect.height > scr_h) {
                                    popupRect.y = scr_h - popupRect.height;
                                }

                                SclWindowOpener opener;
                                opener.window = window;
                                opener.key = key_index;
                                windows->open_popup(
                                    opener,
                                    popupRect,
                                    popup_input_mode,
                                    popupLayoutId,
                                    coordinate->popup_type,
                                    sclres_input_mode_configure[popup_input_mode].use_virtual_window,
                                    sclres_input_mode_configure[popup_input_mode].use_dim_window,
                                    coordinate->extract_offset_x,
                                    coordinate->extract_offset_y,
                                    sclres_input_mode_configure[popup_input_mode].timeout
                                );
                                windows->hide_window(windows->get_magnifier_window());
                                _play_tts_for_input_mode_name(popup_input_mode);
                            }
                        }
                    }
                    break;
                    case POPUP_TYPE_AUTO_POPUP:
                    case POPUP_TYPE_BTN_PRESS_POPUP_DRAG:
                    case POPUP_TYPE_NONE:
                        /* Nothing to do in here */
                        break;
                    }
                }

                lastFiredWin = window;
                lastFiredKey = key_index;
            }
        }

        /* If this button was pressed, initialize the button context regardless of event  */
        if (btncontext->state == BUTTON_STATE_PRESSED) {
            /* But, if this button should be in pressed state in other multitouch id, do not initialize */
            sclboolean found = FALSE;
            for(sclint loop = 0;loop < context->get_multi_touch_context_num() && !found;loop++) {
                SclUIEventDesc desc;
                context->get_multi_touch_event(loop, &desc);
                if (desc.touch_id != touch_id) {
                    MultiTouchContext *mulctx = context->find_multi_touch_context(desc.touch_id);
                    if (mulctx) {
                        if (mulctx->cur_pressed_window == window && mulctx->cur_pressed_key == key_index) {
                            found = TRUE;
                        }
                    }
                }
            }
            if (!found) {
                btncontext->state = BUTTON_STATE_NORMAL;
                redraw = TRUE;
            }
        }

        /* If this button needs to be redrawn */
        if (redraw) {
#ifdef DIRECTLY_DRAW_ON_EVENTS
            CSCLUIBuilder *builder = CSCLUIBuilder::get_instance();
            if (builder) {
                builder->draw_button(window, NULL, key_index, btncontext->state, TRUE);
            }
#else
            if (savedInputMode == context->get_input_mode()) {
                CSCLWindows *windows = CSCLWindows::get_instance();
                if (windows) {
                    windows->update_window(window, coordinate->x, coordinate->y, coordinate->width, coordinate->height);
                }
            }

#endif
        }
    }

    return ret;
}

sclboolean
CSCLController::mouse_press(sclwindow window, sclint x, sclint y, scltouchdevice touch_id, sclboolean actual_event)
{
    SCL_DEBUG();
    sclboolean ret = FALSE;

    if (m_input_events_disabled) return FALSE;

    //utils->log("Controller::mouse_press : %d %d\n", x, y);

    /* Adjust x,y coordinate by touch offset */
    CSCLErrorAdjustment *adjustment = CSCLErrorAdjustment::get_instance();

    CSCLContext *context = CSCLContext::get_instance();
    CSCLResourceCache *cache = CSCLResourceCache::get_instance();
    CSCLActionState *state = CSCLActionState::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLUtils *utils = CSCLUtils::get_instance();

    sclint btnIndex = NOT_USED;
    SclWindowContext *winctx = NULL;
    if (windows) {
        const SclLayout *layout = cache->get_cur_layout(window);
        if (layout) {
            x += layout->mouse_manipulate_x;
            y += layout->mouse_manipulate_y;
        }
        winctx = windows->get_window_context(window);
        /* If the dim window is virtual and currently active, let's just skip this event */
        if (windows->is_base_window(window)) {
            SclWindowContext *dimctx = windows->get_window_context(windows->get_dim_window());
            if (dimctx) {
                if (dimctx->is_virtual && !(dimctx->hidden)) {
                    window = windows->get_dim_window();
                    winctx = dimctx;
                }
            }
        }
    }

    if (cache && state && windows && context && winctx) {
        SCLDisplayMode display_mode = context->get_display_mode();
        SclResParserManager *sclres_manager = SclResParserManager::get_instance();
        PSclDefaultConfigure default_configure = NULL;
        if (sclres_manager) {
            default_configure = sclres_manager->get_default_configure();
        }
        if (default_configure) {
            adjustment->apply_touch_offset(default_configure->touch_offset_level[display_mode], &x, &y);
        }

        sclboolean isSubEvent = FALSE;
        if (context->get_multi_touch_context_num() > 0) {
            SclUIEventDesc desc;
            context->get_multi_touch_event(0, &desc);
            sclwindow pressed_window = context->get_cur_pressed_window(desc.touch_id);
            scl8 pressed_key = context->get_cur_pressed_key(desc.touch_id);
            SclLayoutKeyCoordinate *coordinate = cache->get_cur_layout_key_coordinate(pressed_window, pressed_key);
            if (coordinate) {
                if (coordinate->multitouch_type == SCL_MULTI_TOUCH_TYPE_GRAB_SUB_EVENTS) {
                    isSubEvent = TRUE;
                    utils->play_vibration(DEFAULT_VIBRATION_STYLE, DEFAULT_VIBRATION_DURATION);
                }
            }
        }
        context->create_multi_touch_context(touch_id, isSubEvent);
        context->set_cur_pressed_window(touch_id, window);
        context->set_cur_pressed_point(touch_id, x, y);
        context->set_cur_pressed_time(touch_id);
        context->set_cur_move_window(touch_id, window);
        context->set_cur_move_point(touch_id, x, y);
        context->set_last_touch_device_id(touch_id);
        context->set_cur_drag_state(touch_id, SCL_DRAG_STATE_NONE);
        context->set_cur_key_modifier(touch_id, KEY_MODIFIER_NONE);
        for(sclint labelidx = 0;labelidx < MAX_SIZE_OF_LABEL_FOR_ONE;labelidx++) {
            context->set_custom_magnifier_label(touch_id, labelidx, NULL);
        }

        /* If there is postponed update of button, update it now */
        CSCLEvents *events = CSCLEvents::get_instance();
        sclwindow last_win = context->get_last_pressed_window();
        scl8 last_key = context->get_last_pressed_key();
        if (last_win != SCLWINDOW_INVALID && last_key != NOT_USED) {
            const SclLayoutKeyCoordinate* coords = cache->get_cur_layout_key_coordinate(last_win, last_key);
            if (coords) {
                windows->update_window(last_win, coords->x, coords->y, coords->width, coords->height);
            }
        }
        context->set_prev_pressed_window(touch_id, SCLWINDOW_INVALID);
        context->set_prev_pressed_key(touch_id, NOT_USED);
        context->set_prev_drag_state(touch_id, SCL_DRAG_STATE_NONE);
        context->set_prev_move_point(touch_id, x, y);

        /* Destroy key related timers */
        events->destroy_timer(SCL_TIMER_BUTTON_DELAY);
        events->destroy_timer(SCL_TIMER_AUTOPOPUP);
        events->destroy_timer(SCL_TIMER_SHORT_LONGKEY);
        events->destroy_timer(SCL_TIMER_LONGKEY);
        events->destroy_timer(SCL_TIMER_REPEATKEY);

        /* Do what has to be done when mouse gets pressed */
        handle_engine_signal(SCL_SIG_MOUSE_PRESS, window);

        /* Adjust event x and y positions as relative position to the virtual window */
        if (winctx) {
            /*if (winctx->isVirtual) {
                SclWindowContext *basectx = windows->get_window_context(windows->get_base_window());
                if (basectx) {
                    x -= (winctx->x - basectx->x);
                    y -= (winctx->y - basectx->y);
                }
            }*/
        }

        if (!isSubEvent) {
            /* Iterate all the buttons and inform the event */
            sclboolean ended = FALSE;
            for (int loop = 0;loop < MAX_KEY && !ended;loop++) {
                SclButtonContext *btncontext = cache->get_cur_button_context(window, loop);
                const SclLayoutKeyCoordinate *coordinate = cache->get_cur_layout_key_coordinate(window, loop);
                if (btncontext && coordinate) {
                    if (!(btncontext->used)) {
                        ended = TRUE;
                    } else if (btncontext->state != BUTTON_STATE_DISABLED &&
                                coordinate->button_type != BUTTON_TYPE_UIITEM) {
                        if (process_button_pressed_event(window, x, y, loop, touch_id, actual_event)) {
                            if (windows->is_base_window(window)) {
                                state->set_cur_action_state(ACTION_STATE_BASE_PRESS);
                            } else {
                                state->set_cur_action_state(ACTION_STATE_POPUP_PRESS);
                            }
                            btnIndex = loop;
                            ret = TRUE;
                        }
                    }
                }
            }

            /* For covering a missing area about 1 pixel */
            if (!ret) {
                for (int loop = 0;loop < MAX_KEY;loop++) {
                    SclButtonContext *btncontext = cache->get_cur_button_context(window, loop);
                    const SclLayoutKeyCoordinate* coordinate = cache->get_cur_layout_key_coordinate(window, loop);
                    if (btncontext && coordinate) {
                        if (!(btncontext->used)) {
                            break;
                        } else if (btncontext->state != BUTTON_STATE_DISABLED &&
                                    coordinate->button_type != BUTTON_TYPE_UIITEM) {
                            if (process_button_pressed_event(window, x+1, y+1, loop, touch_id, actual_event)) {
                                if (windows->is_base_window(window)) {
                                    state->set_cur_action_state(ACTION_STATE_BASE_PRESS);
                                } else {
                                    state->set_cur_action_state(ACTION_STATE_POPUP_PRESS);
                                }
                                btnIndex = loop;
                                break;
                            }
                        }
                    }
                }
            }
        }

        sclwindow skipwindow = window;
        if (ret && btnIndex != NOT_USED) {
            const SclLayoutKeyCoordinate *coordinate = cache->get_cur_layout_key_coordinate(window, btnIndex);
            if (coordinate) {
                sclboolean dont_close_popup = FALSE;
                if (coordinate->dont_close_popup) {
                    dont_close_popup = TRUE;
                }
                /* If the button's popup type is drag type, the opened popup could be the one opened by this press event */
                if (coordinate->popup_type == POPUP_TYPE_BTN_PRESS_POPUP_DRAG) {
                    /* Check the opened popup was opened by this button */
                    sclwindow popupwin = windows->get_nth_window_in_Z_order_list(SCL_WINDOW_Z_TOP);
                    SclWindowContext *popupctx = windows->get_window_context(popupwin);
                    if (popupctx) {
                        SclWindowOpener opener = popupctx->opener;
                        if (opener.window == window && opener.key == btnIndex) {
                            dont_close_popup = TRUE;
                        }
                    }
                }
                if (dont_close_popup) {
                    skipwindow = windows->get_nth_window_in_Z_order_list(SCL_WINDOW_Z_TOP);
                }
            }
        }
        windows->close_all_popups(skipwindow);

        /* When press event occured in popup window, reset POPUP_TIMEOUT timer */
        if (!(windows->is_base_window(window))) {
            if (winctx->timeout > 0) {
                events->destroy_timer(SCL_TIMER_POPUP_TIMEOUT);
                events->create_timer(SCL_TIMER_POPUP_TIMEOUT, winctx->timeout, 0, TRUE);
            }
        } else if (skipwindow != window) { /* Or the pressed button has dont_close_popup property, reset POPUP_TIMEOUT timer */
            //SclWindowContext *skipwinctx = windows->get_window_context(skipwindow, FALSE);
            SclWindowContext *skipwinctx = windows->get_window_context(skipwindow);
            if (skipwinctx) {
                if (skipwinctx->timeout > 0) {
                    events->destroy_timer(SCL_TIMER_POPUP_TIMEOUT);
                    events->create_timer(SCL_TIMER_POPUP_TIMEOUT, skipwinctx->timeout, 0, TRUE);
                }
            }
        }
    }

    return TRUE;
}

sclboolean
CSCLController::mouse_release(sclwindow window, sclint x, sclint y, scltouchdevice touch_id, sclboolean actual_event)
{
    SCL_DEBUG();
    sclboolean ret = FALSE;

    //if (m_input_events_disabled) return FALSE;

    //utils->log("Controller::mouse_release : %d %d\n", x, y);
    /* Adjust x,y coordinate by touch offset */
    CSCLErrorAdjustment *adjustment = CSCLErrorAdjustment::get_instance();

    CSCLContext *context = CSCLContext::get_instance();
    CSCLResourceCache *cache = CSCLResourceCache::get_instance();
    CSCLActionState *state = CSCLActionState::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLUtils *utils = CSCLUtils::get_instance();
    CSCLEventHandler *handler = CSCLEventHandler::get_instance();

    sclint btnIndex = NOT_USED;

    if (cache && state && windows && context && utils && handler && context->find_multi_touch_context(touch_id)) {
        const SclLayout *layout = cache->get_cur_layout(window);
        if (layout) {
            x += layout->mouse_manipulate_x;
            y += layout->mouse_manipulate_y;
        }

        sclwindow skipwindow = SCLWINDOW_INVALID;
        SCLDisplayMode display_mode = context->get_display_mode();
        
        SclResParserManager *sclres_manager = SclResParserManager::get_instance();
        PSclDefaultConfigure default_configure = NULL;
        if (sclres_manager) {
            default_configure = sclres_manager->get_default_configure();
        }
        if (default_configure) {
            adjustment->apply_touch_offset(default_configure->touch_offset_level[display_mode], &x, &y);
        }

        context->set_cur_move_window(touch_id, SCLWINDOW_INVALID);

        sclwindow pressed_window = context->get_cur_pressed_window(touch_id);
        scl8 pressed_key = context->get_cur_pressed_key(touch_id);
        //SclWindowContext *winctx = windows->get_window_context(window, TRUE);
        SclWindowContext *winctx = windows->get_window_context(window);
        /* Adjust event x and y positions as relative position to the virtual window */
        if (winctx) {
            /*if (winctx->isVirtual) {
                SclWindowContext *basectx = windows->get_window_context(windows->get_base_window());
                if (basectx) {
                    x -= (winctx->x - basectx->x);
                    y -= (winctx->y - basectx->y);
                }
            }*/
            /* If the dim window is virtual and currently active, consider base window's event is occured in dim window */
            if (windows->is_base_window(window)) {
                SclWindowContext *dimctx = windows->get_window_context(windows->get_dim_window());
                if (dimctx) {
                    if (dimctx->is_virtual && !(dimctx->hidden)) {
                        window = windows->get_dim_window();
                        winctx = dimctx;
                    }
                }
            }
        }

        /* Iterate all the buttons and inform the event */
        sclboolean ended = FALSE;
        CSCLUtils *utils = CSCLUtils::get_instance();

        /* FIXME : The routine below seems to be removed, which was originally requested by Vodafone,
         * to slow down the speed of repeat key right before stopping repeatkey event */
        /* if (state->get_cur_action_state() == ACTION_STATE_BASE_REPEATKEY) {
            if (m_key_repeated_num > 10) {
                utils->sleep(100);
                process_button_repeat_pressed_event(pressed_window, pressed_key, touch_id);
            }
            ended = TRUE;
        }*/

        if (context->get_cur_pressed_window(touch_id) == window) {
            if (abs(context->get_cur_pressed_point(touch_id).x - x) > utils->get_scale_x(SCL_FLICK_GESTURE_RECOG_THRESHOLD) ||
                abs(context->get_cur_pressed_point(touch_id).y - y) > utils->get_scale_y(SCL_FLICK_GESTURE_RECOG_THRESHOLD) )
            {
                struct timeval t0 = context->get_cur_pressed_time(touch_id);
                struct timeval t1;
                gettimeofday(&t1, NULL);
                float etime;
                etime = ((t1.tv_sec * 1000000 + t1.tv_usec) - (t0.tv_sec * 1000000 + t0.tv_usec))/1000.0;
                if (etime < SCL_FLICK_GESTURE_RECOG_TIME) {
                    sclint direction = DRAG_NONE;
                    if (x > context->get_cur_pressed_point(touch_id).x + utils->get_scale_x(SCL_FLICK_GESTURE_RECOG_THRESHOLD)) {
                        direction = DRAG_RIGHT;
                    }
                    if (x < context->get_cur_pressed_point(touch_id).x - utils->get_scale_x(SCL_FLICK_GESTURE_RECOG_THRESHOLD)) {
                        direction = DRAG_LEFT;
                    }
                    if (y > context->get_cur_pressed_point(touch_id).y + utils->get_scale_y(SCL_FLICK_GESTURE_RECOG_THRESHOLD)) {
                        direction = DRAG_DOWN;
                    }
                    if (y < context->get_cur_pressed_point(touch_id).y - utils->get_scale_y(SCL_FLICK_GESTURE_RECOG_THRESHOLD)) {
                        direction = DRAG_UP;
                    }
                    if (handler->on_event_notification(SCL_UINOTITYPE_GESTURE_FLICK, direction)) {
                        ended = TRUE;
                    }
                }
            }
        }

        /* FIXME : We should consider this kind of action in general manner, not only specific to autopopup */
        /* And also, this kind of implementation only selects button that was highlighted at least once. */
        /* iPhone supports highlighting autopopup buttons with its direction, even if the pointer never goes up on the button */
        // {
        //SclWindowContext *pressedCtx = windows->get_window_context(pressed_window, FALSE);
        SclWindowContext *pressedCtx = windows->get_window_context(pressed_window);
        if (pressedCtx) {
            utils->log("PRESSED CTX : %p %d %d\n", pressed_window, pressedCtx->geometry.x, pressedCtx->geometry.y);
            //if (pressedCtx->popuptype == POPUP_TYPE_AUTO_POPUP) {
            sclboolean grab_event = FALSE;
            const SclLayout *layout = cache->get_cur_layout(pressed_window);
            if (layout) {
                if (layout->style == LAYOUT_STYLE_POPUP_GRAB) {
                    grab_event = TRUE;
                }
                /* If the topmost window has the POPUP_GRAB style, find the nearest button to the mouse pointer */
                if (grab_event) {
                    /* If the layout's addGrab* values are defined, process this event only if the event occured inside grab area */
                    sclboolean in_grab_area = TRUE;
                    if (layout->add_grab_left != NOT_USED && x < (pressedCtx->geometry.x - layout->add_grab_left)) {
                        in_grab_area = FALSE;
                    }
                    if (layout->add_grab_right != NOT_USED &&
                        x > (pressedCtx->geometry.x + pressedCtx->geometry.width + layout->add_grab_right)) {
                        in_grab_area = FALSE;
                    }
                    if (layout->add_grab_top != NOT_USED && y < (pressedCtx->geometry.y - layout->add_grab_top)) {
                        in_grab_area = FALSE;
                    }
                    if (layout->add_grab_bottom != NOT_USED &&
                        y > (pressedCtx->geometry.y + pressedCtx->geometry.height + layout->add_grab_bottom)) {
                        in_grab_area = FALSE;
                    }
                    if (in_grab_area) {
                        SclLayoutKeyCoordinate *coord = cache->get_cur_layout_key_coordinate(pressed_window, pressed_key);
                        if (coord) {
                            x = coord->x + (coord->width / 2);
                            y = coord->y + (coord->height / 2);

                            for (int loop = 0;loop < MAX_KEY && !ended;loop++) {
                                SclButtonContext *btncontext = cache->get_cur_button_context(pressed_window, loop);
                                const SclLayoutKeyCoordinate *coordinate = cache->get_cur_layout_key_coordinate(window, loop);
                                if (btncontext && coordinate) {
                                    if (!(btncontext->used)) {
                                        ended = TRUE;
                                    } else if (btncontext->state != BUTTON_STATE_DISABLED &&
                                                coordinate->button_type != BUTTON_TYPE_UIITEM) {
                                        if (process_button_release_event(pressed_window, x, y, loop, touch_id, actual_event)) {
                                            ret = TRUE;
                                            ended = TRUE;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        // }

        SclButtonContext *btncontext = cache->get_cur_button_context(pressed_window, pressed_key);
        const SclLayoutKeyCoordinate *coordinate = cache->get_cur_layout_key_coordinate(pressed_window, pressed_key);

        /* FIXME : The rule below would not be a general requirement. A policy is needed regarding this. */
        /* Ignore base window's release event if a popup window is opened */
        if (state->get_cur_action_state() == ACTION_STATE_POPUP_INIT ||
                state->get_cur_action_state() == ACTION_STATE_POPUP_PRESS ||
                state->get_cur_action_state() == ACTION_STATE_POPUP_MOVING ||
                state->get_cur_action_state() == ACTION_STATE_POPUP_RELEASE ||
                state->get_cur_action_state() == ACTION_STATE_POPUP_REPEATKEY ||
                state->get_cur_action_state() == ACTION_STATE_POPUP_LONGKEY) {
            if (windows->is_base_window(window)) {
                ended = TRUE;
                /* In case of direction button, the release event on other window should be processed */
                if (coordinate && winctx && pressedCtx) {
                    if (coordinate->button_type == BUTTON_TYPE_DIRECTION || coordinate->button_type == BUTTON_TYPE_RELATIVE_DIRECTION) {
                        sclint relx = (winctx->geometry.x + x) - pressedCtx->geometry.x;
                        sclint rely = (winctx->geometry.y + y) - pressedCtx->geometry.y;
                        if (process_button_release_event(pressed_window, relx, rely, pressed_key, touch_id, actual_event)) {
                            btnIndex = pressed_key;
                            ret = TRUE;
                            x = coordinate->x + (coordinate->width / 2);
                            y = coordinate->y + (coordinate->height / 2);
                            skipwindow = pressed_window;
                        }
                    }
                }
            }
        }

        MultiTouchContext *mulctx = context->find_multi_touch_context(touch_id);
        if (mulctx) {
            if (!(mulctx->is_sub_event)) {
                /* First check if the event occured in pressed key's threshold area */
                if (btncontext && coordinate && !ended) {
                    if (btncontext->used && btncontext->state != BUTTON_STATE_DISABLED) {
                        if (process_button_release_event(pressed_window, x, y, pressed_key, touch_id, actual_event)) {
                            btnIndex = pressed_key;
                            ret = TRUE;
                            x = coordinate->x + (coordinate->width / 2);
                            y = coordinate->y + (coordinate->height / 2);
                        }
                    }
                }
                for (int loop = 0;loop < MAX_KEY && !ended;loop++) {
                    SclButtonContext *btncontext = cache->get_cur_button_context(window, loop);
                    const SclLayoutKeyCoordinate *coordinate = cache->get_cur_layout_key_coordinate(window, loop);
                    if (btncontext && coordinate) {
                        if (!(btncontext->used)) {
                            ended = TRUE;
                        } else if (btncontext->state != BUTTON_STATE_DISABLED &&
                                    coordinate->button_type != BUTTON_TYPE_UIITEM) {
                            if (window != pressed_window || loop != pressed_key) {
                                if (process_button_release_event(window, x, y, loop, touch_id, actual_event)) {
                                    btnIndex = loop;
                                    ret = TRUE;
                                }
                            }
                        }
                    }
                }
            } else {
                SclUIEventDesc key_event_desc = {0};
                key_event_desc.key_value = NULL;
                key_event_desc.key_event = NOT_USED;
                key_event_desc.key_modifier = KEY_MODIFIER_NONE;
                key_event_desc.event_type = EVENT_TYPE_NONE;
                key_event_desc.touch_id = touch_id;
                key_event_desc.mouse_pressed_point = context->get_cur_pressed_point(touch_id);
                key_event_desc.mouse_current_point = context->get_cur_move_point(touch_id);
                key_event_desc.mouse_farthest_point = context->get_farthest_move_point(touch_id);

                key_event_desc.touch_event_order = context->get_multi_touch_event_order(touch_id);

                handler->on_event_key_clicked(key_event_desc);
            }
        }

        /* For covering a missing area about 1 pixel */
        if (!ret) {
            ended = FALSE;

            if (state->get_cur_action_state() == ACTION_STATE_POPUP_INIT ||
                    state->get_cur_action_state() == ACTION_STATE_POPUP_PRESS ||
                    state->get_cur_action_state() == ACTION_STATE_POPUP_MOVING ||
                    state->get_cur_action_state() == ACTION_STATE_POPUP_RELEASE ||
                    state->get_cur_action_state() == ACTION_STATE_POPUP_REPEATKEY ||
                    state->get_cur_action_state() == ACTION_STATE_POPUP_LONGKEY) {
                if (windows->is_base_window(window)) {
                    ended = TRUE;
                }
            }

            for (int loop = 0;loop < MAX_KEY && !ended;loop++) {
                SclButtonContext *btncontext = cache->get_cur_button_context(window, loop);
                if (btncontext && coordinate) {
                    if (!(btncontext->used)) {
                        ended = TRUE;
                        break;
                    } else if (btncontext->state != BUTTON_STATE_DISABLED &&
                                coordinate->button_type != BUTTON_TYPE_UIITEM) {
                        if (process_button_release_event(window, x+1, y+1, loop, touch_id)) {
                            btnIndex = loop;
                            ret = TRUE;
                            break;
                        }
                    }
                }
            }
        }

        if (windows->is_base_window(window)) {
            state->set_cur_action_state(ACTION_STATE_BASE_INIT);
        } else {
            state->set_cur_action_state(ACTION_STATE_POPUP_INIT);
        }

        /* Restore previously pressed button's context and redraw it */
        if (btncontext && coordinate) {
            btncontext->state = BUTTON_STATE_NORMAL;
            /* Commented below line to postpone some of the feedback for releasing */
            //windows->update_window(pressed_window, coordinate->x, coordinate->y, coordinate->width, coordinate->height);
        }

        /* If there is postponed update of button, update it now */
        CSCLEvents *events = CSCLEvents::get_instance();
        sclwindow last_win = context->get_last_pressed_window();
        scl8 last_key = context->get_last_pressed_key();
        if (last_win != SCLWINDOW_INVALID && last_key != NOT_USED) {
            const SclLayoutKeyCoordinate* coords = cache->get_cur_layout_key_coordinate(last_win, last_key);
            if (coords) {
                windows->update_window(last_win, coords->x, coords->y, coords->width, coords->height);
            }
        }

        /* To postpone some of the feedback for releasing */
        context->set_last_pressed_key(context->get_cur_pressed_key(touch_id));
        context->set_last_pressed_window(context->get_cur_pressed_window(touch_id));

        /* Do what has to be done when mouse gets released */
        sclboolean signaled = FALSE;
        if (coordinate) {
            switch (coordinate->popup_type) {
            case POPUP_TYPE_BTN_RELEASE_POPUP:
            case POPUP_TYPE_BTN_RELEASE_POPUP_ONCE:
            case POPUP_TYPE_BTN_LONGPRESS_POPUP:
            case POPUP_TYPE_BTN_LONGPRESS_POPUP_ONCE:
                /* Fix me : We should consider z-order */
                skipwindow = windows->get_nth_window_in_Z_order_list(SCL_WINDOW_Z_TOP);
                handle_engine_signal(SCL_SIG_MOUSE_RELEASE, skipwindow);
                signaled = TRUE;
                break;
            }
        }
        if (!signaled) {
            //SclWindowContext *ctx = windows->get_window_context(window, FALSE);
            SclWindowContext *ctx = windows->get_window_context(window);
            if (ctx) {
                if (ctx->popup_type == POPUP_TYPE_BTN_RELEASE_POPUP ||
                    ctx->popup_type == POPUP_TYPE_BTN_LONGPRESS_POPUP) {
                    /* Don't close window if the clicked button is a child of ReleasePopup window */
                    skipwindow = window;
                    handle_engine_signal(SCL_SIG_MOUSE_RELEASE, window);
                    signaled = TRUE;
                }
            }
            if (!signaled) {
                handle_engine_signal(SCL_SIG_MOUSE_RELEASE);
            }
        }

        context->set_cur_pressed_key(touch_id, NOT_USED);
        context->set_cur_pressed_window(touch_id, SCLWINDOW_INVALID);

        if (ret && btnIndex != NOT_USED) {
            const SclLayoutKeyCoordinate *coordinate = cache->get_cur_layout_key_coordinate(window, btnIndex);
            if (coordinate) {
                if (coordinate->dont_close_popup) {
                    skipwindow = windows->get_nth_window_in_Z_order_list(SCL_WINDOW_Z_TOP);
                }
            }
        } else {
            if (pressed_window == windows->get_nth_window_in_Z_order_list(SCL_WINDOW_Z_TOP)) {
                if (pressedCtx) {
                    if (pressedCtx->popup_type != POPUP_TYPE_BTN_RELEASE_POPUP_ONCE &&
                        pressedCtx->popup_type != POPUP_TYPE_BTN_LONGPRESS_POPUP_ONCE &&
                        pressedCtx->popup_type != POPUP_TYPE_AUTO_POPUP &&
                        pressedCtx->popup_type != POPUP_TYPE_BTN_PRESS_POPUP_DRAG)
                    {
                        skipwindow = pressed_window;
                    }
                }
            }
        }
        windows->close_all_popups(skipwindow);

        /* Destroy key related timers */
        events->destroy_timer(SCL_TIMER_AUTOPOPUP);
        events->destroy_timer(SCL_TIMER_SHORT_LONGKEY);
        events->destroy_timer(SCL_TIMER_LONGKEY);
        events->destroy_timer(SCL_TIMER_REPEATKEY);

        /* If there are more than 1 active multitouch ids, don't play button_delay trick */
        if (context->get_multi_touch_context_num() == 1) {
            /* To postpone some of the feedback for releasing */
            events->create_timer(SCL_TIMER_BUTTON_DELAY, m_button_delay_duration, 0);
        } else {
            sclwindow last_win = context->get_last_pressed_window();
            scl8 last_key = context->get_last_pressed_key();

            if (last_win != SCLWINDOW_INVALID && last_key != NOT_USED) {
                const SclLayoutKeyCoordinate* coordinate = cache->get_cur_layout_key_coordinate(last_win, last_key);
                if (coordinate) {
                    windows->update_window(last_win,
                        coordinate->x, coordinate->y, coordinate->width, coordinate->height);
                }
            }

            windows->hide_window(windows->get_magnifier_window());
            context->set_last_pressed_window(SCLWINDOW_INVALID);
            context->set_last_pressed_key(NOT_USED);
        }
    }

    if (context) {
        if (touch_id == context->get_last_touch_device_id()) {
            context->set_last_touch_device_id(SCLTOUCHDEVICE_INVALID);
        }
        context->destroy_multi_touch_context(touch_id);
    }

    return ret;
}

SCLDragState get_drag_state(sclint deltax, sclint deltay)
{
    SCLDragState ret = SCL_DRAG_STATE_MAX;

    sclfloat ratio = fabs((sclfloat)deltay / (deltax ? deltax : 1));
    /* If tan(theta) is smaller than our predefined value */
    if (ratio <= (1 / SCL_DRAG_CURVE_4_DIRECTION_ANGLE_VALUE)) {
        if (deltax > 0) {
            ret = SCL_DRAG_STATE_RIGHT;
        } else {
            ret = SCL_DRAG_STATE_LEFT;
        }
    } /* If tan(theta) is bigger than our predefined value */
    else if (ratio >= SCL_DRAG_CURVE_4_DIRECTION_ANGLE_VALUE) {
        if (deltay > 0) {
            ret = SCL_DRAG_STATE_DOWN;
        } else {
            ret = SCL_DRAG_STATE_UP;
        }
    } else {
        ret = SCL_DRAG_STATE_INVALID;
    }

    return ret;
}

sclboolean
CSCLController::mouse_move(sclwindow window, sclint x, sclint y, scltouchdevice touch_id, sclboolean actual_event)
{
    SCL_DEBUG();
    sclboolean ret = FALSE;

    if (m_input_events_disabled) return FALSE;

    //utils->log("Controller::mouse_move : %d %d\n", x, y);

    /* Adjust x,y coordinate by touch offset */
    CSCLErrorAdjustment *adjustment = CSCLErrorAdjustment::get_instance();

    CSCLContext *context = CSCLContext::get_instance();
    CSCLResourceCache *cache = CSCLResourceCache::get_instance();
    CSCLActionState *state = CSCLActionState::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLEvents *events = CSCLEvents::get_instance();
    CSCLUtils *utils = CSCLUtils::get_instance();
    CSCLEventHandler *handler = CSCLEventHandler::get_instance();
    SclResParserManager *sclres_manager = SclResParserManager::get_instance();
    PSclModifierDecoration sclres_modifier_decoration = sclres_manager->get_modifier_decoration_table();
    assert(sclres_modifier_decoration != NULL);

    if (cache && state && windows && context && utils && adjustment) {
        const SclLayout *layout = cache->get_cur_layout(window);
        if (layout) {
            x += layout->mouse_manipulate_x;
            y += layout->mouse_manipulate_y;
        }

        if (!(context->find_multi_touch_context(touch_id))) return FALSE;

        SclResParserManager *sclres_manager = SclResParserManager::get_instance();
        PSclDefaultConfigure default_configure = NULL;
        if (sclres_manager) {
            default_configure = sclres_manager->get_default_configure();
        }
        if (default_configure) {
            SCLDisplayMode display_mode = context->get_display_mode();
            adjustment->apply_touch_offset(default_configure->touch_offset_level[display_mode], &x, &y);
        }

        //SclWindowContext *winctx = windows->get_window_context(window, FALSE);
        SclWindowContext *winctx = windows->get_window_context(window);
        /* Adjust event x and y positions as relative position to the virtual window */
        if (winctx) {
            /*if (winctx->isVirtual) {
                SclWindowContext *basectx = windows->get_window_context(windows->get_base_window());
                if (basectx) {
                    x -= (winctx->x - basectx->x);
                    y -= (winctx->y - basectx->y);
                }
            }*/
            /* If the dim window is virtual and currently active, let's just skip this event */
            if (windows->is_base_window(window)) {
                SclWindowContext *dimctx = windows->get_window_context(windows->get_dim_window());
                if (dimctx) {
                    if (dimctx->is_virtual && !(dimctx->hidden)) {
                        return FALSE;
                    }
                }
            }
            /* If the pressed event was occured in dim window, let's just skip this move event */
            if (context->get_last_pressed_window() == windows->get_dim_window()) {
                return FALSE;
            }
        }

        sclwindow pressed_window = context->get_cur_pressed_window(touch_id);
        scl8 pressed_key = context->get_cur_pressed_key(touch_id);
        SclButtonContext *btncontext = cache->get_cur_button_context(pressed_window, pressed_key);
        const SclLayoutKeyCoordinate *coordinate = cache->get_cur_layout_key_coordinate(pressed_window, pressed_key);

        /* If the multitouch type is SETTLE_PREVIOUS and is not the last touch device, let's ignore move events */
        if (coordinate) {
            if (coordinate->multitouch_type == SCL_MULTI_TOUCH_TYPE_SETTLE_PREVIOUS) {
                if (context->get_last_touch_device_id() != touch_id) {
                    return FALSE;
                }
            }
        }

        context->set_cur_move_point(touch_id, x, y);
        context->set_cur_move_window(touch_id, window);

        /* If in longkey state, do not process, just return */
        if (state->get_cur_action_state() == ACTION_STATE_BASE_LONGKEY ||
                state->get_cur_action_state() == ACTION_STATE_BASE_REPEATKEY ||
                state->get_cur_action_state() == ACTION_STATE_POPUP_LONGKEY ||
                state->get_cur_action_state() == ACTION_STATE_POPUP_REPEATKEY) {
            return FALSE;
        }
        /* FIXME : The rule below would not be a general requirement. A policy is needed regarding this. */
        /* And if the event occured in popup window, don't come back to base window */
        if (state->get_cur_action_state() == ACTION_STATE_POPUP_INIT ||
                state->get_cur_action_state() == ACTION_STATE_POPUP_PRESS ||
                state->get_cur_action_state() == ACTION_STATE_POPUP_MOVING ||
                state->get_cur_action_state() == ACTION_STATE_POPUP_RELEASE ||
                state->get_cur_action_state() == ACTION_STATE_POPUP_REPEATKEY ||
                state->get_cur_action_state() == ACTION_STATE_POPUP_LONGKEY) {
            if (windows->is_base_window(window)) {
                return FALSE;
            }
        }

        /* FIXME : Add a condition to skip this code if longkey timer is not active */
        /* If the mouse has moved out of threshold value of longkey keypress area, destroy longkey timer */
        if (m_long_key_cancel_distance > 0) {
            sclfloat dist = utils->get_distance(x, y,
                context->get_cur_pressed_point(touch_id).x, context->get_cur_pressed_point(touch_id).y);
            if (m_long_key_cancel_distance < dist) {
                events->destroy_timer(SCL_TIMER_LONGKEY);
            }
        }

        if (windows->is_base_window(window)) {
            state->set_cur_action_state(ACTION_STATE_BASE_MOVING);
        } else {
            state->set_cur_action_state(ACTION_STATE_POPUP_MOVING);
        }

        /* Iterate all the buttons and inform the event */
        sclboolean ended = FALSE;

        /* Check farthest move point and update it */
        sclint originx = x;
        sclint originy = y;
        if (pressed_window != window) {
            //SclWindowContext *pressedwinctx = windows->get_window_context(pressed_window, FALSE);
            SclWindowContext *pressedwinctx = windows->get_window_context(pressed_window);
            if (winctx && pressedwinctx) {
                originx = (winctx->geometry.x - pressedwinctx->geometry.x) + x;
                originy = (winctx->geometry.y - pressedwinctx->geometry.y) + y;
            }
        }
        sclint startx = originx;
        sclint starty = originy;

        /* Check if we should recognize drag curve */
        if (coordinate) {
            startx = context->get_cur_pressed_point(touch_id).x;
            starty = context->get_cur_pressed_point(touch_id).y;
            sclint deltax = originx - startx;
            sclint deltay = originy - starty;
            sclfloat dist = utils->get_approximate_distance(originx, originy, startx, starty);
            CSCLUtils *utils = CSCLUtils::get_instance();

            sclboolean update_magnifier = FALSE;
            sclboolean drag_state_changed = FALSE;
            SCLDragState cur_drag_state = context->get_cur_drag_state(touch_id);
            SCLDragState next_drag_state = SCL_DRAG_STATE_NONE;
            sclfloat direction_recog_dist = SCL_DIRECTION_RECOG_DIST * utils->get_smallest_scale_rate();
            if (coordinate->is_side_button) {
                direction_recog_dist = SCL_DIRECTION_RECOG_DIST_SIDE * utils->get_smallest_scale_rate();
            };

            if (coordinate->button_type == BUTTON_TYPE_DIRECTION) {
                /* Do not check farthest move point if current drag state is SCL_DRAG_STATE_RETURN */
                if (context->get_cur_drag_state(touch_id) != SCL_DRAG_STATE_RETURN) {
                    if (dist > context->get_farthest_move_dist(touch_id)) {
                        context->set_farthest_move_point(touch_id, originx, originy);
                    }
                }

                if (cur_drag_state == SCL_DRAG_STATE_RETURN) {
                    direction_recog_dist *= SCL_DRAG_RETURN_RECOG_THRESHOLD_RETURN;
                } else if (cur_drag_state != SCL_DRAG_STATE_NONE) {
                    direction_recog_dist *= SCL_DRAG_RETURN_RECOG_THRESHOLD_OTHER;
                }
                if (dist > direction_recog_dist) {
                    next_drag_state = get_drag_state(deltax, deltay);
                    /* Disable longkey if dragging is recognized */
                    events->destroy_timer(SCL_TIMER_LONGKEY);
                }
                if (cur_drag_state != next_drag_state) {
                    drag_state_changed = TRUE;
                }
                if (cur_drag_state == SCL_DRAG_STATE_NONE) {
                    //if (nextDragState != SCL_DRAG_STATE_INVALID) {
                        cur_drag_state = next_drag_state;
                    //}
                } else if (cur_drag_state != next_drag_state) {
                    if (next_drag_state == SCL_DRAG_STATE_NONE) {
                        cur_drag_state = SCL_DRAG_STATE_RETURN;
                    } else {
                        cur_drag_state = next_drag_state;
                    }
                }

                context->set_cur_drag_state(touch_id, cur_drag_state);
                sclboolean check_farthest = FALSE;
                sclshort display = context->get_display_mode();
                if (!scl_check_arrindex(display, DISPLAYMODE_MAX)) display = 0;
                sclfloat dist = utils->get_distance(originx, originy,
                    context->get_cur_pressed_point(touch_id).x, context->get_cur_pressed_point(touch_id).y);
                if (dist < direction_recog_dist && context->get_cur_drag_state(touch_id) == SCL_DRAG_STATE_RETURN) {
                    if (coordinate->extra_option == DIRECTION_EXTRA_OPTION_8_DIRECTIONS_WITH_RETURN ||
                        coordinate->extra_option == DIRECTION_EXTRA_OPTION_4_DIRECTIONS_WITH_RETURN ||
                        coordinate->extra_option == DIRECTION_EXTRA_OPTION_4_DIRECTIONS_WITH_RETURN_AND_CURVE) {
                            deltax = context->get_farthest_move_point(touch_id).x -
                                context->get_cur_pressed_point(touch_id).x;
                            deltay = context->get_farthest_move_point(touch_id).y -
                                context->get_cur_pressed_point(touch_id).y;
                            dist = utils->get_distance(context->get_farthest_move_point(touch_id),
                                context->get_cur_pressed_point(touch_id));
                            check_farthest = TRUE;
                    }
                }
                SCLKeyModifier key_modifier = get_drag_key_modifier(deltax, deltay, dist,
                    check_farthest, touch_id, coordinate->extra_option);
                if (dist > direction_recog_dist) {
                    context->set_cur_key_modifier(touch_id, key_modifier);
                }
                /* If this button needs to be decorated when dragged */
                if (coordinate->modifier_decorator) {
                    const SclModifierDecoration *decoration = NULL;
                    /* FIXME */
                    /*if (scl_check_arrindex(coordinate->modifier_decorator,
                        sizeof(sclres_modifier_decoration) / sizeof(SclModifierDecoration ))) {*/
                    scl8 decoration_id = sclres_manager->get_modifier_decoration_id(coordinate->modifier_decorator);
                    if (scl_check_arrindex(decoration_id, MAX_SCL_MODIFIER_DECORATION_NUM)) {
                        if (sclres_modifier_decoration[decoration_id].valid) {
                            decoration = &(sclres_modifier_decoration[decoration_id]);
                        }
                    }
                    /* Check if the button really needs to be redrawn (whether it has non-null bg_image_path information */
                    if (decoration) {
                        if (decoration->bg_image_path[display][key_modifier]) {
                            windows->update_window(window,
                                coordinate->x, coordinate->y, coordinate->width, coordinate->height);
                        }
                    }
                }
                if (dist > direction_recog_dist) {
                    SclUIEventDesc desc = {0};
                    SCLShiftState shiftidx = context->get_shift_state();
                    desc.key_type = coordinate->key_type;
                    desc.key_value = coordinate->key_value[shiftidx][0];
                    desc.key_event = coordinate->key_event[shiftidx][0];
                    desc.event_type = EVENT_TYPE_MOVE;
                    desc.mouse_pressed_point = context->get_cur_pressed_point(touch_id);
                    desc.mouse_current_point = context->get_cur_move_point(touch_id);
                    desc.mouse_farthest_point = context->get_farthest_move_point(touch_id);
                    desc.key_modifier = key_modifier;

                    if (handler->on_event_drag_state_changed(desc) && context->get_magnifier_enabled()) {
                        update_magnifier = TRUE;
                    }
                }
            } else if (coordinate->button_type == BUTTON_TYPE_RELATIVE_DIRECTION) {
                if (cur_drag_state != SCL_DRAG_STATE_NONE) {
                    startx = context->get_prev_move_point(touch_id).x;
                    starty = context->get_prev_move_point(touch_id).y;
                    dist = utils->get_approximate_distance(originx, originy, startx, starty);
                    direction_recog_dist = SCL_DIRECTION_RELATIVE_RECOG_DIST * utils->get_smallest_scale_rate();
                }
                deltax = originx - startx;
                deltay = originy - starty;
                //printf("DIST : %f, RECOG : %f\n", dist, direction_recog_dist);
                if (dist > direction_recog_dist) {
                    next_drag_state = get_drag_state(deltax, deltay);
                    /* Disable longkey if dragging is recognized */
                    events->destroy_timer(SCL_TIMER_LONGKEY);

                    if (cur_drag_state != next_drag_state) {
                        drag_state_changed = TRUE;
                    }
                    if (next_drag_state != SCL_DRAG_STATE_NONE) {
                        cur_drag_state = next_drag_state;
                    }
                    context->set_cur_drag_state(touch_id, cur_drag_state);

                    startx = context->get_farthest_move_point(touch_id).x;
                    starty = context->get_farthest_move_point(touch_id).y;
                    deltax = originx - startx;
                    deltay = originy - starty;
                    sclfloat dist_farthest = utils->get_approximate_distance(originx, originy, startx, starty);
                    printf("%d %d %d %d %f, %d %d\n", originx, originy, startx, starty, dist_farthest, cur_drag_state, next_drag_state);
                    /* Let's see how much we are away from the last farthest point */
                    sclfloat diffdir_recog_dist = SCL_DIRECTION_RELATIVE_DIFFDIR_RECOG_DIST * utils->get_smallest_scale_rate();
                    /* If we moved certain amount from the point where direction changed, process drag state change routine */
                    if (dist_farthest > diffdir_recog_dist || context->get_cur_drag_state(touch_id) == SCL_DRAG_STATE_NONE) {
                        sclshort display = context->get_display_mode();
                        SCLKeyModifier key_modifier = get_drag_key_modifier(deltax, deltay, dist_farthest,
                            FALSE, touch_id, coordinate->extra_option);
                        context->set_cur_key_modifier(touch_id, key_modifier);
                        /* If this button needs to be decorated when dragged */
                        if (coordinate->modifier_decorator) {
                            const SclModifierDecoration  *decoration = NULL;
                            /* FIXME */
                            /*if (scl_check_arrindex(coordinate->modifier_decorator,
                                sizeof(sclres_modifier_decoration) / sizeof(SclModifierDecoration ))) {*/
                            scl8 decoration_id = sclres_manager->get_modifier_decoration_id(coordinate->modifier_decorator);
                            if (scl_check_arrindex(decoration_id, MAX_SCL_MODIFIER_DECORATION_NUM)) {
                                if (sclres_modifier_decoration[decoration_id].valid) {
                                    decoration = &(sclres_modifier_decoration[decoration_id]);
                                }
                            }
                            /* Check if the button really needs to be redrawn (whether it has non-null bg_image_path information */
                            if (decoration) {
                                if (decoration->bg_image_path[display][key_modifier]) {
                                    windows->update_window(window,
                                        coordinate->x, coordinate->y, coordinate->width, coordinate->height);
                                }
                            }
                        }

                        SclUIEventDesc desc = {0};
                        SCLShiftState shiftidx = context->get_shift_state();
                        desc.key_type = coordinate->key_type;
                        desc.key_value = coordinate->key_value[shiftidx][0];
                        desc.key_event = coordinate->key_event[shiftidx][0];
                        desc.event_type = EVENT_TYPE_MOVE;
                        desc.mouse_pressed_point = context->get_cur_pressed_point(touch_id);
                        desc.mouse_current_point = context->get_cur_move_point(touch_id);
                        desc.mouse_farthest_point = context->get_farthest_move_point(touch_id);
                        desc.key_modifier = key_modifier;

                        if (handler->on_event_drag_state_changed(desc) && context->get_magnifier_enabled()) {
                            update_magnifier = TRUE;
                        }
                    }
                    context->set_prev_move_point(touch_id, originx, originy);
                }
                if (drag_state_changed) {
                    /* When the dragging direction changes, save the current position as farthest point for future comparison */
                    context->set_farthest_move_point(touch_id, originx, originy);
                    printf("SET_FARTHEST : %d %d %d\n", originx, originy, context->get_cur_drag_state(touch_id));
                }
            }

            if (update_magnifier) {
                SclResParserManager *sclres_manager = SclResParserManager::get_instance();
                PSclMagnifierWndConfigure magnifier_configure = NULL;
                if (sclres_manager) {
                    magnifier_configure = sclres_manager->get_magnifier_configure();
                }

                const SclLayout *layout = cache->get_cur_layout(windows->get_base_window());
                if (layout && magnifier_configure) {
                    sclfloat scale_rate_x, scale_rate_y;
                    if (layout->display_mode == DISPLAYMODE_PORTRAIT) {
                        scale_rate_x = utils->get_scale_rate_x();
                        scale_rate_y = utils->get_scale_rate_y();
                    } else {
                        scale_rate_x = utils->get_scale_rate_y();
                        scale_rate_y = utils->get_scale_rate_x();
                    }

                    SclPoint zoomwinpos = {0,};
                    /* calculates x position to be set */
                    zoomwinpos.x = (coordinate->x + (coordinate->width / 2)) -
                        (magnifier_configure->width / 2);

                    /* calculates y position to be set */
                    sclint scnWidth, scnHeight;
                    utils->get_screen_resolution(&scnWidth, &scnHeight);

                    zoomwinpos.y =  coordinate->y - magnifier_configure->height;
                    SclWindowContext *winctx = windows->get_window_context(window);
                    if (winctx) {
                        zoomwinpos.x += winctx->geometry.x;
                        zoomwinpos.y += winctx->geometry.y;
                    }
                    if (zoomwinpos.x < 0 - magnifier_configure->padding_x) {
                        zoomwinpos.x = 0 - magnifier_configure->padding_x;
                    }
                    if (zoomwinpos.x > scnWidth + magnifier_configure->padding_x - magnifier_configure->width) {
                        zoomwinpos.x = scnWidth + magnifier_configure->padding_x - magnifier_configure->width;
                    }
                    zoomwinpos.y += magnifier_configure->padding_y;

                    zoomwinpos.x += coordinate->magnifier_offset_x;
                    zoomwinpos.y += coordinate->magnifier_offset_y;
                    windows->move_window(windows->get_magnifier_window(), zoomwinpos.x, zoomwinpos.y);
                    windows->show_window(windows->get_magnifier_window(), 0);
                }
            }
        }

        sclboolean grab_event = FALSE;
        if (layout) {
            if (layout->style == LAYOUT_STYLE_POPUP_GRAB) {
                grab_event = TRUE;
            }
            /* If the topmost window has the POPUP_GRAB style, find the nearest button to the mouse pointer */
            if (grab_event && winctx) {
                /* If the layout's addGrab* values are defined, process this event only if the event occured inside grab area */
                sclboolean in_grab_area = TRUE;
                if (layout->add_grab_left != NOT_USED && x < -(layout->add_grab_left)) {
                    in_grab_area = FALSE;
                }
                if (layout->add_grab_right != NOT_USED && x > (winctx->geometry.width + layout->add_grab_right)) {
                    in_grab_area = FALSE;
                }
                if (layout->add_grab_top != NOT_USED && y < -(layout->add_grab_top)) {
                    in_grab_area = FALSE;
                }
                if (layout->add_grab_bottom != NOT_USED && y > (winctx->geometry.height + layout->add_grab_bottom)) {
                    in_grab_area = FALSE;
                }
                if (in_grab_area) {
                    CSCLResourceCache *cache = CSCLResourceCache::get_instance();
                    if (cache) {
                        float min_dist = (float)((unsigned int)(-1));
                        int min_dist_index = NOT_USED;
                        for (int loop = 0;loop < MAX_KEY && !ended && !ret;loop++) {
                            btncontext = cache->get_cur_button_context(window, loop);
                            const SclLayoutKeyCoordinate *coordinate = cache->get_cur_layout_key_coordinate(window, loop);
                            if (btncontext && coordinate) {
                                if (!(btncontext->used)) {
                                    ended = TRUE;
                                } else if (btncontext->state != BUTTON_STATE_DISABLED &&
                                    coordinate->button_type != BUTTON_TYPE_UIITEM) {
                                        const SclLayoutKeyCoordinate *coordinate = cache->get_cur_layout_key_coordinate(window, loop);
                                        if (coordinate) {
                                            float dist = utils->get_approximate_distance(x, y,
                                                    coordinate->x + (coordinate->width / 2), coordinate->y + (coordinate->height / 2));
                                            if (dist < min_dist) {
                                                min_dist_index = loop;
                                                min_dist = dist;
                                            }
                                        }
                                }
                            }
                        }
                        /* When we found the nearest button, generate this event on the button */
                        if (min_dist_index != NOT_USED) {
                            const SclLayoutKeyCoordinate *coordinate = cache->get_cur_layout_key_coordinate(window, min_dist_index);
                            x = coordinate->x + (coordinate->width / 2);
                            y = coordinate->y + (coordinate->height / 2);
                            if (process_button_move_event(window, x, y, min_dist_index, touch_id, actual_event)) {
                                ret = TRUE;
                            }
                        }
                    }
                }
            } else {
                MultiTouchContext *mulctx = context->find_multi_touch_context(touch_id);
                if (mulctx) {
                    if (!(mulctx->is_sub_event)) {
                        /* First check if the event occured in pressed key's threshold area */
                        if (btncontext && coordinate) {
                            if (pressed_window == window) { // Check only when the window is the one initally pressed
                                if (btncontext->used && btncontext->state != BUTTON_STATE_DISABLED) {
                                    if (process_button_move_event(pressed_window, x, y, pressed_key, touch_id, actual_event)) {
                                        ret = TRUE;
                                        x = coordinate->x + (coordinate->width / 2);
                                        y = coordinate->y + (coordinate->height / 2);
                                    }
                                }
                            }
                        }
                        for (int loop = 0;loop < MAX_KEY && !ended && !ret;loop++) {
                            btncontext = cache->get_cur_button_context(window, loop);
                            const SclLayoutKeyCoordinate *coordinate = cache->get_cur_layout_key_coordinate(window, loop);
                            if (btncontext && coordinate) {
                                if (!(btncontext->used)) {
                                    ended = TRUE;
                                } else if (btncontext->state != BUTTON_STATE_DISABLED &&
                                            coordinate->button_type != BUTTON_TYPE_UIITEM) {
                                    if (window != pressed_window || loop != pressed_key) {
                                        if (process_button_move_event(window, x, y, loop, touch_id, actual_event)) {
                                            ret = TRUE;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return ret;
}

sclboolean
CSCLController::mouse_over(sclwindow window, sclint x, sclint y)
{
    SCL_DEBUG();
    sclboolean ret = FALSE;

    if(m_input_events_disabled)
        return FALSE;

    /* Adjust x,y coordinate by touch offset */
    CSCLErrorAdjustment *adjustment = CSCLErrorAdjustment::get_instance();
    /* Iterate all the buttons and inform the event */

    CSCLContext *context = CSCLContext::get_instance();
    CSCLResourceCache *cache = CSCLResourceCache::get_instance();
    CSCLActionState *state = CSCLActionState::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLUtils *utils = CSCLUtils::get_instance();
    SclResParserManager *sclres_manager = SclResParserManager::get_instance();

    if (cache && state && windows && context && utils && adjustment && sclres_manager) {
        const SclLayout *layout = cache->get_cur_layout(window);
        if (layout) {
            x += layout->mouse_manipulate_x;
            y += layout->mouse_manipulate_y;
        }

        SCLDisplayMode cur_display_mode = context->get_display_mode();

        const SclDefaultConfigure *default_configure = sclres_manager->get_default_configure();
        if (default_configure) {
            adjustment->apply_touch_offset(default_configure->touch_offset_level[cur_display_mode], &x, &y);
        }

        /* Iterate all the buttons and inform the event */
        sclboolean ended = FALSE;

        for (int loop = 0; loop < MAX_KEY && !ended && !ret; loop++) {
            SclButtonContext *btncontext = cache->get_cur_button_context(window, loop);
            const SclLayoutKeyCoordinate *coordinate = cache->get_cur_layout_key_coordinate(window, loop);
            if (btncontext && coordinate) {
                if (!(btncontext->used)) {
                    ended = TRUE;
                } else if (btncontext->state != BUTTON_STATE_DISABLED &&
                            coordinate->button_type != BUTTON_TYPE_UIITEM) {
                    if (process_button_over_event(window, x, y, loop)) {
                        ret = TRUE;
                    }
                }
            }
        }
    }

    return ret;
}


/**
 * Processes a timer event
 * If return FALSE, the current timer will be stop
 * ID : SCL_LOWORD(data)
 * value : SCL_HIWORD(data)
 */
sclboolean
CSCLController::timer_event(const scl32 data)
{
    struct my_srtuct {
        struct typeA {short x; short y;};
        struct typeB {short x; short y;};
    } strt;
    SCL_DEBUG();
    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLContext *context = CSCLContext::get_instance();
    CSCLEvents* events = CSCLEvents::get_instance();
    CSCLActionState *state = CSCLActionState::get_instance();
    CSCLResourceCache *cache = CSCLResourceCache::get_instance();
    CSCLEventHandler *handler = CSCLEventHandler::get_instance();

    scl16 id = SCL_LOWORD(data); /* Timer ID */
    scl16 value = SCL_HIWORD(data); /* event unique ID */

    switch (id) {
    case SCL_TIMER_AUTOPOPUP: {
        /* Checks whether my event id is availble */
        if (context->get_cur_pressed_event_id(context->get_last_touch_device_id()) != value ||
                state->get_cur_action_state() == ACTION_STATE_BASE_INIT ||
                //state->get_cur_action_state() == ACTION_STATE_BASE_MOVING ||
                state->get_cur_action_state() == ACTION_STATE_BASE_RELEASE ||
                state->get_cur_action_state() == ACTION_STATE_BASE_LONGKEY ||
                state->get_cur_action_state() == ACTION_STATE_BASE_REPEATKEY ||
                state->get_cur_action_state() == ACTION_STATE_POPUP_INIT ||
                //state->get_cur_action_state() == ACTION_STATE_POPUP_MOVING ||
                state->get_cur_action_state() == ACTION_STATE_POPUP_RELEASE ||
                state->get_cur_action_state() == ACTION_STATE_POPUP_LONGKEY ||
                state->get_cur_action_state() == ACTION_STATE_POPUP_REPEATKEY
           ) {
            /* Ignores if the event id is different */
            return FALSE;
        }

        SclRectangle rect = {0,};
        sclwindow window = context->get_cur_pressed_window(context->get_last_touch_device_id());
        sclbyte keyIndex = context->get_cur_pressed_key(context->get_last_touch_device_id());

        if (configure_autopopup_window(window, keyIndex, &rect)) {
            /* Let's change out pressed button's state back to normal */
            SclButtonContext *btncontext = cache->get_cur_button_context(window, keyIndex);
            const SclLayoutKeyCoordinate *coordinate = cache->get_cur_layout_key_coordinate(window, keyIndex);

            if (btncontext) {
                if (btncontext->state == BUTTON_STATE_PRESSED) {
                    btncontext->state = BUTTON_STATE_NORMAL;
                    CSCLWindows *windows = CSCLWindows::get_instance();
                    if (windows && coordinate) {
                        windows->update_window(window, coordinate->x, coordinate->y, coordinate->width, coordinate->height);
                    }
                }
            }
            /* Hide magnifier window when opening autopopup window */
            windows->hide_window(windows->get_magnifier_window());
            /* Currently, window does not support virtual window */
            SclWindowOpener opener;
            opener.window = window;
            opener.key = keyIndex;
            sclwindow popup_window = windows->open_popup(
                opener,
                rect,
                NOT_USED,
                SCL_LAYOUT_AUTOPOPUP, POPUP_TYPE_AUTO_POPUP,
                FALSE,
                FALSE
            );
            windows->hide_window(windows->get_magnifier_window());
            _play_tts_for_layout_autopopup_name();

            sclwindow move_window = context->get_cur_move_window(context->get_last_touch_device_id());
            SclPoint move_point = context->get_cur_move_point(context->get_last_touch_device_id());
            SclWindowContext *move_ctx = windows->get_window_context(move_window);
            SclWindowContext *popup_ctx = windows->get_window_context(popup_window);
            if (move_ctx && popup_ctx) {
                move_point.x = (move_ctx->geometry.x - popup_ctx->geometry.x) + move_point.x;
                move_point.y = (move_ctx->geometry.y - popup_ctx->geometry.y) + move_point.y;
            }
            printf("AUTOPOPUP : %d %d\n", move_point.x, move_point.y);

            CSCLWindows *windows = CSCLWindows::get_instance();
            if (windows && coordinate) {
                windows->update_window(window, coordinate->x, coordinate->y, coordinate->width, coordinate->height);
            }
        }
        events->destroy_timer(id);

        return FALSE;
    }
    break;

    case SCL_TIMER_LONGKEY: {
        /* Checks whether my event id is availble */
        if (context->get_cur_pressed_event_id(context->get_last_touch_device_id()) != value ||
                state->get_cur_action_state() == ACTION_STATE_BASE_INIT ||
                //state->get_cur_action_state() == ACTION_STATE_BASE_MOVING ||
                state->get_cur_action_state() == ACTION_STATE_BASE_RELEASE ||
                state->get_cur_action_state() == ACTION_STATE_BASE_LONGKEY ||
                state->get_cur_action_state() == ACTION_STATE_BASE_REPEATKEY ||
                state->get_cur_action_state() == ACTION_STATE_POPUP_INIT ||
                //state->get_cur_action_state() == ACTION_STATE_POPUP_MOVING ||
                state->get_cur_action_state() == ACTION_STATE_POPUP_RELEASE ||
                state->get_cur_action_state() == ACTION_STATE_POPUP_LONGKEY ||
                state->get_cur_action_state() == ACTION_STATE_POPUP_REPEATKEY
           ) {
            /* Ignores if the event id is different */
            return FALSE;
        }
        /* Ignores if the event id is different */
        sclwindow window = context->get_cur_pressed_window(context->get_last_touch_device_id());
        sclbyte key_index = context->get_cur_pressed_key(context->get_last_touch_device_id());
        if (process_button_long_pressed_event(window, key_index, context->get_last_touch_device_id())) {
            /* The button processed long key event, now enter longkey mode not to fire any events before releasing */
            handle_engine_signal(SCL_SIG_MOUSE_LONG_PRESS, window);
            windows->update_window(windows->get_magnifier_window());
/*
            SclButtonContext *btncontext = cache->get_cur_button_context(window, key_index);
            if (btncontext->state == BUTTON_STATE_PRESSED) {
                btncontext->state = BUTTON_STATE_NORMAL;
                CSCLWindows *windows = CSCLWindows::get_instance();
                if (windows) {
                    const SclLayoutKeyCoordinate *coordinate = cache->get_cur_layout_key_coordinate(window, key_index);
                    windows->update_window(window, coordinate->x, coordinate->y, coordinate->width, coordinate->height);
                }
            }
            context->set_cur_pressed_window(context->get_last_touch_device_id(), SCLWINDOW_INVALID);
            context->set_cur_pressed_key(context->get_last_touch_device_id(), NOT_USED);
*/
        } else {
            /* Start the repeat key timer for NORMAL or GRAB buttons if longkey is not supported */
            const SclLayoutKeyCoordinate *coordinate = cache->get_cur_layout_key_coordinate(window, key_index);

            if (coordinate) {
                /* This is for enabling backspace key in search layout*/
                //if (coordinate->use_repeat_key) {
                if (coordinate->use_repeat_key
                        || coordinate->key_event[0][0] == MVK_BackSpace) {
                    if (coordinate->button_type == BUTTON_TYPE_NORMAL ||
                            coordinate->button_type == BUTTON_TYPE_GRAB ||
                            coordinate->button_type == BUTTON_TYPE_SELFISH ||
                            coordinate->button_type == BUTTON_TYPE_DIRECTION ||
                            coordinate->button_type == BUTTON_TYPE_RELATIVE_DIRECTION) {
                        m_key_repeated_num = 0;
                        events->create_timer(SCL_TIMER_REPEATKEY, m_repeat_key_duration, value);
                        if (windows->is_base_window(window)) {
                            state->set_cur_action_state(ACTION_STATE_BASE_REPEATKEY);
                        } else {
                            state->set_cur_action_state(ACTION_STATE_POPUP_REPEATKEY);
                        }
                    }
                }
            }
        }
        events->destroy_timer(id);
        return FALSE;
    }
    break;

    case SCL_TIMER_REPEATKEY: {
        /* Checks whether my event id is availble */
        if (context->get_cur_pressed_event_id(context->get_last_touch_device_id()) != value ||
                (state->get_cur_action_state() != ACTION_STATE_BASE_REPEATKEY &&
                 state->get_cur_action_state() != ACTION_STATE_POPUP_REPEATKEY)
           ) {
            /* Ignores if the event id is different */
            return FALSE;
        }
        sclwindow window = context->get_cur_pressed_window(context->get_last_touch_device_id());
        sclbyte keyIndex = context->get_cur_pressed_key(context->get_last_touch_device_id());
        scllong interval = m_repeat_key_duration - (m_key_repeated_num * SCL_REPEATKEY_ACCELERATION);
        if (interval < SCL_REPEATKEY_MIN_DURATION) {
            interval = SCL_REPEATKEY_MIN_DURATION;
        }
        process_button_repeat_pressed_event(window, keyIndex, context->get_last_touch_device_id());
        events->destroy_timer(id);
        events->create_timer(SCL_TIMER_REPEATKEY, interval, value);
        m_key_repeated_num++;
        return FALSE;
    }
    break;
    case SCL_TIMER_BUTTON_DELAY: {
        /* If there is postponed update of button, update it now */
        sclwindow last_window = context->get_last_pressed_window();
        scl8 last_key = context->get_last_pressed_key();

        if (last_window != SCLWINDOW_INVALID && last_key != NOT_USED) {
            const SclLayoutKeyCoordinate* coords = cache->get_cur_layout_key_coordinate(last_window, last_key);
            if (coords) {
                windows->update_window(last_window, coords->x, coords->y, coords->width, coords->height);
            }
        }

        windows->hide_window(windows->get_magnifier_window());
        context->set_last_pressed_window(SCLWINDOW_INVALID);
        context->set_last_pressed_key(NOT_USED);
        events->destroy_timer(id);
        return FALSE;
    }
    break;
    case SCL_TIMER_POPUP_TIMEOUT: {
        windows->close_all_popups();

        handler->on_event_notification(SCL_UINOTITYPE_POPUP_TIMEOUT, data);
        events->destroy_timer(id);
        return FALSE;
    }
    break;
    case SCL_TIMER_AUTOTEST: {
        CSCLResourceCache *cache = CSCLResourceCache::get_instance();
        SCLDisplayMode display_mode =  context->get_display_mode();

        sclint rnd = rand() % 100;
        sclint x = (rand() % (cache->get_cur_layout(windows->get_base_window())->width));
        sclint y = (rand() % (cache->get_cur_layout(windows->get_base_window())->height));

        if (rnd < 80) {
            events->generate_mouse_event(SCL_MOUSE_EVENT_PRESS, x, y);
            events->generate_mouse_event(SCL_MOUSE_EVENT_RELEASE, x, y);
        } else if (rnd < 90) {
            events->generate_mouse_event(SCL_MOUSE_EVENT_MOVE, x, y);
        } else if (rnd < 95) {
            events->generate_mouse_event(SCL_MOUSE_EVENT_PRESS, x, y);
        } else {
            events->generate_mouse_event(SCL_MOUSE_EVENT_RELEASE, x, y);
        }

        m_debug_variable++;
        if (m_debug_variable < SCL_AUTOTEST_NUM) events->create_timer(SCL_TIMER_AUTOTEST, SCL_AUTOTEST_TIMER_INTERVAL, 0, FALSE);
        else m_debug_mode = DEBUGMODE_DISABLED;
        return FALSE;
    }
    break;

    default: {
        events->destroy_timer(id);
    }
    break;
    }

    return TRUE;
}

/* Handles signals to manage contexts mainly focusing on resetting variables and cleaning up states */
void CSCLController::handle_engine_signal( SclInternalSignal signal, sclwindow targetWindow )
{
    SCL_DEBUG();

    enum SIGACTIONS {
        SIGACTION_RESIZE_RESOURCES,
        SIGACTION_DESTROY_TIMERS,
        SIGACTION_CLEAR_PRIVATEKEYS,
        SIGACTION_RECOMPUTE_LAYOUT,
        SIGACTION_FREE_IMAGES,
        SIGACTION_CLOSE_POPUP,
        SIGACTION_CLOSE_MAGNIFIER,
        SIGACTION_UNSET_SHIFT,
        SIGACTION_UNPRESS_KEYS,
        SIGACTION_INIT_DISPLAY,
        SIGACTION_INIT_INPUTMODE,

        SIGACTION_MAXNUM
    };
    const sclboolean SIGNAL_TABLE[SIGACTION_MAXNUM][SCL_SIG_MAXNUM] = {
        //	START,	SHOW,	HIDE,	INPCHNG,	DISPCHNG,	POPUPSHOW,	POPUPHIDE,	MOUSEPRES,	M-LONGPRES,	MOUSEREL,	KEYEVT,	FOCUSCHNG
        // SIGACTION_RESIZE_RESOURCES
        {	TRUE,	0,		0,		0,			0,			0,			0,			0,			0,			0,			0,		0		},
        // SIGACTION_DESTROY_TIMERS
        {	TRUE,	TRUE,	TRUE,	TRUE,		TRUE,		0,			0,			0,			0,			0,			0,		TRUE	},
        // SIGACTION_CLEAR_PRIVATEKEYS
        {	TRUE,	0,		0,		0,			0,			0,			0,			0,			0,			0,			0,		TRUE	},
        // SIGACTION_RECOMPUTE_LAYOUT
        {	0,		TRUE,	0,		TRUE,		TRUE,		TRUE,		0,			0,			0,			0,			0,		TRUE	},
        // SIGACTION_FREE_IMAGES
        {	TRUE,	0,		TRUE,	TRUE,		TRUE,		0,			0,			0,			0,			0,			0,		0		},
        // SIGACTION_CLOSE_POPUP
        {	TRUE,	TRUE,	TRUE,	TRUE,		TRUE,		0,			0,			0,			0,			0,			0,		TRUE	},
        // SIGACTION_CLOSE_MAGNIFIER
        {	TRUE,	TRUE,	TRUE,	TRUE,		TRUE,		0,			0,			0,			0,			0,			0,		TRUE	},
        // SIGACTION_UNSET_SHIFT
        {	TRUE,	0	,	0	,	TRUE,		0,			0,			0,			0,			0,			0,			0,		TRUE	},
        // SIGACTION_UNPRESS_KEYS
        {	TRUE,	TRUE,	TRUE,	TRUE,		TRUE,		0,			0,			0,			0,			0,			0,		TRUE	},
        // SIGACTION_INIT_DISPLAY
        {	TRUE,	0,		0,		0,			0,			0,			0,			0,			0,			0,			0,		0		},
        // SIGACTION_INIT_INPUTMODE
        {	TRUE,	0,		0,		0,			0,			0,			0,			0,			0,			0,			0,		TRUE	},
    };

    scl_assert_return(signal >= 0 && signal < SCL_SIG_MAXNUM);

    CSCLEvents* events = CSCLEvents::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLContext *context = CSCLContext::get_instance();
    CSCLImageProxy *proxy = CSCLImageProxy::get_instance();
    CSCLResourceCache *cache = CSCLResourceCache::get_instance();

    switch (signal) {
        case SCL_SIG_SHOW:
            {
                context->set_hidden_state(FALSE);
            }
            break;
        case SCL_SIG_HIDE:
            {
                context->set_hidden_state(TRUE);
                windows->close_all_popups();
            }
            break;
    }

    int loop = 0;
    for (loop = 0;loop < SIGACTION_MAXNUM;loop++) {
        if (SIGNAL_TABLE[loop][signal] == TRUE) {
            switch (loop) {
            case SIGACTION_RESIZE_RESOURCES:
                break;
            case SIGACTION_DESTROY_TIMERS:
                events->destroy_all_timer();
                break;
            case SIGACTION_CLEAR_PRIVATEKEYS:
                break;
            case SIGACTION_RECOMPUTE_LAYOUT: {
                if (targetWindow != SCLWINDOW_INVALID) {
                    cache->recompute_layout(targetWindow);
                    // EFL testing
                    windows->update_window(targetWindow);
                }
            }
            break;
            case SIGACTION_FREE_IMAGES:
                proxy->free_images();
                break;
            case SIGACTION_CLOSE_POPUP: {
                /* If there is a popup still opened, don't destroy POPUP_TIMEOUT timer */
                if (!(windows->close_all_popups(targetWindow))) {
                    events->destroy_timer(SCL_TIMER_POPUP_TIMEOUT);
                }
            }
            break;
            case SIGACTION_CLOSE_MAGNIFIER: {
                if (signal == SCL_SIG_HIDE) {
                    windows->hide_window(windows->get_magnifier_window(), TRUE);
                } else {
                    windows->hide_window(windows->get_magnifier_window());
                }
            }
            //events->create_timer(SCL_TIMER_BUTTON_DELAY, SCL_BUTTON_MIN_DURATION, 0);
            break;
            case SIGACTION_UNSET_SHIFT: {
                CSCLEventHandler *handler = CSCLEventHandler::get_instance();
                if (handler) {
                    if (SCL_EVENT_PASS_ON ==
                        handler->on_event_notification(SCL_UINOTITYPE_SHIFT_STATE_CHANGE, SCL_SHIFT_STATE_OFF)) {
                        context->set_shift_state(SCL_SHIFT_STATE_OFF);
                    }
                }
            }
            break;
            case SIGACTION_UNPRESS_KEYS:
                context->set_cur_pressed_key(context->get_last_touch_device_id(), NOT_USED);
                context->set_cur_pressed_window(context->get_last_touch_device_id(), SCLWINDOW_INVALID);
            break;
            case SIGACTION_INIT_DISPLAY:
            break;
            case SIGACTION_INIT_INPUTMODE:
            break;
            }
        }
    }
}

/**
 * Sets the duration value for longkey
 * If not set, it will use default longkey duration. see sclconfig
 */
sclboolean
CSCLController::set_longkey_duration(scllong msc)
{
    SCL_DEBUG();
    sclboolean ret = FALSE;
    if (msc > 0) {
        m_long_key_duration = msc;
        ret = TRUE;
    }
    return ret;
}

/**
* Sets the distance value for cancel longkey
* If not set, it will use default longkey duration. see sclconfig
*/
sclboolean
CSCLController::set_longkey_cancel_dist(sclshort dist)
{
    SCL_DEBUG();
    sclboolean ret = FALSE;
    if (dist > 0) {
        m_long_key_cancel_distance = dist;
        ret = TRUE;
    }
    return ret;
}

/**
* Sets the duration value for repeatkey
* If not set, it will use default repeatkey duration. see sclconfig
*/
sclboolean
CSCLController::set_repeatkey_duration(scllong msc)
{
    SCL_DEBUG();
    sclboolean ret = FALSE;
    if (msc > 0) {
        m_repeat_key_duration = msc;
        ret = TRUE;
    }
    return ret;
}

/**
 * Sets the duration value for autopopup key
 * If not set, it will use default short longkey duration. see sclconfig
 */
sclboolean
CSCLController::set_autopopup_key_duration(scllong msc)
{
    SCL_DEBUG();
    sclboolean ret = FALSE;
    if (msc > 0) {
        m_autopopup_key_duration = msc;
        ret = TRUE;
    }
    return ret;
}

/**
 * Sets the amount value for button delay
 * If not set, it will use default button delay amount. see sclconfig
 */
sclboolean
CSCLController::set_button_delay_duration(scllong msc)
{
    SCL_DEBUG();
    sclboolean ret = FALSE;
    if (msc > 0) {
        m_button_delay_duration = msc;
        ret = TRUE;
    }
    return ret;
}

/**
 * Configures the variables for auto-popup window
 * It will return rectangle area
 * @return FALSE It's not avaiable popup key
 */
sclboolean
CSCLController::configure_autopopup_window(sclwindow window, sclbyte key_index, SclRectangle* rect)
{
    SCL_DEBUG();
    scl_assert_return_false(window);
    scl_assert_return_false(key_index >= 0);

    sclboolean ret = TRUE;

    sclbyte num_keys, num_columns, num_rows;

    CSCLUtils *utils = CSCLUtils::get_instance();
    CSCLContext *context = CSCLContext::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLResourceCache *cache = CSCLResourceCache::get_instance();

    const SclLayoutKeyCoordinate *coordinate = NULL;

    if (cache) {
        coordinate = cache->get_cur_layout_key_coordinate(window, key_index);
    }

    SclResParserManager *sclres_manager = SclResParserManager::get_instance();
    PSclAutoPopupConfigure autopopup_configure = NULL;
    if (sclres_manager) {
        autopopup_configure = sclres_manager->get_autopopup_configure();
    }

    if (utils && context && windows && cache && coordinate && rect && autopopup_configure) {
        SCLShiftState shiftidx = context->get_shift_state();
        if (shiftidx < 0 || shiftidx >= SCL_SHIFT_STATE_MAX) shiftidx = SCL_SHIFT_STATE_OFF;
        if (utils->get_autopopup_window_variables(coordinate->autopopup_key_labels[shiftidx],
                &num_keys, &num_columns, &num_rows, &rect->width, &rect->height)) {

            /* There is no need for an autopopup window if number of keys are equal to or less than 0 */
            if (!(num_keys > 0)) {
                ret = FALSE;
            }
            /* calculates y position to be set */
            SclRectangle baseWndRect;
            int scrwidth, scrheight;
            utils->get_screen_resolution(&scrwidth, &scrheight);

            windows->get_window_rect(windows->get_base_window(), &baseWndRect);
            /* Let the autopopup have its position right above the pressed button, with center alignment) */
            rect->x = baseWndRect.x + coordinate->x + (coordinate->width / 2) - (rect->width / 2);
            rect->y = baseWndRect.y + coordinate->y - rect->height + autopopup_configure->decoration_size;
            /* First check the growing direction of this autopopup window */
            if (coordinate->x < baseWndRect.width / 2) {
                /* We're growing left to right, caculate the left start point */
                rect->x = baseWndRect.x + coordinate->x + (coordinate->width / 2) -
                    (autopopup_configure->button_width / 2) - autopopup_configure->bg_padding;
                if (rect->x + rect->width > baseWndRect.x + baseWndRect.width) {
                    sclint relocate_unit = autopopup_configure->button_width +
                        autopopup_configure->button_spacing;
                    rect->x -= (((rect->x + rect->width - (baseWndRect.x + baseWndRect.width)) /
                            relocate_unit) + 1) * relocate_unit;
                }
            } else {
                /* We're growing right to left, caculate the right end point */
                rect->x = baseWndRect.x + coordinate->x + (coordinate->width / 2) +
                    (autopopup_configure->button_width / 2) + autopopup_configure->bg_padding;
                rect->x -= rect->width;
                if (rect->x < baseWndRect.x) {
                    sclint relocate_unit = autopopup_configure->button_width +
                        autopopup_configure->button_spacing;
                    rect->x += (((baseWndRect.x - rect->x) /
                            relocate_unit) + 1) * relocate_unit;
                }
            }
            //rect->y = (scrheight - layout->height) + coordinate->y - rect->height + autopopup_configure->decoration_size;
            /* Check if the window goes out of screen boundary */
            //if (rect->x + rect->width > scrwidth + utils->get_scale_x(scl_autopopup_configure.decoration_size)) rect->x = (scrwidth + utils->get_scale_x(scl_autopopup_configure.decoration_size)) - rect->width;
            if (rect->x + rect->width > scrwidth) rect->x = (scrwidth) - rect->width;
            if (rect->y + rect->height > scrheight) rect->y = scrheight - rect->height;
            if (rect->x < 0 - autopopup_configure->decoration_size) rect->x = 0 - autopopup_configure->decoration_size;
            // restrict to 0
            if (rect->x < 0) rect->x = 0;
            if (rect->y < 0) rect->y = 0;
        } else {
            ret = FALSE;
        }
    }
    return ret;
}

/**
 * If the mouse was pressed on the button A and moved to B without releasing,
 * this function decides whether we should allow event transition, meaning
 * button A gets restored to its initial state and B gets pressed instead.
 */
sclboolean
CSCLController::check_event_transition_enabled(const SclLayoutKeyCoordinate *btnFrom, const SclLayoutKeyCoordinate *btnTo)
{
    sclboolean ret = FALSE;
    sclbyte typeFrom = BUTTON_TYPE_NORMAL; /* To enable event transition even if no button is pressed currently */
    sclbyte typeTo = MAX_BUTTON_TYPE;

    const sclboolean TRANSITION_TABLE[MAX_BUTTON_TYPE][MAX_BUTTON_TYPE] = {
        //	NORMAL	GRAB	SELFISH	DRAG	MULTITAP	ROTATION	DIRECTION	R_DIRECTION	UIITEM
        //	From : NORMAL
        {	TRUE,	0,	0,		TRUE,	TRUE,		TRUE,		0,			0,			0},
        //	From : GRAB
        {	0,		0,	0,		0,		0,			0,			0,			0,			0},
        //	From : SELFISH
        {	0,		0,	0,		0,		0,			0,			0,			0,			0},
        //	From : DRAG
        {	TRUE,	0,	0,		TRUE,	TRUE,		TRUE,		0,			0,			0},
        //	From : MULTITAP
        {	TRUE,	0,	0,		TRUE,	TRUE,		TRUE,		0,			0,			0},
        //	From : ROTATION
        {	TRUE,	0,	0,		TRUE,	TRUE,		TRUE,		0,			0,			0},
        //	From : DIRECTION
        {	0,		0,	0,		0,		0,			0,			0,			0,			0},
        //	From : R_DIRECTION
        {	0,		0,	0,		0,		0,			0,			0,			0,			0},
        //	From : UIITEM
        {	0,		0,	0,		0,		0,			0,			0,			0,			0},
    };

    if (btnFrom) typeFrom = btnFrom->button_type;
    if (btnTo) typeTo = btnTo->button_type;

    scl_assert_return_false(typeFrom >= 0 && typeFrom < MAX_BUTTON_TYPE);
    scl_assert_return_false(typeTo >= 0 && typeTo < MAX_BUTTON_TYPE);

    if (typeFrom < MAX_BUTTON_TYPE && typeTo < MAX_BUTTON_TYPE) {
        ret = TRANSITION_TABLE[typeFrom][typeTo];
    }

    return ret;
}

SCLDebugMode
CSCLController::get_debug_mode()
{
#ifdef SCL_DEBUG_ON
    return m_debug_mode;
#else
    return m_debug_mode;
    return DEBUGMODE_DISABLED;
#endif
}

void
CSCLController::set_debug_mode(SCLDebugMode mode)
{
    CSCLEvents *events = CSCLEvents::get_instance();
    CSCLUtils *utils = CSCLUtils::get_instance();

    m_debug_mode = mode;
    m_debug_variable = 0;

    if (m_debug_mode == DEBUGMODE_AUTOTEST) {
        srand(time(NULL));
        if (events && utils) {
            events->create_timer(SCL_TIMER_AUTOTEST, SCL_AUTOTEST_TIMER_INITIAL_INTERVAL, 0, FALSE);
            utils->log("mode : %d\n", mode);
        }
    }
}

void
CSCLController::disable_input_events(sclboolean disabled)
{
    m_input_events_disabled = disabled;
}
