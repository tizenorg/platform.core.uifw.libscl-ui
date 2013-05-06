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

#include "sclwindows-gtk.h"
#include "scldebug.h"
#include "sclcontroller.h"
#include "sclcontext.h"
#include "sclresourcecache.h"

using namespace scl;

/**
 * Constructor
 */
CSCLWindowsImplGtk::CSCLWindowsImplGtk()
{
    SCL_DEBUG();
    /* Initializes all window resources */
}

/**
 * De-constructor
 */
CSCLWindowsImplGtk::~CSCLWindowsImplGtk()
{
    SCL_DEBUG();
}

/**
 * Make a transparent window
 */
static void
make_transparent_window (GtkWidget *window)
{
    SCL_DEBUG();
    GdkScreen *screen;
    GdkColormap *colormap;

    screen = gtk_widget_get_screen (window);
    if (!screen) {
        g_warning ("failed to get window's screen");
        return;
    }

    colormap = gdk_screen_get_rgba_colormap (screen);
    if (!colormap) {
        g_warning ("failed to get RGBA colormap");
        return;
    }

    gtk_widget_set_colormap (window, colormap);
    gtk_widget_set_app_paintable (window, TRUE);
    gtk_widget_realize (window);
    //gdk_gc_set_alpha(window->style->bg_gc[GTK_STATE_NORMAL], 0);
    //gdk_window_set_alpha(window->window, 0);
}


/**
 * Create a content window and binds it into given parent window as a child
 */
sclwindow
CSCLWindowsImplGtk::create_base_window(const sclwindow parent, SclWindowContext *winctx, scl16 width, scl16 height)
{
    SCL_DEBUG();

    GtkWidget* window = NULL;

    /* pre-condition */
    scl_assert(parent != NULL);
    scl_assert(winctx != NULL);
    scl_assert(winctx->etc_info == NULL);

    if (winctx->window == NULL) {
        window = (GtkWidget*)parent;

        if (GTK_WIDGET_TOPLEVEL(window)) {
            window = (GtkWidget*)parent;
            winctx->etc_info = NULL;
            winctx->window = window;
            //gtk_window_set_keep_above(GTK_WINDOW(window), TRUE);
            make_transparent_window(window);
        } else {
            GtkWidget *drawarea = NULL;
            drawarea = gtk_drawing_area_new();
            gtk_widget_set_size_request(window, width, height);
            gtk_widget_set_size_request(drawarea, width, height);
            gtk_box_pack_start(GTK_BOX(window), drawarea, TRUE, TRUE, 0);
            gtk_widget_show (drawarea);
            gtk_widget_show (window);
            winctx->etc_info = static_cast<void*>(drawarea);
            winctx->window = drawarea;
            make_transparent_window(drawarea);
        }
    }
    /* post-condition */
    return winctx->window;
}

/**
 * Creates a window
 */
sclwindow
CSCLWindowsImplGtk::create_window(const sclwindow parent, SclWindowContext *winctx, scl16 width, scl16 height)
{
    SCL_DEBUG();
    /* pre-condition */
    scl_assert(parent);
    scl_assert(winctx);
    scl_assert(winctx->etc_info == NULL);

    GtkWidget* window = NULL;

    if (winctx->window == NULL) {
        window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
        gtk_window_set_type_hint(GTK_WINDOW (window), GDK_WINDOW_TYPE_HINT_UTILITY);
#ifdef NO_SOCKETPLUG
        //gtk_window_set_type_hint(GTK_WINDOW (window), GDK_WINDOW_TYPE_HINT_NOTIFICATION);
        //gtk_window_set_keep_above(GTK_WINDOW(window), TRUE);
#endif
        gtk_window_set_accept_focus(GTK_WINDOW(window), FALSE);
#ifdef NO_SOCKETPLUG
        GtkWidget* toplevel = gtk_widget_get_toplevel((GtkWidget*)parent);
        gtk_window_set_transient_for(GTK_WINDOW(window), GTK_WINDOW(toplevel));
#endif
        /* Sets a window relation between parent and popup */
        make_transparent_window(window);
        gtk_window_set_default_size(GTK_WINDOW(window), width, height);
        //gtk_window_set_keep_above(GTK_WINDOW(window), TRUE);
#ifndef NO_SOCKETPLUG
        //gtk_window_set_keep_above(GTK_WINDOW(window), TRUE);
        gtk_widget_set_app_paintable(window, TRUE);
#endif
        //gtk_effect_set_enable (GTK_WIDGET(window), FALSE);
        winctx->etc_info = NULL;
        winctx->window = window;


        /* Window rotation*/
        CSCLContext* context = CSCLContext::get_instance();

        if (context->get_display() == DISPLAY_LANDSCAPE) {
            set_window_rotation(window, 90);
        }

        //gtk_widget_realize(window);
    }

    return window;
}

