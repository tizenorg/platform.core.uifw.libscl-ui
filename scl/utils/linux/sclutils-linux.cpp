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

#include "sclutils.h"
#include "scldebug.h"
#include "sclcontext.h"

#include <stdarg.h>

#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_Evas.h>
#include <feedback.h>
#include <Elementary.h>
#include <tts.h>

#include <dlog.h>
#ifndef LOG_TAG
#define LOG_TAG "LIBSCL_UI"
#endif

using namespace scl;

static tts_h tts = NULL;
static Eina_Bool _get_default_zone_geometry_info (Ecore_X_Window root, scluint *x, scluint *y, scluint *w, scluint *h)
{
    Ecore_X_Atom zone_geometry_atom;
    Ecore_X_Window default_zone;
    Ecore_X_Window* zone_lists;
    sclint num_zone_lists;
    sclint num_ret;
    Eina_Bool ret;

    zone_geometry_atom = ecore_x_atom_get("_E_ILLUME_ZONE_GEOMETRY");
    if (!zone_geometry_atom) {
        /* Error... */
        return EINA_FALSE;
    }

    scluint geom[4];
    num_zone_lists = ecore_x_window_prop_window_list_get(root, ECORE_X_ATOM_E_ILLUME_ZONE_LIST, &zone_lists);
    if (num_zone_lists > 0) {
        num_ret = ecore_x_window_prop_card32_get(zone_lists[0], zone_geometry_atom, geom, 4);
        if (num_ret == 4) {
            if (x) *x = geom[0];
            if (y) *y = geom[1];
            if (w) *w = geom[2];
            if (h) *h = geom[3];
            ret = EINA_TRUE;
        } else {
            ret = EINA_FALSE;
        }
    } else {
        /* if there is no zone available */
        ret = EINA_FALSE;
    }

    if (zone_lists) {
        /* We must free zone_lists */
        free (zone_lists);
    }

    return ret;
}

/* FIXME : Currently the screen resolution is locally cached, should be updated when it gets changed */
sclboolean
CSCLUtilsImplLinux::get_screen_resolution(sclint *x, sclint *y) {
    SCL_DEBUG();
    //*x = gdk_screen_get_width(gdk_screen_get_default());
    //*y = gdk_screen_get_height(gdk_screen_get_default());
    CSCLContext *context = CSCLContext::get_instance();

    static Evas_Coord scr_w = 0, scr_h = 0;
    if (scr_w == 0 || scr_h == 0) {
        scluint w, h;
        if (_get_default_zone_geometry_info(ecore_x_window_root_first_get(), NULL, NULL, &w, &h)) {
            scr_w = w;
            scr_h = h;
        } else {
            ecore_x_window_size_get(ecore_x_window_root_first_get(), &scr_w, &scr_h);
        }
    }
    if (context && x && y) {
        if (context->get_display_mode() == DISPLAYMODE_LANDSCAPE) {
            *x = scr_h;
            *y = scr_w;
        } else {
            *x = scr_w;
            *y = scr_h;
        }
    }
    return TRUE;
}

sclboolean
CSCLUtilsImplLinux::play_tts(const sclchar* str) {
    SCL_DEBUG();

    if (str) {
        int r;
        int utt_id = 0;
        tts_state_e current_state;

        r = tts_get_state(tts, &current_state);
        if (TTS_ERROR_NONE != r) {
            LOGD("Fail to get state from TTS : ret(%d)\n", r);
        }

        if (TTS_STATE_PLAYING == current_state)  {
            r = tts_stop(tts);
            if (TTS_ERROR_NONE != r) {
                LOGD("Fail to stop TTS : ret(%d)\n", r);
            }
        }
        r = tts_add_text(tts, str, "en_US", TTS_VOICE_TYPE_FEMALE, TTS_SPEED_NORMAL, &utt_id);
        if (TTS_ERROR_NONE == r) {
            r = tts_play(tts);
            if (TTS_ERROR_NONE != r) {
                LOGD("Fail to play TTS : ret(%d)\n", r);
            }
        } else {
            LOGD("Fail to add text : ret(%d)\n", r);
        }
    }

    return TRUE;
}

sclboolean
CSCLUtilsImplLinux::play_sound(const sclchar* snd_style) {
    SCL_DEBUG();

    typedef struct {
        const char *name;
        feedback_pattern_e type_value;
    } SOUND_TYPES;

    static const SOUND_TYPES internal_types[] = {
        {"Backspace", FEEDBACK_PATTERN_SIP_BACKSPACE},
    };
    static const sclint SOUND_TYPES_NUM = (sizeof(internal_types) / sizeof(SOUND_TYPES));

    sclboolean ret = FALSE;
    int r;

    sclboolean found = FALSE;
    sclint loop = 0;
    for (loop = 0; loop < SOUND_TYPES_NUM;loop++) {
        if (strncmp(snd_style, internal_types[loop].name, strlen(internal_types[loop].name)) == 0) {
            found = TRUE;
            r = feedback_play_type(FEEDBACK_TYPE_SOUND, internal_types[loop].type_value);
        }
    }
    if (!found) {
        r = feedback_play_type(FEEDBACK_TYPE_SOUND, FEEDBACK_PATTERN_SIP);
    }

    if (r != FEEDBACK_ERROR_NONE) {
        LOGD("Cannot play sound : %d\n", r);
    } else {
        ret = TRUE;
    }

    return ret;
}

