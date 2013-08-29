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

#include "sclgraphics.h"
#include "sclimageproxy.h"
#include "scldebug.h"

using namespace scl;

/**
* Constructor
*/
CSCLImageProxy::CSCLImageProxy()
{
    SCL_DEBUG();

    for (int loop = 0;loop < IMAGE_PROXY_SIZE;loop++) {
        m_image_cache_items[loop].image_data = NULL;
        memset(m_image_cache_items[loop].image_path, 0x00, sizeof(m_image_cache_items[loop].image_path));
    }
}

/**
* De-constructor
*/
CSCLImageProxy::~CSCLImageProxy()
{
    SCL_DEBUG();
    free_images();
}

CSCLImageProxy* CSCLImageProxy::get_instance()
{
    static CSCLImageProxy instance;
    return &instance;
}

sclimage
CSCLImageProxy::get_image(const sclchar* image_path)
{
    SCL_DEBUG();

    sclimage ret = NULL;

    /* FIXME : Better to use std::map for searching image cache item */
    for (int loop = 0;loop < IMAGE_PROXY_SIZE && ret == NULL;loop++) {
        if (strcmp(image_path, m_image_cache_items[loop].image_path) == 0) {
            ret = m_image_cache_items[loop].image_data;
            break;
        }
    }

    if (ret == NULL) {
        sclboolean isEmpty = FALSE;
        CSCLGraphics *graphics = CSCLGraphics::get_instance();
        ret = graphics->load_image(image_path);

        if (ret) {
            for (int loop = 0;loop < IMAGE_PROXY_SIZE && !isEmpty;loop++) {
                if (m_image_cache_items[loop].image_data == NULL) {
                    isEmpty = TRUE;
                    strncpy(m_image_cache_items[loop].image_path, image_path, MAX_IMAGE_PATH_LEN);
                    m_image_cache_items[loop].image_path[MAX_IMAGE_PATH_LEN] = '\0';
                    m_image_cache_items[loop].image_data = ret;
                }
            }
        }
    }

    return ret;
}

void
CSCLImageProxy::free_images()
{
    SCL_DEBUG();

    for (int loop = 0;loop < IMAGE_PROXY_SIZE;loop++) {
        if (m_image_cache_items[loop].image_data) {
            CSCLGraphics *graphics = CSCLGraphics::get_instance();
            graphics->unload_image(m_image_cache_items[loop].image_data);
            m_image_cache_items[loop].image_data = NULL;
        }
    }
}
