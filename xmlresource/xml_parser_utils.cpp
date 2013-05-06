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

#include "xml_parser_utils.h"
#include <string.h>
#include <assert.h>

int
get_content_int(const xmlNodePtr cur_node) {
    assert(cur_node != NULL);

    int num = -1;

    xmlChar* key = xmlNodeGetContent(cur_node);
    if (key!= NULL) {
        if (0 == strcmp("NOT_USED", (const char*)key)) {
            num = -1;
        } else {
            num = atoi( (const char*)key );
        }
        xmlFree(key);
    }
    return num;
}

bool
get_content_bool(const xmlNodePtr cur_node) {
    assert(cur_node != NULL);

    bool bVal = false;

    xmlChar* key = xmlNodeGetContent(cur_node);

    if (key != NULL) {
        if (0 == xmlStrcmp(key, (const xmlChar*)"true")) {
            bVal = true;
        }
        xmlFree(key);
    }
    return bVal;
}

bool
equal_prop(const xmlNodePtr cur_node, const char* prop, const char* str) {
    bool val = false;

    xmlChar* key = xmlGetProp(cur_node, (const xmlChar*)prop);

    if (NULL != key) {
        if (0 == xmlStrcmp(key, (const xmlChar*)str)) {
            val = true;
        }
        xmlFree(key);
    }

    return val;
}

bool
get_prop_bool(const xmlNodePtr cur_node, const char* prop, sclboolean *ret) {
    assert(cur_node != NULL);

    bool succeeded = false;

    xmlChar* key = xmlGetProp(cur_node, (const xmlChar*)prop);

    if (ret && key != NULL) {
        succeeded = true;
        if (0 == xmlStrcmp(key, (const xmlChar*)"true")) {
            *ret = true;
        } else {
            *ret = false;
        }
        xmlFree(key);
    }

    return succeeded;
}

int
dex_string_to_int(const char* str) {
    assert(str != NULL);

    int val = -1;
    sscanf(str, "%x",&val);

    return val;
}

int
get_content_dex_string_int(const xmlNodePtr cur_node) {
    assert(NULL != cur_node);

    int val = -1;

    xmlChar* key = xmlNodeGetContent(cur_node);

    if (key != NULL) {
        val = dex_string_to_int((const char*)key);
        xmlFree(key);
    }

    return val;
}

