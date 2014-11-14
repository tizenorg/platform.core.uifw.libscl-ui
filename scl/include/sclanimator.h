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

#include "scltypes.h"
#include "sclconfig.h"
#include "sclstructs.h"

#include <map>

#ifndef __SCL_ANIMATOR_H__
#define __SCL_ANIMATOR_H__

#ifdef __cplusplus
//SCL_BEGIN_DECLS
#endif

namespace scl
{

/* Currently this SCL Animation class is not designed for specific animtions, not for general use.
   Should refine later to provide general animation behavior */
typedef enum {
    ANIMATION_TYPE_NONE,
    ANIMATION_TYPE_HIGHLIGHT_UI,

    ANIMATION_TYPE_MAX
}SCLAnimationType;

typedef struct {
    SCLAnimationType type;
    sclint length;

    sclwindow window_from;
    sclwindow window_to;

    SclRectangle rect_from;
    SclRectangle rect_to;

    sclboolean circular;
}SclAnimationDesc;

typedef struct {
    sclboolean active;

    SclAnimationDesc desc;
    sclint step;

    SclRectangle rect_cur;
}SclAnimationState;


class CSCLAnimatorImpl
{
    virtual void init() = 0;
    virtual void fini() = 0;

    /* By default, animation is not supported */
    virtual sclboolean check_animation_supported() { return FALSE; }
    virtual void animator_timer(SclAnimationState *state) = 0;

    friend class CSCLAnimator;
private:

};

class CSCLAnimator
{
private:
    CSCLAnimator();
public :
    ~CSCLAnimator();

    static CSCLAnimator* get_instance();

    void init();
    void fini();

    /* Some backends may not provide animation feature */
    sclboolean check_animation_supported();

    sclint create_animator(SclAnimationDesc *desc);
    sclboolean destroy_animator(sclint id);

    sclint find_animator_by_type(SCLAnimationType type);

    SclAnimationState* get_animation_state(sclint id);

    sclboolean start_animator(sclint id);
    sclboolean stop_animator(sclint id);

    sclboolean animator_timer();

protected :
    CSCLAnimatorImpl* get_scl_animator_impl();

    sclboolean animator_timer_highlight_ui(SclAnimationState *state);
private :
    CSCLAnimatorImpl* m_impl;

    std::map<sclint, SclAnimationState> m_animators;
};


} /* End of scl namespace */

#ifdef __cplusplus
//SCL_END_DECLS
#endif

#endif //__SCL_ANIMATOR_H__
