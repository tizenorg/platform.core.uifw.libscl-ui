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

    void draw_image(sclwindow window, const scldrawctx drawCtx, sclchar* imgPath, sclint destX, sclint destY,
                                  sclint destWidth, sclint destHeight, sclint srcX, sclint srcY, sclint srcWidth, sclint srcHeight);
    void draw_text(sclwindow window, const scldrawctx drawCtx, const SclFontInfo& fontinfo, const SclColor& color,
                   const sclchar *str, sclint posx, sclint posy, sclint width, sclint height,
                   SCLLabelAlignment align, sclbyte padding);
    sclimage load_image(const sclchar* imgPath);
    void unload_image(sclimage imgData);
    scldrawctx begin_paint(const sclwindow window, const sclboolean forcedraw = FALSE);
    void end_paint(const sclwindow window, scldrawctx drawCtx);
    sclfont create_font(const SclFontInfo& info);
    void destroy_font(sclfont font);
    void draw_rectangle(sclwindow window, const scldrawctx drawCtx, scldouble posx, scldouble posy,
                        scldouble width, scldouble height, const scldouble lineWidth, const SclColor& lineColor, sclboolean fill, const SclColor& fillColor, scldouble radius, sclfloat alpha);
    SclSize get_image_size(sclchar* imgPath);
private:
};
} /* End of scl namespace */
#endif __SCL_GRAPHICS_CAIRO_H__