/**
 * Creates the dim window
 */
sclwindow
CSCLWindowsImplGtk::create_dim_window(const sclwindow parent, SclWindowContext *winctx, scl16 width, scl16 height)
{
    SCL_DEBUG();
    /* pre-condition */
    scl_assert(parent);
    scl_assert(winctx);
    scl_assert(winctx->etc_info == NULL);
    GtkWidget* window = NULL;

    if (winctx->window == NULL) {
        window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_type_hint(GTK_WINDOW (window), GDK_WINDOW_TYPE_HINT_UTILITY);
#ifdef NO_SOCKETPLUG
        //gtk_window_set_type_hint(GTK_WINDOW (window), GDK_WINDOW_TYPE_HINT_NOTIFICATION);
#endif
        gtk_widget_set_name(window, "dialog_dim_window");
#ifdef NO_SOCKETPLUG
        gtk_window_set_transient_for(GTK_WINDOW(window), GTK_WINDOW(gtk_widget_get_toplevel((GtkWidget*)parent)));
#else
        gtk_window_set_type_hint(GTK_WINDOW(window), GDK_WINDOW_TYPE_HINT_POPUP_MENU);
        //gtk_window_set_transient_for(GTK_WINDOW(gtk_widget_get_toplevel((GtkWidget*)parent)), GTK_WINDOW(window));
#endif
        //gtk_window_set_keep_above(GTK_WINDOW(window), TRUE);
        gtk_window_set_accept_focus(GTK_WINDOW(window), FALSE);
        gtk_widget_set_size_request(window, width, height);
        gtk_window_set_opacity(GTK_WINDOW(window), 0.5);
        gtk_widget_realize(window);
        //gtk_effect_set_enable (GTK_WIDGET(window), FALSE);
        winctx->etc_info = NULL;
        winctx->window = window;
    }

    return window;
}

/**
 * Make a window relation between parent and child
 * Caution: Currently, If we use transient_for them the ISE will occure some crash. It needs to check X11
 */
void
CSCLWindowsImplGtk::set_parent(const sclwindow parentWindow, const sclwindow window)
{
    SCL_DEBUG();
    scl_assert_return(window);
    scl_assert_return(parentWindow);
#ifdef NO_SOCKETPLUG
    GtkWidget* widget = static_cast<GtkWidget*>(parentWindow);
    GtkWidget* toplevel = gtk_widget_get_toplevel(widget);
#else
    GtkWindow* gtkParentWindow = static_cast<GtkWindow*>(parentWindow);
#endif
    GtkWindow* gtkWindow = static_cast<GtkWindow*>(window);
#ifdef NO_SOCKETPLUG
    gtk_window_set_transient_for(gtkWindow, GTK_WINDOW(toplevel));
#endif
}

/**
 * Destroys the given window
 */
bool
CSCLWindowsImplGtk::destroy_window(sclwindow window)
{
    SCL_DEBUG();
    scl_assert_return_false(window);
    /* add codes for destroying window */
    GtkWidget* widget = static_cast<GtkWidget*>(window);
    gtk_widget_destroy(widget);
    return TRUE;
}

/**
 * Shows the given window
 */
void
CSCLWindowsImplGtk::show_window(const sclwindow window, sclboolean queue)
{
    SCL_DEBUG();
    scl_assert_return(window);
    GtkWidget* widget = static_cast<GtkWidget*>(window);
    gtk_widget_show(widget);

    /*Forcing expose on the magnifier window*/
    //if (widget->window) {
    if (queue) {
        gtk_widget_queue_draw(widget);
    } else {
        gdk_window_invalidate_rect(widget->window, NULL, TRUE);
        gdk_window_process_updates(widget->window, TRUE);
    }
    //}

    /*we will use always on top style when the window is created */
    /*we cannot use keep_above, becase the other toplevel window cannot appear if we use it*/
    /* X team will fix it */
    CSCLWindows *windows = CSCLWindows::get_instance();
    if (windows->get_base_window() != window && windows->get_dim_window() != window) {
#ifndef NO_SOCKETPLUG
        gdk_window_raise(widget->window);
#endif
        //gtk_window_set_keep_above(GTK_WINDOW(window), TRUE);
    } else if (windows->get_dim_window() == window) {
#ifndef NO_SOCKETPLUG
        gdk_window_raise(widget->window);
#endif
    } else if (windows->get_base_window() == window) {
#ifndef NO_SOCKETPLUG
        gdk_window_raise(gtk_widget_get_toplevel(widget)->window);
#endif
    }
}

