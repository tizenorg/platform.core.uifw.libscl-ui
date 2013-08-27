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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scluibuilder.h"
#include "scldebug.h"
#include "sclresourcecache.h"
#include "sclcontext.h"
#include "sclimageproxy.h"
#include "sclactionstate.h"

//#include "sclresource.h"
#include <assert.h>
#include "sclres_manager.h"

using namespace scl;

//extern sclboolean g_key_spacing_off;

CSCLUIBuilder* CSCLUIBuilder::m_instance = NULL; /* For singleton */

CSCLUIBuilder::CSCLUIBuilder()
{
    SCL_DEBUG();

    m_gwes = NULL;
    m_utils = NULL;
}

CSCLUIBuilder::~CSCLUIBuilder()
{
    SCL_DEBUG();
    if (m_gwes) delete m_gwes;
    m_gwes = NULL;
    if (m_utils) delete m_utils;
    m_utils = NULL;
}

CSCLUIBuilder*
CSCLUIBuilder::get_instance()
{
    if (!m_instance) {
        m_instance = new CSCLUIBuilder();
    }
    return (CSCLUIBuilder*)m_instance;
}

void
CSCLUIBuilder::init(sclwindow parent)
{
    SCL_DEBUG();

    SclResParserManager *sclres_manager = SclResParserManager::get_instance();

    m_gwes = CSCLGwes::get_instance();
    m_utils = CSCLUtils::get_instance();
    /* It will create the base window */

    scl8 mode = NOT_USED;
    sclbyte layout = NOT_USED;
    SCLDisplayMode display_mode = DISPLAYMODE_PORTRAIT;

    PSclDefaultConfigure default_configure = NULL;
    PSclMagnifierWndConfigure magnifier_configure = NULL;
    if (sclres_manager) {
        PSclInputModeConfigure sclres_input_mode_configure = sclres_manager->get_input_mode_configure_table();
        PSclLayout sclres_layout = sclres_manager->get_layout_table();

        assert(sclres_input_mode_configure != NULL);
        assert(sclres_layout != NULL);

        default_configure = sclres_manager->get_default_configure();
        magnifier_configure = sclres_manager->get_magnifier_configure();
        if (default_configure) {
            display_mode = default_configure->display_mode;
            mode = sclres_manager->get_inputmode_id(default_configure->input_mode);
        }
        /* Make sure these variables does not exceed the size of our array */
        if (!scl_check_arrindex(mode, MAX_SCL_INPUT_MODE) || !scl_check_arrindex(mode, sclres_manager->get_inputmode_size())) {
            mode = 0;
        }
        if (!scl_check_arrindex(display_mode, DISPLAYMODE_MAX)) {
            display_mode = DISPLAYMODE_PORTRAIT;
        }

        layout = sclres_manager->get_layout_id(sclres_input_mode_configure[mode].layouts[display_mode]);
        if (!scl_check_arrindex_unsigned(layout, MAX_SCL_LAYOUT) ||
            !scl_check_arrindex_unsigned(layout, sclres_manager->get_layout_size())) {
            layout = 0;
        }
        m_gwes->init(parent,
                    m_utils->get_scaled_x(sclres_layout[layout].width),
                    m_utils->get_scaled_y(sclres_layout[layout].height)
                   );

        CSCLContext *context = CSCLContext::get_instance();
        context->set_display_mode(display_mode);
        context->set_input_mode(mode);
        context->set_base_layout(sclres_manager->get_layout_id(sclres_input_mode_configure[mode].layouts[display_mode]));

        CSCLResourceCache *cache = CSCLResourceCache::get_instance();
        sclwindow window = m_gwes->m_windows->get_base_window();
        cache->recompute_layout(window);

        /* Creates the magnifier window */
        if (default_configure && magnifier_configure) {
            if (default_configure->use_magnifier_window) {
                sclwindow magnifier_window = m_gwes->m_windows->create_magnifier_window(parent, 0, 0,
                    magnifier_configure->width, magnifier_configure->height);
                m_gwes->m_events->connect_window_events(magnifier_window, SCL_EVENT_EXPOSE | SCL_EVENT_MOUSE);
            }
        }

        /* Creates the dim window */
        /* FIXME */
        //if (scl_check_arrindex(defaultLayoutIdx, MAX_LAYOUT)) {
        m_gwes->m_windows->create_dim_window(window, NULL, sclres_layout[layout].width, sclres_layout[layout].height);

        /* m_gwes->m_events->set_touch_event_offset(scl_default_configure.touch_offset);*/
        /*Moved to Show Layout*/

        /* For testing pre-image load */
        /*CSCLImageProxy *proxy = CSCLImageProxy::get_instance();
        sclchar composed_path[_POSIX_PATH_MAX] = {0,};
        for (sclint loop = 0; loop < MAX_PRELOAD_IMG_CNT; loop++) {
            memset(composed_path, 0x00, sizeof(sclchar)*_POSIX_PATH_MAX);
            m_utils->get_composed_path(composed_path, scl_preload_image[loop]);
            proxy->get_image(composed_path);
        }*/
    }
}

/**
 * Shows the current layout and contexts
 * This function will be called by expose event and invalidate rect
 */
