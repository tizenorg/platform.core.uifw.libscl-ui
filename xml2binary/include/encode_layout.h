/*
 * Copyright 2012-2013 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Flora License, Version 1.0 (the "License");
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

#ifndef __ENCODE_LAYOUT_H__
#define __ENCODE_LAYOUT_H__
#include "metadata.h"
#include "imetadata_helper.h"
int encode_layout_file(const char* file, IMetaData_Helper& md_helper);

int encode_layout_file(const char* file, int& offset, IMetaData_Helper& md_helper);
int encode_layout_file(ResourceStorage& storage, IMetaData_Helper& md_helper);
#endif