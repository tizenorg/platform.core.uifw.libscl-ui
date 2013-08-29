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

#ifndef __BinAutoPopupConfigParser__H__
#define __BinAutoPopupConfigParser__H__
#include "sclres_type.h"
#include "string_collector.h"
#include "iparserinfo_provider.h"
#include "file_storage_impl.h"
#include "_auto_metadata.h"

class BinAutoPopupConfigParser {
public:
    ~BinAutoPopupConfigParser();
    static BinAutoPopupConfigParser *get_instance();
    void init(const FileStorage& storage, int, int, IParserInfo_Provider*);

    PSclAutoPopupConfigure get_autopopup_configure();

private:
    BinAutoPopupConfigParser();
    void parsing_autopopup_configure();
    void decode_color(SclColor&, int width);
private:
    SclAutoPopupConfigure m_autopopup_configure;
    StringCollector m_string_collector;
    FileStorage m_storage;
    IParserInfo_Provider* parser_info_provider;
};


#endif
