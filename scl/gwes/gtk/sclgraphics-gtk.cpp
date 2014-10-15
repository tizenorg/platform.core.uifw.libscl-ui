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

#include "sclgraphics-gtk.h"
#include "sclimageproxy.h"
#include "sclfontproxy.h"
#include "scldebug.h"
#include "sclwindows.h"

using namespace scl;

/**
 * Constructor
 */
CSCLGraphicsImplGtk::CSCLGraphicsImplGtk()
{
    SCL_DEBUG();
    /* Initializes all window resources */
}

/**
 * De-constructor
 */
CSCLGraphicsImplGtk::~CSCLGraphicsImplGtk()
{
    SCL_DEBUG();
}


void
CSCLGraphicsImplGtk::draw_image(sclwindow window, const scldrawctx draw_ctx,
                                sclchar* image_path, SclImageCachedInfo *cachedinfo,
                                sclint dest_x, sclint dest_y, sclint dest_width, sclint dest_height,
                                sclint src_x, sclint src_y, sclint src_width, sclint src_height)
{
    SCL_DEBUG();
    //printf("\n %d destX, %d destY,\n %d destWidth, %d destHeight, %d srcX, %d srcY,\n %d srcWidth, %d srcHeight\n", destX, destY, destWidth, destHeight,  srcX,  srcWidth,  srcHeight);
    /* pre-condition */
    scl_assert_return(imgPath);

    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLImageProxy *proxy = CSCLImageProxy::get_instance();

    GdkPixbuf *pixbuf;
    GdkPixbuf *temp;
    GdkColormap *rgba;
    GdkBitmap* mask = NULL;

    if (windows && proxy) {
        GdkPixmap* imgdata = static_cast<GdkPixmap*>(proxy->get_image(imgPath));
        GtkWidget* widget = static_cast<GtkWidget*>(window);


        //gdk_gc_set_alpha(  widget->style->fg_gc[GTK_STATE_NORMAL],TRUE);
        if (imgdata && widget) {
            gdk_draw_drawable(
                widget->window,
                widget->style->fg_gc[GTK_STATE_NORMAL],
                imgdata,
                srcX, srcY,
                destX, destY,
                destWidth, destHeight
            );
        }
    }
}

static GdkPixmap*
load_pixmap(const gchar *filename)
{
    SCL_DEBUG();

    GdkPixbuf *pixbuf = NULL;
    GdkPixbuf *temp = NULL;
    guchar *data = NULL;
    int width = 0;
    int height = 0;
    int stride = 0;
    int i =0;

    GdkPixmap *pixmap = NULL;
    GdkColormap *rgba;
    GdkBitmap* mask = NULL;
    if (g_file_test(filename,G_FILE_TEST_EXISTS)) {
        pixbuf = gdk_pixbuf_new_from_file(filename, NULL);
    } else {
        return NULL;
    }

    if (!gdk_pixbuf_get_has_alpha(pixbuf)) {
        temp = pixbuf;
        pixbuf = gdk_pixbuf_add_alpha(temp, FALSE, 0, 0, 0);
        if (temp)
            g_object_unref((temp));
    }

    width = gdk_pixbuf_get_width(pixbuf);
    height = gdk_pixbuf_get_height(pixbuf);
    pixmap = gdk_pixmap_new(NULL, width, height, 32);
    gdk_draw_pixbuf (pixmap,
                     NULL,
                     pixbuf,
                     0,
                     0,
                     0,
                     0,
                     width,
                     height,
                     GDK_RGB_DITHER_NONE,
                     0,
                     0);
    g_object_unref(pixbuf);
    return pixmap;
}

sclimage
CSCLGraphicsImplGtk::load_image(const sclchar* image_path)
{
    SCL_DEBUG();
    GdkPixmap* imgData = NULL;

    imgData = load_pixmap(imgPath);

    return imgData;
}

void
CSCLGraphicsImplGtk::unload_image(sclimage image_data)
{
    SCL_DEBUG();
    /* pre-condition */
    scl_assert_return(imgData);

    g_object_unref(static_cast<GdkPixmap*>(imgData));
}

/**
 * Initializes the drawing context for double-buffering.
 * This func should be called before using a drawing primitive at first.
 */
scldrawctx
CSCLGraphicsImplGtk::begin_paint(const sclwindow window, const sclboolean force_draw /* = FALSE */)
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
CSCLGraphicsImplGtk::end_paint(const sclwindow window, scldrawctx draw_ctx)
{
    cairo_destroy(static_cast<cairo_t*>(draw_ctx));
}

sclfont
CSCLGraphicsImplGtk::create_font(const SclFontInfo& info)
{
    return NULL;
}

void
CSCLGraphicsImplGtk::destroy_font(sclfont font)
{
}

/**
 * Draws the given text on cairo-surface
 */
void
CSCLGraphicsImplGtk::draw_text(sclwindow window, const scldrawctx draw_ctx, const SclFontInfo& font_info,
                               const SclColor& color, const sclchar *str, SclTextCachedInfo *cachedinfo,
                               sclint pos_x, sclint pos_y, sclint width, sclint height,
                               SCLLabelAlignment align, sclbyte padding)
{
    SCL_DEBUG();
    /* pre-condition */
    scl_assert_return(str);
    scl_assert_return(draw_ctx);

    cairo_t* cr = static_cast<cairo_t*>(draw_ctx);
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

    if (textExtents.width > width) {
        int newSize = fontinfo.fontsize - (fontinfo.fontsize * (float)(textExtents.width - width)) / width - 1;
        //printf("%s , %ld %ld %ld %ld %d\n", str, (int)(textExtents.width), (int)(textExtents.x_bearing), (int)(textExtents.x_advance), (int)(width), fontinfo.fontsize);
        cairo_set_font_size(cr, newSize);

        cairo_text_extents(cr, str, &textExtents);
        cairo_text_extents(cr, "|", &barExtents);
        //printf("%s , %ld %ld %ld %ld %d\n", str, (int)(textExtents.width), (int)(textExtents.x_bearing), (int)(textExtents.x_advance), (int)(width), newSize);
    }

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
 * Draws a rectangle on cairo-surface
 */
void
CSCLGraphicsImplGtk::draw_rectangle(sclwindow window, const scldrawctx draw_ctx,
                                    scldouble pos_x, scldouble pos_y, scldouble width, scldouble height,
                                    const scldouble line_width, const SclColor& line_color, sclboolean fill,
                                    const SclColor& fill_color, scldouble radius, sclfloat alpha)
{
    SCL_DEBUG();
    /* pre-condition */
    scl_assert_return(draw_ctx);

    cairo_t* cr = static_cast<cairo_t*>(draw_ctx);
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

SclSize
CSCLGraphicsImplGtk::get_image_size(sclchar* image_path)
{
    SCL_DEBUG();
    SclSize ret = { 0, 0 };
    GdkPixbuf *pixbuf = NULL;
    if (g_file_test(imgPath,G_FILE_TEST_EXISTS)) {
        pixbuf = gdk_pixbuf_new_from_file(imgPath, NULL);
    } else {
        return ret;
    }

    ret.width = gdk_pixbuf_get_width(pixbuf);
    ret.height = gdk_pixbuf_get_height(pixbuf);
    g_object_unref(pixbuf);
    return ret;
}

SclSize
CSCLGraphicsImplGtk::get_text_size(const SclFontInfo & fontinfo, const sclchar *str)
{
    SCL_DEBUG();
    SclSize ret = {0, 0};

    return ret;
}
