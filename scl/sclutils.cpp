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

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <stdarg.h>

#include "sclutils.h"
#include "sclconfig.h"
#include "scldebug.h"
#include "sclcontext.h"
#include "sclresource.h"
#include "sclresourcecache.h"
#include "sclres_manager.h"

using namespace scl;

#define SCL_MAX_UNIQUE_ID 1000

CSCLUtils::CSCLUtils()
{
    SCL_DEBUG();
    m_scn_rate_x = 1.0f;
    m_scn_rate_y = 1.0f;
    m_custom_scale_rate_x = 1.0f;
    m_custom_scale_rate_y = 1.0f;
    m_scn_resolution_x = 0;
    m_scn_resolution_y = 0;
}

CSCLUtils::~CSCLUtils()
{
    SCL_DEBUG();
}

CSCLUtils* CSCLUtils::get_instance()
{
    static CSCLUtils instance;
    return &instance;
}

void
CSCLUtils::init()
{
    SCL_DEBUG();

    CSCLUtilsImpl *impl = GetCSCLUtilsImpl();
    if (impl) {
        impl->init();
    }

    SclResParserManager *sclres_manager = SclResParserManager::get_instance();
    PSclDefaultConfigure default_configure = NULL;
    if (sclres_manager) {
        default_configure = sclres_manager->get_default_configure();
    }

    if (default_configure) {
        scl_assert(default_configure->target_screen_width);
        scl_assert(default_configure->target_screen_height);
        if (default_configure->target_screen_width != 0 &&default_configure->target_screen_height != 0) {
            GetCSCLUtilsImpl()->get_screen_resolution(&m_scn_resolution_x, &m_scn_resolution_y);
            m_scn_rate_x = m_scn_resolution_x / (float)default_configure->target_screen_width;
            m_scn_rate_y = m_scn_resolution_y / (float)default_configure->target_screen_height;

            if (default_configure->auto_detect_landscape) {
                /* If the width of screen is bigger than the height, switch portrait mode and landscape mode */
                if (m_scn_resolution_x > m_scn_resolution_y) {
                    m_scn_rate_x = m_scn_resolution_x / (float)default_configure->target_screen_height;
                    m_scn_rate_y = m_scn_resolution_y / (float)default_configure->target_screen_width;
                }
            }
        }
    }

    m_nine_patch_info_map.clear();
}

void CSCLUtils::fini()
{
    SCL_DEBUG();

    CSCLUtilsImpl *impl = GetCSCLUtilsImpl();
    if (impl) {
        impl->fini();
    }
}

/**
 * Returns a scale rate (see default screen resolution in sclconfig.h file)
 */
sclfloat
CSCLUtils::get_smallest_scale_rate()
{
    /* Try to return smaller scale rate, to avoid UI crash */
    return (m_scn_rate_x < m_scn_rate_y ) ? m_scn_rate_x : m_scn_rate_y ;
}

/**
* Returns a scale rate X (see default screen resolution in sclconfig.h file)
*/
sclfloat
CSCLUtils::get_scale_rate_x()
{
    /* Try to return smaller scale rate, to avoid UI crash */
    return m_scn_rate_x;
}

/**
* Returns a scale rate Y (see default screen resolution in sclconfig.h file)
*/
sclfloat
CSCLUtils::get_scale_rate_y()
{
    /* Try to return smaller scale rate, to avoid UI crash */
    return m_scn_rate_y;
}

/**
 * Recalculates x value according to the current screen resolution
 */
void
CSCLUtils::scale_x(scl16 *x)
{
    if (x) (*x) *= m_scn_rate_x;
}

/**
* Recalculates y value according to the current screen resolution
 */
void
CSCLUtils::scale_y(scl16 *y)
{
    if (y) (*y) *= m_scn_rate_y;
}

/**
 * Returns a calculated x value according to the current screen resolution
 */
scl16
CSCLUtils::get_scaled_x(scl16 x)
{
    return static_cast<scl16>(x * m_scn_rate_x);
}

/**
 * Returns a calculated y value according to the current screen resolution
 */
scl16
CSCLUtils::get_scaled_y(scl16 y)
{
    return static_cast<scl16>(y * m_scn_rate_y);
}

/**
 * Returns the smallest custom scale rate
 */
sclfloat
CSCLUtils::get_smallest_custom_scale_rate()
{
    /* Try to return smaller scale rate, to avoid UI crash */
    return (m_custom_scale_rate_x < m_custom_scale_rate_y) ? m_custom_scale_rate_x : m_custom_scale_rate_y;
}

/**
* Returns the current custom scale rate X
*/
sclfloat
CSCLUtils::get_custom_scale_rate_x()
{
    return m_custom_scale_rate_x;
}

