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

#ifndef __SCL_FONT_PROXY_H__
#define __SCL_FONT_PROXY_H__

#include "sclgraphics.h"

namespace scl
{
class CSCLFontProxy
{
private:
    CSCLFontProxy();
public :
    virtual ~CSCLFontProxy();

    static CSCLFontProxy* get_instance();

    sclfont get_font(const SclFontInfo& info);
    void free_fonts();

private:
    typedef struct _SclFontCacheItem {
        sclchar font_name[MAX_FONT_NAME_LEN + 1];
        sclshort font_size;
        sclboolean is_italic;
        sclboolean is_bold;
        sclfont font;
    } SclFontCacheItem;

    static const sclint FONT_PROXY_SIZE = 128;
    SclFontCacheItem m_font_cache_items[FONT_PROXY_SIZE];
};
} /* End of scl namespace */
#endif
