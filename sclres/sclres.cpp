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

#include "sclres.h"

using namespace sclres;

SclRes::SclRes()
{
    m_resource_directory.clear();
}

SclRes::~SclRes()
{
}

void
SclRes::set_resource_directory(const char *directory)
{
    m_resource_directory = directory;
}

const char*
SclRes::get_resource_directory()
{
    return m_resource_directory.c_str();
}
