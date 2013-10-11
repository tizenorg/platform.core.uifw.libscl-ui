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

#ifndef __SCL_WINDOWS_H__
#define __SCL_WINDOWS_H__

#ifdef __cplusplus
//SCL_BEGIN_DECLS
#endif

namespace scl
{
#define MAX_POPUP_WINDOW 1
#define MAX_ZORDER_NUM ( MAX_POPUP_WINDOW + 1 ) /* Popup + Base */

#define SCL_WINDOW_Z_TOP 0

/**@brief A struct for identifying who opened this window */
typedef struct _SclWindowOpener {
    sclwindow   window;
    sclint      key;
} SclWindowOpener;

/**@brief  window context structure */
typedef struct _SclWindowContext {
    sclboolean          is_virtual;
    sclwindow           window;
    sclshort            inputmode;
    sclshort            layout;
    sclboolean          hidden;
    SCLPopupType        popup_type;

    SclWindowOpener     opener;
    SclRectangle        geometry;

    sclint              timeout;

    SclPoint            layout_image_offset;
    void*               etc_info;
} SclWindowContext;

/**
 * @brief The base class to work as a soft-based keyboard
 *
 * This class implements all functions for working as a soft-based keyboard
 * In side of ISE developer, they can modify it by their requirements.
 */
class CSCLWindowsImpl
{
public :
    virtual void init() = 0;
    virtual void fini() = 0;

    virtual sclwindow create_base_window(const sclwindow parent, SclWindowContext *winctx, scl16 width, scl16 height) = 0;
    virtual sclwindow create_window(const sclwindow parent, SclWindowContext *winctx, scl16 width, scl16 height) = 0;
    virtual sclwindow create_magnifier_window(const sclwindow parent, SclWindowContext *winctx, scl16 width, scl16 height) = 0;
    virtual sclwindow create_dim_window(const sclwindow parent, SclWindowContext *winctx, scl16 width, scl16 height) = 0;
    virtual bool destroy_window(sclwindow window) = 0;
    virtual void set_parent(const sclwindow parent, const sclwindow window) = 0;
    virtual void show_window(const sclwindow window, sclboolean queue) = 0;
    virtual void set_window_rotation(const sclwindow window, SCLRotation rotation) = 0;
    virtual void hide_window(const sclwindow window,  sclboolean force = FALSE) = 0;
    virtual void move_window(const sclwindow window, scl16 x, scl16 y) = 0;
    virtual void move_resize_window(const sclwindow window, scl16 x, scl16 y, scl16 width, scl16 height) = 0;
    virtual void resize_window(const sclwindow window, scl16 width, scl16 height) = 0;
    virtual void update_window(const sclwindow window, scl16 x = 0, scl16 y = 0, scl16 width = 0, scl16 height = 0) = 0;
    virtual sclboolean get_window_rect(const sclwindow window, SclRectangle *rect) = 0;

    virtual void set_keep_above(const sclwindow window, sclboolean keep_above) = 0;
};

class CSCLWindows
{
private:
    CSCLWindows();
public :
    ~CSCLWindows();

    static CSCLWindows* get_instance();

    void init();
    void fini();

    sclwindow open_popup(const SclWindowOpener opener, const SclRectangle& geometry, sclshort inputmode, sclshort layout, SCLPopupType popup_type, sclboolean is_virtual, sclboolean use_dim_window, sclint img_offset_x = 0, sclint img_offset_y = 0, sclint timeout = 0);
    bool close_popup(sclwindow window);
    bool close_all_popups(sclwindow skip_window = SCLWINDOW_INVALID);

    sclwindow create_base_window(const sclwindow parent, scl16 width, scl16 height);

    void show_window(const sclwindow window, sclboolean queue = FALSE);
    void hide_window(const sclwindow window,  sclboolean force = FALSE);
    void set_parent(const sclwindow parent, const sclwindow window);
    void set_keep_above(const sclwindow window, sclboolean keep_above);

    void set_window_rotation(const sclwindow window, SCLRotation rotation);

    void move_window(const sclwindow window, scl16 x, scl16 y);
    void resize_window(const sclwindow window, scl16 width, scl16 height);
    void move_resize_window(const sclwindow window, scl16 x, scl16 y, scl16 width, scl16 height);
    void update_window(const sclwindow window, scl16 x = 0, scl16 y = 0, scl16 width = 0, scl16 height = 0);
    sclboolean get_window_rect(const sclwindow window, SclRectangle *rect);
    sclwindow get_base_window();
    sclboolean is_base_window(sclwindow window);
    sclwindow find_by_etcinfo(void* etc_info);
    scl8 find_popup_window_index(sclwindow window);
    //SclWindowContext* get_window_context(sclwindow window, sclboolean geometry_update = FALSE);
    SclWindowContext* get_window_context(sclwindow window);
    void set_window_context(sclwindow window, SclWindowContext* context);

    sclwindow create_magnifier_window(const sclwindow parent, scl16 x, scl16 y, scl16 width, scl16 height);
    sclwindow get_magnifier_window();

    sclwindow get_nth_window_in_Z_order_list(sclbyte index);
    sclwindow get_nth_popup_window(sclbyte index);
    sclbyte get_Z_order(sclwindow window);

    sclwindow create_dim_window(const sclwindow parent, SclWindowContext *winctx, scl16 width, scl16 height);
    sclwindow get_dim_window();

    void set_update_pending(sclboolean pend);
    sclboolean get_update_pending();

protected :
    CSCLWindowsImpl* get_scl_windows_impl();

    sclwindow create_window(const SclWindowOpener opener, const SclRectangle &geometry, sclshort inputmode, sclshort layout, SCLPopupType popup_type, sclboolean is_virtual, sclint img_offset_x = 0, sclint img_offset_y = 0, sclint timeout = 0);
    bool destroy_window(sclwindow window);

    void push_window_in_Z_order_list(sclwindow window);
    void pop_window_in_Z_order_list(sclwindow window);

private :
    CSCLWindowsImpl* m_impl;
    SclWindowContext m_base_winctx;
    SclWindowContext m_popup_winctx[MAX_POPUP_WINDOW];

    SclWindowContext m_magnifier_winctx;
    SclWindowContext m_dim_winctx;

    sclboolean m_pending_update;
    sclboolean m_initialized;

    sclwindow m_Z_order_list[MAX_ZORDER_NUM];
};


} /* End of scl namespace */

#ifdef __cplusplus
//SCL_END_DECLS
#endif

#endif //__SCL_WINDOWS_H__