sclboolean
CSCLUIBuilder::show_layout(const sclwindow window, const scl16 x, const scl16 y, const scl16 width, const scl16 height)
{
    SCL_DEBUG();

    sclboolean ret = FALSE;

    CSCLEvents *events = CSCLEvents::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLGraphics *graphics = CSCLGraphics::get_instance();
    CSCLResourceCache *cache = CSCLResourceCache::get_instance();
    CSCLContext *context = CSCLContext::get_instance();

    SclResParserManager *sclres_manager = SclResParserManager::get_instance();
    PSclDefaultConfigure default_configure = NULL;
    if (sclres_manager) {
        default_configure = sclres_manager->get_default_configure();
    }
    if (events && windows && graphics && cache && context && default_configure) {
    /* FIXME : The draw_ctx should be acquired from the base window also, if the target window is virtual
       However, for ease of developement, leave the drawctx to be acquired from the target window for now
       Should modify the EFLObject list management routine after fixing the issue described above
    */
        scldrawctx draw_ctx = graphics->begin_paint(window);
        SCLDisplayMode display_mode =  context->get_display_mode();
        events->set_touch_event_offset(default_configure->touch_offset[display_mode]);

        if (window == windows->get_magnifier_window()) {
            /* For the magnifier window */
            ret = show_magnifier(window, draw_ctx);
        } else if (window == windows->get_dim_window()) {
            SclWindowContext *dimctx = windows->get_window_context(window);
            if (dimctx) {
                SclSize size;
                SclColor color;
                size.width = dimctx->geometry.width;
                size.height = dimctx->geometry.height;
                color = default_configure->dim_color;
                draw_window_bg_by_sw(window, draw_ctx, size, 0.0, color, color);
                /*sclchar composed_path[_POSIX_PATH_MAX] = {0,};
                const SclLayout* layout = cache->get_cur_layout(windows->get_base_window());
                m_utils->get_composed_path(composed_path, layout->image_path[BUTTON_STATE_NORMAL]);
                graphics->draw_image(window, draw_ctx, composed_path, 0, 0, layout->width, layout->height);*/
            }
        } else {
            /* For the base and popup window */
            const SclLayout* layout = cache->get_cur_layout(window);
            // FIXME implement later
            // SclLayoutInfoCache *info_cache = cache->get_cur_layout_info_cache(window);
            if (layout) {
                CSCLWindows *windows = CSCLWindows::get_instance();
                //SclWindowContext *winctx = windows->get_window_context(window, FALSE);
                SclWindowContext *winctx = windows->get_window_context(window);
                scl_assert_return_false(layout);

                if (layout->use_sw_background) {
                    if (winctx) {
                        SclSize size;
                        size.width = winctx->geometry.width;
                        size.height = winctx->geometry.height;
                        draw_window_bg_by_sw(window, draw_ctx, size, layout->bg_line_width, layout->bg_line_color, layout->bg_color);
                    }
                } else if (layout->image_path[BUTTON_STATE_NORMAL]) {
                    /* If the target window is virtual window, let's draw it on the base window */
                    sclint targetx = 0;
                    sclint targety = 0;
                    sclwindow targetwin = window;
                    if (winctx) {
                        if (winctx->is_virtual) {
                            //SclWindowContext *basectx = windows->get_window_context(windows->get_base_window(), FALSE);
                            SclWindowContext *basectx = windows->get_window_context(windows->get_base_window());
                            if (basectx) {
                                targetwin = windows->get_base_window();
                                targetx = winctx->geometry.x - basectx->geometry.x;
                                targety = winctx->geometry.y - basectx->geometry.y;
                            }
                        }
                        if (strlen(layout->image_path[BUTTON_STATE_NORMAL]) > 0) {
                            /*SclImageCachedInfo cached_info = {0, };
                            cached_info.nine_patch_left = info_cache->bg_image_path[BUTTON_STATE_NORMAL].left;
                            cached_info.nine_patch_right = info_cache->bg_image_path[BUTTON_STATE_NORMAL].right;
                            cached_info.nine_patch_top = info_cache->bg_image_path[BUTTON_STATE_NORMAL].top;
                            cached_info.nine_patch_bottom = info_cache->bg_image_path[BUTTON_STATE_NORMAL].bottom;*/

                            sclchar composed_path[_POSIX_PATH_MAX] = {0,};
                            m_utils->get_composed_path(composed_path, IMG_PATH_PREFIX, layout->image_path[BUTTON_STATE_NORMAL]);
                            // Temporary testing for EFL backend.. Otherwise the background image covers other buttons
                            if (winctx && (x + y + width + height == 0)) {
                                //graphics->draw_image(targetwin, draw_ctx, composed_path, &cached_info, targetx, targety, layout->width, layout->height, winctx->layout_image_offset.x, winctx->layout_image_offset.y, -1, -1, layout->extract_background);
                                graphics->draw_image(targetwin, draw_ctx, composed_path, NULL, targetx, targety, layout->width, layout->height,
                                                        winctx->layout_image_offset.x, winctx->layout_image_offset.y, -1, -1, layout->extract_background);
                            }
                        }
                    }
                }
                draw_button_all(window, draw_ctx, x, y, width, height);
            }
        }
        graphics->end_paint(window, draw_ctx);
    }

    return ret;
}

/**
 * Draws all of buttons
 */
sclboolean
CSCLUIBuilder::draw_button_all(const sclwindow window, const scldrawctx draw_ctx, const scl16 x /* = 0 */, const scl16 y /* = 0 */, const scl16 width /* = 0 */, const scl16 height /* = 0 */)
{
    SCL_DEBUG();
    scl_assert_return_false(window);

    sclboolean drawall = FALSE;
    SclRectangle updatearea = {x, y, width, height};
    if (x + y + width + height == 0) {
        drawall = TRUE;
    }
    CSCLContext *context = CSCLContext::get_instance();
    CSCLResourceCache *cache = CSCLResourceCache::get_instance();
    CSCLUtils *utils = CSCLUtils::get_instance();
    SclLayoutKeyCoordinate* coordinate = NULL;
    SclButtonContext* btncontext = NULL;

    if (context && cache && utils) {
        for (sclint idx = 0; idx < MAX_KEY; idx++) {
            coordinate = cache->get_cur_layout_key_coordinate(window, idx);
            btncontext = cache->get_cur_button_context(window, idx);
            if (coordinate && btncontext) {
                /* First check if this button is enabled in current active sublayout */
                sclboolean subLayoutMatch = TRUE;
                if (coordinate->sub_layout && context->get_cur_sublayout()) {
                    if (strncmp(coordinate->sub_layout, context->get_cur_sublayout(), MAX_SIZE_OF_SUBLAYOUT_STRING) != 0) {
                        subLayoutMatch = FALSE;
                    }
                }
                if (coordinate->valid && subLayoutMatch) {
                    SclRectangle itemrect = {coordinate->x, coordinate->y, coordinate->width, coordinate->height};
                    if (drawall || utils->is_rect_overlap(itemrect, updatearea)) {
                        if (!draw_button(window, draw_ctx, idx, btncontext->state)) {
                            break;
                        }
                    }
                }
            }
        }
    }
    return TRUE;
}

/**
 * Draws the button of the given key index by type of the button
 * @remark draw_button_all
 */
