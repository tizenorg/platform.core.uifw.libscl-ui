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

#ifndef __SIMPLE_DEBUG_H__
#define __SIMPLE_DEBUG_H__
#define _PRINT(label, ...)\
    do{\
        fprintf(stderr, "[%s] %s at %s:", label, __FILE__, __FUNCTION__); \
        fprintf(stderr, __VA_ARGS__);\
        fprintf(stderr, "\n");\
    }while (0);

#define LOG(...) _PRINT("log", __VA_ARGS__)
#define DEBUG(...) _PRINT("debug", __VA_ARGS__)
#endif
