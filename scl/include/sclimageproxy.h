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

#ifndef __SCL_IMAGE_PROXY_H__
#define __SCL_IMAGE_PROXY_H__

namespace scl
{
class CSCLImageProxy
{
private:
	CSCLImageProxy();
public :
    ~CSCLImageProxy();
    static CSCLImageProxy* get_instance();

    sclimage get_image(const sclchar* image_path);
    void free_images();

private:
    static const sclint MAX_IMAGE_PATH_LEN = 255;
    typedef struct _SclImageCacheItem {
        sclchar image_path[MAX_IMAGE_PATH_LEN + 1];
        sclimage image_data;
    } SclImageCacheItem;

    static const sclint IMAGE_PROXY_SIZE = 1;
    SclImageCacheItem m_image_cache_items[IMAGE_PROXY_SIZE];
};
} /* End of scl namespace */
#endif
