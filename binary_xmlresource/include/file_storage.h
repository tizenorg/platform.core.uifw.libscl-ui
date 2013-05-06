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

#ifndef __FILE_STORAGE_H__
#define __FILE_STORAGE_H__

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "istring_provider.h"
#include "istring_collector.h"

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

typedef int64 sint_t;
typedef uint64 uint_t;
typedef float64 float_t;
using namespace std;
class FileStorage{
    public:
        FileStorage(const IString_Provider* string_provider = NULL);
    ~FileStorage();

    int offset() const;
    void set_str_provider(const IString_Provider*);
    template <typename T>
    T get(int width);

    const char* get_str(int width);

    void get_str(char** p, int width, IStringCollector& collector);

    const char* get_str();

    int loadFile(const char* file_name);
    int loadFile(const char* file_name, int offset, int size);
    /** brief get_storage from a field of another storage
     */
    int get_storage(const FileStorage&, int offset, int block_size);

    void advance(int bytes);
    int size()const;

    private:
    /** brief unify get_xx method name
     */
    template <typename T>
    T get_primitive_data();

    uint8 get_uint8();
    uint16 get_uint16();
    uint32 get_uint32();
    uint64 get_uint64();
    int8 get_int8();
    int16 get_int16();
    int32 get_int32();
    int64 get_int64();
    float32 get_float32();
    float64 get_float64();

    char* m_storage; /* memory that read from bin file */
    int m_offset; /* current offset of the memory */
    int m_size; /* the size of the memory */

    /** brief Infact, the strings' ids are stored for structures
    * m_string_provider can get the actual string by id
    */
    const IString_Provider* m_string_provider;
};

#endif