sclboolean
CSCLUtilsImplLinux::play_vibration(const sclchar* vibe_style, const scllong duration) {
    SCL_DEBUG();

    typedef struct {
        const char *name;
        feedback_pattern_e type_value;
    } VIBRATION_TYPES;

    static const VIBRATION_TYPES internal_types[] = {
        {"Backspace", FEEDBACK_PATTERN_SIP_BACKSPACE},
    };
    static const sclint VIBRATION_TYPES_NUM = (sizeof(internal_types) / sizeof(VIBRATION_TYPES));

    sclboolean ret = FALSE;
    int r;

    sclboolean found = FALSE;
    sclint loop = 0;
    for (loop = 0; loop < VIBRATION_TYPES_NUM;loop++) {
        if (strncmp(vibe_style, internal_types[loop].name, strlen(internal_types[loop].name)) == 0) {
            found = TRUE;
            r = feedback_play_type(FEEDBACK_TYPE_VIBRATION, internal_types[loop].type_value);
        }
    }
    if (!found) {
        r = feedback_play_type(FEEDBACK_TYPE_VIBRATION, FEEDBACK_PATTERN_SIP);
    }

    if (r != FEEDBACK_ERROR_NONE) {
        LOGD("Cannot play vibration : %d\n", r);
    } else {
        ret = TRUE;
    }

    return ret;
}

sclboolean
CSCLUtilsImplLinux::open_devices() {
    SCL_DEBUG();

    int r = feedback_initialize(); // Initialize feedback

    if (r != FEEDBACK_ERROR_NONE) {
        LOGD("FEEDBACK INITIALIZATION FAILED : %d\n", r);
    } else {
        LOGD("FEEDBACK INITIALIZATION SUCCESSFUL : %d\n", r);
    }

    r = tts_create(&tts);
    if (TTS_ERROR_NONE != r) {
        LOGD("tts_create FAILED : result(%d)", r);
    } else {
        tts_set_mode (tts, TTS_MODE_SCREEN_READER);
    }
    if (TTS_ERROR_NONE != r) {
        LOGD("tts_set_mode FAILED : result(%d)", r);
    }

    tts_state_e current_state;
    tts_get_state(tts, &current_state);

    if (TTS_STATE_CREATED == current_state)  {
        r = tts_prepare(tts);
    }
    if (TTS_ERROR_NONE != r) {
        LOGD("tts_prepare FAILED : ret(%d)\n", r);
    }

    return TRUE;
}

sclboolean
CSCLUtilsImplLinux::close_devices() {
    SCL_DEBUG();

    int r = feedback_deinitialize(); // Deinitialize feedback

    if (r != FEEDBACK_ERROR_NONE) {
        LOGD("FEEDBACK DEINITIALIZATION FAILED : %d\n", r);
    } else {
        LOGD("FEEDBACK DEINITIALIZATION SUCCESSFUL : %d\n", r);
    }

    r = tts_unprepare(tts);
    if (TTS_ERROR_NONE != r) {
        LOGD("tts_unprepare FAILED : result(%d)", r);
    }

    r = tts_destroy(tts);
    if (TTS_ERROR_NONE != r) {
        LOGD("tts_destroy FAILED : result(%d)", r);
    }
    return TRUE;
}

sclint CSCLUtilsImplLinux::log(const sclchar *msg)
{
    SCL_DEBUG();
    sclint ret = 0;

    LOGD("%s", msg);

    return ret;
}

/*
sclboolean
CSCLUtilsImplLinux::find_first_file(FILEFINDINFO &sFileFindInfo, const sclchar *szSuffix, const sclchar *szDirPath)
{
    sclboolean bSuccess = false;

    sFileFindInfo.Uninitialize();

    sFileFindInfo.pDirInfo = opendir(szDirPath);
    if (sFileFindInfo.pDirInfo) {
        sclboolean bEnd = false;
        while (!bEnd) {
            struct dirent *pDirEnt = readdir(sFileFindInfo.pDirInfo);
            if (NULL == pDirEnt) {
                bEnd = true;
            } else {
                if (szSuffix) {
                    if (strlen(szSuffix) <= strlen(pDirEnt->d_name)) {
                        if (strcmp(pDirEnt->d_name + strlen(pDirEnt->d_name) - strlen(szSuffix), szSuffix) == 0) {
                            bSuccess = true;
                            sFileFindInfo.pDirEnt = pDirEnt;
                            bEnd = true;
                        }
                    }
                } else {
                    bSuccess = true;
                    sFileFindInfo.pDirEnt = pDirEnt;
                    bEnd = true;
                }
            }
        }
    }

    return bSuccess;
}

sclboolean
CSCLUtilsImplLinux::find_next_file(FILEFINDINFO &sFileFindInfo, const sclchar *szSuffix)
{
    sclboolean bSuccess = false;

    if (sFileFindInfo.pDirInfo) {
        sclboolean bEnd = false;
        while (!bEnd) {
            struct dirent *pDirEnt = readdir(sFileFindInfo.pDirInfo);
            if (NULL == pDirEnt) {
                bEnd = true;
            } else {
                if (szSuffix) {
                    if (strlen(szSuffix) <= strlen(pDirEnt->d_name)) {
                        if (strcmp(pDirEnt->d_name + strlen(pDirEnt->d_name) - strlen(szSuffix), szSuffix) == 0) {
                            bSuccess = true;
                            sFileFindInfo.pDirEnt = pDirEnt;
                            bEnd = true;
                        }
                    }
                } else {
                    bSuccess = true;
                    sFileFindInfo.pDirEnt = pDirEnt;
                    bEnd = true;
                }
            }
        }
    }

    return bSuccess;
}
*/
