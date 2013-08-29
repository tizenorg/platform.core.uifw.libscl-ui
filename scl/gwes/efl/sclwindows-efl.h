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

#include "sclwindows.h"
#include <Evas.h>
#include <Eina.h>

#ifndef __SCL_WINDOWS_EFL_H__
#define __SCL_WINDOWS_EFL_H__

typedef enum {
    EFLOBJECT_NONE,
    EFLOBJECT_IMAGE,
    EFLOBJECT_CLIPOBJECT,
    EFLOBJECT_TEXTBLOCK,
    EFLOBJECT_RECTANGLE,
} EFLOBJECT_TYPE;

namespace scl
{
typedef struct {
    EFLOBJECT_TYPE type;
    SclRectangle position;
    Evas_Object *object;
    char *etc_info;
    sclboolean extracted;
    void *data;
} EFLObject;
class CSCLWindowsImplEfl : public CSCLWindowsImpl
{
public :
    CSCLWindowsImplEfl();
    ~CSCLWindowsImplEfl();

    void init();
    void fini();

    sclwindow create_base_window(const sclwindow parent, SclWindowContext *winctx, scl16 width, scl16 height);
    sclwindow create_window(const sclwindow parent, SclWindowContext *winctx, scl16 width, scl16 height);
    sclwindow create_magnifier_window(const sclwindow parent, SclWindowContext *winctx, scl16 width, scl16 height);
    sclwindow create_dim_window(const sclwindow parent, SclWindowContext *winctx, scl16 width, scl16 height);
    bool destroy_window(sclwindow window);
    void show_window(const sclwindow window, sclboolean queue);
    void hide_window(const sclwindow window,  sclboolean fForce = FALSE);
    void move_window(const sclwindow window, scl16 x, scl16 y);
    void resize_window(const sclwindow window, scl16 width, scl16 height);
    void move_resize_window(const sclwindow window, scl16 x, scl16 y, scl16 width, scl16 height);
    void update_window(const sclwindow window, scl16 x = 0, scl16 y = 0, scl16 width = 0, scl16 height = 0);
    void set_window_rotation(const sclwindow window, SCLRotation rotation);
    void show_message_box(const sclwindow parent, scl8 msgType, sclchar* title, sclchar* msg);
    sclboolean get_window_rect(const sclwindow window, SclRectangle *rect);
    void set_parent(const sclwindow parent, const sclwindow window);
    void set_keep_above(const sclwindow window, sclboolean keepabove);


#ifndef APPLY_WINDOW_MANAGER_CHANGE
    /* EFL specific utility functions */
    void set_window_accepts_focus(const sclwindow window, sclboolean accepts);
#endif
};
} /* End of scl namespace */
#endif
