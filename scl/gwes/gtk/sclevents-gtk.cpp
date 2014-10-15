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

#include "sclevents-gtk.h"
#include "scldebug.h"
#include "sclcontroller.h"
#include "sclgraphics.h"
#include "scluibuilder.h"

using namespace scl;

sclboolean mouse_pressed = FALSE; /* Checks whether mouse is pressed or not */
sclboolean skip_mouse_events_until_pressed = FALSE; /*Explicitly filter out any mouse events until mouse press occurs */

#define MIN_XY_DIFF 14
static sclint latest_pos_x = 0;
static sclint latest_pos_y = 0;

/**
 * Constructor
 */
CSCLEventsImplGtk::CSCLEventsImplGtk()
{
    SCL_DEBUG();
    /* Initializes all window resources */
}

/**
 * De-constructor
 */
CSCLEventsImplGtk::~CSCLEventsImplGtk()
{
    SCL_DEBUG();
}

/*
static void
get_time(char *func)
{
    char buffer[30];
    struct timeval tv;
    time_t curtime;

    gettimeofday(&tv, NULL);
    curtime=tv.tv_sec;

    strftime(buffer, 30, "%m-%d-%Y %T.", localtime(&curtime));
    g_print("[%s]%s%ld\n",func, buffer, tv.tv_usec);

}
*/

/* We have to remember the previous position of magnifier window, since it gets changed when mouse press occurs */
sclint magpressposx = 0;
sclint magpressposy = 0;
/**
* Generates a mouse press event to CSCLController class
*/
bool
mouse_press(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
    SCL_DEBUG();
    /* pre-condition */
    CSCLController *controller;
    sclboolean processed = FALSE;

    skip_mouse_events_until_pressed = FALSE;

    controller = CSCLController::get_instance();

    CSCLWindows *windows = CSCLWindows::get_instance();
    GtkWidget* magnifier = static_cast<GtkWidget*>(windows->get_magnifier_window());
    gdk_window_raise(magnifier->window);
    if (widget == windows->get_magnifier_window()) {
        SclWindowContext *magnifier_window_context = windows->get_window_context(widget, FALSE);
        if (magnifier_window_context) {
            magpressposx = magnifier_window_context->x;
            magpressposy = magnifier_window_context->y;
        }
        /* First convert the local coordinate to global coordinate */
        sclwindow window = SCLWINDOW_INVALID;
        sclbyte index = 0;
        SclWindowContext *context = NULL;
        context = windows->get_window_context(static_cast<sclwindow>(user_data), FALSE);

        sclint x = event->x;
        sclint y = event->y;

        x = context->x + x;
        y = context->y + y;

        do {
            window = windows->get_nth_window_in_Z_order_list(index);
            context = windows->get_window_context(window, FALSE);

            if (context) {
                if ((x >= context->x && x <= (context->x + context->width)) &&
                        (y >= context->y && y <= (context->y + context->height))) {
                    /* Now convert the global coordinate to appropriate local coordinate */
                    x = x - context->x;
                    y = y - context->y;

                    if (controller) {
                        controller->mouse_press(window, x, y);
                        mouse_pressed = TRUE;
                        processed = TRUE;
                    }
                }
            }

            index++;
        } while (index < MAX_ZORDER_NUM && window != SCLWINDOW_INVALID);
    } else if (widget == windows->get_dim_window()) {
        controller->mouse_press(static_cast<sclwindow>(user_data), event->x, event->y);
        processed = TRUE;
        skip_mouse_events_until_pressed = TRUE;
    }

    controller = CSCLController::get_instance();
    if (controller && !mouse_pressed && !processed) {
        controller->mouse_press(static_cast<sclwindow>(user_data), event->x, event->y);
        mouse_pressed = TRUE;
    }

    latest_pos_x = event->x;
    latest_pos_x = event->y;

    /* post-condition */
    return TRUE;
}

/**
 * Generates a mouse release event to CSCLController class
 */
