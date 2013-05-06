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

#include "file_storage_impl.h"

FileStorage::FileStorage(const IString_Provider* string_provider):
    m_string_provider(string_provider),
    m_storage(NULL),
    m_size(0),
    m_offset(0) {
    }

FileStorage::~FileStorage() {
    if (m_storage) {
        delete []m_storage;
    }
    m_storage = NULL;
    m_size = 0;
    m_offset = 0;
}

int FileStorage::
offset() const {
    return m_offset;
}
void FileStorage::
set_str_provider(const IString_Provider* string_provider) {
    this->m_string_provider = string_provider;
}
const char* FileStorage::
get_str(int width) {
    assert(m_string_provider);

    if (width <= 0) return NULL;

    int string_no = get<sint_t>(width);
    return m_string_provider->get_string_by_id(string_no);
}

void FileStorage::
get_str(char** p, int width, IStringCollector& collector) {
    if (p == NULL) return;
    const char* str = get_str(width);
    if (str != NULL) {
        int len = strlen(str) + 1;
        *p = new char[len];
        strncpy((*p), str, len);
        collector.add(*p);
    } else {
        *p = NULL;
    }
}

void FileStorage::
advance(int bytes) {
    m_offset += bytes;
}

const char* FileStorage::
get_str() {
    // the length of the string, not include '\0'
    int len = get<sint_t>(1);
    // the string stored in storage are end with '\0'
    const char* str = m_storage + m_offset;
    m_offset += len + 1;
    return str;
}

int FileStorage::
    loadFile(const char* file_name) {

    if (file_name == NULL) return -1;

    FILE* fp = fopen(file_name, "rb");
    if (fp == NULL) {
        return -1;
    }

    if (m_storage != NULL) {
        delete[] m_storage;
    }
    m_storage = NULL;

    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);

    if (size > 0) {
        m_storage = new char[size];
    }
    if (m_storage == NULL) {
        fclose(fp);
        return -1;
    }

    if (0 != fseek(fp, 0L, SEEK_SET)) {
        if (m_storage) {
            delete []m_storage;
        }
        m_storage = NULL;
        fclose(fp);
        return -1;
    }
    if ( 1 != fread(m_storage, size, 1, fp)) {
        if (m_storage) {
            delete []m_storage;
        }
        m_storage = NULL;
        fclose(fp);
        return -1;
    }
    fclose(fp);

    m_size = size;
    m_offset = 0;
    return m_size;
}
int FileStorage::
    loadFile(const char* file_name, int offset, int size) {
    if (file_name == NULL) return -1;
    if (offset < 0 || size <= 0) return -1;

    FILE* fp = fopen(file_name, "rb");
    if (fp == NULL) {
        return -1;
    }

    if (m_storage != NULL) {
        delete[] m_storage;
    }
    m_storage = NULL;

    fseek(fp, 0, SEEK_END);
    int file_size = ftell(fp);
    if (file_size < offset + size) {
        fclose(fp);
        return -1;
    }
    m_storage = new char[size];
    if (m_storage == NULL) {
        fclose(fp);
        return -1;
    }

    int nbytes = 0;
    if (fseek(fp, offset, 0) != -1) {
        nbytes = fread(m_storage, size, 1, fp);
    }
    if (nbytes != 1) {
        if (m_storage) {
            delete []m_storage;
        }
        m_storage = NULL;
        fclose(fp);
        return -1;
    }
    fclose(fp);

    m_size = size;
    m_offset = 0;
    return m_size;
}
int FileStorage::
size()const{
    return m_size;
}
int FileStorage::
get_storage(const FileStorage& storage, int offset, int block_size) {
    if (offset < 0 || block_size <= 0) return -1;
    if (storage.size() < offset + block_size) return -1;

    if (m_storage != NULL) {
        delete[] m_storage;
    }

    m_storage = new char[block_size];
    if (m_storage == NULL) {
        return -1;
    }

    memcpy(m_storage, storage.m_storage + offset, block_size);

    m_size = block_size;
    m_offset = 0;
    return 0;
}
