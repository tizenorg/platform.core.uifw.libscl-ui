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

#ifndef __ENCODE_DEFAULT_CONFIGURE_H__
#define __ENCODE_DEFAULT_CONFIGURE_H__
#include "imetadata_helper.h"
#include "resource_storage_impl.h"
int encode_default_configure_file(ResourceStorage& storage, IMetaData_Helper& metadata);

int encode_default_configure_file(const char* file, int& offset, IMetaData_Helper& metadata);
int encode_default_configure_file(const char* file, IMetaData_Helper& metadata);
#endif