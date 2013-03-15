/*
 * Copyright 2012-2013 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Flora License, Version 1.0 (the "License");
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

#ifndef __AutoPopup_Configure_Parser__H__
#define __AutoPopup_Configure_Parser__H__
#include "sclres_type.h"

typedef SclAutoPopupConfigure *PSclAutoPopupConfigure;

class AutoPopupConfigureParserImpl;

class AutoPopup_Configure_Parser {
    AutoPopupConfigureParserImpl *m_impl;
    public:
        int init(const char* file);
        PSclAutoPopupConfigure get_autopopup_configure();
    public:
        ~AutoPopup_Configure_Parser();
        static AutoPopup_Configure_Parser *get_instance();
    private:
        AutoPopup_Configure_Parser();
        static AutoPopup_Configure_Parser *m_instance;
};


#endif
