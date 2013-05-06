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

#ifndef __SCL_TYPES_H__
#define __SCL_TYPES_H__

#define SCL_BEGIN_DECLS extern "C" {
#define SCL_END_DECLS }

#ifndef	FALSE
#define	FALSE (0)
#endif

#ifndef	TRUE
#define	TRUE (!FALSE)
#endif

#ifndef	NOT_USED
#define	NOT_USED -1
#endif

#ifndef	USED
#define	USED 1
#endif

typedef signed char scl8;
typedef signed short scl16;
typedef signed int scl32;

typedef unsigned char sclu8;
typedef unsigned short sclu16;
typedef unsigned int sclu32;

typedef char sclchar;
typedef short sclshort;
typedef long scllong;
typedef int sclint;
typedef char sclboolean;

typedef unsigned char sclbyte;
typedef unsigned char scluchar;
typedef unsigned short sclushort;
typedef unsigned long sclulong;
typedef unsigned int scluint;

typedef float sclfloat;
typedef double scldouble;

/* sclwindow should be a struct - containing a flag whether it is a virtual window or not*/
#define SCLWINDOW_INVALID 0
#define SCLWINDOW_VIRTUAL_DIM -1
#define SCLWINDOW_VIRTUAL_MAGNIFIER -2

typedef enum _SCLEventReturnType {
    SCL_EVENT_PASS_ON, /* This event needs further handling of default SCL event handler */
    SCL_EVENT_DONE,    /* We're done with this event, do not call any other default SCL event handlers */
} SCLEventReturnType;
/*
typedef struct _SclWindow {
	operator =(void *ptr) {is = false}
    sclboolean isVirtual;
    void* ptr;
} SclWindow;*/

typedef void* sclwindow;
typedef void* sclimage;
typedef void* sclfont;
typedef void* scldrawctx;
typedef sclint scltouchdevice;
#define SCLTOUCHDEVICE_INVALID -1

typedef struct _SclPoint {
    sclint x, y;
}SclPoint;

typedef struct _SclSize {
    sclint width, height;
}SclSize;

typedef struct _SclRect {
    sclint left, top, right, bottom;
}SclRect;

typedef struct _SclRectangle {
    sclint x, y, width, height;
}SclRectangle;

typedef struct _SclColor {
    sclbyte r, g, b, a;
}SclColor;

#define SCL_MAKEWORD(low, high) ((sclu16)(((sclu8)(low)) | ((sclu16)((sclu8)(high))) << 8))
#define SCL_MAKELONG(low, high) ((sclu32)(((sclu16)(low)) | ((sclu32)((sclu16)(high))) << 16))
#define SCL_LOWORD(i32) ((sclu16)((i32) & 0xFFFF))
#define SCL_HIWORD(i32) ((sclu16)(((sclu32)(i32) >> 16) & 0xFFFF))
#define SCL_LOBYTE(i16) ((sclu8)((i16) & 0xFF))
#define SCL_HIBYTE(i16) ((sclu8)(((sclu16)(i16) >> 8) & 0xFF))

/* Checks if the given character is in between '!'(0x21) and '~'(0x7e) range */
#define SCL_ISCHAR(ch) ((ch) >= '!' && (ch) <= '~')

#endif //__SCL_TYPES_H__
