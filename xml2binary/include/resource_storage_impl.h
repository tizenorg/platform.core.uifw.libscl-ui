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

#ifndef __RESOURCE_STORAGE_IMPL_H__
#define __RESOURCE_STORAGE_IMPL_H__
#include "resource_storage.h"

inline ResourceStorage::
ResourceStorage() {
    init();
}

inline void ResourceStorage::init() {
    m_storage = new char[__NEW_LENGTH__];
    if (m_storage == NULL) {
        //throw "init error of ResourceStorage";
        m_size = 0;
        m_capability = 0;
    } else {
        m_size = 0;
        m_capability = __NEW_LENGTH__;
    }
}
inline ResourceStorage::
~ResourceStorage() {
    delete []m_storage;
    m_size = 0;
    m_capability = 0;
}

inline const int ResourceStorage::
get_size() const{
    return m_size;
}


inline void ResourceStorage::
put(const char* str) {
    /*Not allowed str null here*/
    assert(str != NULL);
    int len = strlen(str);
    /*1(record string length) + len + 1(\0))*/
    int width = 1 + len + 1;
    check_storage(width);
    *(m_storage + m_size) = len;
    m_size += 1;

    strncpy(m_storage + m_size, str, len);
    m_size += len;

    /*Add '\0' to the end*/
    *(m_storage + m_size) = '\0';
    m_size += 1;
}

inline const int ResourceStorage::
capability() const{
    return m_capability;
}

inline int ResourceStorage::
toFile(const char* fileName) {
    if (m_storage == NULL) {
        return -1;
    }
    FILE *fp = fopen(fileName, "rb+");
    if (!fp) {
        perror(fileName);
        return -1;
    }

    int actual_size = m_size;
    fwrite(m_storage, actual_size, 1, fp);
    fclose(fp);

    return actual_size;
}
inline int ResourceStorage::
toFile(const char* fileName, int& offset) {
    if (m_storage == NULL) {
        return -1;
    }
    FILE *fp = fopen(fileName, "rb+");
    if (!fp) {
        perror(fileName);
        return -1;
    }

    if (0 != fseek(fp, offset, 0)) {
        perror(fileName);
        fclose(fp);
        return -1;
    }

    if (m_size < 0) {
        fclose(fp);
        return -1;
    }
    fwrite(m_storage, m_size, 1, fp);
    fclose(fp);

    offset += m_size;
    return m_size;
}
inline void ResourceStorage::
reserve(int bytes) {
    check_storage(bytes);
    m_size += bytes;
}
inline int ResourceStorage::
storage_cat(ResourceStorage& storage) {
    if (storage.get_size() == 0) return 0;

    int size = storage.get_size();
    check_storage(size);
    memcpy(m_storage + m_size, storage.m_storage, size);

    m_size += size;
    return m_size;
}

inline void ResourceStorage::
check_storage(int width) {

    if (m_size + width > m_capability) {
        expand_storage();
    }
}
inline void ResourceStorage::
expand_storage() {
    unsigned int _new_size = (unsigned int)(m_capability + __RE_NEW_LENGTH__);
    if (_new_size > (unsigned int)__MAX_NEW_SIZE__) {
        printf("expand_storage failed: size is limited to %d\n", __MAX_NEW_SIZE__);
        return;
    }

    char* _p = new char[_new_size];
    if(_p == NULL) {
        printf("expand_storage error");
        return;
    }
    memset(_p, 0x00, _new_size);
    memcpy(_p, m_storage, m_size);
    delete[] m_storage;

    m_storage = _p;
    m_capability = _new_size;

    _p = NULL;
}
///////////////////////////////////////
inline void ResourceStorage::
put_uint8(uint8 data) {
    check_storage(sizeof(uint8));
    uint8* p = (uint8*)(m_storage + m_size);
    *p = data;
    m_size += sizeof(uint8);
}
inline void ResourceStorage::
put_uint16(uint16 data) {
    check_storage(sizeof(uint16));
    uint16* p = (uint16*)(m_storage + m_size);
    *p = data;
    m_size += sizeof(uint16);
}
inline void ResourceStorage::
put_uint32(uint32 data) {
    check_storage(sizeof(uint32));
    uint32* p = (uint32*)(m_storage + m_size);
    *p = data;
    m_size += sizeof(uint32);
}
inline void ResourceStorage::
put_uint64(uint64 data) {
    check_storage(sizeof(uint64));
    uint64* p = (uint64*)(m_storage + m_size);
    *p = data;
    m_size += sizeof(uint64);
}
// signed value --> unsigned value
//that is ok
inline void ResourceStorage::
put_int8(int8 data) {
    put_uint8(data);
}
inline void ResourceStorage::
put_int16(int16 data) {
    put_uint16(data);
}
inline void ResourceStorage::
put_int32(int32 data) {
    put_uint32(data);
}
inline void ResourceStorage::
put_int64(int64 data) {
    put_uint64(data);
}