bool
mouse_release(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
    SCL_DEBUG();
    /* pre-condition */
    CSCLController *controller;
    CSCLWindows *windows;

    controller = CSCLController::get_instance();
    windows = CSCLWindows::get_instance();

    sclint x = event->x;
    sclint y = event->y;

    if (mouse_pressed) {
        sclboolean processed = FALSE;
        sclbyte index = 0;
        sclwindow window = SCLWINDOW_INVALID;
        SclWindowContext *context = NULL;

        /* First convert the local coordinate to global coordinate */
        context = windows->get_window_context(static_cast<sclwindow>(user_data), FALSE);
        if (widget == windows->get_magnifier_window()) {
            //context->x = magpressposx;
            //context->y = magpressposy;

            /* Sometimes the event struct contains incorrect mouse position, when magnifier window gets moved by clicking on it */
            GtkWidget* widget = static_cast<GtkWidget*>(windows->get_base_window());
            gdk_window_get_pointer(widget->window, &x, &y, NULL);
            context = windows->get_window_context(windows->get_base_window(), FALSE);
        }

        x = context->x + x;
        y = context->y + y;

        do {
            window = windows->get_nth_window_in_Z_order_list(index);
            context = windows->get_window_context(window, FALSE);

            if (context) {
                if ((x >= context->x && x <= (context->x + context->width)) &&
                        (y >= context->y && y <= (context->y + context->height))) {
                    /* Now convert the global coordinate to appropriate local coordinate */
                    x = x - context->x;
                    y = y - context->y;

                    if (controller) {
                        controller->mouse_release(window, x, y);
                        processed = TRUE;
                    }
                }
            }

            index++;
        } while (index < MAX_ZORDER_NUM && window != SCLWINDOW_INVALID);

        /* No window exists on that point */
        if (!processed) {
            if (controller) {
                controller->mouse_release(static_cast<sclwindow>(user_data), event->x, event->y);
            }
        }
    } else if (!skip_mouse_events_until_pressed) {
        scl_assert((FALSE && "Mouse released but mouse_pressed is FALSE"));
        if (controller) {
            controller->mouse_release(static_cast<sclwindow>(user_data), event->x, event->y);
        }
    }

    mouse_pressed = FALSE;
    /* post-condition */
    return TRUE;
}

/**
 * Generates a update window event to CSCLController class
 */
bool
update_window (GtkWidget *widget, GdkEventExpose *event,gpointer user_data)
{
    SCL_DEBUG();
    /* pre-condition */
    CSCLGwes* gwes = CSCLGwes::get_instance();
    CSCLUIBuilder *builder = CSCLUIBuilder::get_instance();

    if (gwes && builder)	{
        sclwindow window = static_cast<sclwindow>(widget);

#if 0
        /* For making the transparent window */
        cairo_t   *cr;
        cr = gdk_cairo_create (widget->window);
        /* Make it transparent */
        cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
        gdk_cairo_region (cr, event->region);
        cairo_fill_preserve (cr);
        /* Make it half-transparent */
        if (gwes->mWnd->get_base_window() == window) {
            cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 0.9);
        } else {
            cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 0.0);
        }
        cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
        cairo_fill (cr);
        cairo_destroy (cr);
#endif

        return builder->show_layout(window);
    }

    /* post-condition */
    return FALSE;
}


/**
 * Generates a mouse move event to CSCLController class
 */
bool
mouse_move(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
    SCL_DEBUG();
    /* pre-condition */
    CSCLController *controller;
    CSCLWindows *windows;
    CSCLUtils *utils;

    controller = CSCLController::get_instance();
    windows = CSCLWindows::get_instance();
    utils = CSCLUtils::get_instance();

    sclint x = event->x;
    sclint y = event->y;

    if (abs(latest_pos_x -x) < utils->get_scale_x(MIN_XY_DIFF) && abs(latest_pos_x -y) < utils->get_scale_x(MIN_XY_DIFF)) {
        return FALSE;
    } else {
        latest_pos_x = x;
        latest_pos_x = y;
    }

    if (mouse_pressed) {
        sclbyte index = 0;
        sclwindow window = SCLWINDOW_INVALID;
        SclWindowContext *context = NULL;

        /* First convert the local coordinate to global coordinate */
        context = windows->get_window_context(static_cast<sclwindow>(user_data), FALSE);
        if (widget == windows->get_magnifier_window()) {
            //context->x = magpressposx;
            //context->y = magpressposy;

            GtkWidget* widget = static_cast<GtkWidget*>(windows->get_base_window());
            gdk_window_get_pointer(widget->window, &x, &y, NULL);
            context = windows->get_window_context(windows->get_base_window(), FALSE);
        }
        x = context->x + x;
        y = context->y + y;

        sclboolean bProcessed = FALSE;

        do {
            window = windows->get_nth_window_in_Z_order_list(index);
            context = windows->get_window_context(window, FALSE);

            if (context) {
                if ((x >= context->x && x <= (context->x + context->width)) &&
                        (y >= context->y && y <= (context->y + context->height))) {
                    /* Now convert the global coordinate to appropriate local coordinate */
                    x = x - context->x;
                    y = y - context->y;

                    if (controller) {
                        controller->mouse_move(window, x, y);
                        //bProcessed = TRUE;
                    }
                }
            }

            index++;
        } while (index < MAX_ZORDER_NUM && window != SCLWINDOW_INVALID && !bProcessed);

    } else if (!skip_mouse_events_until_pressed) {
        if (controller) {
            controller->mouse_move(static_cast<sclwindow>(user_data), event->x, event->y);
        }
    }
    /* post-condition */
    return TRUE;
}

