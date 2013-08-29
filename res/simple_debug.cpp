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

#include "simple_debug.h"
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

SclLog::SclLog() : m_flog(NULL) {
    char *env = NULL;
    char *scllog = NULL;
    char log_path[128];

    /*
     * Use export scllog="true" to enable scllog
     */
    scllog = getenv("scllog");
    if (scllog == NULL || 0 != strcmp(scllog, "true")) {
        return;
    }

    env = getenv("HOME");
    if (env == NULL) {
        return;
    }

    snprintf(log_path, 128, "%s/scl.log", env);
    m_flog = fopen(log_path, "w+");
    if (m_flog == NULL) {
        fprintf(stderr, "Could not open file: %s", log_path);
    }
}

SclLog::~SclLog() {
    if (m_flog != NULL) {
        fclose(m_flog);
    }
}

SclLog*
SclLog::get_instance() {
    static SclLog instance;
    return &instance;
}

void
SclLog::log(enum LOG_LEVEL level, char* fmt, ...) {
    static const char *log_message[MAX_LOG_LEVEL] = {
        "message", "warning", "debug", "error"
    };

    if (m_flog == NULL) {
        return;
    }

    char str_log[128];
    va_list ap;
    va_start(ap, fmt);
    vsprintf(str_log, fmt, ap);
    va_end(ap);

    /*
     * log current time
     */
    time_t now;
    struct tm *timenow;

    time(&now);
    timenow = localtime(&now);

    fprintf(m_flog, "[ %s ] %s\n", log_message[level], asctime(timenow));
    fprintf(m_flog, "\t%s\n", str_log);

    fflush(m_flog);
}

void
SCLLOG(enum SclLog::LOG_LEVEL level, char* fmt, ...) {
    SclLog *log = SclLog::get_instance();
    if (log) {
        char str_log[128];
        va_list ap;
        va_start(ap, fmt);
        vsprintf(str_log, fmt, ap);
        va_end(ap);
        log->log(level, str_log);
    }
}

static struct timeval g_time_begin;
static struct timeval g_time_end;
void
SCLLOG_TIME_BEGIN() {
    gettimeofday(&g_time_begin, NULL);
}

void
SCLLOG_TIME_END(char *msg) {
    gettimeofday(&g_time_end, NULL);

    long diff = (g_time_end.tv_sec - g_time_begin.tv_sec) * 1000000 + g_time_end.tv_usec - g_time_begin.tv_usec;
    fprintf(stdout, "%s use %ld usec\n", msg, diff);
}
