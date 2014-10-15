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

#include "sclgraphics.h"
#include <gtk/gtk.h>

#ifndef __SCL_GRAPHICS_CAIRO_H__
#define __SCL_GRAPHICS_CAIRO_H__

namespace scl
{
class CSCLGraphicsImplCairo : public CSCLGraphicsImpl
{
public :
    CSCLGraphicsImplCairo();
    ~CSCLGraphicsImplCairo();

    void draw_image(sclwindow window, const scldrawctx draw_ctx, sclchar* image_path,
                    sclint dest_x, sclint dest_y, sclint dest_width, sclint dest_height,
                    sclint src_x, sclint src_y, sclint src_width, sclint src_height);
    void draw_text(sclwindow window, const scldrawctx draw_ctx, const SclFontInfo& font_info, const SclColor& color,
                   const sclchar *str, sclint pos_x, sclint pos_y, sclint width, sclint height,
                   SCLLabelAlignment align, sclbyte padding);
    sclimage load_image(const sclchar* image_path);
    void unload_image(sclimage image_data);
    scldrawctx begin_paint(const sclwindow window, const sclboolean force_draw = FALSE);
    void end_paint(const sclwindow window, scldrawctx draw_ctx);
    sclfont create_font(const SclFontInfo& info);
    void destroy_font(sclfont font);
    void draw_rectangle(sclwindow window, const scldrawctx draw_ctx, scldouble pos_x, scldouble pos_y,
                        scldouble width, scldouble height, const scldouble line_width, const SclColor& line_color,
                        sclboolean fill, const SclColor& fill_color, scldouble radius, sclfloat alpha);
    SclSize get_image_size(sclchar* image_path);
private:
};
} /* End of scl namespace */
#endif __SCL_GRAPHICS_CAIRO_H__
