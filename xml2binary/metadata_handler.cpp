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

#include "metadata_handler.h"

MetaData_Handler::
MetaData_Handler(const char* file) {
    this->file_name = file;
    m_current_metadata_record_id = -1;
    parsing_metadata();
}

void MetaData_Handler::
set_current_metadata_record(const char* record_name) {
    if (record_name == NULL) {
        m_current_metadata_record_id = -1;
        return;
    }
    int idx = find_metadata_record_index(record_name);

    m_current_metadata_record_id = idx;
}

int MetaData_Handler::
encode(const char* file)const{
    ResourceStorage storage;
    encode_metadata(storage, metadata);
    storage.toFile(file);

    return storage.get_size();
}

int MetaData_Handler::
encode(const char* file, int& offset)const{
    ResourceStorage storage;
    encode_metadata(storage, metadata);
    storage.toFile(file, offset);

    return storage.get_size();
}

int MetaData_Handler::
encode(ResourceStorage& storage)const{
    encode_metadata(storage, metadata);
    return storage.get_size();
}

const unsigned short MetaData_Handler::
get_width(const char* field_name) const{
    if (field_name == NULL) return 0;

    int idx = m_current_metadata_record_id;
    if (idx < 0 || idx > (int)metadata.m_vec_metadata_record.size()) return 0;

    const MetaData_Record&  metadata_record = metadata.m_vec_metadata_record.at(idx);

    int width = get_width(field_name, metadata_record);

    return width;
}

const unsigned short MetaData_Handler::
get_width(const char* name, const MetaData_Record& metadata_record) const{
    assert(name);
    for (size_t i = 0; i < metadata_record.vField.size(); ++i) {
        if ( 0 == strcmp(name, metadata_record.vField.at(i).m_name)) {
            return metadata_record.vField.at(i).m_width;
        }
    }
    return 0;
}

const int MetaData_Handler::
find_metadata_record_index(const char* name)const{
    assert(name);

    for ( size_t i = 0; i < metadata.m_vec_metadata_record.size(); ++i) {
        const MetaData_Record& metadata_record = metadata.m_vec_metadata_record.at(i);

        if (0 == strcmp(metadata_record.m_name, name)) {
            return i;
        }
    }

    return -1;

}

int MetaData_Handler::
parsing_field(const xmlNodePtr node, MetaData_Field& data, const MetaData_Width& metadataWidth) {
    memset(data.m_name, 0, sizeof(data.m_name));
    memset(data.m_type, 0, sizeof(data.m_type));
    data.m_width = 0;

    xmlChar* name = xmlGetProp(node, (const xmlChar*)"name");
    if (name == NULL) return -1;
    /*FIXME strncpy ?*/
    strncpy(data.m_name, (const char*)name, sizeof(data.m_name));
    data.m_name[sizeof(data.m_name)-1] = '\0';
    xmlFree(name);

    xmlChar* type = xmlGetProp(node, (const xmlChar*)"type");
    if (type == NULL) return -1;

    int ret = 0;
    if (0 == xmlStrcmp(type, (const xmlChar*)"string_id")) {
        /*FIXME use vaule instead string*/
        strncpy(data.m_type, (const char*)type, sizeof(data.m_type));
        data.m_type[sizeof(data.m_type) - 1] = '\0';
        data.m_width = metadataWidth.string_id_width;
    } else if ( 0 == xmlStrcmp(type, (const xmlChar*)"int8")) {
        strncpy(data.m_type, (const char*)type, sizeof(data.m_type));
        data.m_type[sizeof(data.m_type) - 1] = '\0';
        data.m_width = 1;
    } else if ( 0 == xmlStrcmp(type, (const xmlChar*)"int16")) {
        strncpy(data.m_type, (const char*)type, sizeof(data.m_type));
        data.m_type[sizeof(data.m_type) - 1] = '\0';
        data.m_width = 2;
    } else if ( 0 == xmlStrcmp(type, (const xmlChar*)"int32")) {
        strncpy(data.m_type, (const char*)type, sizeof(data.m_type));
        data.m_type[sizeof(data.m_type) - 1] = '\0';
        data.m_width = 4;
    } else if ( 0 == xmlStrcmp(type, (const xmlChar*)"float32")) {
        strncpy(data.m_type, (const char*)type, sizeof(data.m_type));
        data.m_type[sizeof(data.m_type) - 1] = '\0';
        data.m_width = 4;
    } else if ( 0 == xmlStrcmp(type, (const xmlChar*)"float64")) {
        strncpy(data.m_type, (const char*)type, sizeof(data.m_type));
        data.m_type[sizeof(data.m_type) - 1] = '\0';
        data.m_width = 8;
    }
    else{
        ret = -1;
    }

    xmlFree(type);

    return ret;
}