/**
* Returns the current custom scale rate Y
*/
sclfloat
CSCLUtils::get_custom_scale_rate_y()
{
    return m_custom_scale_rate_y;
}

void
CSCLUtils::set_custom_scale_rate_x(sclfloat x)
{
    if (x > 0.0f) {
        m_custom_scale_rate_x = x;
    }
}

void
CSCLUtils::set_custom_scale_rate_y(sclfloat y)
{
    if (y > 0.0f) {
        m_custom_scale_rate_y = y;
    }
}

/**
 * portable sleep function
 */
sclboolean
CSCLUtils::sleep(sclulong msc)
{
    SCL_DEBUG();
    fd_set dummy;
    struct timeval interval;
    FD_ZERO(&dummy);
    interval.tv_sec = msc / 1000;
    interval.tv_usec = (msc % 1000) * 1000;
    select(0, &dummy, NULL, NULL, &interval);
    return TRUE;
}

/* FIXME : let's just look for appropriate directory once and cache it 
           and one more thing, let's find a closest resolution when caching the appropriate directory
*/
/**
 * returns the composed path
 */
sclchar*
CSCLUtils::get_composed_path(sclchar* buf, const sclchar* prefix, const sclchar* path)
{
    SCL_DEBUG();

    SclResParserManager *sclres_manager = SclResParserManager::get_instance();

    if (buf && path && sclres_manager) {
        const sclchar *base_path = "";
        PSclDefaultConfigure default_configure = sclres_manager->get_default_configure();
        if (default_configure) {
            if (default_configure->image_file_base_path) {
                base_path = default_configure->image_file_base_path;
            }
        }
        snprintf(buf, _POSIX_PATH_MAX, "%s/%s/%s/%s",
            sclres_manager->get_resource_directory(), base_path, IMG_PATH_PREFIX, path);
    }

    return buf;
}

/**
 * returns the decomposed path
 */
sclchar*
CSCLUtils::get_decomposed_path(sclchar* buf, const sclchar* prefix, const sclchar* path)
{
    SCL_DEBUG();

    SclResParserManager *sclres_manager = SclResParserManager::get_instance();

    if (buf && path && sclres_manager) {
        sclchar comparand[_POSIX_PATH_MAX];

        snprintf(comparand, _POSIX_PATH_MAX, "%s/%s/",
            sclres_manager->get_resource_directory(), IMG_PATH_PREFIX);
        if (strncmp(path, comparand, strlen(comparand)) == 0) {
            snprintf(buf, _POSIX_PATH_MAX, "%s", path + strlen(comparand));
        } else {
            snprintf(buf, _POSIX_PATH_MAX, "%s", path);
        }
    }

    return buf;
}

sclboolean
CSCLUtils::get_autopopup_window_variables(sclchar * const autopopup_keys[MAX_SIZE_OF_AUTOPOPUP_STRING], sclbyte *num_keys, sclbyte *num_columns, sclbyte *num_rows, sclint *width, sclint *height)
{
    SCL_DEBUG();
    scl_assert_return_false(autopopup_keys);
    scl_assert_return_false(num_keys && num_columns && num_rows && width && height);

    int loop;
    sclboolean ret = FALSE;
    sclint scrx, scry;

    SclResParserManager *sclres_manager = SclResParserManager::get_instance();
    PSclAutoPopupConfigure autopopup_configure = NULL;
    if (sclres_manager) {
        autopopup_configure = sclres_manager->get_autopopup_configure();
    }
    if (autopopup_configure && autopopup_keys && num_keys && num_columns && num_rows && width && height) {
        *num_keys = 0;
        for (loop = 0;loop < MAX_SIZE_OF_AUTOPOPUP_STRING;loop++) {
            if (autopopup_keys[loop]) {
                if (strlen(autopopup_keys[loop]) > 0) {
                    (*num_keys)++;
                }
            }
        }
        if (*num_keys > 0) {
            ret = TRUE;
        }

        this->get_screen_resolution(&scrx, &scry);

        CSCLContext *context = CSCLContext::get_instance();
        if (context->get_display_mode() == DISPLAYMODE_LANDSCAPE) {
            int  swap = scrx;
            scrx = scry;
            scry = swap;
        }

        *num_columns = (scrx - (2 * autopopup_configure->bg_padding * get_smallest_custom_scale_rate())) /
                      (autopopup_configure->button_width * get_smallest_custom_scale_rate() +
                      autopopup_configure->button_spacing * get_smallest_custom_scale_rate());
        if (*num_columns > autopopup_configure->max_column && autopopup_configure->max_column > 0)
            *num_columns = autopopup_configure->max_column;
        *num_rows = ((*num_keys - 1) / *num_columns) + 1;
        if (*num_columns > *num_keys) *num_columns = *num_keys;

        /* Try to spread evenly on each lines */
        *num_columns = (*num_keys) / (*num_rows);
        if ((*num_keys) % *num_rows != 0) (*num_columns)++;

        *width =
            (*num_columns * autopopup_configure->button_width * get_custom_scale_rate_x()) +
            (2 * autopopup_configure->bg_padding * get_smallest_custom_scale_rate()) +
            ((*num_columns - 1) * autopopup_configure->button_spacing * get_smallest_custom_scale_rate());
        *height =
            (*num_rows * autopopup_configure->button_height * get_custom_scale_rate_y()) +
            (2 * autopopup_configure->bg_padding * get_smallest_custom_scale_rate()) +
            ((*num_rows - 1) * autopopup_configure->button_spacing * get_smallest_custom_scale_rate());

        *width += (2 * autopopup_configure->decoration_size * get_smallest_custom_scale_rate());
        *height += (2 * autopopup_configure->decoration_size * get_smallest_custom_scale_rate());
    }

    return ret;
}

