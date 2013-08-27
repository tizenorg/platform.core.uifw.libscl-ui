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

#include <map>

#include "scltypes.h"
#include "sclconfig.h"
#include "sclstructs.h"

#ifndef __SCL_UTILS_H__
#define __SCL_UTILS_H__

#ifdef __cplusplus
//SCL_BEGIN_DECLS
#endif

#ifndef _POSIX_PATH_MAX
#define _POSIX_PATH_MAX 255
#endif

namespace scl
{

/**
 * @brief The base class to work as a soft-based keyboard
 *
 * This class implements all functions for working as a soft-based keyboard
 * In side of ISE developer, they can modify it by their requirements.
 */
class CSCLUtilsImpl
{
public :
    virtual sclboolean get_screen_resolution(sclint *x, sclint *y) = 0;
    virtual sclboolean play_tts(const sclchar *str) = 0;
    virtual sclboolean play_sound(const sclchar* snd_style) = 0;
    virtual sclboolean play_vibration(const sclchar* vibe_style, const scllong duration) = 0;

    virtual sclboolean open_devices() = 0;
    virtual sclboolean close_devices() = 0;

    virtual sclint log(const sclchar *msg) = 0;
};

class CSCLUtilsImplLinux : public CSCLUtilsImpl
{
public :
    sclboolean get_screen_resolution(sclint *x, sclint *y);
    sclboolean play_tts(const sclchar *str);
    sclboolean play_sound(const sclchar* snd_style);
    sclboolean play_vibration(const sclchar* vibe_style, const scllong duration);

    sclboolean open_devices();
    sclboolean close_devices();

    sclint log(const sclchar *msg);
};

class CSCLUtilsImplWin32 : public CSCLUtilsImpl
{
public :
    sclboolean get_screen_resolution(sclint *x, sclint *y);
    sclboolean play_tts(const sclchar *str);
    sclboolean play_sound(const sclchar* snd_style);
    sclboolean play_vibration(const sclchar* vibe_style, const scllong duration);

    sclboolean open_devices();
    sclboolean close_devices();

    sclint log(const sclchar *msg);
};

class _CSCLUtils
{
public :
    _CSCLUtils() {
        m_impl = 0;
    }
    virtual sclboolean get_screen_resolution(sclint *x, sclint *y) = 0;
    virtual sclboolean play_tts(const sclchar *str) = 0;
    virtual sclboolean play_sound(const sclchar* snd_style) = 0;
    virtual sclboolean play_vibration(const sclchar* vibe_style, const scllong duration) = 0;

    virtual sclboolean open_devices() = 0;
    virtual sclboolean close_devices() = 0;

protected :
    CSCLUtilsImpl* GetCSCLUtilsImpl() {
        if (m_impl == 0) {
#ifdef  __WIN32__
            m_impl = new CSCLUtilsImplWin32;
#else
            m_impl = new CSCLUtilsImplLinux;
#endif
        }
        return m_impl;
    }

private :
    CSCLUtilsImpl* m_impl;
};

class CSCLUtils : public _CSCLUtils
{
private:
    static CSCLUtils* m_instance; /* For singleton */
    CSCLUtils();
public :
    ~CSCLUtils();

    static CSCLUtils* get_instance();

    void init();

    void scale_x(scl16 *x);
    void scale_y(scl16 *y);
    scl16 get_scaled_x(scl16 x);
    scl16 get_scaled_y(scl16 y);
    sclfloat get_smallest_scale_rate();
    sclfloat get_scale_rate_x();
    sclfloat get_scale_rate_y();
    sclfloat get_smallest_custom_scale_rate();
    sclfloat get_custom_scale_rate_x();
    sclfloat get_custom_scale_rate_y();

    void set_custom_scale_rate_x(sclfloat x);
    void set_custom_scale_rate_y(sclfloat y);

    sclchar* get_str_dup(const sclchar* str);

    sclboolean get_screen_resolution(sclint *x, sclint *y) {
        return GetCSCLUtilsImpl()->get_screen_resolution(x, y);
    }

    sclboolean play_tts(const sclchar* str) {
        return GetCSCLUtilsImpl()->play_tts(str);
    }

    sclboolean play_sound(const sclchar* snd_style) {
        return GetCSCLUtilsImpl()->play_sound(snd_style);
    }

    sclboolean play_vibration(const sclchar* vibe_style, const scllong duration) {
        return GetCSCLUtilsImpl()->play_vibration(vibe_style, duration);
    }

    sclboolean open_devices() {
        return GetCSCLUtilsImpl()->open_devices();
    }
    sclboolean close_devices() {
        return GetCSCLUtilsImpl()->close_devices();
    }

    sclboolean sleep(sclulong msc);

    sclchar* get_composed_path(sclchar* buf, const sclchar* prefix, const sclchar* path);
    sclchar* get_decomposed_path(sclchar* buf, const sclchar* prefix, const sclchar* path);
    const SclNinePatchInfo* get_nine_patch_info(const sclchar *image_path);

    sclboolean get_autopopup_window_variables(sclchar * const autopopup_keys[MAX_SIZE_OF_AUTOPOPUP_STRING], sclbyte *num_keys,
        sclbyte *num_columns, sclbyte *num_rows, sclint *width, sclint *height);

    scl16 get_unique_id();

    sclboolean is_rect_overlap(SclRectangle rect1, SclRectangle rect2);
    sclfloat get_distance(sclint x1, sclint y1, sclint x2, sclint y2);
    sclfloat get_distance(SclPoint pt1, SclPoint pt2);

    sclfloat get_approximate_distance(sclint x1, sclint y1, sclint x2, sclint y2);
    sclfloat get_approximate_distance(SclPoint pt1, SclPoint pt2);

    sclint log(const sclchar *fmt, ...);
private :
    sclfloat m_scn_rate_x;
    sclfloat m_scn_rate_y;

    sclfloat m_custom_scale_rate_x;
    sclfloat m_custom_scale_rate_y;

    sclint m_scn_resolution_x;
    sclint m_scn_resolution_y;

    std::map<sclchar*, SclNinePatchInfo> m_nine_patch_info_map;
};


} /* End of scl namespace */

#ifdef __cplusplus
//SCL_END_DECLS
#endif

#endif //__SCL_UTILS_H__