sclboolean
CSCLUIBuilder::draw_button(const sclwindow window, scldrawctx draw_ctx, const scl16 key_index, const SCLButtonState state, const sclboolean force_draw_bg /* = FALSE */)
{
    SCL_DEBUG();
    scl_assert_return_false(window);
    scl_assert_return_false(key_index > NOT_USED && key_index < MAX_KEY);
    scl_assert_return_false(state >= BUTTON_STATE_NORMAL && state < SCL_BUTTON_STATE_MAX);

    CSCLContext *context = CSCLContext::get_instance();
    CSCLResourceCache *cache = CSCLResourceCache::get_instance();
    CSCLGraphics *graphics = CSCLGraphics::get_instance();

    if (cache && context && graphics) {
        SclButtonContext* btncontext = cache->get_cur_button_context(window, key_index);

        if (btncontext) {
            if (!btncontext->used) {
                return FALSE;
            }

            /* creates a cairo surface if the value of the given draw_ctx is NULL */
            sclboolean need_endpaint = FALSE;
            if (draw_ctx == NULL) {
                draw_ctx = graphics->begin_paint(window);
                need_endpaint = TRUE;
            }

            /* FIXME : There is a case that begin_pain fails. Inspection on the root cause is needed */
            if (draw_ctx) {
                SCLShiftState shiftidx = context->get_shift_state();
                if (shiftidx < 0 || shiftidx >= SCL_SHIFT_STATE_MAX) shiftidx = SCL_SHIFT_STATE_OFF;

                const SclLayout* layout = cache->get_cur_layout(window);
                const SclLayoutKeyCoordinate* coordinate = cache->get_cur_layout_key_coordinate(window, key_index);

                /* 1. drawing the background of the button */
                /* check it according to the following check-list */
                /* check it whether uses SW style button */
                if (layout && coordinate) {
                    if (layout->use_sw_button) {
                        /* case 1 (uses Software button) */
                        draw_button_bg_by_sw(window, draw_ctx, key_index, state);
                    } else {
                        /* check it whether uses an individual images */
                        if (coordinate->bg_image_path[shiftidx][state]) {
                            if (strcmp(coordinate->bg_image_path[shiftidx][state], SCL_BACKGROUND_IMAGE_STRING) != 0) {
                                /* case 2 (uses an indivisual image) */
                                draw_button_bg_by_img(window, draw_ctx, key_index, state, shiftidx);
                            } else {
                                /* case 3 (uses the layout background image) */
                                draw_button_bg_by_layoutimg(window, draw_ctx, key_index, state, shiftidx);
                            }
                        } else if (force_draw_bg) {
                            draw_button_bg_by_layoutimg(window, draw_ctx, key_index, state, shiftidx);
                        }
                        /* case 4 (don't draw anything for button's background if image_path is NULL) */
                    }

                    /* 2. displaying the label of the button */
                    draw_button_label(window, draw_ctx, key_index, state, shiftidx);
                }
            }

            /* destrorys the cairo surface if the value of the given(parameter) draw_ctx is NULL */
            if (need_endpaint) {
                graphics->end_paint(window, draw_ctx);
            }
        } else {
            return FALSE;
        }
    }
    return TRUE;
}

/**
 * Draws labels for the target button
 * @remark draw_button
 */
