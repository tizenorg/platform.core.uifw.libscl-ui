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

#ifndef __KEY_COORDINATE_FRAME_BIN_PARSER__H__
#define __KEY_COORDINATE_FRAME_BIN_PARSER__H__
#include "sclres_type.h"
#include "iparserinfo_provider.h"
#include <vector>
#include "string_collector.h"
#include "file_storage_impl.h"
#include "_auto_metadata.h"
class BinKeyCoordFrameParser {
    public:
        static BinKeyCoordFrameParser *get_instance();
    void init(const FileStorage& storage, int, int, IParserInfo_Provider*);
    ~BinKeyCoordFrameParser();

    PSclLayoutKeyCoordinatePointerTable get_key_coordinate_pointer_frame();
    void load(int);
    bool loaded(int);
    void unload();
    private:
    BinKeyCoordFrameParser();
    void parsing_key_coordinate_frame();
    void decode_key_coordinate_record(FileStorage& storage, const PSclLayoutKeyCoordinate cur, const Key_coordinate_record_width&);

    private:
        SclLayoutKeyCoordinate* m_key_coordinate_pointer_frame[MAX_SCL_LAYOUT][MAX_KEY];

        IParserInfo_Provider *parser_info_provider;
        StringCollector m_string_collector;
        FileStorage m_storage;
};


#endif
