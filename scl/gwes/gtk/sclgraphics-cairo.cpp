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

#include <cairo.h>

#include "sclgraphics-cairo.h"
#include "sclimageproxy.h"
#include "scldebug.h"
#include "sclwindows.h"


using namespace scl;

/**
 * Constructor
 */
CSCLGraphicsImplCairo::CSCLGraphicsImplCairo()
{
    SCL_DEBUG();
    /* Initializes all window resources */
}

/**
 * De-constructor
 */
CSCLGraphicsImplCairo::~CSCLGraphicsImplCairo()
{
    SCL_DEBUG();
}


/**
 * Draws the given text on cairo-surface
 */
void
CSCLGraphicsImplCairo::draw_text(sclwindow window, const scldrawctx drawCtx, const SclFontInfo& fontinfo, const SclColor& color,
                                 const sclchar *str, sclint posx, sclint posy, sclint width, sclint height,
                                 SCLLabelAlignment align, sclbyte padding)
{
    SCL_DEBUG();
    /* pre-condition */
    scl_assert_return(str);
    scl_assert_return(drawCtx);

    cairo_t* cr = static_cast<cairo_t*>(drawCtx);
    cairo_identity_matrix(cr);
    cairo_set_source_rgba(cr, color.r / 256.0, color.g / 256.0, color.b / 256.0, 1.0);
    cairo_select_font_face(cr, fontinfo.fontname,
                           fontinfo.isItalic ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_NORMAL,
                           fontinfo.isBold ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL);

    cairo_set_font_size(cr, fontinfo.fontsize);

    cairo_text_extents_t textExtents;
    cairo_text_extents(cr, str, &textExtents);
    cairo_text_extents_t barExtents;
    cairo_text_extents(cr, "|", &barExtents);

    if (align == LABEL_ALIGN_CENTER_TOP || align == LABEL_ALIGN_CENTER_MIDDLE || align == LABEL_ALIGN_CENTER_BOTTOM) {
        posx = posx + ((width - textExtents.width) / 2);
    } else if (align == LABEL_ALIGN_RIGHT_TOP || align == LABEL_ALIGN_RIGHT_MIDDLE || align == LABEL_ALIGN_RIGHT_BOTTOM) {
        posx = posx + (width - textExtents.width) - padding;
    } else {
        posx += padding;
    }
    if (align == LABEL_ALIGN_LEFT_MIDDLE || align == LABEL_ALIGN_CENTER_MIDDLE || align == LABEL_ALIGN_RIGHT_MIDDLE) {
        posy = posy + ((height - barExtents.height) / 2);
    } else if (align == LABEL_ALIGN_LEFT_BOTTOM || align == LABEL_ALIGN_CENTER_BOTTOM || align == LABEL_ALIGN_RIGHT_BOTTOM) {
        posy = posy + (height - barExtents.height) - padding;
    } else {
        posy += padding;
    }

    /* FIXME : A workaround for displaying '_' correctly on its position */
    /* MUST change the line below, with a full understanding on font drawing system */
    cairo_move_to(cr, posx - textExtents.x_bearing, posy - barExtents.y_bearing);
    cairo_show_text(cr, str);
}

/**
 * Draws the given image on cairo-surface
 */