sclboolean
CSCLUIBuilder::draw_button_label(const sclwindow window, const scldrawctx draw_ctx, const scl16 key_index, const SCLButtonState state, const sclboolean shift)
{
    SCL_DEBUG();

    CSCLUtils *utils = CSCLUtils::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLGraphics *graphics = CSCLGraphics::get_instance();
    CSCLResourceCache *cache = CSCLResourceCache::get_instance();
    const SclLayoutKeyCoordinate* coordinate = cache->get_cur_layout_key_coordinate(window, key_index);

    scl_assert_return_false(window);
    scl_assert_return_false(draw_ctx);
    scl_assert_return_false(coordinate);

    if (utils && windows && graphics && cache && coordinate) {
        /* If the target window is virtual window, let's draw it on the base window */
        sclint targetaddx = 0;
        sclint targetaddy = 0;
        sclwindow targetwin = window;
        //SclWindowContext *winctx = windows->get_window_context(window, FALSE);
        SclWindowContext *winctx = windows->get_window_context(window);
        if (winctx) {
            if (winctx->is_virtual) {
                //SclWindowContext *basectx = windows->get_window_context(windows->get_base_window(), FALSE);
                SclWindowContext *basectx = windows->get_window_context(windows->get_base_window());
                if (basectx) {
                    targetwin = windows->get_base_window();
                    targetaddx = winctx->geometry.x - basectx->geometry.x;
                    targetaddy = winctx->geometry.y - basectx->geometry.y;
                }
            }
        }

        /* for image label  */
        if (coordinate->image_label_path[shift][state]) {
            if (strlen(coordinate->image_label_path[shift][state]) > 0) {
                sclchar composed_path[_POSIX_PATH_MAX] = {0,};
                m_utils->get_composed_path(composed_path, IMG_PATH_PREFIX, coordinate->image_label_path[shift][state]);

                SclSize imgSize = m_gwes->m_graphics->get_image_size(composed_path);
                if (imgSize.width == 0 && imgSize.height == 0) {
                    imgSize.width = coordinate->width;
                    imgSize.height = coordinate->height;
                }
                imgSize.width *= m_utils->get_smallest_scale_rate() * m_utils->get_smallest_custom_scale_rate();
                imgSize.height *= m_utils->get_smallest_scale_rate() * m_utils->get_smallest_custom_scale_rate();

                /* Make sure the image label is not bigger than the given coordinate */
                if (imgSize.width > coordinate->width) {
                    imgSize.width = coordinate->width;
                }
                if (imgSize.height > coordinate->height) {
                    imgSize.height = coordinate->height;
                }

                SclPoint pos = {0,};
                const SclLabelProperties *labelproperties = cache->get_label_properties(coordinate->image_label_type, 0);
                if (labelproperties) {
                    SCLLabelAlignment align = labelproperties->alignment;
                    sclshort padding_x = labelproperties->padding_x * utils->get_custom_scale_rate_x();
                    sclshort padding_y = labelproperties->padding_y * utils->get_custom_scale_rate_y();
                    if (align == LABEL_ALIGN_LEFT_MIDDLE ||
                        align == LABEL_ALIGN_CENTER_MIDDLE ||
                        align == LABEL_ALIGN_RIGHT_MIDDLE) {
                            pos.y = coordinate->y + ((coordinate->height - imgSize.height) / 2) + padding_y;
                    } else if (align == LABEL_ALIGN_LEFT_BOTTOM ||
                        align == LABEL_ALIGN_CENTER_BOTTOM ||
                        align == LABEL_ALIGN_RIGHT_BOTTOM) {
                            pos.y = coordinate->y + (coordinate->height - imgSize.height) - padding_y;
                    } else {
                        pos.y = coordinate->y + padding_y;
                    }
                    if (align == LABEL_ALIGN_CENTER_TOP ||
                        align == LABEL_ALIGN_CENTER_MIDDLE ||
                        align == LABEL_ALIGN_CENTER_BOTTOM) {
                            pos.x = coordinate->x + ((coordinate->width - imgSize.width) / 2) + padding_x;
                    } else if (align == LABEL_ALIGN_RIGHT_TOP ||
                        align == LABEL_ALIGN_RIGHT_MIDDLE ||
                        align == LABEL_ALIGN_RIGHT_BOTTOM) {
                            pos.x = coordinate->x + (coordinate->width - imgSize.width) - padding_x;
                    } else {
                        pos.x = coordinate->x + padding_x;
                    }
                } else {
                    pos.x = coordinate->x + ((coordinate->width - imgSize.width) / 2);
                    pos.y = coordinate->y + ((coordinate->height - imgSize.height) / 2);
                }

                /*
                graphics->draw_image(
                    targetwin,
                    draw_ctx,
                    composed_path,
                    pos.x + targetaddx,
                    pos.y + targetaddy,
                    imgSize.width,
                    imgSize.height
                );*/
                graphics->draw_image(
                    targetwin,
                    draw_ctx,
                    composed_path,
                    NULL,
                    pos.x + targetaddx,
                    pos.y + targetaddy,
                    imgSize.width,
                    imgSize.height
                );
            }
        }

        /* for text */
        for (int idx = 0; idx < coordinate->label_count; idx++) {
            SclFontInfo info;
            CSCLResourceCache *cache = CSCLResourceCache::get_instance();
            const SclLabelProperties *labelproperties = cache->get_label_properties(coordinate->label_type, idx);
            if (labelproperties) {
                const sclchar *label = coordinate->label[shift][idx];
                label = cache->find_substituted_string(label);

                /* If the button type is BUTTON_TYPE_ROTATION, display current keyvalue */
                if (idx == 0) {
                    if (coordinate->button_type == BUTTON_TYPE_ROTATION) {
                        SclButtonContext* btncontext = cache->get_cur_button_context(window, key_index);
                        if (btncontext) {
                            if (btncontext->multikeyIdx < MAX_SIZE_OF_MULTITAP_CHAR) {
                                label = coordinate->key_value[shift][btncontext->multikeyIdx];
                            }
                        }
                    }
                }
                if (labelproperties->font_name) {
                    strncpy(info.font_name, labelproperties->font_name, MAX_FONT_NAME_LEN - 1);
                    info.font_name[MAX_FONT_NAME_LEN - 1] = '\0';
                    info.font_size = labelproperties->font_size * utils->get_smallest_custom_scale_rate();
                    info.is_bold = info.is_italic = true;

                    CSCLContext *context = CSCLContext::get_instance();
                    SCLShiftState shiftstate = context->get_shift_state();
                    if (labelproperties->shadow_distance > 0 && labelproperties->shadow_color[shiftstate][state].a != 0) {
                        sclint deltax = 0;
                        sclint deltay = 0;

                        if (labelproperties->shadow_direction == SHADOW_DIRECTION_LEFT_TOP ||
                                labelproperties->shadow_direction == SHADOW_DIRECTION_LEFT_MIDDLE ||
                                labelproperties->shadow_direction == SHADOW_DIRECTION_LEFT_BOTTOM) {
                            deltax -= labelproperties->shadow_distance * utils->get_smallest_custom_scale_rate();
                        } else if (labelproperties->shadow_direction == SHADOW_DIRECTION_RIGHT_TOP ||
                                   labelproperties->shadow_direction == SHADOW_DIRECTION_RIGHT_MIDDLE ||
                                   labelproperties->shadow_direction == SHADOW_DIRECTION_RIGHT_BOTTOM) {
                            deltax += labelproperties->shadow_distance * utils->get_smallest_custom_scale_rate();
                        }

                        if (labelproperties->shadow_direction == SHADOW_DIRECTION_LEFT_TOP ||
                                labelproperties->shadow_direction == SHADOW_DIRECTION_CENTER_TOP ||
                                labelproperties->shadow_direction == SHADOW_DIRECTION_RIGHT_TOP) {
                            deltay -= labelproperties->shadow_distance * utils->get_smallest_custom_scale_rate();
                        } else if (labelproperties->shadow_direction == SHADOW_DIRECTION_LEFT_BOTTOM ||
                                   labelproperties->shadow_direction == SHADOW_DIRECTION_CENTER_BOTTOM ||
                                   labelproperties->shadow_direction == SHADOW_DIRECTION_RIGHT_BOTTOM) {
                            deltay += labelproperties->shadow_distance * utils->get_smallest_custom_scale_rate();
                        }

                        graphics->draw_text(
                            targetwin,
                            draw_ctx,
                            info,
                            labelproperties->shadow_color[shiftstate][state],
                            label,
                            NULL,
                            (sclint)coordinate->x + deltax + targetaddx,
                            (sclint)coordinate->y + deltax + targetaddy,
                            (sclint)coordinate->width,
                            (sclint)coordinate->height,
                            labelproperties->alignment,
                            labelproperties->padding_x * utils->get_custom_scale_rate_x(),
                            labelproperties->padding_y * utils->get_custom_scale_rate_x(),
                            labelproperties->inner_width * utils->get_custom_scale_rate_x(),
                            labelproperties->inner_height * utils->get_custom_scale_rate_x()
                        );
                    }
                    graphics->draw_text(
                        targetwin,
                        draw_ctx,
                        info,
                        labelproperties->font_color[shiftstate][state],
                        label,
                        NULL,
                        (sclint)coordinate->x + targetaddx,
                        (sclint)coordinate->y + targetaddy,
                        (sclint)coordinate->width,
                        (sclint)coordinate->height,
                        labelproperties->alignment,
                        labelproperties->padding_x * utils->get_custom_scale_rate_x(),
                        labelproperties->padding_y * utils->get_custom_scale_rate_x(),
                        labelproperties->inner_width * utils->get_custom_scale_rate_x(),
                        labelproperties->inner_height * utils->get_custom_scale_rate_x()
                    );
                }
            }
        }
    }

    return TRUE;
}

