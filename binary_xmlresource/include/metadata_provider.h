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

#ifndef __METADATA_PROVIDER_H__
#define __METADATA_PROVIDER_H_
#include "imetadata_provider.h"
#include "metadata_bin_parser.h"
#include <dlog.h>

class Metadata_Provider:public IMetadata_Provider{
    public:
        Metadata_Provider(const Metadata_Bin_Parser* metadataParser):m_metadataParser(metadataParser) {
            current_record_name = NULL;
        }

        const int get_width(const char* field_name)const{
            const MetaData* metadata = m_metadataParser->get_metadata();
            const MetaData_Record& metadata_record = metadata->m_vec_metadata_record.at(curRecordIndex);
            std::vector<MetaData_Field>::const_iterator it;
            for(it = metadata_record.vField.begin(); it != metadata_record.vField.end(); ++it) {
                if (0 == strcmp(it->m_name,  field_name)) {
                    return it->m_width;
                }
            }

            return 0;

        }
        void set_current_metadata_record(const char*record_name) {
            assert(record_name != NULL);
            if ( current_record_name == NULL ||0 != strcmp(current_record_name, record_name)) {
                current_record_name = record_name;
                curRecordIndex = (int)find_metadata_record_index(record_name);
                if (curRecordIndex == -1) {
                    LOGW("Can not find %s metadata record.", record_name);
                }
            }
        }
    private:
       size_t find_metadata_record_index(const char* name)const{
            if (name == NULL) return -1;

            const MetaData* metadata = m_metadataParser->get_metadata();
            for ( size_t i = 0; i < metadata->m_vec_metadata_record.size(); ++i) {
                const MetaData_Record& metadata_record = metadata->m_vec_metadata_record.at(i);

                if (0 == strcmp(metadata_record.m_name, name)) {
                    return i;
                }
            }

            return -1;
        }
        const Metadata_Bin_Parser *m_metadataParser;
        const char* current_record_name;
        int curRecordIndex;
};
#endif