void
CSCLGraphicsImplCairo::draw_image(sclwindow window, const scldrawctx drawCtx, sclchar* imgPath, sclint destX, sclint destY,
                                  sclint destWidth, sclint destHeight, sclint srcX, sclint srcY, sclint srcWidth, sclint srcHeight)
{
    SCL_DEBUG();
    /* pre-condition */
    scl_assert_return(imgPath);
    scl_assert_return(drawCtx);

    cairo_t* cr = static_cast<cairo_t*>(drawCtx);
    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLImageProxy *proxy = CSCLImageProxy::get_instance();

    if (windows && proxy) {
        cairo_surface_t* imgSurface = static_cast<cairo_surface_t*>(proxy->get_image(imgPath));

        if (imgSurface && cr) {
            if (cairo_surface_status(imgSurface) == CAIRO_STATUS_SUCCESS) {
                cairo_identity_matrix(cr);

                cairo_translate(cr, destX - srcX, destY - srcY);

                if (destWidth > 0 && destHeight > 0 && srcWidth == -1) {
                    sclint imagew = cairo_image_surface_get_width (imgSurface);
                    sclint imageh = cairo_image_surface_get_height (imgSurface);

                    double scaleX = (double)destWidth / imagew;
                    double scaleY = (double)destHeight / imageh;
                    cairo_scale(cr, scaleX, scaleY);
                }

                if (srcWidth > 0) {
                    cairo_rectangle (cr, srcX, srcY, srcWidth, srcHeight);
                    cairo_clip (cr);
                    cairo_new_path (cr);
                }

                cairo_set_source_surface(cr, imgSurface, 0, 0);
                cairo_paint(cr);
            }
        }
    }
}

/**
 * Draws a rectangle on cairo-surface
 */
void
CSCLGraphicsImplCairo::draw_rectangle(sclwindow window, const scldrawctx drawCtx, scldouble posx, scldouble posy,
                                      scldouble width, scldouble height, const scldouble lineWidth, const SclColor& lineColor, sclboolean fill, const SclColor& fillColor, scldouble radius, sclfloat alpha)
{
    SCL_DEBUG();
    /* pre-condition */
    scl_assert_return(drawCtx);

    cairo_t* cr = static_cast<cairo_t*>(drawCtx);
    cairo_identity_matrix(cr);

    /* a custom shape that could be wrapped in a function */
    //double radius = 0;//102.4;   /* and an approximate curvature radius */

    double x1,y1;

    x1=posx+width;
    y1=posy+height;
    if (!width || !height)
        return;
    if (width/2<radius) {
        if (height/2<radius) {
            cairo_move_to  (cr, posx, (posy + y1)/2);
            cairo_curve_to (cr, posx ,posy, posx, posy, (posx + x1)/2, posy);
            cairo_curve_to (cr, x1, posy, x1, posy, x1, (posy + y1)/2);
            cairo_curve_to (cr, x1, y1, x1, y1, (x1 + posx)/2, y1);
            cairo_curve_to (cr, posx, y1, posx, y1, posx, (posy + y1)/2);
        } else {
            cairo_move_to  (cr, posx, posy + radius);
            cairo_curve_to (cr, posx ,posy, posx, posy, (posx + x1)/2, posy);
            cairo_curve_to (cr, x1, posy, x1, posy, x1, posy + radius);
            cairo_line_to (cr, x1 , y1 - radius);
            cairo_curve_to (cr, x1, y1, x1, y1, (x1 + posx)/2, y1);
            cairo_curve_to (cr, posx, y1, posx, y1, posx, y1- radius);
        }
    } else {
        if (height/2<radius) {
            cairo_move_to  (cr, posx, (posy + y1)/2);
            cairo_curve_to (cr, posx , posy, posx , posy, posx + radius, posy);
            cairo_line_to (cr, x1 - radius, posy);
            cairo_curve_to (cr, x1, posy, x1, posy, x1, (posy + y1)/2);
            cairo_curve_to (cr, x1, y1, x1, y1, x1 - radius, y1);
            cairo_line_to (cr, posx + radius, y1);
            cairo_curve_to (cr, posx, y1, posx, y1, posx, (posy + y1)/2);
        } else {
            cairo_move_to  (cr, posx, posy + radius);
            cairo_curve_to (cr, posx , posy, posx , posy, posx + radius, posy);
            cairo_line_to (cr, x1 - radius, posy);
            cairo_curve_to (cr, x1, posy, x1, posy, x1, posy + radius);
            cairo_line_to (cr, x1 , y1 - radius);
            cairo_curve_to (cr, x1, y1, x1, y1, x1 - radius, y1);
            cairo_line_to (cr, posx + radius, y1);
            cairo_curve_to (cr, posx, y1, posx, y1, posx, y1- radius);
        }
    }
    cairo_close_path (cr);

    if (fill) {
        cairo_set_source_rgba (cr, fillColor.r / 256.0, fillColor.g / 256.0, fillColor.b / 256.0, alpha); //0.9
        cairo_fill_preserve (cr);
    }

    cairo_set_source_rgb (cr, lineColor.r / 256.0, lineColor.g / 256.0, lineColor.b / 256.0); // 1  . 0
    cairo_set_line_width (cr, lineWidth);//5.0
    cairo_stroke (cr);
}