/**
* Draws window's background using software
* @remark
*/
sclboolean
CSCLUIBuilder::draw_window_bg_by_sw(const sclwindow window, const scldrawctx draw_ctx, const SclSize size,
                                    const scldouble line_width, const SclColor line_color, const SclColor fill_color)
{
    SCL_DEBUG();
    scl_assert_return_false(window);
    scl_assert_return_false(draw_ctx);

    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLGraphics *graphics = CSCLGraphics::get_instance();

    if (graphics && windows) {
        /* If the target window is virtual window, let's draw it on the base window */
        sclwindow targetwin = window;
        //SclWindowContext *winctx = windows->get_window_context(window, FALSE);
        SclWindowContext *winctx = windows->get_window_context(window);
        if (winctx) {
            if (winctx->is_virtual) {
                targetwin = windows->get_base_window();
            }
        }

        graphics->draw_rectangle(targetwin, draw_ctx, 0, 0, size.width, size.height, line_width, line_color, TRUE, fill_color);
    }

    return TRUE;
}

/**
 * Draws a button using software
 * @remark draw_button
 */
sclboolean
CSCLUIBuilder::draw_button_bg_by_sw(const sclwindow window, const scldrawctx draw_ctx, const scl16 key_index, const SCLButtonState state)
{
    //SCL_DEBUG();

    //CSCLResourceCache *cache = CSCLResourceCache::get_instance();
    //const SclLayoutKeyCoordinate* coords = cache->get_cur_layout_key_coordinate(window, key_index);

    //scl_assert_return_false(window);
    //scl_assert_return_false(draw_ctx);
    //scl_assert_return_false(coords);

    //scl_assert_return_false(state >= BUTTON_STATE_NORMAL && state < SCL_BUTTON_STATE_MAX);

    //scldouble line_width = scl_swbutton_style[scl_default_configure.sw_button_style].line_width[state];
    //SclColor &line_color = scl_swbutton_style[scl_default_configure.sw_button_style].line_color[state];
    //SclColor &bg_color = scl_swbutton_style[scl_default_configure.sw_button_style].bg_color[state];
    //scldouble line_curve = scl_swbutton_style[scl_default_configure.sw_button_style].line_curve;
    //sclfloat bg_alpha = scl_swbutton_style[scl_default_configure.sw_button_style].bg_alpha;

    ///*if (g_key_spacing_off) {
    //    m_gwes->m_graphics->draw_rectangle(window,
    //        draw_ctx,
    //        (sclint)coords->x - (sclint)coords->add_hit_left,
    //        (sclint)coords->y - (sclint)coords->add_hit_top,
    //        (sclint)coords->width + (sclint)coords->add_hit_left + (sclint)coords->add_hit_right,
    //        (sclint)coords->height + (sclint)coords->add_hit_top + (sclint)coords->add_hit_bottom,
    //        line_width,
    //        line_color,
    //        TRUE,
    //        bg_color,
    //        line_curve,
    //        bg_alpha);
    //} else {*/
    //    m_gwes->m_graphics->draw_rectangle(window,
    //                                 draw_ctx,
    //                                 coords->x,
    //                                 coords->y,
    //                                 coords->width,
    //                                 coords->height,
    //                                 line_width,
    //                                 line_color,
    //                                 TRUE,
    //                                 bg_color,
    //                                 line_curve,
    //                                 bg_alpha);
    ////}
    return TRUE;
}

/**
 * Draws a button using the set image
 * @remark draw_button
 */
sclboolean
CSCLUIBuilder::draw_button_bg_by_img(const sclwindow window, const scldrawctx draw_ctx,const scl16 key_index, const SCLButtonState state, const sclboolean shift)
{
    SCL_DEBUG();

    CSCLContext *context = CSCLContext::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLGraphics *graphics = CSCLGraphics::get_instance();
    CSCLResourceCache *cache = CSCLResourceCache::get_instance();
    const SclLayoutKeyCoordinate* coordinate = cache->get_cur_layout_key_coordinate(window, key_index);

    SclResParserManager *sclres_manager = SclResParserManager::get_instance();
    PSclModifierDecoration sclres_modifier_decoration = sclres_manager->get_modifier_decoration_table();
    assert(sclres_modifier_decoration != NULL);
    scl_assert_return_false(window);
    scl_assert_return_false(draw_ctx);

    scl_assert_return_false(state >= BUTTON_STATE_NORMAL && state < SCL_BUTTON_STATE_MAX);

    sclchar composed_path[_POSIX_PATH_MAX] = {0,};

    if (context && graphics && cache && coordinate) {
        sclboolean path_composed = FALSE;
        /* Check if we need to decorate the button's drag state */
        //if (context->get_cur_drag_state(context->get_last_touch_device_id()) != SCL_DRAG_STATE_NONE &&
        if (context->get_cur_key_modifier(context->get_last_touch_device_id()) != KEY_MODIFIER_NONE &&
            context->get_cur_pressed_window(context->get_last_touch_device_id()) == window && 
            context->get_cur_pressed_key(context->get_last_touch_device_id()) == key_index &&
            coordinate->modifier_decorator) {
                sclchar *decoration_bg_img = NULL;
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
                if (decoration) {
                    sclshort display = context->get_display_mode();
                    if (!scl_check_arrindex(display, DISPLAYMODE_MAX)) display = 0;
                    SCLKeyModifier modifier = context->get_cur_key_modifier(context->get_last_touch_device_id());
                    if (!scl_check_arrindex(modifier, KEY_MODIFIER_MAX)) modifier = KEY_MODIFIER_NONE;
                    decoration_bg_img = decoration->bg_image_path[display][modifier];
                    /*sclshort dragstate = context->get_cur_drag_state(context->get_last_touch_device_id());
                    if (!scl_check_arrindex(dragstate, SCL_DRAG_STATE_MAX)) dragstate = 0;
                    decoration_bg_img = decoration->bg_image_path[display][dragstate];*/
                }
                if (decoration_bg_img) {
                    if (strlen(decoration_bg_img) > 0) {
                        m_utils->get_composed_path(composed_path, IMG_PATH_PREFIX, decoration_bg_img);
                        path_composed = TRUE;
                    }
                }
        }
        if (!path_composed) {
            m_utils->get_composed_path(composed_path, IMG_PATH_PREFIX, coordinate->bg_image_path[shift][state]);
        }

        /* If the target window is virtual window, let's draw it on the base window */
        sclint targetx = coordinate->x;
        sclint targety = coordinate->y;
        sclwindow targetwin = window;
        //SclWindowContext *winctx = windows->get_window_context(window, FALSE);
        SclWindowContext *winctx = windows->get_window_context(window);
        if (winctx) {
            if (winctx->is_virtual) {
                //SclWindowContext *basectx = windows->get_window_context(windows->get_base_window(), FALSE);
                SclWindowContext *basectx = windows->get_window_context(windows->get_base_window());
                if (basectx) {
                    targetwin = windows->get_base_window();
                    targetx += winctx->geometry.x - basectx->geometry.x;
                    targety += winctx->geometry.y - basectx->geometry.y;
                }
            }
        }

        /*if (g_key_spacing_off) {
            m_gwes->m_graphics->draw_image(
                window,
                draw_ctx,
                composed_path,
                (sclint)coordinate->x - (sclint)coordinate->add_hit_left,
                (sclint)coordinate->y - (sclint)coordinate->add_hit_top,
                (sclint)coordinate->width + (sclint)coordinate->add_hit_left + (sclint)coordinate->add_hit_right,
                (sclint)coordinate->height + (sclint)coordinate->add_hit_top + (sclint)coordinate->add_hit_bottom
                );
        } else {*/
            graphics->draw_image(
                targetwin,
                draw_ctx,
                composed_path,
                NULL,
                (sclint)targetx,
                (sclint)targety,
                (sclint)coordinate->width,
                (sclint)coordinate->height
                );
        //}
    }

    return TRUE;
}

