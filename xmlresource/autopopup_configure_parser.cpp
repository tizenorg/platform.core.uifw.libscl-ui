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

#include <assert.h>
#include <string.h>
#include <libxml/parser.h>

#include "autopopup_configure_parser.h"
#include "xml_parser_utils.h"
#include "simple_debug.h"
using namespace std;

class AutoPopupConfigureParserImpl {
    public:
        AutoPopupConfigureParserImpl() {
            memset((void*)&m_autopopup_configure, 0x00, sizeof(SclAutoPopupConfigure));

            m_autopopup_configure.add_grab_left = NOT_USED;
            m_autopopup_configure.add_grab_right = NOT_USED;
            m_autopopup_configure.add_grab_top = NOT_USED;
            m_autopopup_configure.add_grab_bottom= NOT_USED;
        }

        ~AutoPopupConfigureParserImpl() {
            /* Let's create de-initializing function for this resource releasement */
            sclint loop;
            for(loop = 0;loop < SCL_BUTTON_STATE_MAX;loop++) {
                if (m_autopopup_configure.button_image_path[loop]) {
                    xmlFree(m_autopopup_configure.button_image_path[loop]);
                    m_autopopup_configure.button_image_path[loop] = NULL;
                }
            }
            for(loop = 0;loop < MAX_WND_DECORATOR;loop++) {
                if (m_autopopup_configure.decoration_image_path[loop]) {
                    xmlFree(m_autopopup_configure.decoration_image_path[loop]);
                    m_autopopup_configure.decoration_image_path[loop] = NULL;
                }
            }
            if (m_autopopup_configure.label_type) {
                xmlFree(m_autopopup_configure.label_type);
                m_autopopup_configure.label_type = NULL;
            }

        }