/**
 * Returns a duplicated string pointer by given str
 */
sclchar*
CSCLUtils::get_str_dup(const sclchar* str)
{
    SCL_DEBUG();
    sclchar *new_str;
    sclint length;

    if (str) {
        length = strlen(str) + 1;
        new_str = (sclchar*)malloc(length);
        if (new_str) {
            memcpy(new_str, str, length);
            new_str[length - 1] = '\0';
        }
    } else {
        new_str = NULL;
    }

    return new_str;
}

/**
 * Returns an unique ID
 */
scl16
CSCLUtils::get_unique_id()
{
    SCL_DEBUG();
    static scl16 uniId = 0;
    if (uniId < SCL_MAX_UNIQUE_ID) {
        uniId++;
    } else {
        uniId = 0;
    }
    return uniId;
}

sclboolean
CSCLUtils::is_rect_overlap(SclRectangle rect1, SclRectangle rect2)
{
    if (rect1.x < rect2.x + rect2.width && rect1.x + rect1.width > rect2.x &&
            rect1.y < rect2.y + rect2.height && rect1.y + rect1.height > rect2.y) {
        /*if (rect1.x == rect2.x && rect1.width == rect2.width &&
                rect1.y == rect2.y && rect1.height == rect2.height) {*/
        return true;
    }
    return false;
}

sclfloat
CSCLUtils::get_distance( sclint x1, sclint y1, sclint x2, sclint y2 )
{
    sclint dx = x1 - x2;
    sclint dy = y1 - y2;
    return sqrt((dx * dx) + (dy * dy));
}

sclfloat
CSCLUtils::get_distance( SclPoint pt1, SclPoint pt2 )
{
    return get_distance(pt1.x, pt1.y, pt2.x, pt2.y);
}

sclfloat
CSCLUtils::get_approximate_distance( sclint x1, sclint y1, sclint x2, sclint y2 )
{
    sclint dx = abs(x1 - x2);
    sclint dy = abs(y1 - y2);
    return dx + dy;
}

sclfloat
CSCLUtils::get_approximate_distance( SclPoint pt1, SclPoint pt2 )
{
    return get_approximate_distance(pt1.x, pt1.y, pt2.x, pt2.y);
}

const SclNinePatchInfo*
CSCLUtils::get_nine_patch_info(const sclchar *image_path)
{
    SCL_DEBUG();

    /* FIXME - Let's fix this function not to use static */
    static SclNinePatchInfo ret;
    if (!image_path) return NULL;

    sclchar buf[_POSIX_PATH_MAX] = {0};
    sclboolean finished = FALSE;
    //get_decomposed_path(buf, IMG_PATH_PREFIX, image_path);
    for (sclint loop = strlen(image_path);!finished && loop > 0;loop--) {
        if (image_path[loop] == '\\' || image_path[loop] == '/') {
            strncpy(buf, image_path + loop + 1, _POSIX_PATH_MAX - 1);
            finished = TRUE;
        }
    }
    
    SclResParserManager *sclres_manager = SclResParserManager::get_instance();
    if (sclres_manager) {
        if (sclres_manager->get_nine_patch_info(buf, &ret)) {
            return &ret;
        }
    }

    return NULL;
}

sclint CSCLUtils::log(const sclchar *fmt, ...)
{
    SCL_DEBUG();
    va_list argptr;
    sclint ret = 0;

    va_start(argptr, fmt);
    CSCLUtilsImpl *impl = GetCSCLUtilsImpl();
    if (impl) {
        char buf[_POSIX_PATH_MAX] = {0};
        vsnprintf(buf, _POSIX_PATH_MAX - 1, fmt, argptr);
        ret = impl->log(buf);
    }
    va_end(argptr);

    return ret;
}