/**
 * Draws a button using the set layout image
 * @remark draw_button
 */
sclboolean
CSCLUIBuilder::draw_button_bg_by_layoutimg(const sclwindow window, const scldrawctx draw_ctx, const scl16 key_index, const SCLButtonState state, const sclboolean shift)
{
    SCL_DEBUG();

    CSCLContext *context = CSCLContext::get_instance();
    CSCLResourceCache *cache = CSCLResourceCache::get_instance();
    const SclLayout* layout = cache->get_cur_layout(window);
    const SclLayoutKeyCoordinate* coordinate = cache->get_cur_layout_key_coordinate(window, key_index);
    SclResParserManager *sclres_manager = SclResParserManager::get_instance();
    PSclModifierDecoration sclres_modifier_decoration = sclres_manager->get_modifier_decoration_table();
    assert(sclres_modifier_decoration != NULL);

    scl_assert_return_false(window);
    scl_assert_return_false(draw_ctx);

    scl_assert_return_false(state >= BUTTON_STATE_NORMAL && state < SCL_BUTTON_STATE_MAX);

    CSCLWindows *windows = CSCLWindows::get_instance();
    //SclWindowContext *winctx = windows->get_window_context(window, FALSE);
    SclWindowContext *winctx = windows->get_window_context(window);

    sclchar composed_path[_POSIX_PATH_MAX] = {0,};
    if (context && cache && coordinate && winctx) {
        sclboolean path_composed = FALSE;
        /* Check if we need to decorate the button's drag state */
        //if (context->get_cur_drag_state(context->get_last_touch_device_id()) != SCL_DRAG_STATE_NONE &&
        if (context->get_cur_key_modifier(context->get_last_touch_device_id()) != KEY_MODIFIER_NONE &&
            context->get_cur_pressed_window(context->get_last_touch_device_id()) == window && 
            context->get_cur_pressed_key(context->get_last_touch_device_id()) == key_index &&
            coordinate->modifier_decorator) {
                sclchar *decoration_bg_img = NULL;
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
                if (decoration) {
                    sclshort display = context->get_display_mode();
                    if (!scl_check_arrindex(display, DISPLAYMODE_MAX)) display = 0;
                    SCLKeyModifier modifier = context->get_cur_key_modifier(context->get_last_touch_device_id());
                    if (!scl_check_arrindex(modifier, KEY_MODIFIER_MAX)) modifier = KEY_MODIFIER_NONE;
                    decoration_bg_img = decoration->bg_image_path[display][modifier];
                    /*sclshort dragstate = context->get_cur_drag_state(context->get_last_touch_device_id());
                    if (!scl_check_arrindex(dragstate, SCL_DRAG_STATE_MAX)) dragstate = 0;
                    decoration_bg_img = decoration->bg_image_path[display][dragstate];*/
                }
                if (decoration_bg_img) {
                    if (strlen(decoration_bg_img) > 0) {
                        m_utils->get_composed_path(composed_path, IMG_PATH_PREFIX, decoration_bg_img);
                        path_composed = TRUE;
                    }
                }
        }
        if (!path_composed) {
            m_utils->get_composed_path(composed_path, IMG_PATH_PREFIX, layout->image_path[state]);
        }
        /*if (g_key_spacing_off) {
            m_gwes->m_graphics->draw_image(
                window,
                draw_ctx,
                composed_path,
                (sclint)coordinate->x - (sclint)coordinate->add_hit_left,
                (sclint)coordinate->y - (sclint)coordinate->add_hit_top,
                (sclint)coordinate->width + (sclint)coordinate->add_hit_left + (sclint)coordinate->add_hit_right,
                (sclint)coordinate->height + (sclint)coordinate->add_hit_top + (sclint)coordinate->add_hit_bottom,
                winctx->imgOffsetx + (sclint)coordinate->x - (sclint)coordinate->add_hit_left,
                winctx->imgOffsety + (sclint)coordinate->y - (sclint)coordinate->add_hit_top,
                (sclint)coordinate->width + (sclint)coordinate->add_hit_left + (sclint)coordinate->add_hit_right,
                (sclint)coordinate->height + (sclint)coordinate->add_hit_top + (sclint)coordinate->add_hit_bottom,
                TRUE
                );
        } else {*/
            sclint dest_x = coordinate->x;
            sclint dest_y = coordinate->y;
            if (winctx->is_virtual) {
                SclWindowContext *basectx = windows->get_window_context(windows->get_base_window());
                if (basectx) {
                    dest_x += (winctx->geometry.x - basectx->geometry.x);
                    dest_y += (winctx->geometry.y - basectx->geometry.y);
                }
            }

            m_gwes->m_graphics->draw_image(
                window,
                draw_ctx,
                composed_path,
                NULL,
                dest_x,
                dest_y,
                (sclint)coordinate->width,
                (sclint)coordinate->height,
                winctx->layout_image_offset.x + (sclint)coordinate->x,
                winctx->layout_image_offset.y + (sclint)coordinate->y,
                (sclint)coordinate->width,
                (sclint)coordinate->height,
                TRUE
            );
        //}
    }
    return TRUE;
}

