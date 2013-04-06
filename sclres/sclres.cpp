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