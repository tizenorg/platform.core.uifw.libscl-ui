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

#ifndef __SCL_DEBUG_H__
#define __SCL_DEBUG_H__

#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#if 0
#define SCL_DEBUG_ON
    /* Definition for debug message trace */
#define scldebugc(m_code)						"\x1b[" # m_code
#define scldebug_sync()							(void)fflush(stdout)
#define scldebug_puts(m_code)					(void)fputs(m_code,stdout)
#define scldebug_clear_screen()					scldebug_puts(scldebugc(H) scldebugc(J))
#define scldebug_move(m_x,m_y)					scldebug_puts(scldebugc(m_y;m_x ## H))
#define scldebug_move_x(m_y)					scldebug_puts(scldebugc(;m_y ## H))
#define scldebug_move_y(m_y)					scldebug_puts(scldebugc(m_y ## d))
#define scldebug_puts_xy(m_x,m_y,m_message)	scldebug_puts(scldebugc(m_y;m_x ## H) m_message)
#define mc_normal		scldebugc(0m)
#define mc_black		scldebugc(1;30m)
#define mc_red			scldebugc(1;31m)
#define mc_green		scldebugc(1;32m)
#define mc_yellow		scldebugc(1;33m)
#define mc_blue			scldebugc(1;34m)
#define mc_magenta		scldebugc(1;35m)
#define mc_cyan		scldebugc(1;36m)
#define mc_white		scldebugc(1;37m)

#define MEASURE_START	0
#define MEASURE_END	1

    /* assert macro */
#define scl_assert(_e) \
	if (!(_e)) printf(mc_red "scl assert!(%s) failed at file :%s, line %04d, func: %s" mc_normal "\n", #_e, __FILE__, __LINE__, __FUNCTION__);

#define scl_assert_return(_e) \
	if (!(_e)) { printf(mc_red "scl assert!(%s) failed at file :%s, line %04d, func: %s" mc_normal "\n", #_e, __FILE__, __LINE__, __FUNCTION__); \
       return;}

#define scl_assert_return_false(_e) \
	if (!(_e)) {printf(mc_red "scl assert!(%s) failed at file :%s, line %04d, func: %s" mc_normal "\n", #_e, __FILE__, __LINE__, __FUNCTION__); \
       return 0;}

#define scl_assert_return_null scl_assert_return_false

    /* Measure elapased time */
    float
    SCL_DEBUG_TIME(const char* fileStr, int line, const char* str);

    /* Measure elapased time */
    float
    SCL_DEBUG_ELAPASED_TIME(const char* fileStr, int line, const char* str, int type);

#define SCL_DEBUG_ELAPASED_TIME_START() SCL_DEBUG_ELAPASED_TIME(__FILE__, __LINE__, __FUNCTION__, MEASURE_START);

#define SCL_DEBUG_ELAPASED_TIME_END() SCL_DEBUG_ELAPASED_TIME(__FILE__, __LINE__, __FUNCTION__, MEASURE_END);

//#define SCL_DEBUG() SCL_DEBUG_TIME(__FILE__, __LINE__, __FUNCTION__ );
#define SCL_DEBUG() NULL;

#define SCL_DEBUG_START() \
    static struct timeval t0;\
    static struct timeval t1;\
    gettimeofday(&t0, NULL);\
    printf("[%-20.20s][%04d][" mc_blue "%s" mc_normal "]\n", __FILE__, __LINE__, __FUNCTION__);

#define SCL_DEBUG_END() \
    gettimeofday(&t1, NULL);\
    float etime;\
    etime = ((t1.tv_sec * 1000000 + t1.tv_usec) - (t0.tv_sec * 1000000 + t0.tv_usec))/1000000.0;\
    printf("[%-20.20s][%04d][%s] E: " mc_green "%f" mc_normal " (S:%u) \n", __FILE__, __LINE__, __FUNCTION__, etime, (t0.tv_sec * 1000000 + t0.tv_usec) );

#define IO_TEST

#else
#define SCL_DEBUG_ELAPASED_TIME_START()
#define SCL_DEBUG_ELAPASED_TIME_END()
#define SCL_DEBUG()
#define SCL_DEBUG_START()
#define SCL_DEBUG_END()
//#define IO_TEST

#define scl_assert(_e)
#define scl_assert_return(_e)
#define scl_assert_return_false(_e)
#define scl_assert_return_null scl_assert_return_false

#endif

#define scl_check_arrindex(idx, arrsize) (((idx) >= 0) && ((idx) < (arrsize)))
#define scl_check_arrindex_unsigned(idx, arrsize) ((idx) < (arrsize))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* __SCL_DEBUG_H__ */