/**
 * Shows the magnifier window
 */
sclboolean
CSCLUIBuilder::show_magnifier(const sclwindow window, scldrawctx draw_ctx)
{
    SCL_DEBUG();
    scl_assert_return_false(window);

    CSCLUtils *utils = CSCLUtils::get_instance();
    CSCLContext *context = CSCLContext::get_instance();
    CSCLResourceCache *cache = CSCLResourceCache::get_instance();
    sclwindow pressed_window = context->get_cur_pressed_window(context->get_last_touch_device_id());
    scl8 pressed_key = context->get_cur_pressed_key(context->get_last_touch_device_id());

    /* Due to the explicit delay on hiding mangnifier window, there is a case pressed key has been already reset */
    if (pressed_key == NOT_USED) {
        pressed_window = context->get_prev_pressed_window(context->get_last_touch_device_id());
        pressed_key = context->get_prev_pressed_key(context->get_last_touch_device_id());
    }

    if (pressed_key == NOT_USED || pressed_key > MAX_KEY) {
        //utils->log("show_magnifier pressed_key == NOT_USED || pressed_key > MAX_KEY \n");
        return TRUE;
    }

    CSCLActionState *state = CSCLActionState::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();
    const SclLayout *layout = cache->get_cur_layout(windows->get_base_window());
    SclLayoutKeyCoordinate* coordinate = cache->get_cur_layout_key_coordinate(pressed_window, pressed_key);
    SclButtonContext* btncontext = cache->get_cur_button_context(pressed_window, pressed_key);
    SCLShiftState shiftidx = context->get_shift_state();
    if (shiftidx < 0 || shiftidx >= SCL_SHIFT_STATE_MAX) shiftidx = SCL_SHIFT_STATE_OFF;

    /* Do not show if current layout does not allow magnifier */
    if (!(layout->use_magnifier_window)) {
        if (utils) {
            utils->log("!(layout->use_magnifier_window \n");
        }
        return FALSE;
    }
    if (coordinate) {
        /* Some key types do not use the magnifier window */
        if ((coordinate->key_type == KEY_TYPE_CONTROL &&
            coordinate->key_event[0][0] != MVK_space  &&
            coordinate->key_event[0][0] != MVK_BackSpace) ||
                (coordinate->key_type == KEY_TYPE_MODECHANGE ||
                coordinate->key_type == KEY_TYPE_NONE ||
                coordinate->button_type == BUTTON_TYPE_ROTATION)) {
            return FALSE;
        }
        /* FIXME : workaround for not showing magnifier for recent symbols */
        /* Do not show if there's nothing to show */
        //const char *targetstr = coordinate->key_value[shiftidx][btncontext->multikeyIdx];
        const char *targetstr = coordinate->label[shiftidx][0];
        if (state->get_cur_action_state() == ACTION_STATE_BASE_LONGKEY ||
            state->get_cur_action_state() == ACTION_STATE_POPUP_LONGKEY ) {
                targetstr = coordinate->long_key_value;
        }
        const sclchar* customstr = NULL;
        for(sclint label_index = 0;label_index < MAX_SIZE_OF_LABEL_FOR_ONE && !customstr;label_index++) {
            const sclchar *tempstr = context->get_custom_magnifier_label(context->get_last_touch_device_id(), label_index);
            if (tempstr) {
                customstr = tempstr;
            }
        }
        if (customstr) {
            targetstr = customstr;
        }
        if (targetstr == NULL) {
            if (utils) {
                utils->log("coordinate->key_value[shift][btncontext->multikeyIdx] == NULL \n");
            }
            return FALSE;
        } else if (strlen(targetstr) == 0) {
            if (utils) {
                utils->log("coordinate->key_value[shift][btncontext->multikeyIdx]) == 0 \n");
            }
            return FALSE;
        }
    }

#if 0
    SclPoint pos = {0,};
    /* calculates x position to be set */
    pos.x = (coordinate->x + (coordinate->width / 2)) - (utils->get_scale_x(scl_magnifier_configure.width) / 2);

    /* calculates y position to be set */
    sclint scnWidth, scnHeight;
    utils->get_screen_resolution(&scnWidth, &scnHeight);
    pos.y = (scnHeight - layout->height) + coordinate->y - utils->get_scale_y(scl_magnifier_configure.height);
    windows->move_window(windows->get_magnifier_window(), pos.x, pos.y);
#endif

    SclResParserManager *sclres_manager = SclResParserManager::get_instance();
    PSclMagnifierWndConfigure magnifier_configure = NULL;
    if (sclres_manager) {
        magnifier_configure = sclres_manager->get_magnifier_configure();
    }
    if (coordinate && magnifier_configure) {
        sclchar composed_path[_POSIX_PATH_MAX] = {0,};
        sclfloat scale_rate_x, scale_rate_y;
        if (layout->display_mode == DISPLAYMODE_PORTRAIT) {
            scale_rate_x = m_utils->get_scale_rate_x();
            scale_rate_y = m_utils->get_scale_rate_y();
        } else {
            scale_rate_x = m_utils->get_scale_rate_y();
            scale_rate_y = m_utils->get_scale_rate_x();
        }
        if (state->get_cur_action_state() == ACTION_STATE_BASE_LONGKEY) {
            m_utils->get_composed_path(composed_path, IMG_PATH_PREFIX, magnifier_configure->bg_long_key_image_path);
            m_gwes->m_graphics->draw_image(window, draw_ctx, composed_path, NULL, 0, 0,
                magnifier_configure->width * utils->get_custom_scale_rate_x(),
                magnifier_configure->height * utils->get_custom_scale_rate_y());
        } else {
            if (context->get_shift_state() == SCL_SHIFT_STATE_LOCK) {
                m_utils->get_composed_path(composed_path, IMG_PATH_PREFIX, magnifier_configure->bg_shift_lock_image_path);
                m_gwes->m_graphics->draw_image(window, draw_ctx, composed_path, NULL, 0, 0,
                    magnifier_configure->width * utils->get_custom_scale_rate_x(),
                    magnifier_configure->height * utils->get_custom_scale_rate_y());
            } else if (context->get_shift_state() == SCL_SHIFT_STATE_ON) {
                m_utils->get_composed_path(composed_path, IMG_PATH_PREFIX, magnifier_configure->bg_shift_image_path);
                m_gwes->m_graphics->draw_image(window, draw_ctx, composed_path, NULL, 0, 0,
                    magnifier_configure->width * utils->get_custom_scale_rate_x(),
                    magnifier_configure->height * utils->get_custom_scale_rate_y());
            } else {
                m_utils->get_composed_path(composed_path, IMG_PATH_PREFIX, magnifier_configure->bg_image_path);
                m_gwes->m_graphics->draw_image(window, draw_ctx, composed_path, NULL, 0, 0,
                    magnifier_configure->width * utils->get_custom_scale_rate_x(),
                    magnifier_configure->height * utils->get_custom_scale_rate_y());
            }
        }

        sclboolean ended = FALSE;
        for(int loop = 0;loop < MAX_SIZE_OF_LABEL_FOR_ONE && !ended;loop++) {
            const SclLabelProperties *labelproperties = cache->get_label_properties(magnifier_configure->label_type, loop);
            if (labelproperties) {
                if (labelproperties->valid) {
                    SCLShiftState shiftidx = context->get_shift_state();
                    if (shiftidx < 0 || shiftidx >= SCL_SHIFT_STATE_MAX) shiftidx = SCL_SHIFT_STATE_OFF;
                    if (magnifier_configure->show_shift_label) {
                        shiftidx = SCL_SHIFT_STATE_ON;
                    }
                    if (coordinate->use_long_key_magnifier && state->get_cur_action_state() == ACTION_STATE_BASE_LONGKEY ||
                        state->get_cur_action_state() == ACTION_STATE_POPUP_LONGKEY) {
                            const sclchar* targetstr = coordinate->long_key_value;
                            const sclchar* customstr = context->get_custom_magnifier_label(context->get_last_touch_device_id(), loop);
                            if (customstr) {
                                if (coordinate->long_key_value == NULL) {
                                    targetstr = customstr;
                                } else if (strlen(coordinate->long_key_value) == 0) {
                                    targetstr = customstr;
                                } else if (strcmp(coordinate->long_key_value, " ") == 0) {
                                    targetstr = customstr;
                                }
                                draw_magnifier_label(window, draw_ctx, loop, targetstr);
                            } else if (loop == 0) {
                                targetstr = cache->find_substituted_string(targetstr);
                                draw_magnifier_label(window, draw_ctx, 0, targetstr);
                            }
                    } else {
                        const sclchar* targetstr = NULL;
                        const sclchar* customstr = context->get_custom_magnifier_label(context->get_last_touch_device_id(), loop);
                        if (customstr) {
                            targetstr = customstr;
                        } else if (coordinate->magnifier_label[shiftidx][loop]) {
                            targetstr = coordinate->magnifier_label[shiftidx][loop];
                            targetstr = cache->find_substituted_string(targetstr);
                        } else if (loop == 0) {
                            /* Don't display sublabels of each buttons in magnifier window - this policy can be changed, but for now */
                            targetstr = coordinate->label[shiftidx][btncontext->multikeyIdx];
                            targetstr = cache->find_substituted_string(targetstr);
                        }
                        if (targetstr) {
                            draw_magnifier_label(window, draw_ctx, loop, targetstr);
                        }
                    }
                } else {
                    ended = TRUE;
                }
            }
        }
    }

    return TRUE;

}


