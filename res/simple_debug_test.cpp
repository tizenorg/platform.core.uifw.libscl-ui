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

/*
 * Use `g++ -I. simple_debug.cpp simple_debug_test.cpp` to compiler it
 */
#include <simple_debug.h>
#include <stdio.h>

int main() {
    /* Test SclLog */
    SclLog *log = SclLog::get_instance();
    if (log) {
        log->log(SclLog::WARNING, "Test %d\n", 100);
    }

    /* Test SCLLOG */
    SCLLOG(SclLog::ERROR, "%s/%d", "SCLLOG", 100);
    return 0;
}
