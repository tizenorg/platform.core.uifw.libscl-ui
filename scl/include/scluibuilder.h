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

#include <map>
#include <string>
#include "sclgwes.h"
#include "sclutils.h"

#ifndef __SCL_UIBUILDER_H__
#define __SCL_UIBUILDER_H__

//SCL_BEGIN_DECLS

namespace scl
{

/**
 * @brief The base class to work as a soft-based keyboard
 *
 * This class implements all functions for working as a soft-based keyboard
 * In side of ISE developer, they can modify it by their requirements.
 */
class CSCLUIBuilder
{
    friend class CSCLFontProxy;

private:
    CSCLUIBuilder();
public:
    ~CSCLUIBuilder();

    static CSCLUIBuilder* get_instance();

    void init(sclwindow parent);
    sclboolean show_layout(const sclwindow window, const scl16 x = 0, const scl16 y = 0, const scl16 width = 0, const scl16 height = 0);
    sclboolean draw_button(const sclwindow window, scldrawctx draw_ctx, const scl16 key_index, const SCLButtonState state, const sclboolean force_draw_bg = FALSE);
    sclboolean show_magnifier(const sclwindow window, scldrawctx draw_ctx);
    sclboolean show_autopopup(const sclwindow parent, scldrawctx draw_ctx, const scl16 key_index);

private:
    sclboolean draw_button_all(const sclwindow window, const scldrawctx draw_ctx, const scl16 x, const scl16 y, const scl16 width, const scl16 height);
    sclboolean draw_button_bg_by_sw(const sclwindow window, const scldrawctx draw_ctx, const scl16 key_index, const SCLButtonState state);
    sclboolean draw_window_bg_by_sw(const sclwindow window, const scldrawctx draw_ctx, const SclSize size,
        const scldouble line_width, const SclColor line_color, const SclColor fill_color);
    sclboolean draw_button_bg_by_img(const sclwindow window, const scldrawctx draw_ctx, const scl16 key_index,
        SCLButtonState state, SCLShiftState shift);
    sclboolean draw_button_bg_by_layoutimg(const sclwindow window, const scldrawctx draw_ctx, const scl16 key_index,
        const SCLButtonState state, const sclboolean shift);
    sclboolean draw_button_label(const sclwindow window, const scldrawctx draw_ctx, const scl16 key_index,
                                 SCLButtonState state, SCLShiftState shift);

    sclboolean draw_magnifier_label(const sclwindow window, const scldrawctx draw_ctx, const scl16 label_index, const sclchar* label);

protected:

    /* Backend Primitive */
    CSCLGwes *m_gwes;
    CSCLUtils *m_utils;
};

}

//SCL_END_DECLS

#endif //__SCL_UIBUILDER_H__