int MetaData_Handler::
parsing_record(const xmlNodePtr curNode, MetaData_Record& metadataRecord, const MetaData_Width& metadataWidth) {
    //parsing struct name
    xmlChar* name = xmlGetProp(curNode, (const xmlChar*)"name");
    if (name == NULL) return -1;
    strncpy(metadataRecord.m_name, (const char*)name, sizeof(metadataRecord.m_name));
    metadataRecord.m_name[sizeof(metadataRecord.m_name)-1] = '\0';
    xmlFree(name);

    xmlNodePtr childNode = curNode->xmlChildrenNode;

    while (childNode != NULL) {
        if (0 == xmlStrcmp(childNode->name, (const xmlChar *)"field")) {
            MetaData_Field data;
            int ret = parsing_field(childNode, data, metadataWidth);
            if (ret != -1) {
                metadataRecord.vField.push_back(data);
            }
        }

        childNode = childNode->next;
    }
    return 0;
}
int MetaData_Handler::
parsing_metadata_type(const xmlNodePtr curNode, MetaData_Width& metadataWidth) {
    assert(curNode != NULL);
    xmlNodePtr childNode = curNode->xmlChildrenNode;

    while (childNode != NULL) {
        if (0 == xmlStrcmp(childNode->name, (const xmlChar *)"type")) {
            xmlChar* xmlname = xmlGetProp(childNode, (const xmlChar*)"name");
            if (xmlname == NULL) continue;
            xmlChar* xmlwidth = xmlGetProp(childNode, (const xmlChar*)"width");
            if (xmlwidth == NULL) {
                xmlFree(xmlname);
                continue;
            }

            /*FIXME how to assume that the atoi will get the correct num*/
            int width = atoi((const char*)xmlwidth);

            if ( 0 == xmlStrcmp(xmlname, (const xmlChar*)"string_id")) {
                metadataWidth.string_id_width = width;
            }

            xmlFree(xmlname);
            xmlFree(xmlwidth);
        }

        childNode = childNode->next;
    }
    return 0;
}
void MetaData_Handler::
parsing_metadata() {
    xmlDocPtr doc;
    xmlNodePtr curNode;

    doc = xmlReadFile(file_name, NULL, 0);
    if (doc == NULL) {
        LOGE("Could not load file.\n");
        return;
    }

    curNode = xmlDocGetRootElement(doc);
    if (curNode == NULL) {
        LOGE("empty document.\n");
        xmlFreeDoc(doc);
        return;
    }
    if (0 != xmlStrcmp(curNode->name, (const xmlChar*)"metadata"))
    {
        LOGE("root name %s error!\n", curNode->name);
        xmlFreeDoc(doc);
        return;
    }

    xmlChar* version = xmlGetProp(curNode, (const xmlChar*)"version");
    if (version) {
        strncpy(metadata.m_version, (const char*)version, sizeof(metadata.m_version));
        metadata.m_version[sizeof(metadata.m_version)-1] = '\0';
        xmlFree(version);
    } else {
        metadata.m_version[0] = '\0';
    }

    MetaData_Width metadataWidth;
    curNode = curNode->xmlChildrenNode;
    while (curNode) {
        if (0 == xmlStrcmp(curNode->name, (const xmlChar*)"metadata_type")) {
            parsing_metadata_type(curNode, metadataWidth);
        }
        else if ( 0 == xmlStrcmp(curNode->name, (const xmlChar*)"record")) {
            MetaData_Record metadataRecord;
            int ret = parsing_record(curNode, metadataRecord, metadataWidth);
            if (ret != -1) {
                metadata.m_vec_metadata_record.push_back(metadataRecord);
            }
        }
        curNode = curNode->next;
    }
    xmlFreeDoc(doc);

}

inline const MetaData* MetaData_Handler::
get_metadata()const{
    return &metadata;
}
