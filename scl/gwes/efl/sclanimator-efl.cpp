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

#include "sclanimator-efl.h"
#include "scldebug.h"

#include <Ecore.h>
#include <Elementary.h>

#include "sclutils.h"
#include "sclwindows.h"

using namespace scl;

/**
 * Constructor
 */
CSCLAnimatorImplEfl::CSCLAnimatorImplEfl()
{
    SCL_DEBUG();

    m_highlight_ui_object = NULL;
    m_highlight_ui_object_alternate = NULL;
}

/**
 * De-constructor
 */
CSCLAnimatorImplEfl::~CSCLAnimatorImplEfl()
{
    SCL_DEBUG();
}

void CSCLAnimatorImplEfl::init()
{

}

void CSCLAnimatorImplEfl::fini()
{
    if (m_highlight_ui_object) {
        evas_object_del(m_highlight_ui_object);
        m_highlight_ui_object = NULL;
    }
    if (m_highlight_ui_object_alternate) {
        evas_object_del(m_highlight_ui_object_alternate);
        m_highlight_ui_object_alternate = NULL;
    }
}


sclboolean
CSCLAnimatorImplEfl::check_animation_supported()
{
    /* EFL Backend finished implementing animation support */
    return TRUE;
}

void
CSCLAnimatorImplEfl::animator_timer(SclAnimationState *state)
{
    CSCLUtils *utils = CSCLUtils::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();

    if (state && windows && utils) {
        switch (state->desc.type) {
            case ANIMATION_TYPE_HIGHLIGHT_UI: {
                if (state->active) {
                    SclRectangle rect = state->rect_cur;
                    if (m_highlight_ui_object == NULL) {
                        Evas_Object *window_object = static_cast<Evas_Object*>(windows->get_base_window());
                        Evas *evas = evas_object_evas_get(window_object);
                        m_highlight_ui_object = evas_object_image_add(evas);
                        sclchar composed_path[_POSIX_PATH_MAX] = {0,};
                        utils->get_composed_path(composed_path, IMG_PATH_PREFIX, SCL_HIGHLIGHT_UI_IMAGE);
                        evas_object_image_file_set(m_highlight_ui_object, composed_path, NULL);
                    }
                    evas_object_move(m_highlight_ui_object, rect.x, rect.y);
                    evas_object_image_fill_set(m_highlight_ui_object, 0, 0, rect.width, rect.height);
                    evas_object_resize(m_highlight_ui_object, rect.width, rect.height);
                    evas_object_raise(m_highlight_ui_object);
                    evas_object_show(m_highlight_ui_object);

                    sclboolean circular = FALSE;
                    if (state->desc.circular) {
                        if (m_highlight_ui_object_alternate == NULL) {
                            Evas_Object *window_object = static_cast<Evas_Object*>(windows->get_base_window());
                            Evas *evas = evas_object_evas_get(window_object);
                            m_highlight_ui_object_alternate = evas_object_image_add(evas);
                            sclchar composed_path[_POSIX_PATH_MAX] = {0,};
                            utils->get_composed_path(composed_path, IMG_PATH_PREFIX, SCL_HIGHLIGHT_UI_IMAGE);
                            evas_object_image_file_set(m_highlight_ui_object_alternate, composed_path, NULL);
                        }
                        SclWindowContext *window_context = windows->get_window_context(windows->get_base_window());
                        if (window_context) {
                            if (rect.x < 0) {
                                evas_object_move(m_highlight_ui_object_alternate,
                                    window_context->geometry.width + rect.x, rect.y);
                                evas_object_image_fill_set(m_highlight_ui_object_alternate,
                                    0, 0, rect.width, rect.height);
                                evas_object_resize(m_highlight_ui_object_alternate,
                                    rect.width, rect.height);
                                evas_object_raise(m_highlight_ui_object_alternate);
                                evas_object_show(m_highlight_ui_object_alternate);
                                circular = TRUE;
                            } else if (rect.x + rect.width > window_context->geometry.width) {
                                evas_object_move(m_highlight_ui_object_alternate,
                                    -(window_context->geometry.width - rect.x), rect.y);
                                evas_object_image_fill_set(m_highlight_ui_object_alternate,
                                    0, 0, rect.width, rect.height);
                                evas_object_resize(m_highlight_ui_object_alternate,
                                    rect.width, rect.height);
                                evas_object_raise(m_highlight_ui_object_alternate);
                                evas_object_show(m_highlight_ui_object_alternate);
                                circular = TRUE;
                            }
                        }
                    }
                    if (!circular) {
                        if (m_highlight_ui_object_alternate) {
                            evas_object_hide(m_highlight_ui_object_alternate);
                        }
                    } else {
                        if (m_highlight_ui_object_alternate) {
                            sclint window_layer = 29000;
                            if (!windows->is_base_window(state->desc.window_from) ||
                                !windows->is_base_window(state->desc.window_to)) {
                                    window_layer = 29010;
                            }
                            evas_object_layer_set(m_highlight_ui_object_alternate, window_layer + 1);
                        }
                    }

                    sclint window_layer = 29000;
                    if (!windows->is_base_window(state->desc.window_from) ||
                        !windows->is_base_window(state->desc.window_to)) {
                            window_layer = 29010;
                    }
                    evas_object_layer_set(m_highlight_ui_object, window_layer + 1);
                } else {
                    evas_object_hide(m_highlight_ui_object);
                }
            }
            break;
            default:
            break;
        }
    }
}