//float32
inline void ResourceStorage::
put_float32(float32 data) {
    union{
        float32 m;
        uint32 n;
    };

    m = data;
    put_uint32(n);
}
//float64
inline void ResourceStorage::
put_float64(float64 data) {
    union{
        float64 m;
        uint64 n;
    };

    m = data;
    put_uint64(n);
}

//unify put_xx method name
//the width is sizeof(T)
template <>
inline void ResourceStorage::
put_primitive_data<uint8>(uint8 data) {
    put_uint8(data);
}
template <>
inline void ResourceStorage::
put_primitive_data<uint16>(uint16 data) {
    put_uint16(data);
}
template <>
inline void ResourceStorage::
put_primitive_data<uint32>(uint32 data) {
    put_uint32(data);
}
template <>
inline void ResourceStorage::
put_primitive_data<uint64>(uint64 data) {
    put_uint64(data);
}
template <>
inline void ResourceStorage::
put_primitive_data<int8>(int8 data) {
    put_int8(data);
}
template <>
inline void ResourceStorage::
put_primitive_data<int16>(int16 data) {
    put_int16(data);
}
template <>
inline void ResourceStorage::
put_primitive_data<int32>(int32 data) {
    put_int32(data);
}
template <>
inline void ResourceStorage::
put_primitive_data<int64>(int64 data) {
    put_int64(data);
}
template <>
inline void ResourceStorage::
put_primitive_data<float32>(float32 data) {
    put_float32(data);
}
template <>
inline void ResourceStorage::
put_primitive_data<float64>(float64 data) {
    put_float64(data);
}

//put integer value to storage
//width is assigned by para "width"
template <>
inline int ResourceStorage::
put<sint_t>(sint_t data, int width) {
    switch (width) {
        case 0:
            /*Nothing to put*/
            break;
        case 1:
            put_primitive_data<int8>((int8)data);
            break;
        case 2:
            put_primitive_data<int16>((int16)data);
            break;
        case 4:
            put_primitive_data<int32>((int32)data);
            break;
        case 8:
            put_primitive_data<int64>((int64)data);
            break;
        default:
            return -1;
    }

    return 0;
}
template <>
inline int ResourceStorage::
put<uint_t>(uint_t data, int width) {
    switch (width) {
        case 0:
            /*Nothing to put*/
            break;
        case 1:
            put_primitive_data<uint8>((uint8)data);
            break;
        case 2:
            put_primitive_data<uint16>((uint16)data);
            break;
        case 4:
            put_primitive_data<uint32>((uint32)data);
            break;
        case 8:
            put_primitive_data<uint64>((uint64)data);
            break;
        default:
            return -1;
    }

    return 0;
}
template <>
inline int ResourceStorage::
put<float_t>(float_t data, int width) {
    switch (width) {
        case 0:
            break;
        case 4:
            put_primitive_data<float32>((float32)data);
            break;
        case 8:
            put_primitive_data<float64>((float64)data);
            break;
        default:
            return -1;
    }
    return 0;
}
template<>
inline int ResourceStorage::
random_put<sint_t>(sint_t data, int width, int offset) {
    if (offset <0 || offset + width > m_size) {
        return -1;
    }

    //remember current_offset
    int current_offset = m_size;

    //temperary turn to random_offset
    int random_offset = offset;
    m_size = random_offset;

    int ret = put<sint_t>(data, width);
    //reback to current_offset
    m_size = current_offset;

    return ret;
}
template<>
inline int ResourceStorage::
random_put<uint_t>(uint_t data, int width, int offset) {
    if (offset <0 || offset + width > m_size) {
        return -1;
    }

    //remember current_offset
    int current_offset = m_size;

    //temperary turn to random_offset
    int random_offset = offset;
    m_size = random_offset;

    int ret = put<uint_t>(data, width);
    //reback to current_offset
    m_size = current_offset;

    return ret;
}
template <>
inline int ResourceStorage::
random_put<float_t>(float_t data, int width, int offset) {
    if (offset <0 || offset + width > m_size) {
        return -1;
    }

    //remember current_offset
    int current_offset = m_size;

    //temperary turn to random_offset
    int random_offset = offset;
    m_size = random_offset;

    int ret = put<float_t>(data, width);
    //reback to current_offset
    m_size = current_offset;

    return ret;
}

///////////////////////////////////////
inline void ResourceStorage::
encode_string(const char* str, int width) {
    if (width <= 0) return;

    int id = string_encoder.add(str);
    put<sint_t>(id, width);
}


#endif
