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

#ifndef __RESOURCE_STORAGE_H__
#define __RESOURCE_STORAGE_H__
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <sstream>
#include "istring_encoder.h"

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;
typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;
typedef float float32;
typedef double float64;

typedef uint64 uint_t;
typedef int64 sint_t;
typedef float64 float_t;

/* use string_encoder to collect strings and encode them when necessary*/
extern IString_Encoder& string_encoder;

class ResourceStorage{
    private:
    static const int __NEW_LENGTH__ = 0x10000;/*64k*/
    static const int __RE_NEW_LENGTH__ = 0x10000;/*64k*/
    static const int __MAX_NEW_SIZE__ = 0x2000000;/*32M*/
    public:
    ResourceStorage();
    ~ResourceStorage();

    // use reserve for temp reserve
    // latter we can use random_put to put values
    void reserve(int bytes);

    // put the string to storage
    void put(const char* str);

    // encode the string id
    void encode_string(const char* str, int width);

    // put the data to storage with width assigned as para
    // It is specailly for:
    // 1) sint_t(signed integer, includes int8, int16, int32, int64),
    // 2) uint_t(unsigned integer, includes uint8, uint16, uint32, uint64),
    // 3) float_t(float type, includes float32, float64)
    // other type will get a complie error
    template <typename T>
        int put(T data, int width);

    // put the data to a random_offset assigned by parameter "random_offset"
    // after done, it will go back to current offset of the file
    template <typename T>
        int random_put(T data, int width, int random_offset);

    // get the current used size
    const int size()const;

    // get the capability of the storage
    const int capability() const;

    // put the storage array to file
    // the fileName assigned
    // return size that put
    int toFile(const char* fileName);

    // put the storage array to file
    // the fileName and offset assigned
    // return size that put
    int toFile(const char* fileName, int& offset);

    // put the storage array to a big storage
    // return size that put
    int storage_cat(ResourceStorage& storage);
    private:
    void init();

    /*template <typename T>
    void put_primitive_data(T data, MetaType type, bool ignore = false);*/

    // This function is used to check whether the 
    // storage is enough
    // the para "width" is the size of request to use
    void check_storage(int width);

    // If the space is not enough, use this function to 
    // new a new storage
    void expand_storage();

    /*****************put methods****************/

    //unify put_xx method name
    //the width is sizeof(T)
    template <typename T>
    void put_primitive_data(T data);

    void put_uint8(uint8 data);
    void put_uint16(uint16 data);
    void put_uint32(uint32 data);
    void put_uint64(uint64 data);
    void put_int8(int8 data);
    void put_int16(int16 data);
    void put_int32(int32 data);
    void put_int64(int64 data);
    void put_float32(float32 data);
    void put_float64(float64 data);
    /*****************put methods end****************/


    //Pointer to the storage
    char* m_storage;

    //Record the actual used size
    int m_size;

    //The total size of the Storage
    int m_capability;
};
#endif