/**
 * Regists a event callback func to given window.
 * In this function, it should call serveral event functions of CSCLController class whenever an event has occured
 * The below list shows what event function should be called.
 * - mouse_press (when the user presses mouse button)
 * - mouse_release (when the user releases mouse button)
 * - mouse_move (when the user drags mouse button)
 * - show_base_layout (when the expost event has occured)
 */
void
CSCLEventsImplGtk::connect_window_events(const sclwindow window, const sclint event)
{
    SCL_DEBUG();
    /* pre-condition */
    scl_assert_return(wnd != NULL);

    GtkWidget* widget = static_cast<GtkWidget*>(wnd);

    gtk_widget_add_events ((GtkWidget*)widget, GDK_BUTTON_PRESS_MASK |
                           GDK_BUTTON_RELEASE_MASK |
                           GDK_BUTTON_MOTION_MASK |
                           GDK_POINTER_MOTION_HINT_MASK);

    if (evt & SCL_EVENT_MOUSE) {
        g_signal_connect(G_OBJECT(widget), "button_press_event", G_CALLBACK(mouse_press), static_cast<gpointer>(wnd));
        g_signal_connect(G_OBJECT(widget), "button_release_event", G_CALLBACK(mouse_release), static_cast<gpointer>(wnd));
        g_signal_connect(G_OBJECT(widget), "motion_notify_event", G_CALLBACK(mouse_move), static_cast<gpointer>(wnd));
    }

    if (evt & SCL_EVENT_EXPOSE) {
        g_signal_connect(G_OBJECT(widget), "expose_event", G_CALLBACK(update_window), static_cast<gpointer>(wnd));
    }

    /* post-condition */
}

/**
 * Generates a timer event to CSCLController class
 */
gboolean
timer_event(gpointer data)
{
    SCL_DEBUG();
    scl32 sendData = static_cast<scl32>(reinterpret_cast<uintptr_t>(data) & 0xffffffff);
    CSCLController *controller;
    controller = CSCLController::get_instance();
    if (controller) {
        return controller->timer_event(sendData);
    }
    return TRUE;
}

/**
 * Creates a timer
 * In this function, it should call timer_event of CSCLController class
 */
void
CSCLEventsImplGtk::create_timer(const scl16 id, const scl32 interval, scl16 value, sclboolean add_to_map)
{
    SCL_DEBUG();
    sclint data = SCL_MAKELONG(id, value);
    gint timerId = gtk_timeout_add (static_cast<guint32>(interval), timer_event, (gpointer)data);
    if (add_to_map) {
        id_map[id] = timerId;
    }
}

/**
 * Destroys the given ID's timer
 */
void
CSCLEventsImplGtk::destroy_timer(const scl32 id)
{
    SCL_DEBUG();
    for ( std::map<int, int>::iterator idx = id_map.begin(); idx != id_map.end(); ++idx) {
        if ((*idx).first == id) {
            gtk_timeout_remove ((*idx).second);
            id_map.erase((*idx).first);
            break;
        }
    }
}

/**
 * Destroys all of created timer
 */
void
CSCLEventsImplGtk::destroy_all_timer()
{
    SCL_DEBUG();
    for ( std::map<int, int>::iterator idx = id_map.begin(); idx != id_map.end(); ++idx) {
        gtk_timeout_remove ((*idx).second);
    }
    id_map.clear();
}

