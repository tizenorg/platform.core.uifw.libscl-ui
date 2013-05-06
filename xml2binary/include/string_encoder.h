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

#ifndef __STRING_ENCODER_H__
#define __STRING_ENCODER_H__
#include "istring_encoder.h"
#include "resource_storage_impl.h"
#include <vector>
#include <string>
/* limit the max num to 2^32-1*/
const int MAX_NUM_WIDTH = 4;

class String_Encoder:public IString_Encoder{
    public:
        String_Encoder() {
        }

        void set_path(const char* file) {
            this->file = file;
        }

        /* @ insert str to string vector */
        /* @ return the ID of the str */
        const int add(const char* str) {
            if (str == NULL) return -1;

            std::string str_temp = std::string(str);

            /* check if the str_temp is already in the vector*/
            std::vector<std::string>::iterator it;
            for (it = m_vec_string.begin(); it != m_vec_string.end(); ++it) {
                if (str_temp == *it) {
                    return (it - m_vec_string.begin());
                }
            }

            /* the str_temp is not in the vector, so insert it to the vector*/
            m_vec_string.push_back(str_temp);
            return m_vec_string.size() -1;
        }
        int encode() const{
            ResourceStorage storage;
            encode(storage);
            storage.toFile(file);
            return storage.size();
        }
        int encode(int& offset) const{
            ResourceStorage storage;
            encode(storage);
            storage.toFile(file, offset);

            return storage.size();
        }

        int encode(ResourceStorage& storage) const{
            /* record the strings' num*/
            storage.put<sint_t>(m_vec_string.size(), MAX_NUM_WIDTH);

            /* record each string*/
            std::vector<std::string>::const_iterator it;
            for (it = m_vec_string.begin(); it != m_vec_string.end(); ++it) {
                storage.put(it->c_str());
            }

            return storage.size();
        }

    private:
        std::vector<std::string> m_vec_string;
        const char* file;
};
#endif
