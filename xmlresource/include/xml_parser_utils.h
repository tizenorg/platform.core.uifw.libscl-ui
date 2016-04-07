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

#ifndef __XML_PARSER_UTILS_H__
#define __XML_PARSER_UTILS_H__
#include <assert.h>
#include <limits.h>
#include <libxml/parser.h>
#include "scltypes.h"

#ifndef LIBSCL_EXPORT_API
#define LIBSCL_EXPORT_API
#endif // LIBSCL_EXPORT_API


using namespace scl;

LIBSCL_EXPORT_API int get_content_int(const xmlNodePtr cur_node);
LIBSCL_EXPORT_API bool get_content_bool(const xmlNodePtr cur_node);


LIBSCL_EXPORT_API bool equal_prop(const xmlNodePtr cur_node, const char* prop, const char* str);

template <class T>
LIBSCL_EXPORT_API bool get_prop_number(const xmlNodePtr cur_node, const char* prop, T *ret) {
    assert(cur_node != NULL);

    bool succeeded = false;

    xmlChar* key = xmlGetProp(cur_node, (const xmlChar*)prop);

    if (ret && key != NULL) {
        succeeded = true;
        *ret = (T)atoi((const char*)key);
        xmlFree(key);
    }

    return succeeded;
}
LIBSCL_EXPORT_API bool get_prop_bool(const xmlNodePtr cur_node, const char* prop, sclboolean *ret);
LIBSCL_EXPORT_API int dex_string_to_int(const char* str);
LIBSCL_EXPORT_API int get_content_dex_string_int(const xmlNodePtr cur_node);
#endif
