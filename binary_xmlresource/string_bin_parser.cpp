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

#include <string>
#include <string.h>
#include <stdio.h>
#include "string_bin_parser.h"
using namespace std;

String_Bin_Parser::String_Bin_Parser(const FileStorage& storage, int offset, int size) {
    m_storage.get_storage(storage, offset, size);
    parse();
}

/*delete all strings*/
String_Bin_Parser::~String_Bin_Parser() {
    m_vec_str_depository.clear();
}
void
String_Bin_Parser::parse() {
    /* from the string encoder rule*/
    static const int MAX_NUM_WIDTH = 4;

    /* get the strings' num*/
    int size = m_storage.get<sint_t>(MAX_NUM_WIDTH);

    /* get each string */
    for (int i = 0; i < size; ++i) {
        const char* temp = m_storage.get_str();
        m_vec_str_depository.push_back(temp);
    }

}

