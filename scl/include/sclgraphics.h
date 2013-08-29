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

#include "scltypes.h"
#include "sclconfig.h"
#include "sclstructs.h"

#ifndef __SCL_GRAPHICS_H__
#define __SCL_GRAPHICS_H__

#ifdef __cplusplus
//SCL_BEGIN_DECLS
#endif

namespace scl
{
#define MAX_FONT_NAME_LEN 32
/**@brief  font information structure */
typedef struct _SclFontInfo {
    sclchar font_name[MAX_FONT_NAME_LEN];
    sclshort font_size;
    sclboolean is_italic;
    sclboolean is_bold;
} SclFontInfo;

typedef struct _SclImageCachedInfo {
    sclint nine_patch_left;
    sclint nine_patch_right;
    sclint nine_patch_top;
    sclint nine_patch_bottom;
}SclImageCachedInfo;

typedef struct _SclTextCachedInfo {
    SclSize actual_size;
}SclTextCachedInfo;

const SclColor SCLCOLOR_WHITE = {255, 255, 255};
const SclColor SCLCOLOR_GREY = {128, 128, 128};
const SclColor SCLCOLOR_BLACK = {0, 0, 0};
const SclColor SCLCOLOR_RED = {255, 0, 0};
const SclColor SCLCOLOR_BLUE = {0, 0, 255};

/**
 * @brief The base class to work as a soft-based keyboard
 *
 * This class implements all functions for working as a soft-based keyboard
 * In side of ISE developer, they can modify it by their requirements.
 */
class CSCLGraphicsImpl
{
    virtual void init() = 0;
    virtual void fini() = 0;

    friend class CSCLGraphics;
private:
    virtual void draw_image(sclwindow window, const scldrawctx draw_ctx, sclchar* image_path,
                            SclImageCachedInfo *cachedinfo,
                            sclint dest_x, sclint dest_y, sclint dest_width, sclint dest_height,
                            sclint src_x, sclint src_y, sclint src_width, sclint src_height,
                            sclboolean extrace_image) = 0;
    virtual void draw_text(sclwindow window, const scldrawctx draw_ctx, const SclFontInfo& font_info, const SclColor& color,
                           const sclchar *str, SclTextCachedInfo *cachedinfo, sclint pos_x, sclint pos_y,
                           sclint width, sclint height, SCLLabelAlignment align,
                           sclint padding_x, sclint padding_y, sclint inner_width, sclint inner_height) = 0;
    virtual scldrawctx begin_paint(const sclwindow window, const sclboolean force_draw = FALSE) = 0;
    virtual void end_paint(const sclwindow window, scldrawctx draw_ctx) = 0;
    virtual sclimage load_image(const sclchar *image_path) = 0;
    virtual void unload_image(sclimage image_data) = 0;
    virtual sclfont create_font(const SclFontInfo& info) = 0;
    virtual void destroy_font(sclfont font) = 0;
    virtual void draw_rectangle(sclwindow window, const scldrawctx draw_ctx, scldouble pos_x, scldouble pos_y,
                                scldouble width, scldouble height, const scldouble line_width, const SclColor& line_color,
                                sclboolean fill, const SclColor& fill_color, scldouble radius, sclfloat alpha) = 0;
    virtual SclSize get_image_size(sclchar* image_path) = 0;
    virtual SclSize get_text_size(const SclFontInfo &fontinfo, const sclchar *str) = 0;
};

class CSCLGraphics
{
    friend class CSCLImageProxy;
    friend class CSCLFontProxy;

private:
    CSCLGraphics();
public :
    ~CSCLGraphics();

    static CSCLGraphics* get_instance();

    void init();
    void fini();

    void draw_image(sclwindow window, const scldrawctx draw_ctx, sclchar* image_path, SclImageCachedInfo *cachedinfo, sclint dest_x, sclint dest_y,
                    sclint dest_width = -1, sclint dest_height = -1, sclint src_x = 0, sclint src_y = 0, sclint src_width = -1, sclint src_height = -1, sclboolean extrace_image = FALSE) {
        get_scl_graphics_impl()->draw_image(window, draw_ctx, image_path, cachedinfo, dest_x, dest_y, dest_width, dest_height, src_x, src_y, src_width, src_height, extrace_image);
    }

    void draw_text(sclwindow window, const scldrawctx draw_ctx, const SclFontInfo& font_info, const SclColor& color,
                   const sclchar *str, SclTextCachedInfo *cachedinfo, sclint pos_x, sclint pos_y, sclint width = 0, sclint height = 0,
                   SCLLabelAlignment align = LABEL_ALIGN_LEFT_TOP,
                   sclint padding_x = 0, sclint padding_y = 0, sclint inner_width = 0, sclint inner_height = 0) {
        get_scl_graphics_impl()->draw_text(window, draw_ctx, font_info, color, str, cachedinfo,
            pos_x, pos_y, width, height, align, padding_x, padding_y, inner_width, inner_height);
    }

    scldrawctx begin_paint(const sclwindow window, const sclboolean force_draw = FALSE) {
        return get_scl_graphics_impl()->begin_paint(window, force_draw);
    }

    void end_paint(const sclwindow window, scldrawctx draw_ctx) {
        return get_scl_graphics_impl()->end_paint(window, draw_ctx);
    }

    void draw_rectangle(sclwindow window, const scldrawctx draw_ctx, scldouble pos_x, scldouble pos_y,
                        scldouble width, scldouble height, const scldouble line_width, const SclColor& line_color,
                        sclboolean fill, const SclColor& fill_color, scldouble radius = 0, sclfloat alpha = 1.0) {
        get_scl_graphics_impl()->draw_rectangle(window, draw_ctx, pos_x, pos_y, width, height, line_width, line_color, fill, fill_color, radius, alpha);
    }

    SclSize get_image_size(sclchar* image_path) {
        return get_scl_graphics_impl()->get_image_size(image_path);
    }

    SclSize get_text_size(const SclFontInfo & fontinfo, const sclchar *str) {
        return get_scl_graphics_impl()->get_text_size(fontinfo, str);
    }
private:
    sclimage load_image(const sclchar *image_path) {
        return get_scl_graphics_impl()->load_image(image_path);
    }

    void unload_image(sclimage image_data) {
        return get_scl_graphics_impl()->unload_image(image_data);
    }

    sclfont create_font(const SclFontInfo& info) {
        return get_scl_graphics_impl()->create_font(info);
    }
    void destroy_font(sclfont font) {
        get_scl_graphics_impl()->destroy_font(font);
    }

protected :
    CSCLGraphicsImpl* get_scl_graphics_impl();

private :
    CSCLGraphicsImpl* m_impl;
};


} /* End of scl namespace */

#ifdef __cplusplus
//SCL_END_DECLS
#endif

#endif //__SCL_GRAPHICS_H__
