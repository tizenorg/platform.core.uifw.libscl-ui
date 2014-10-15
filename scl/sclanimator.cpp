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

#include "sclanimator.h"
#ifdef  __WIN32__
#include "sclanimator-win32.h"
#elif defined(__EFL__)
#include "sclanimator-efl.h"
#elif __GTK__
#include "sclanimator-gtk.h"
#else
#include "sclanimator-cairo.h"
#endif
#include "scldebug.h"

#include "sclevents.h"
#include "sclwindows.h"

using namespace scl;

CSCLAnimator::CSCLAnimator()
{
    SCL_DEBUG();
    m_impl = NULL;
}

CSCLAnimator::~CSCLAnimator()
{
    SCL_DEBUG();

    if (m_impl) {
        delete m_impl;
        m_impl = NULL;
    }
}

void CSCLAnimator::init()
{
    CSCLAnimatorImpl *impl = get_scl_animator_impl();
    if (impl) {
        impl->init();
    }
}

void CSCLAnimator::fini()
{
    CSCLAnimatorImpl *impl = get_scl_animator_impl();
    if (impl) {
        impl->fini();
    }
}

sclboolean CSCLAnimator::check_animation_supported()
{
    sclboolean ret = FALSE;

    CSCLAnimatorImpl *impl = get_scl_animator_impl();
    if (impl) {
        ret = impl->check_animation_supported();
    }
    return ret;
}

CSCLAnimatorImpl* CSCLAnimator::get_scl_animator_impl()
{
    SCL_DEBUG();
    if (m_impl == 0) {
#ifdef  __WIN32__
        m_impl = new CSCLAnimatorImplWin32;
#elif defined(__EFL__)
        m_impl = new CSCLAnimatorImplEfl;
#elif __GTK__
        m_impl = new CSCLAnimatorImplGtk;
#else
        m_impl = new CSCLAnimatorImplCairo;
#endif
    }
    return m_impl;
}

CSCLAnimator* CSCLAnimator::get_instance()
{
    static CSCLAnimator instance;
    return &instance;
}

sclint
CSCLAnimator::create_animator(SclAnimationDesc *desc)
{
    static sclint animator_id = 0;

    if (desc) {
        animator_id++;
        /* Just in case for overflow */
        if (animator_id < 0) animator_id = 0;

        SclAnimationState state;
        state.active = FALSE;
        state.rect_cur = desc->rect_from;
        state.step = 0;

        state.desc = *desc;

        destroy_animator(animator_id);
        m_animators[animator_id] = state;
    } else {
        return NOT_USED;
    }

    return animator_id;
}

sclboolean
CSCLAnimator::destroy_animator(sclint id)
{
    sclboolean ret = TRUE;

    std::map<sclint, SclAnimationState>::iterator iter;
    iter = m_animators.find(id);
    if (iter != m_animators.end()) {
        SclAnimationState *state = &(iter->second);
        state->active = FALSE;
        m_animators.erase(iter->first);
    } else {
        ret = FALSE;
    }

    sclboolean destroy_timer = TRUE;
    for(iter = m_animators.begin();iter != m_animators.end();std::advance(iter, 1)) {
        if (iter != m_animators.end()) {
            SclAnimationState *state = &(iter->second);
            if (state->active) {
                destroy_timer = FALSE;
            }
        }
    }

    if (destroy_timer) {
        CSCLEvents *events = CSCLEvents::get_instance();
        if (events) {
            events->destroy_timer(SCL_TIMER_ANIMATION);
        }
    }

    return ret;
}

sclint
CSCLAnimator::find_animator_by_type(SCLAnimationType type)
{
    sclint ret = NOT_USED;

    for(std::map<sclint, SclAnimationState>::iterator iter = m_animators.begin();
        iter != m_animators.end();std::advance(iter, 1)) {
            if (iter != m_animators.end()) {
                SclAnimationState *state = &(iter->second);
                if (state->desc.type == type) {
                    ret = iter->first;
                }
            }
    }

    return ret;
}


SclAnimationState*
CSCLAnimator::get_animation_state(sclint id)
{
    SclAnimationState *ret = NULL;

    std::map<sclint, SclAnimationState>::iterator iter = m_animators.find(id);
    if (iter != m_animators.end()) {
        ret = &(iter->second);
    } else {
        ret = NULL;
    }

    return ret;
}

sclboolean
CSCLAnimator::start_animator(sclint id)
{
    sclboolean ret = TRUE;

    std::map<sclint, SclAnimationState>::iterator iter = m_animators.find(id);
    if (iter != m_animators.end()) {
        SclAnimationState *state = &(iter->second);
        state->active = TRUE;
    } else {
        ret = FALSE;
    }

    /*
    sclboolean start_timer = TRUE;
    for(std::map<sclint, SclAnimationState>::iterator iter = m_animators.begin();
        iter != m_animators.end();std::advance(iter, 1)) {
            if (iter != m_animators.end()) {
                SclAnimationState *state = &(iter->second);
                if (state->active) {
                    start_timer = FALSE;
                }
            }
    }

    if (start_timer) {
        CSCLEvents *events = CSCLEvents::get_instance();
        if (events) {
            events->create_timer(SCL_TIMER_ANIMATION, SCL_ANIMATION_TIMER_INTERVAL, 0);
        }
    }
    */
    CSCLEvents *events = CSCLEvents::get_instance();
    if (events) {
        events->destroy_timer(SCL_TIMER_ANIMATION);
        events->create_timer(SCL_TIMER_ANIMATION, SCL_ANIMATION_TIMER_INTERVAL, 0);
    }

    return ret;
}

