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

#ifndef __Magnifier_Configure_Bin_Parser__H__
#define __Magnifier_Configure_Bin_Parser__H__
#include "sclres_type.h"
#include "string_collector.h"
#include "iparserinfo_provider.h"
#include "file_storage_impl.h"
#include "_auto_metadata.h"

class Magnifier_Configure_Bin_Parser {
public:
    ~Magnifier_Configure_Bin_Parser();
    static Magnifier_Configure_Bin_Parser *get_instance();
    void init(const FileStorage& storage, int, int, IParserInfo_Provider*);

    PSclMagnifierWndConfigure get_magnifier_configure();
private:
    Magnifier_Configure_Bin_Parser();
    void parsing_magnifier_configure();

private:
    static Magnifier_Configure_Bin_Parser *m_instance;
    SclMagnifierWndConfigure m_magnifier_configure;
    StringCollector m_string_collector;
    FileStorage m_storage;
    IParserInfo_Provider* parser_info_provider;

private:
    class DestructHelper {
    public:
        ~DestructHelper() {
            if (Magnifier_Configure_Bin_Parser::m_instance != NULL)
                delete m_instance;
        }
    };
    static DestructHelper des;
};


#endif
