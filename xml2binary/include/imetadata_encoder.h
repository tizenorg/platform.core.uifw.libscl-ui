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

#ifndef __INTERFACE_METADATA_ENCODER_H__
#define __INTERFACE_METADATA_ENCODER_H__
class IMetaData_Encoder{
    public:
        virtual int encode(const char* file) const = 0;
        virtual int encode(const char* file, int& offset) const = 0;
        virtual ~IMetaData_Encoder() {}
};
#endif
