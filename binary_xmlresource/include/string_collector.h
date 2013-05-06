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

#ifndef __NEW_STRING_COLLECTOR_H__
#define __NEW_STRING_COLLECTOR_H__
#include <vector>
#include "istring_collector.h"
class StringCollector: public IStringCollector{
    public:
        StringCollector() {
        }
        void add(char* str) {
            if (str != NULL) {
                m_vec_string_collection.push_back(str);
            }
        }
        ~StringCollector() {
            std::vector<char*>::iterator it;
            for (it = m_vec_string_collection.begin();
                    it != m_vec_string_collection.end(); ++it) {
                char* p = *it;
                if (p) {
                    delete[] p;
                }

                p = NULL;
            }

            m_vec_string_collection.clear();

        }
    private:
        std::vector<char*> m_vec_string_collection;
};
#endif
