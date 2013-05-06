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

#ifndef __PARSERINFO_PROVIDER_H__
#define __PARSERINFO_PROVIDER_H__
#include "iparserinfo_provider.h"
#include "istring_provider.h"
#include "imetadata_provider.h"

class ParserInfo_Provider: public IParserInfo_Provider{
    public:
        ParserInfo_Provider(IMetadata_Provider* metadataProvider,
                            const IString_Provider* stringProvider) :m_metadataProvider(metadataProvider),
                            m_stringProvider(stringProvider) {}
        void set_current_metadata_record(const char* record_name) {
            assert(m_metadataProvider);
            if (m_metadataProvider != NULL) {
                m_metadataProvider->set_current_metadata_record(record_name);
            }
        }
         const int get_width(const char* field_name)const{
            assert(m_metadataProvider != NULL);
            if (m_metadataProvider != NULL)
                return m_metadataProvider->get_width(field_name);

            return 0 ;
        }

        const char* get_string_by_id(const int id)const{
            if (m_stringProvider != NULL) {
                return m_stringProvider->get_string_by_id(id);
            }
            return NULL;
        }
    private:
        IMetadata_Provider *m_metadataProvider;
        const IString_Provider *m_stringProvider;
};
#endif
