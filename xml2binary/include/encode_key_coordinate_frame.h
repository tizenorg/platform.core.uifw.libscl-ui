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

#ifndef __ENCODE_KEY_COORDINATE_FRAME_H__
#define __ENCODE_KEY_COORDINATE_FRAME_H__
#include "metadata.h"
#include "imetadata_helper.h"
#include "resource_storage_impl.h"

#ifndef LIBSCL_EXPORT_API
#define LIBSCL_EXPORT_API 
#endif // LIBSCL_EXPORT_API

LIBSCL_EXPORT_API int encode_key_coordinate_frame_file(ResourceStorage& storage, IMetaData_Helper& md_helper);
LIBSCL_EXPORT_API int encode_key_coordinate_frame_file(const char* file, int& offset, IMetaData_Helper& md_helper);
LIBSCL_EXPORT_API int encode_key_coordinate_frame_file(const char* file, IMetaData_Helper& md_helper);

#endif