sclboolean
CSCLAnimator::stop_animator(sclint id)
{
    sclboolean ret = TRUE;

    std::map<sclint, SclAnimationState>::iterator iter;
    iter = m_animators.find(id);
    if (iter != m_animators.end()) {
        SclAnimationState *state = &(iter->second);
        state->active = FALSE;
        m_animators.erase(iter->first);
    } else {
        ret = FALSE;
    }

    sclboolean destroy_timer = TRUE;
    for(iter = m_animators.begin();iter != m_animators.end();std::advance(iter, 1)) {
        if (iter != m_animators.end()) {
            SclAnimationState *state = &(iter->second);
            if (state->active) {
                destroy_timer = FALSE;
            }
        }
    }

    if (destroy_timer) {
        CSCLEvents *events = CSCLEvents::get_instance();
        if (events) {
            events->destroy_timer(SCL_TIMER_ANIMATION);
        }
    }

    return ret;
}

sclboolean
CSCLAnimator::animator_timer_highlight_ui(SclAnimationState *state)
{
    sclboolean ret = TRUE;

    CSCLWindows *windows = CSCLWindows::get_instance();

    if (state && windows) {
        SclRectangle rect_from = state->desc.rect_from;
        SclRectangle rect_to = state->desc.rect_to;

        sclint delta_x = 0; /* We will calculate the X considering circulation */
        sclint delta_y = rect_to.y - rect_from.y;
        sclint delta_width = rect_to.width - rect_from.width;
        sclint delta_height = rect_to.height - rect_from.height;

        state->rect_cur.y = rect_from.y +
            ((delta_y) * state->step * SCL_ANIMATION_TIMER_INTERVAL) / state->desc.length;
        state->rect_cur.width = rect_from.width +
            ((delta_width) * state->step * SCL_ANIMATION_TIMER_INTERVAL) / state->desc.length;
        state->rect_cur.height = rect_from.height +
            ((delta_height) * state->step * SCL_ANIMATION_TIMER_INTERVAL) / state->desc.length;

        if (state->desc.circular) {
            SclWindowContext *base_window_context = windows->get_window_context(windows->get_base_window());
            if (base_window_context) {
                if (rect_from.x > rect_to.x) {
                    delta_x = rect_to.x;
                    delta_x += (base_window_context->geometry.width - rect_from.x);
                } else {
                    delta_x = -(rect_from.x);
                    delta_x -= (base_window_context->geometry.width - rect_to.x);
                }

                state->rect_cur.x = rect_from.x +
                    ((delta_x) * state->step * SCL_ANIMATION_TIMER_INTERVAL) / state->desc.length;

                if (state->rect_cur.x + state->rect_cur.width <= 0) {
                    /* Make the highlight UI come out from the right side of the window */
                    state->rect_cur.x += base_window_context->geometry.width;
                } else if (state->rect_cur.x > base_window_context->geometry.width) {
                    state->rect_cur.x -= base_window_context->geometry.width;
                }
            }
        } else {
            delta_x = rect_to.x - rect_from.x;

            state->rect_cur.x = rect_from.x +
                ((delta_x) * state->step * SCL_ANIMATION_TIMER_INTERVAL) / state->desc.length;
        }
    }

    return ret;
}
sclboolean
CSCLAnimator::animator_timer()
{
    sclboolean destroy_timer = TRUE;
    for(std::map<sclint, SclAnimationState>::iterator iter = m_animators.begin();
        iter != m_animators.end();std::advance(iter, 1)) {
            if (iter != m_animators.end()) {
                CSCLWindows *windows = CSCLWindows::get_instance();
                SclAnimationState *state = &(iter->second);
                if (state && state->active && windows) {
                    state->step++;
                    if (SCL_ANIMATION_TIMER_INTERVAL * state->step >= state->desc.length) {
                        state->active = FALSE;
                    } else {
                        if (state->desc.type == ANIMATION_TYPE_HIGHLIGHT_UI) {
                            animator_timer_highlight_ui(state);
                        }
                    }

                    CSCLAnimatorImpl *impl = get_scl_animator_impl();
                    if (impl) {
                        impl->animator_timer(state);
                    }

                    if (state->active == FALSE) {
                        windows->update_window(state->desc.window_to,
                            state->desc.rect_to.x, state->desc.rect_to.y,
                            state->desc.rect_to.width, state->desc.rect_to.height);
                    } else {
                        destroy_timer = FALSE;
                    }
                }
            }
    }
    if (destroy_timer) {
        CSCLEvents *events = CSCLEvents::get_instance();
        if (events) {
            events->destroy_timer(SCL_TIMER_ANIMATION);
        }
    }
    return TRUE;
}