#if 0
/**
 * Creates a surface for cairo
 */
static cairo_surface_t*
load_cairo_surface(const gchar *filename)
{
    SCL_DEBUG();

    GdkPixmap* pixmap = NULL;
    GdkPixbuf* pixbuf = NULL;
    GdkBitmap* mask = NULL;
    GdkScreen *screen;
    GdkColormap *rgba;

    rgba = gdk_screen_get_rgba_colormap(gdk_screen_get_default());

    pixbuf = gdk_pixbuf_new_from_file(filename, NULL);
    gdk_pixbuf_render_pixmap_and_mask_for_colormap (pixbuf,  rgba, &pixmap, &mask, 128);

    return GDK_DRAWABLE_GET_CLASS (GDK_DRAWABLE (pixmap))->ref_cairo_surface (GDK_DRAWABLE (pixmap));


}
#else
/**
* Creates a surface for cairo
*/
static cairo_surface_t*
load_cairo_surface(const gchar *filename)
{
    SCL_DEBUG();
    cairo_surface_t *image = NULL;
    image = cairo_image_surface_create_from_png(filename);
    return   image;
}
#endif


/**
 * Loads the given image
 * This function will be used in the image proxy class
 */
sclimage
CSCLGraphicsImplCairo::load_image(const sclchar *imgPath)
{
    SCL_DEBUG();
    return load_cairo_surface(imgPath);
}

/**
 * Destroys and removes the given image from cache
 */
void
CSCLGraphicsImplCairo::unload_image(sclimage imgData)
{
    SCL_DEBUG();
    cairo_surface_destroy(static_cast<cairo_surface_t*>(imgData));
}

/**
 * Initializes the drawing context for double-buffering.
 * This func should be called before using a drawing primitive at first.
 */
scldrawctx
CSCLGraphicsImplCairo::begin_paint(const sclwindow window, const sclboolean forcedraw /* = FALSE */)
{
    SCL_DEBUG();
    /* pre-condition */
    cairo_t *cr = NULL;
    /* Cairo contexts created in a GTK+ expose event handler cannot
    be cached and reused between different expose events */
    GtkWidget* widget;
    widget = static_cast<GtkWidget*>(window);
    if (!GTK_WIDGET_REALIZED(widget)) {
        gtk_widget_realize(widget);
    }
    cr = gdk_cairo_create (widget->window);
    return static_cast<scldrawctx>(cr);
}

/**
 * Notices that drawing tasks have done.
 */
void
CSCLGraphicsImplCairo::end_paint(const sclwindow window, scldrawctx drawCtx)
{
    cairo_destroy(static_cast<cairo_t*>(drawCtx));
}

sclfont
CSCLGraphicsImplCairo::create_font(const SclFontInfo& info)
{
    return NULL;
}

void
CSCLGraphicsImplCairo::destroy_font(sclfont font)
{
}

SclSize CSCLGraphicsImplCairo::get_image_size(sclchar* imgPath)
{
    SCL_DEBUG();
    SclSize ret = { 0, 0 };
    CSCLImageProxy *proxy = CSCLImageProxy::get_instance();
    cairo_surface_t* imgSurface = static_cast<cairo_surface_t*>(proxy->get_image(imgPath));
    ret.width = cairo_image_surface_get_width(imgSurface);
    ret.height = cairo_image_surface_get_height(imgSurface);
    return ret;
}
