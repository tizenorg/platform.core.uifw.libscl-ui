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

#ifndef __FILE_STORAGE_IMPL_H__
#define __FILE_STORAGE_IMPL_H__
#include "file_storage.h"
///////////////////////////////////////
inline uint8 FileStorage::
get_uint8() {
    uint8* p = (uint8*)(m_storage + m_offset);
    m_offset += sizeof(uint8);
    return *p;
}
inline uint16 FileStorage::
get_uint16() {
    uint16* p = (uint16*)(m_storage + m_offset);
    m_offset += sizeof(uint16);
    return *p;
}
inline uint32 FileStorage::
get_uint32() {
    uint32* p = (uint32*)(m_storage + m_offset);
    m_offset += sizeof(uint32);
    return *p;
}
inline uint64 FileStorage::
get_uint64() {
    uint64* p = (uint64*)(m_storage + m_offset);
    m_offset += sizeof(uint64);
    return *p;
}
// in put_methods,
// inline put_intx(intx data) {
//    put_uintx(data);
// }
// so below is ok
inline int8 FileStorage::
get_int8() {
    return (int)get_uint8();
}

inline int16 FileStorage::
get_int16() {
    return (int)get_uint16();
}
inline int32 FileStorage::
get_int32() {
    return (int)get_uint32();
}
inline int64 FileStorage::
get_int64() {
    return (int)get_uint64();
}
//float32
inline float32 FileStorage::
get_float32() {
    union{
        float32 m;
        uint32 n;
    };

    n = get_uint32();
    return m;
}
//float64
inline float64 FileStorage::
get_float64() {
    union{
        float64 m;
        uint64 n;
    };

    n = get_uint64();
    return m;
}

//unify put_xx method name
//the width is sizeof(T)
template <>
inline uint8 FileStorage::
get_primitive_data<uint8>() {
    return get_uint8();
}
template <>
inline uint16 FileStorage::
get_primitive_data<uint16>() {
    return get_uint16();
}

template <>
inline uint32 FileStorage::
get_primitive_data<uint32>() {
    return get_uint32();
}
template <>
inline uint64 FileStorage::
get_primitive_data<uint64>() {
    return get_uint64();
}
template <>
inline int8 FileStorage::
get_primitive_data<int8>() {
    return get_int8();
}
template <>
inline int16 FileStorage::
get_primitive_data<int16>() {
    return get_int16();
}

template <>
inline int32 FileStorage::
get_primitive_data<int32>() {
    return get_int32();
}
template <>
inline int64 FileStorage::
get_primitive_data<int64>() {
    return get_int64();
}
template <>
inline float32 FileStorage::
get_primitive_data<float32>() {
    return get_float32();
}
template <>
inline float64 FileStorage::
get_primitive_data<float64>() {
    return get_float64();
}
//put sint_t value to storage
//width is assigned by para "width"
template <>
inline sint_t FileStorage::
get<sint_t>(int width) {
    switch (width) {
        case 0:
            /*Nothing to put*/
            break;
        case 1:
            return get_primitive_data<int8>();
        case 2:
            return get_primitive_data<int16>();
        case 4:
            return get_primitive_data<int32>();
        case 8:
            return get_primitive_data<int64>();
        default:
            break;
    }
    return -1;
}
template <>
inline uint_t FileStorage::
get<uint_t>(int width) {
    switch (width) {
        case 0:
            /*Nothing to put*/
            break;
        case 1:
            return get_primitive_data<uint8>();
        case 2:
            return get_primitive_data<uint16>();
        case 4:
            return get_primitive_data<uint32>();
        case 8:
            return get_primitive_data<uint64>();
        default:
            break;
    }
    return -1;
}

template <>
inline float_t FileStorage::
get<float_t>(int width) {
    switch (width) {
        case 0:
            return 0;
        case 4:
            return (float_t)get_primitive_data<float32>();
        case 8:
            return (float_t)get_primitive_data<float64>();
        default:
            break;
    }
    return -1;
}
#endif