        int parsing_autopopup_configure(const char* input_file) {
            xmlDocPtr doc;
            xmlNodePtr cur_node;

            doc = xmlReadFile(input_file, NULL, 0);
            if (doc == NULL) {
                SCLLOG(SclLog::WARNING, "Could not load file: %s.", input_file);
                return -1;
            }

            cur_node = xmlDocGetRootElement(doc);
            if (cur_node == NULL) {
                SCLLOG(SclLog::WARNING, "AutoPopupConfigParser: empty document.\n");
                xmlFreeDoc(doc);
                return -1;
            }
            if (0 != xmlStrcmp(cur_node->name, (const xmlChar*)"autopopup_configure"))
            {
                SCLLOG(SclLog::WARNING, "AutoPopup_Configure: root name error: %s\n!", (char *)cur_node->name);
                xmlFreeDoc(doc);
                return -1;
            }

            cur_node = cur_node->xmlChildrenNode;

            while (cur_node != NULL) {
                if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"background_image_path")) {
                    xmlChar* temp = xmlNodeGetContent(cur_node);
                    m_autopopup_configure.bg_image_path = (sclchar *)temp;
                }
                else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"background_color")) {
                    parsing_background_color(cur_node);
                }
                else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"background_line_width")) {
                    m_autopopup_configure.bg_line_width = get_content_int(cur_node);
                }
                else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"background_line_color")) {
                    parsing_background_line_color(cur_node);
                }
                else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"background_padding")) {
                    m_autopopup_configure.bg_padding = get_content_int(cur_node);
                }
                else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"button_image_path")) {
                    parsing_button_image_path(cur_node);
                }
                else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"sw_button_style")) {
                    m_autopopup_configure.sw_button_style = get_content_int(cur_node);
                }
                else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"button_size")) {
                    parsing_button_size(cur_node);
                }
                else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"button_spacing")) {
                    m_autopopup_configure.button_spacing = get_content_int(cur_node);
                }
                else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"label_type")) {
                    xmlChar* temp = xmlNodeGetContent(cur_node);
                    m_autopopup_configure.label_type = (sclchar *)temp;
                }
                else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"window_decorator")) {
                    get_prop_number(cur_node, "size", &(m_autopopup_configure.decoration_size));
                    parsing_window_decorator_image_path(cur_node);
                }
                else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"max_column")) {
                    m_autopopup_configure.max_column = get_content_int(cur_node);
                }
                else if (0 == xmlStrcmp(cur_node->name, (const xmlChar *)"grab_area")) {
                    parsing_grab_area(cur_node);
                }

                cur_node = cur_node->next;
            }
            xmlFreeDoc(doc);
            return 0;

        }
        void parsing_background_color(const xmlNodePtr cur_node) {
            assert(cur_node != NULL);

            xmlNodePtr child_node = cur_node->xmlChildrenNode;
            while (child_node!=NULL) {
                if (0 == xmlStrcmp(child_node->name, (const xmlChar *)"r") ) {
                    m_autopopup_configure.bg_color.r = get_content_int(child_node);
                }
                else if (0 == xmlStrcmp(child_node->name, (const xmlChar *)"g") ) {
                    m_autopopup_configure.bg_color.g = get_content_int(child_node);
                }
                else if (0 == xmlStrcmp(child_node->name, (const xmlChar *)"b") ) {
                    m_autopopup_configure.bg_color.b = get_content_int(child_node);
                }
                else if (0 == xmlStrcmp(child_node->name, (const xmlChar *)"a") ) {
                    m_autopopup_configure.bg_color.a = get_content_int(child_node);
                }

                child_node = child_node->next;
            }

        }

        void parsing_background_line_color(const xmlNodePtr cur_node) {
            assert(cur_node != NULL);

            xmlNodePtr child_node = cur_node->xmlChildrenNode;
            while (child_node!=NULL) {
                if (0 == xmlStrcmp(child_node->name, (const xmlChar *)"r") ) {
                    m_autopopup_configure.bg_line_color.r = get_content_int(child_node);
                }
                else if (0 == xmlStrcmp(child_node->name, (const xmlChar *)"g") ) {
                    m_autopopup_configure.bg_line_color.g = get_content_int(child_node);
                }
                else if (0 == xmlStrcmp(child_node->name, (const xmlChar *)"b") ) {
                    m_autopopup_configure.bg_line_color.b = get_content_int(child_node);
                }
                else if (0 == xmlStrcmp(child_node->name, (const xmlChar *)"a") ) {
                    m_autopopup_configure.bg_line_color.a = get_content_int(child_node);
                }

                child_node = child_node->next;
            }
        }

        void parsing_button_image_path(const xmlNodePtr cur_node) {
            assert(cur_node != NULL);
            assert(0 == xmlStrcmp(cur_node->name, (const xmlChar*)"button_image_path"));
            xmlNodePtr child_node = cur_node->xmlChildrenNode;

            while (child_node != NULL) {
                if (0 == xmlStrcmp(child_node->name, (const xmlChar*)"image") ) {
                    int button_state = get_button_state_prop(child_node);
                    if (button_state >= 0 && button_state < SCL_BUTTON_STATE_MAX) {
                        m_autopopup_configure.button_image_path[button_state] = (sclchar*)xmlNodeGetContent(child_node);
                    }
                }
                child_node = child_node->next;
            }

        }
        void parsing_button_size(const xmlNodePtr cur_node) {
            assert(cur_node != NULL);

            xmlNodePtr child_node = cur_node->xmlChildrenNode;
            while (child_node!=NULL) {
                if (0 == xmlStrcmp(child_node->name, (const xmlChar*)"width") ) {
                    m_autopopup_configure.button_width = get_content_int(child_node);
                } else if (0 == xmlStrcmp(child_node->name, (const xmlChar*)"height") ) {
                    m_autopopup_configure.button_height = get_content_int(child_node);
                }
                child_node = child_node->next;
            }

        }

        void parsing_window_decorator_image_path(const xmlNodePtr cur_node) {
            assert(cur_node != NULL);
            assert(0 == xmlStrcmp(cur_node->name, (const xmlChar*)"window_decorator"));
            xmlNodePtr child_node = cur_node->xmlChildrenNode;

            while (child_node != NULL) {
                if (0 == xmlStrcmp(child_node->name, (const xmlChar*)"image") ) {
                    if (equal_prop(cur_node, "direction", "top_left")) {
                        m_autopopup_configure.decoration_image_path[WND_DECORATOR_TOP_LEFT] = (sclchar *)xmlNodeGetContent(cur_node);;
                    }
                    else if (equal_prop(cur_node, "direction", "top_center")) {
                        m_autopopup_configure.decoration_image_path[WND_DECORATOR_TOP_CENTER] = (sclchar *)xmlNodeGetContent(cur_node);;
                    }
                    else if (equal_prop(cur_node, "direction", "top_right")) {
                        m_autopopup_configure.decoration_image_path[WND_DECORATOR_TOP_RIGHT] = (sclchar *)xmlNodeGetContent(cur_node);;
                    }
                    else if (equal_prop(cur_node, "direction", "middle_left")) {
                        m_autopopup_configure.decoration_image_path[WND_DECORATOR_MIDDLE_LEFT] = (sclchar *)xmlNodeGetContent(cur_node);;
                    }
                    else if (equal_prop(cur_node, "direction", "middle_right")) {
                        m_autopopup_configure.decoration_image_path[WND_DECORATOR_MIDDLE_RIGHT] = (sclchar *)xmlNodeGetContent(cur_node);;
                    }
                    else if (equal_prop(cur_node, "direction", "bottom_left")) {
                        m_autopopup_configure.decoration_image_path[WND_DECORATOR_BOTTOM_LEFT] = (sclchar *)xmlNodeGetContent(cur_node);;
                    }
                    else if (equal_prop(cur_node, "direction", "bottom_center")) {
                        m_autopopup_configure.decoration_image_path[WND_DECORATOR_BOTTOM_CENTER] = (sclchar *)xmlNodeGetContent(cur_node);;
                    }
                    else if (equal_prop(cur_node, "direction", "bottom_right")) {
                        m_autopopup_configure.decoration_image_path[WND_DECORATOR_BOTTOM_RIGHT] = (sclchar *)xmlNodeGetContent(cur_node);;
                    }
                }
                child_node = child_node->next;
            }

        }

        void parsing_grab_area(const xmlNodePtr cur_node) {
            assert(cur_node != NULL);

            xmlNodePtr child_node = cur_node->xmlChildrenNode;
            while (child_node!=NULL) {
                if (0 == xmlStrcmp(child_node->name, (const xmlChar*)"left") ) {
                    m_autopopup_configure.add_grab_left = get_content_int(child_node);
                }
                else if (0 == xmlStrcmp(child_node->name, (const xmlChar*)"right") ) {
                    m_autopopup_configure.add_grab_right = get_content_int(child_node);
                }
                else if (0 == xmlStrcmp(child_node->name, (const xmlChar*)"top") ) {
                    m_autopopup_configure.add_grab_top = get_content_int(child_node);
                }
                else if (0 == xmlStrcmp(child_node->name, (const xmlChar*)"bottom") ) {
                    m_autopopup_configure.add_grab_bottom = get_content_int(child_node);
                }
                child_node = child_node->next;
            }

        }

        int get_button_state_prop(const xmlNodePtr cur_node) {
            assert(cur_node != NULL);
            int button_state = -1;

            if (equal_prop(cur_node, "button_state", "pressed")) {
                button_state = BUTTON_STATE_PRESSED;
            } else if (equal_prop(cur_node, "button_state", "normal")) {
                button_state = BUTTON_STATE_NORMAL;
            }
            else if (equal_prop(cur_node, "button_state", "disabled")) {
                button_state = BUTTON_STATE_DISABLED;
            }
            return button_state;

        }
        SCLDisplayMode get_content_displaymode(const xmlNodePtr cur_node) {
            assert(cur_node != NULL);

            SCLDisplayMode display_mode = DISPLAYMODE_PORTRAIT;

            xmlChar* key = xmlNodeGetContent(cur_node);
            if (key!= NULL) {
                if (0 == strcmp("landscape", (const char*)key)) {
                    display_mode = DISPLAYMODE_LANDSCAPE;
                }
                xmlFree(key);
            }

            return display_mode;

        }
        SclAutoPopupConfigure m_autopopup_configure;
};

AutoPopupConfigParser::AutoPopupConfigParser() {
    m_impl = new AutoPopupConfigureParserImpl;
}

AutoPopupConfigParser::~AutoPopupConfigParser() {
    if (m_impl) {
        SCLLOG(SclLog::MESSAGE, "~AutoPopupConfigParser() has called");
        delete m_impl;
        m_impl = NULL;
    }
}

AutoPopupConfigParser*
AutoPopupConfigParser::get_instance() {
    static AutoPopupConfigParser instance;
    return &instance;
}

int
AutoPopupConfigParser::init(const char* file) {
    return m_impl->parsing_autopopup_configure(file);
}

PSclAutoPopupConfigure
AutoPopupConfigParser::get_autopopup_configure() {
    return &m_impl->m_autopopup_configure;
}