/**
 * Hides the given window
 */
void
CSCLWindowsImplGtk::hide_window(const sclwindow window,  sclboolean fForce)
{
    SCL_DEBUG();
    scl_assert_return(window);
    GtkWidget* widget = static_cast<GtkWidget*>(window);
    CSCLWindows *windows = CSCLWindows::get_instance();
    if (windows->get_magnifier_window() == window && fForce == FALSE) {
        /* Fix me : The below is a temporary code for magnifier speed enhancement */
#ifdef NO_SOCKETPLUG
        gint root_x, root_y;
        gtk_window_get_position (GTK_WINDOW(widget), &root_x, &root_y);
        if (GTK_WIDGET_MAPPED(widget)) {

            /* Window rotation*/
            CSCLContext* context = CSCLContext::get_instance();
            if (context->get_display() == DISPLAY_LANDSCAPE) {
                gtk_window_move(GTK_WINDOW(widget), gdk_screen_get_height(gdk_screen_get_default()) - 1, gdk_screen_get_width(gdk_screen_get_default())-1);
            } else {
                gtk_window_move(GTK_WINDOW(widget), gdk_screen_get_width(gdk_screen_get_default()) - 1, gdk_screen_get_height(gdk_screen_get_default())-1);
            }
#else
        if (GTK_WIDGET_VISIBLE(widget)) {
            gtk_window_move(GTK_WINDOW(widget), 0, gdk_screen_get_height(gdk_screen_get_default())+1);
#endif
        }
    } else {
        if (GTK_WIDGET_VISIBLE(widget)) {
            gtk_widget_hide(widget);
        }
    }
}

/**
 * Moves the window to the given position
 */
void
CSCLWindowsImplGtk::move_window(const sclwindow window, scl16 x, scl16 y) {
    SCL_DEBUG();
    scl_assert_return(window);
    GtkWidget* widget = static_cast<GtkWidget*>(window);
    gtk_window_move(GTK_WINDOW(widget), x, y);
}

/**
* Resizes the window to the given metric
*/
void
CSCLWindowsImplGtk::resize_window(const sclwindow window, scl16 width, scl16 height) {
    SCL_DEBUG();
    scl_assert_return(window);
    GtkWidget* widget = static_cast<GtkWidget*>(window);
    if (GTK_IS_WINDOW(widget)) {
        //gtk_window_set_default_size(GTK_WINDOW(widget), width, height);
        gtk_widget_set_size_request(GTK_WIDGET(widget), width, height);
    }
}

/**
* Resizes the window to the given metric
*/
void
CSCLWindowsImplGtk::move_resize_window(const sclwindow window, scl16 x, scl16 y, scl16 width, scl16 height) {
    SCL_DEBUG();
    scl_assert_return(window);
    GtkWidget* widget = static_cast<GtkWidget*>(window);
    gdk_window_move_resize(GDK_WINDOW(widget->window), x, y, width, height);
}


/**
* Update the window to redraw given area
*/
void
CSCLWindowsImplGtk::update_window(const sclwindow window, scl16 x, scl16 y, scl16 width, scl16 height) {
    SCL_DEBUG();
    scl_assert_return(window);
    GtkWidget* widget = static_cast<GtkWidget*>(window);

    if (!GTK_WIDGET_VISIBLE(widget)) gtk_widget_show(widget);

    if (x + y + width + height == 0) {
        gdk_window_invalidate_rect(widget->window, NULL, TRUE);
    } else {
        GdkRectangle rect = { x, y, width, height} ;
        gdk_window_invalidate_rect(widget->window, &rect, TRUE);
    }

    gdk_window_process_updates(widget->window, TRUE);

    /* It needs test
    gtk_widget_queue_draw(widget);
    gtk_widget_queue_draw_area(widget, x, y, width, height);
    gdk_window_invalidate_rect(widget->window, NULL, FALSE);
    GdkRectangle rect = { x, y, width, height} ;
    gdk_window_invalidate_rect(widget->window, &rect, FALSE);
    */
}

/**
 * Returns the position of x,y,width,height of the given window
 */
sclboolean
CSCLWindowsImplGtk::get_window_rect(const sclwindow window, SclRectangle *rect) {
    SCL_DEBUG();
    sclboolean ret = FALSE;
    GtkWidget* widget = static_cast<GtkWidget*>(window);
    if (rect && GTK_WIDGET_REALIZED(widget)) {

#if 0
        /* FIXME : We need to get appropriate position of the base window somehow */
        // {
        /* Maybe this bunch of code would help
        XTranslateCoordinates (GDK_WINDOW_XDISPLAY (window),
        GDK_WINDOW_XID (window),
        GDK_WINDOW_XROOTWIN (window),
        0, 0, &tx, &ty,
        &child);
        */
#endif
        CSCLContext *context = CSCLContext::get_instance();
        CSCLWindows *windows = CSCLWindows::get_instance();
        if (context->get_display() == DISPLAY_LANDSCAPE) {
            gdk_window_get_root_origin (GDK_WINDOW(gtk_widget_get_toplevel(widget)->window),  &(rect->x), &(rect->y));
            if (window == windows->get_base_window()) {
                CSCLUtils *utils = CSCLUtils::get_instance();
                CSCLResourceCache *cache = CSCLResourceCache::get_instance();

                sclint scnWidth, scnHeight;
                utils->get_screen_resolution(&scnWidth, &scnHeight);
                rect->y = scnWidth - cache->get_cur_layout(window)->height;
            }
        } else {
            gdk_window_get_root_origin (GDK_WINDOW(gtk_widget_get_toplevel(widget)->window),  &(rect->x), &(rect->y));
            if (window == windows->get_base_window()) {
                CSCLUtils *utils = CSCLUtils::get_instance();
                CSCLResourceCache *cache = CSCLResourceCache::get_instance();

                sclint scnWidth, scnHeight;
                utils->get_screen_resolution(&scnWidth, &scnHeight);
                rect->y = scnHeight - cache->get_cur_layout(window)->height;
            }
        }
        gtk_window_get_size (GTK_WINDOW(gtk_widget_get_toplevel(widget)), &(rect->width), &(rect->height));

        ret = TRUE;
    }
    return ret;
}

/**
 * Sets rotation
 */
void
CSCLWindowsImplGtk::set_window_rotation(const sclwindow window, sclint degree) {
    SCL_DEBUG();
    GtkWidget* widget = gtk_widget_get_toplevel(static_cast<GtkWidget*>(window));
    //gtk_window_set_rotate(GTK_WINDOW(widget), degree);
}


/**
 * Shows a message box
 */
void
CSCLWindowsImplGtk::show_message_box(const sclwindow parent, scl8 msgType, sclchar* title, sclchar* msg) {
    SCL_DEBUG();
    scl_assert_return(strlen(msg) > 0);

    GtkWidget *dialog = NULL;

    switch (msgType) {
    case SCL_MSG_BOX_TYPE_INFO:
        dialog = gtk_message_dialog_new((GtkWindow*)parent,
                                        GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_INFO,
                                        GTK_BUTTONS_OK,
                                        msg, "");
        break;
    case SCL_MSG_BOX_TYPE_ERROR:
        dialog = gtk_message_dialog_new((GtkWindow*)parent,
                                        GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_ERROR,
                                        GTK_BUTTONS_OK,
                                        msg);
        break;
    case SCL_MSG_BOX_TYPE_WARNING:
        dialog = gtk_message_dialog_new((GtkWindow*)parent,
                                        GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_WARNING,
                                        GTK_BUTTONS_OK,
                                        msg);
        break;
    default:
        break;
    }

    if (dialog) {
        gtk_window_set_title(GTK_WINDOW(dialog), title);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}


void
CSCLWindowsImplGtk::set_keep_above(const sclwindow window, sclboolean keepabove) {
    SCL_DEBUG();

    gtk_window_set_keep_above(GTK_WINDOW(window), keepabove);
}