/**
 * Draws labels on the rectangle of the magnifier window
 * @remark show_magnifier
 */
sclboolean
CSCLUIBuilder::draw_magnifier_label(const sclwindow window, const scldrawctx draw_ctx, const scl16 label_index, const sclchar* label)
{
    SCL_DEBUG();
    scl_assert_return_false(window);
    scl_assert_return_false(draw_ctx);

    CSCLUtils *utils = CSCLUtils::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLResourceCache *cache = CSCLResourceCache::get_instance();
    CSCLContext *context = CSCLContext::get_instance();
    CSCLGraphics *graphics = CSCLGraphics::get_instance();

    SclResParserManager *sclres_manager = SclResParserManager::get_instance();
    PSclMagnifierWndConfigure magnifier_configure = NULL;
    if (sclres_manager) {
        magnifier_configure = sclres_manager->get_magnifier_configure();
    }

    if (utils && windows && cache && context && magnifier_configure) {
        const SclLabelProperties *labelproperties = NULL;
        if (scl_check_arrindex(label_index, MAX_SIZE_OF_LABEL_FOR_ONE)) {
            labelproperties = cache->get_label_properties(magnifier_configure->label_type, label_index);
        }
        if (labelproperties) {
            SclFontInfo info;
            if (labelproperties->font_name) {
                strncpy(info.font_name, labelproperties->font_name, MAX_FONT_NAME_LEN - 1);
            }
            info.font_name[MAX_FONT_NAME_LEN - 1] = '\0';
            info.font_size = labelproperties->font_size * utils->get_smallest_custom_scale_rate();
            info.is_bold = info.is_italic = true;

            SCLShiftState shiftstate = context->get_shift_state();
            if (scl_check_arrindex(shiftstate, SCL_SHIFT_STATE_MAX)) {
                graphics->draw_text(
                    window,
                    draw_ctx,
                    info,
                    labelproperties->font_color[shiftstate][BUTTON_STATE_NORMAL],
                    label,
                    NULL,
                    magnifier_configure->label_area_rect.left * utils->get_custom_scale_rate_x(),
                    magnifier_configure->label_area_rect.top * utils->get_custom_scale_rate_y(),
                    magnifier_configure->label_area_rect.right * utils->get_custom_scale_rate_x(),
                    magnifier_configure->label_area_rect.bottom * utils->get_custom_scale_rate_y(),
                    labelproperties->alignment,
                    labelproperties->padding_x * utils->get_custom_scale_rate_x(),
                    labelproperties->padding_y * utils->get_custom_scale_rate_y(),
                    labelproperties->inner_width * utils->get_custom_scale_rate_x(),
                    labelproperties->inner_height * utils->get_custom_scale_rate_y()
                );
            }
        }
    }
    return TRUE;
}
