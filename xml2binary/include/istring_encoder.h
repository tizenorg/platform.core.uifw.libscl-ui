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

#ifndef __INTERFACE_STRING_ENCODER_H__
#define __INTERFACE_STRING_ENCODER_H__
class ResourceStorage;
class IString_Encoder{
    public:
        virtual const int add(const char*) = 0;
        virtual int encode() const = 0;
        virtual int encode(int& offset) const = 0;
        virtual int encode(ResourceStorage&) const = 0;
        virtual ~IString_Encoder() {}
};
#endif
