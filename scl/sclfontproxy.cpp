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

#include "sclgraphics.h"
#include "sclfontproxy.h"
#include "scldebug.h"

using namespace scl;

/**
* Constructor
*/
CSCLFontProxy::CSCLFontProxy()
{
    SCL_DEBUG();

    for (int loop = 0;loop < FONT_PROXY_SIZE;loop++) {
        m_font_cache_items[loop].font = NULL;
        memset(m_font_cache_items[loop].font_name, 0x00, sizeof(m_font_cache_items[loop].font_name));
        m_font_cache_items[loop].font_size = 0;
        m_font_cache_items[loop].is_italic = FALSE;
        m_font_cache_items[loop].is_bold = FALSE;
    }
}

/**
* De-constructor
*/
CSCLFontProxy::~CSCLFontProxy()
{
    SCL_DEBUG();
    free_fonts();
}

CSCLFontProxy* CSCLFontProxy::get_instance()
{
    static CSCLFontProxy instance;
    return &instance;
}

sclfont
CSCLFontProxy::get_font(const SclFontInfo& info)
{
    SCL_DEBUG();

    sclfont ret = NULL;

    for (int loop = 0;loop < FONT_PROXY_SIZE && ret == NULL;loop++) {
        if (
            strcmp(info.font_name, m_font_cache_items[loop].font_name) == 0 &&
            info.font_size == m_font_cache_items[loop].font_size &&
            info.is_italic == m_font_cache_items[loop].is_italic &&
            info.is_bold == m_font_cache_items[loop].is_bold
        ) {
            ret = m_font_cache_items[loop].font;
            break;
        }
    }

    if (ret == NULL) {
        sclboolean isEmpty = FALSE;
        CSCLGraphics *graphics = CSCLGraphics::get_instance();
        ret = graphics->create_font(info);

        if (ret) {
            for (int loop = 0;loop < FONT_PROXY_SIZE && !isEmpty;loop++) {
                if (m_font_cache_items[loop].font == NULL) {
                    isEmpty = TRUE;
                    strncpy(m_font_cache_items[loop].font_name, info.font_name, MAX_FONT_NAME_LEN);
                    m_font_cache_items[loop].font_name[MAX_FONT_NAME_LEN] = '\0';
                    m_font_cache_items[loop].font_size = info.font_size;
                    m_font_cache_items[loop].is_italic = info.is_italic;
                    m_font_cache_items[loop].is_bold = info.is_bold;
                    m_font_cache_items[loop].font = ret;
                }
            }
        }
    }

    return ret;
}

void
CSCLFontProxy::free_fonts()
{
    SCL_DEBUG();

    for (int loop = 0;loop < FONT_PROXY_SIZE;loop++) {
        if (m_font_cache_items[loop].font) {
            CSCLGraphics *graphics = CSCLGraphics::get_instance();
            graphics->destroy_font(m_font_cache_items[loop].font);
            m_font_cache_items[loop].font = NULL;
        }
    }
}
