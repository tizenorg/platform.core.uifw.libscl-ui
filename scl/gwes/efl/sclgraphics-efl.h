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

#ifndef __SCL_GRAPHICS_EFL_H__
#define __SCL_GRAPHICS_EFL_H__

//#define DO_NOT_MOVE_MAGNIFIER_WINDOW
//#define FULL_SCREEN_TEST

/* Still an experimental feature.. Will be refined after being stabilized */
//#define TEST_NEWBACKEND
#ifdef TEST_NEWBACKEND
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
    sclint padding_x;
    sclint padding_y;
    sclint inner_width;
    sclint inner_height;
} TextCache;
#else
#endif

namespace scl
{
class CSCLGraphicsImplEfl : public CSCLGraphicsImpl
{
public :
    CSCLGraphicsImplEfl();
    ~CSCLGraphicsImplEfl();

    void init();
    void fini();

    void draw_image(sclwindow window, const scldrawctx draw_ctx, sclchar* image_path, SclImageCachedInfo *cachedinfo,
                    sclint dest_x, sclint dest_y, sclint dest_width, sclint dest_height,
                    sclint src_x, sclint src_y, sclint src_width, sclint src_height, sclboolean extrace_image);
    sclimage load_image(const sclchar* image_path);
    void unload_image(sclimage image_data);

    sclfont create_font(const SclFontInfo& info);
    void destroy_font(sclfont font);
    void draw_text(sclwindow window, const scldrawctx draw_ctx, const SclFontInfo& font_info, const SclColor& color,
                   const sclchar *str, SclTextCachedInfo *cachedinfo, sclint pos_x, sclint pos_y, sclint width, sclint height,
                   SCLLabelAlignment align, sclint padding_x, sclint padding_y, sclint inner_width, sclint inner_height);

    void draw_rectangle(sclwindow window, const scldrawctx draw_ctx, scldouble pos_x, scldouble pos_y,
                        scldouble width, scldouble height, const scldouble line_width, const SclColor& line_color, sclboolean fill, const SclColor& fill_color, scldouble radius, sclfloat alpha);
    scldrawctx begin_paint(const sclwindow window, const sclboolean force_draw = FALSE);
    void end_paint(const sclwindow window, scldrawctx draw_ctx);
    SclSize get_image_size(sclchar* image_path);
    SclSize get_text_size(const SclFontInfo &fontinfo, const sclchar *str);
private:
};
} /* End of scl namespace */
#endif
