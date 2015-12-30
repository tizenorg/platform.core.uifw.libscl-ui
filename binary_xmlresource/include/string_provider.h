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

#ifndef __STRING_PROVIDER_H__
#define __STRING_PROVIDER_H__
#include <string>
#include <vector>
#include "string_bin_parser.h"
#include "istring_provider.h"
class String_Provider: public IString_Provider{
    public:
        String_Provider(const String_Bin_Parser* sp): m_sp(sp) {}
        const char* get_string_by_id(int id)const {
            if (m_sp == NULL) return NULL;

            const std::vector<const char*>& string_depository = m_sp->get_string_depository();
            if (id >= 0 && id < (int)string_depository.size())
                return string_depository.at(id);

            return NULL;
        }
    private:
        const String_Bin_Parser *m_sp;
};
#endif
