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

#include "sclgraphics-efl.h"
#include "sclimageproxy.h"
#include "sclfontproxy.h"
#include "scldebug.h"
#include "sclwindows.h"
#include "sclresourcecache.h"
#include "sclwindows-efl.h"
#include "sclutils.h"

#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Elementary.h>
//#include <list>
#include <vector>

//#define EXTRACT_PARTIMAGE

#ifdef TEST_NEWBACKEND
std::vector<ImageCache> g_ImageCache;
std::vector<TextCache> g_TextCache;
sclint hash_string(const sclchar* str) {
    sclint ret = 0;
    sclint len = strlen(str);
    for(sclint loop = 0;loop < len && str[loop];loop++) {
        ret = ((loop + 1) * str[loop]);
    }

    return ret;
}
#else
#endif
using namespace scl;

int iNumCreate = 0;

extern void mouse_press (void *data, Evas *e, Evas_Object *object, void *event_info);
extern void mouse_release (void *data, Evas *e, Evas_Object *object, void *event_info);
extern void mouse_move (void *data, Evas *e, Evas_Object *object, void *event_info);

/**
 * Constructor
 */
CSCLGraphicsImplEfl::CSCLGraphicsImplEfl()
{
    SCL_DEBUG();
    /* Initializes all window resources */
}

/**
 * De-constructor
 */
CSCLGraphicsImplEfl::~CSCLGraphicsImplEfl()
{
    SCL_DEBUG();
}

void CSCLGraphicsImplEfl::init()
{
    m_highlight_ui_object = NULL;
}

void CSCLGraphicsImplEfl::fini()
{
    if (m_highlight_ui_object) {
        evas_object_del(m_highlight_ui_object);
        m_highlight_ui_object = NULL;
    }
}

Evas_Object* extract_partimage_from_fullimage(
    Evas_Object* fullimage,
    int img_x,int img_y,
    int cell_x,int cell_y,
    int cell_cx, int cell_cy)
{
    unsigned int *data;
    unsigned int *t_data;
    int i,j;
    int w,h;
    Evas_Object *image_ob;

    if (fullimage == NULL) {
        return NULL;
    }
    evas_object_image_size_get(fullimage, &w, &h);

    data = (unsigned int*)evas_object_image_data_get(fullimage,0);
    if (data == NULL) {
        return NULL;
    }

    t_data=(unsigned int*)malloc(sizeof(unsigned int)*cell_cx*cell_cy);
    if (t_data == NULL) {
        return NULL;
    }

    for (i=img_y; i<img_y+cell_cy; i++) {
        for (j=img_x; j<img_x+cell_cx; j++) {
            t_data[(i-img_y)*cell_cx+(j-img_x)] = data[i*w+j];
        }
    }

    image_ob = evas_object_image_add(evas_object_evas_get(fullimage));
    if (image_ob == NULL) {
        free(t_data);
        return NULL;
    }
    evas_object_image_size_set(image_ob,cell_cx,cell_cy);
    evas_object_image_data_set(image_ob,t_data);
    evas_object_image_fill_set(image_ob,0,0,cell_cx,cell_cy);
    evas_object_resize(image_ob,cell_cx,cell_cy);

    evas_object_show(image_ob);

    return image_ob;
}


extern sclint magnifierx, magnifiery;
void
CSCLGraphicsImplEfl::draw_image(sclwindow window, const scldrawctx draw_ctx, sclchar* image_path, SclImageCachedInfo *cachedinfo,
                                sclint dest_x, sclint dest_y, sclint dest_width, sclint dest_height,
                                sclint src_x, sclint src_y, sclint src_width, sclint src_height, sclboolean extrace_image)
{
    SCL_DEBUG();

    scl_assert_return(image_path);

    CSCLResourceCache *cache = CSCLResourceCache::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLUtils *utils = CSCLUtils::get_instance();
    SclWindowContext *window_context = NULL;
    SclWindowContext *target_window_context = NULL;
    if (windows && window) {
        //window_context = windows->get_window_context(window, FALSE);
        window_context = windows->get_window_context(window);
        //target_window_context = windows->get_window_context(draw_ctx, FALSE);
        target_window_context = windows->get_window_context(draw_ctx);
    }

    if (window_context && target_window_context && image_path && utils && cache && windows) {
        sclboolean is_highlight_ui = FALSE;
        sclchar buf[_POSIX_PATH_MAX] = {0};
        utils->get_decomposed_path(buf, IMG_PATH_PREFIX, image_path);
        if (strcmp(buf, SCL_HIGHLIGHT_UI_IMAGE) == 0) {
            is_highlight_ui = TRUE;
        }

        if (strlen(image_path) > 0) {
#ifdef DO_NOT_MOVE_MAGNIFIER_WINDOW
            if (window == windows->get_magnifier_window()) {
                dest_x += magnifierx;
                dest_y += magnifiery;
            }
#endif
#ifdef TEST_NEWBACKEND
            sclboolean bFound = FALSE;
            sclboolean bOrgSizeMinusOne = (src_width == -1 && src_height == -1);
            sclint hashval = hash_string(image_path);
            /*for(std::list<ImageCache>::iterator iter = g_ImageCache.begin();
                bFound && iter != g_ImageCache.end();std::advance(iter, 1)) {*/
            for(sclint loop = 0;loop < g_ImageCache.size() && !bFound;loop++) {
                if (
                    /*  (*iter).used &&
                        window == (*iter).window &&
                        hashval == (*iter).imgPathHash &&
                        dest_x == (*iter).dest_x &&
                        dest_y == (*iter).dest_y &&
                        dest_width == (*iter).dest_width &&
                        dest_height == (*iter).dest_height &&
                        src_x == (*iter).src_x &&
                        src_y == (*iter).src_y &&
                        src_width == (*iter).src_width &&
                        src_height == (*iter).src_height &&
                        extrace_image == (*iter).extrace_image*/
                        g_ImageCache[loop].used &&
                        window == g_ImageCache[loop].window &&
                        hashval == g_ImageCache[loop].imgPathHash &&
                        dest_x == g_ImageCache[loop].dest_x &&
                        dest_y == g_ImageCache[loop].dest_y &&
                        dest_width == g_ImageCache[loop].dest_width &&
                        dest_height == g_ImageCache[loop].dest_height &&
                        src_x == g_ImageCache[loop].src_x &&
                        src_y == g_ImageCache[loop].src_y &&
                        src_width == g_ImageCache[loop].src_width &&
                        src_height == g_ImageCache[loop].src_height &&
                        extrace_image == g_ImageCache[loop].extrace_image
                        )
                    {
                        //if (strcmp(image_path, (*iter).image_path) == 0) {
                        if (strcmp(image_path, g_ImageCache[loop].image_path) == 0) {
                            bFound = TRUE;
                            //evas_object_show((*iter).image);
                            evas_object_show(g_ImageCache[loop].image);
                            evas_object_raise(g_ImageCache[loop].image);
                            if (g_ImageCache[loop].clipper) {
                                evas_object_show(g_ImageCache[loop].clipper);
                                evas_object_raise(g_ImageCache[loop].clipper);
                            }
                        }
                    }
            }
            if (!bFound) {
#endif
            EFLObject *clip_object = NULL;

            Evas_Object *window_object = (Evas_Object*)window;
            if (window_context->is_virtual) {
                window_object = static_cast<Evas_Object*>(windows->get_base_window());
            }

            Evas *evas = evas_object_evas_get(window_object);
            Evas_Object *image_object = NULL;
            if (is_highlight_ui && m_highlight_ui_object) {
                image_object = m_highlight_ui_object;
                const SclNinePatchInfo *nine_patch_info = utils->get_nine_patch_info(image_path);
                if (nine_patch_info) {
                    evas_object_image_border_set(image_object,
                            nine_patch_info->left, nine_patch_info->right, nine_patch_info->top, nine_patch_info->bottom);
                }
                evas_object_move(image_object, dest_x, dest_y);
                evas_object_raise(image_object);
                evas_object_show(image_object);
            } else {
                EFLObject *object = new EFLObject;
                if (object) {
                    image_object = evas_object_image_add(evas);
                    object->extracted = FALSE;

                    if (image_object) {
                        int image_width = 0;
                        int image_height = 0;
                        evas_object_image_file_set(image_object, image_path, NULL);
                        evas_object_image_size_get(image_object, &image_width, &image_height);
                        if (cachedinfo) {
                            evas_object_image_border_set(image_object,
                                    cachedinfo->nine_patch_left,
                                    cachedinfo->nine_patch_right,
                                    cachedinfo->nine_patch_top,
                                    cachedinfo->nine_patch_bottom);
                        } else {
                            const SclNinePatchInfo *nine_patch_info = utils->get_nine_patch_info(image_path);
                            if (nine_patch_info) {
                                evas_object_image_border_set(image_object,
                                        nine_patch_info->left, nine_patch_info->right, nine_patch_info->top, nine_patch_info->bottom);
                            }
                        }
                        const SclLayout *layout = cache->get_cur_layout(window);
                        if (layout) {
                            if (layout->display_mode == DISPLAYMODE_PORTRAIT) {
                                image_width = utils->get_scaled_x(image_width);
                                image_height = utils->get_scaled_y(image_height);
                            } else {
                                image_width = utils->get_scaled_y(image_width);
                                image_height = utils->get_scaled_x(image_height);
                            }
                        }
                        if (src_width == -1 && src_height == -1) {
                            src_width = image_width;
                            src_height = image_height;
                        }
                        if ((src_width > 0 && src_height > 0) &&
                            (image_width != dest_width || image_height != dest_height) && extrace_image) {
    #ifdef EXTRACT_PARTIMAGE
                            Evas_Object *newobj = extract_partimage_from_fullimage(image_object, src_x, src_y, 0, 0, src_width, src_height);
                            object->extracted = TRUE;
                            evas_object_del(image_object);
                            image_object = newobj;
                            evas_object_move(image_object, dest_x, dest_y);
                            if (dest_width > 0 && dest_height > 0) {
                                evas_object_image_fill_set(image_object, 0, 0, dest_width,dest_height);
                                evas_object_resize(image_object, dest_width, dest_height);
                            }
    #else
                            //evas_object_move(image_object, src_x - dest_x, src_y - dest_y);
                            evas_object_move(image_object, dest_x - src_x, dest_y - src_y);
                            evas_object_image_fill_set(image_object, 0, 0, image_width, image_height);
                            evas_object_resize(image_object, image_width, image_height);

                            Evas_Object *clipper = evas_object_rectangle_add(evas);
                            evas_object_color_set(clipper, 255, 255, 255, 255);
                            //evas_object_color_set(clipper, 0, 0, 0, 0);
                            evas_object_move(clipper, dest_x, dest_y);
                            evas_object_resize(clipper, dest_width, dest_height);
                            evas_object_clip_set(image_object, clipper);
                            evas_object_show(clipper);

                            clip_object = new EFLObject;
                            clip_object->object = clipper;
                            clip_object->type = EFLOBJECT_CLIPOBJECT;
                            clip_object->position.x = dest_x;
                            clip_object->position.y = dest_y;
                            clip_object->position.width = dest_width;
                            clip_object->position.height = dest_height;
                            clip_object->etc_info = image_path;
                            clip_object->extracted = FALSE;
                            clip_object->data = NULL;
    #endif
                        } else {
                            evas_object_move(image_object, dest_x, dest_y);
                            if (dest_width > 0 && dest_height > 0) {
                                evas_object_image_fill_set(image_object, 0, 0, dest_width,dest_height);
                                evas_object_resize(image_object, dest_width, dest_height);
                            }
                        }
                        evas_object_raise(image_object);
                        evas_object_show(image_object);

                        //evas_object_event_callback_add((Evas_Object*)image_object, EVAS_CALLBACK_MOUSE_DOWN, mouse_press, window);
                        /*evas_object_event_callback_add((Evas_Object*)image_object, EVAS_CALLBACK_MOUSE_UP, mouse_release, window);
                        evas_object_event_callback_add((Evas_Object*)image_object, EVAS_CALLBACK_MOUSE_MOVE, mouse_move, window);*/

                        object->object = image_object;
                        object->type = EFLOBJECT_IMAGE;
                        object->position.x = dest_x;
                        object->position.y = dest_y;
                        object->position.width = dest_width;
                        object->position.height = dest_height;
                        object->etc_info = image_path;
                        object->data = clip_object;

                        if (is_highlight_ui) {
                            delete object;
                        } else {
                            target_window_context->etc_info =
                                eina_list_append((Eina_List*)(target_window_context->etc_info), object);
                            if (clip_object) {
                                target_window_context->etc_info =
                                    eina_list_append((Eina_List*)(target_window_context->etc_info), clip_object);
                            }
                        }

                        /* FIXME : this is for placing the background image at the lowest depth */
                        sclint window_layer = 29000;
                        if (!windows->is_base_window(reinterpret_cast<sclwindow>(draw_ctx))) {
                            window_layer = 29010;
                        }
                        //SclRectangle rt;
                        //windows->get_window_rect(window, &rt);
                        //if (rt.width == dest_width && rt.height == dest_height) {
                        if (window_context->geometry.width == dest_width &&
                            window_context->geometry.height == dest_height) {
                            //evas_object_lower(image_object);
                            evas_object_layer_set(image_object, window_layer + 0);
                        } else {
                            evas_object_layer_set(image_object, window_layer + 1);
                        }
                    }
                }
            }
#ifdef TEST_NEWBACKEND
                ImageCache cache;
                cache.used = true;
                cache.window = window;
                strncpy(cache.image_path, image_path, sizeof(cache.image_path));
                cache.imgPathHash = hashval;
                cache.dest_x = dest_x;
                cache.dest_y = dest_y;
                cache.dest_width = dest_width;
                cache.dest_height = dest_height;
                cache.src_x = src_x;
                cache.src_y = src_y;
                if (bOrgSizeMinusOne) {
                    cache.src_width = -1;
                    cache.src_height = -1;
                } else {
                    cache.src_width = src_width;
                    cache.src_height = src_height;
                }
                cache.extrace_image= extrace_image;
                cache.image = object->object;
                if (clip_object) {
                    cache.clipper = clip_object->object;
                } else {
                    cache.clipper = NULL;
                }

                //g_ImageCache.insert(g_ImageCache.end(), cache);
                sclboolean bInserted = FALSE;
                for(sclint loop = 0;loop < g_ImageCache.size() && !bInserted;loop++) {
                    if (!g_ImageCache[loop].used) {
                        g_ImageCache[loop] = cache;
                    }
                }
                if (!bInserted) {
                    g_ImageCache.push_back(cache);
                }
            }
#endif
        }
    }
}

sclimage
CSCLGraphicsImplEfl::load_image(const sclchar *image_path)
{
    SCL_DEBUG();
    return NULL;
}

void
CSCLGraphicsImplEfl::unload_image(sclimage image_data)
{
    SCL_DEBUG();
}

/**
 * Initializes the drawing context for double-buffering.
 * This func should be called before using a drawing primitive at first.
 */
scldrawctx
CSCLGraphicsImplEfl::begin_paint(const sclwindow window, const sclboolean force_draw /* = FALSE */)
{
    SCL_DEBUG();

    scldrawctx drawctx = reinterpret_cast<scldrawctx>(window);

    return drawctx;
}

/**
 * Notices that drawing tasks have done.
 */
void
CSCLGraphicsImplEfl::end_paint(const sclwindow window, scldrawctx draw_ctx)
{
    //Evas_Object *window_object = (Evas_Object*)window;
    //Evas *evas = evas_object_evas_get(window_object);
    //evas_render_idle_flush(evas);
}

sclfont
CSCLGraphicsImplEfl::create_font(const SclFontInfo& info)
{
    return NULL;
}

void
CSCLGraphicsImplEfl::destroy_font(sclfont font)
{
}


/**
 * Draws the given text on cairo-surface
 */
void
CSCLGraphicsImplEfl::draw_text(sclwindow window, const scldrawctx draw_ctx, const SclFontInfo& font_info, const SclColor& color,
                               const sclchar *str, SclTextCachedInfo *cachedinfo, sclint pos_x, sclint pos_y, sclint width, sclint height,
                               SCLLabelAlignment align, sclint padding_x, sclint padding_y,
                               sclint inner_width, sclint inner_height)
{
    SCL_DEBUG();

    CSCLWindows *windows = CSCLWindows::get_instance();
    SclWindowContext *window_context = NULL;
    SclWindowContext *target_window_context = NULL;

    if (windows && window) {
        //window_context = windows->get_window_context(window, FALSE);
        window_context = windows->get_window_context(window);
        //target_window_context = windows->get_window_context(draw_ctx, FALSE);
        target_window_context = windows->get_window_context(draw_ctx);
    }

    if (window_context && target_window_context && str && windows) {
        if (strlen(str) > 0) {
#ifdef DO_NOT_MOVE_MAGNIFIER_WINDOW
            if (window == windows->get_magnifier_window()) {
                pos_x += magnifierx;
                pos_y += magnifiery;
            }
#endif
#ifdef TEST_NEWBACKEND
            sclboolean bFound = FALSE;
            sclint hashval = hash_string(str);
            sclint org_posx = pos_x;
            sclint org_posy = pos_y;
            /*for(std::list<TextCache>::iterator iter = g_TextCache.begin();
                bFound && iter != g_TextCache.end();std::advance(iter, 1)) {*/
            for(sclint loop = 0;loop < g_TextCache.size() && !bFound;loop++) {
                    if (
                        /*
                        (*iter).used &&
                        window == (*iter).window &&
                        strncmp(font_info.font_name, (*iter).font_info.font_name, MAX_FONT_NAME_LEN) == 0 &&
                        font_info.font_size == (*iter).font_info.font_size &&
                        font_info.is_bold == (*iter).font_info.is_bold &&
                        font_info.is_italic == (*iter).font_info.is_italic &&
                        memcmp(&color, &((*iter).color), sizeof(SclColor)) == 0 &&
                        hashval == (*iter).strHash &&
                        pos_x == (*iter).pos_x&&
                        pos_y == (*iter).pos_y &&
                        width == (*iter).width &&
                        height == (*iter).height &&
                        align == (*iter).align &&
                        padding_x == (*iter).padding_x &&
                        padding_y == (*iter).padding_y &&
                        inner_width == (*iter).inner_width &&
                        inner_height == (*iter).inner_height */

                        g_TextCache[loop].used &&
                        window == g_TextCache[loop].window &&
                        strncmp(font_info.font_name, g_TextCache[loop].font_info.font_name, MAX_FONT_NAME_LEN) == 0 &&
                        font_info.font_size == g_TextCache[loop].font_info.font_size &&
                        font_info.is_bold == g_TextCache[loop].font_info.is_bold &&
                        font_info.is_italic == g_TextCache[loop].font_info.is_italic &&
                        memcmp(&color, &(g_TextCache[loop].color), sizeof(SclColor)) == 0 &&
                        hashval == g_TextCache[loop].strHash &&
                        pos_x == g_TextCache[loop].pos_x&&
                        pos_y == g_TextCache[loop].pos_y &&
                        width == g_TextCache[loop].width &&
                        height == g_TextCache[loop].height &&
                        align == g_TextCache[loop].align &&
                        padding_x == g_TextCache[loop].padding_x &&
                        padding_y == g_TextCache[loop].padding_y &&
                        inner_width == g_TextCache[loop].inner_width &&
                        inner_height == g_TextCache[loop].inner_height
                        )
                    {
                        //if (strcmp(str, (*iter).str) == 0) {
                        if (strcmp(str, g_TextCache[loop].str) == 0) {
                            bFound = TRUE;
                            //evas_object_show((*iter).text);
                            evas_object_show(g_TextCache[loop].text);
                            evas_object_raise(g_TextCache[loop].text);
                        }
                    }
            }
            if (!bFound) {
#endif
            EFLObject *object = new EFLObject;
            if (object) {
                object->extracted = FALSE;
                Evas_Object *window_object = (Evas_Object*)window;
                if (window_context->is_virtual) {
                    window_object = static_cast<Evas_Object*>(windows->get_base_window());
                }
                Evas *evas = evas_object_evas_get(window_object);
                Evas_Object *text_object = evas_object_textblock_add(evas);

                if (text_object) {
                    if (inner_width > 0 || inner_height > 0) {
                        SclPoint bottom_right;
                        bottom_right.x = pos_x + width;
                        bottom_right.y = pos_y + height;

                        /* The inner width and height value should be bigger than 0 */
                        if (inner_width <= 0) inner_width = width;
                        if (inner_height <= 0) inner_height = height;

                        /* The inner width and height value should not exceed the actual width and height */
                        if (inner_width > width) inner_width = width;
                        if (inner_height > height) inner_height = height;

                        /* We need to make a inner rectangle if inner_width and inner_height are not 0 */
                        if (align == LABEL_ALIGN_CENTER_TOP ||
                            align == LABEL_ALIGN_CENTER_MIDDLE ||
                            align == LABEL_ALIGN_CENTER_BOTTOM) {
                                pos_x = pos_x + ((width - inner_width) / 2) + padding_x;
                        } else if (align == LABEL_ALIGN_RIGHT_TOP ||
                            align == LABEL_ALIGN_RIGHT_MIDDLE ||
                            align == LABEL_ALIGN_RIGHT_BOTTOM) {
                                pos_x = pos_x + (width - inner_width) - padding_x;
                        } else {
                            pos_x += padding_x;
                        }
                        if (align == LABEL_ALIGN_LEFT_MIDDLE ||
                            align == LABEL_ALIGN_CENTER_MIDDLE ||
                            align == LABEL_ALIGN_RIGHT_MIDDLE) {
                                pos_y = pos_y + ((height - inner_height) / 2) + padding_y;
                        } else if (align == LABEL_ALIGN_LEFT_BOTTOM ||
                            align == LABEL_ALIGN_CENTER_BOTTOM ||
                            align == LABEL_ALIGN_RIGHT_BOTTOM) {
                                pos_y = pos_y + (height - inner_height) - padding_y;
                        } else {
                            pos_y += padding_y;
                        }

                        /* Make sure the inner bounding box does not exceed the original bounding box */
                        if (pos_x + inner_width > bottom_right.x) {
                            width = bottom_right.x - pos_x;
                        } else {
                            width = inner_width;
                        }
                        if (pos_y + inner_height > bottom_right.y) {
                            height = bottom_right.y - pos_y;
                        } else {
                            height = inner_height;
                        }

                        align = LABEL_ALIGN_CENTER_MIDDLE;
                        padding_x = 0;
                        padding_y = 0;
                    }

                    sclchar strStyle[128];
                    snprintf(strStyle, 128,
                        "DEFAULT='font=%s font_size=%d align=%s color=#%02X%02X%02X%02X wrap=word left_margin=%d right_margin=%d'",
                            font_info.font_name, font_info.font_size,
                            (((int)align % 3) == 1 ) ? "center" : ((((int)align % 3) == 2 ) ? "right" : "left"),
                            color.r, color.g, color.b, color.a, padding_x, padding_x);

                    Evas_Textblock_Style *st;
                    st = evas_textblock_style_new();
                    evas_textblock_style_set(st, strStyle);
                    evas_object_textblock_style_set(text_object, st);
                    //evas_textblock_style_free(st);

                    //evas_object_textblock_clear(text_object);
                    char *markup = evas_textblock_text_utf8_to_markup(text_object, str);
                    if (markup) {
                        evas_object_textblock_text_markup_set(text_object, markup);
                        free(markup);
                    }
                    evas_object_resize(text_object, width, height);

                    object->extracted = FALSE;
                    object->type = EFLOBJECT_TEXTBLOCK;
                    object->object = text_object;
                    object->position.x = pos_x;
                    object->position.y = pos_y;
                    object->position.width = width;
                    object->position.height = height;
                    object->etc_info = str;
                    object->data = st;

                    sclint calwidth, calheight;
                    if (cachedinfo) {
                        calwidth = cachedinfo->actual_size.width;
                        calheight = cachedinfo->actual_size.height;
                    } else {
                        evas_object_textblock_size_native_get(text_object, &calwidth, &calheight);
                    }
                    // FIXME: float to int may loose precision
                    if (calwidth > 0) {
                        static float _SPACE_RATE = 0.1;
                        calwidth  *= 1 + _SPACE_RATE;
                    }
                    if (calheight > 0) {
                        static float _SPACE_RATE = 0.1;
                        calheight  *= 1 + _SPACE_RATE;
                    }

                    if (calwidth > width || calheight > height) {
                        sclfloat width_rate = (sclfloat)width / (sclfloat)calwidth;
                        sclfloat height_rate = (sclfloat)height / (sclfloat)calheight;
                        sclfloat resize_rate = height_rate;
                        if (width_rate < height_rate) {
                            resize_rate = width_rate;
                        }

                        snprintf(strStyle, 128,
                            "DEFAULT='font=%s font_size=%d align=%s color=#%02X%02X%02X%02X wrap=word left_margin=%d right_margin=%d'",
                            font_info.font_name,
                            (int)(SCL_LABEL_OVERLENGTH_TEXT_RESIZE_RATE * font_info.font_size * resize_rate),
                            (((int)align % 3) == 1 ) ? "center" : ((((int)align % 3) == 2 ) ? "right" : "left"),
                            color.r, color.g, color.b, color.a, padding_x, padding_x);
                        evas_textblock_style_set(st, strStyle);
                        evas_object_textblock_style_set(text_object, st);
                        markup = evas_textblock_text_utf8_to_markup(text_object, str);
                        if (markup) {
                            evas_object_textblock_text_markup_set(text_object, markup);
                            free(markup);
                        }
                        evas_object_resize(text_object, width, height);
                        evas_object_textblock_size_native_get(text_object, &calwidth, &calheight);
                    }

                    /*if (align == LABEL_ALIGN_CENTER_TOP || align == LABEL_ALIGN_CENTER_MIDDLE || align == LABEL_ALIGN_CENTER_BOTTOM) {
                        pos_x = pos_x + ((width - calwidth) / 2) + padding_x;
                    } else if (align == LABEL_ALIGN_RIGHT_TOP || align == LABEL_ALIGN_RIGHT_MIDDLE || align == LABEL_ALIGN_RIGHT_BOTTOM) {
                        pos_x = pos_x + (width - calwidth) - padding_x;
                    } else {
                        pos_x += padding_x;
                    }*/
                    if (align == LABEL_ALIGN_LEFT_MIDDLE ||
                        align == LABEL_ALIGN_CENTER_MIDDLE ||
                        align == LABEL_ALIGN_RIGHT_MIDDLE) {
                            pos_y = pos_y + ((height - calheight) / 2) + padding_y;
                    } else if (align == LABEL_ALIGN_LEFT_BOTTOM ||
                        align == LABEL_ALIGN_CENTER_BOTTOM ||
                        align == LABEL_ALIGN_RIGHT_BOTTOM) {
                            pos_y = pos_y + (height - calheight) - padding_y;
                    } else {
                        pos_y += padding_y;
                    }

                    evas_object_move(text_object, pos_x, pos_y);
                    evas_object_raise(text_object);
                    evas_object_show(text_object);

                    //evas_object_event_callback_add((Evas_Object*)text_object, EVAS_CALLBACK_MOUSE_DOWN, mouse_press, window);
                    /*evas_object_event_callback_add((Evas_Object*)text_object, EVAS_CALLBACK_MOUSE_UP, mouse_release, window);
                    evas_object_event_callback_add((Evas_Object*)text_object, EVAS_CALLBACK_MOUSE_MOVE, mouse_move, window);*/

                    target_window_context->etc_info =
                        eina_list_append((Eina_List*)(target_window_context->etc_info), object);

                    sclint window_layer = 29000;
                    if (!windows->is_base_window(reinterpret_cast<sclwindow>(draw_ctx))) {
                        window_layer = 29010;
                    }
                    evas_object_layer_set(text_object, window_layer + 1);
                }
            }
#ifdef TEST_NEWBACKEND
                TextCache cache;
                cache.used = true;
                cache.window = window;
                cache.font_info = font_info;
                cache.color = color;
                strncpy(cache.font_info.font_name, font_info.font_name, MAX_FONT_NAME_LEN);
                cache.font_info.font_size = font_info.font_size;
                cache.font_info.is_bold = font_info.is_bold;
                cache.font_info.is_italic = font_info.is_italic;
                memcpy(&(cache.color), &(color), sizeof(SclColor));
                strncpy(cache.str, str, sizeof(cache.str));
                cache.strHash = hashval;
                cache.pos_x = org_posx;
                cache.pos_y = org_posy;
                cache.width = width;
                cache.height = height;
                cache.align = align;
                cache.padding_x = padding_x;
                cache.padding_y = padding_y;
                cache.inner_width = inner_width;
                cache.inner_height = inner_height;

                cache.text = object->object;

                //g_TextCache.insert(g_TextCache.end(), cache);
                sclboolean bInserted = FALSE;
                for(sclint loop = 0;loop < g_TextCache.size() && !bInserted;loop++) {
                    if (!g_TextCache[loop].used) {
                        g_TextCache[loop] = cache;
                    }
                }
                if (!bInserted) {
                    g_TextCache.push_back(cache);
                }
            }
#endif
        }
    }
}

/**
 * Draws a rectangle on cairo-surface
 */
void
CSCLGraphicsImplEfl::draw_rectangle(sclwindow window, const scldrawctx draw_ctx, scldouble pos_x, scldouble pos_y,
                                    scldouble width, scldouble height, const scldouble line_width, const SclColor& line_color, sclboolean fill, const SclColor& fill_color, scldouble radius, sclfloat alpha)
{
    SCL_DEBUG();

    CSCLResourceCache *cache = CSCLResourceCache::get_instance();
    CSCLWindows *windows = CSCLWindows::get_instance();
    CSCLUtils *utils = CSCLUtils::get_instance();
    SclWindowContext *window_context = NULL;
    SclWindowContext *target_window_context = NULL;

    if (windows && window) {
        //window_context = windows->get_window_context(window, FALSE);
        window_context = windows->get_window_context(window);
        //target_window_context = windows->get_window_context(draw_ctx, FALSE);
        target_window_context = windows->get_window_context(draw_ctx);
    }

    if (window_context && utils && cache && windows && target_window_context) {
        EFLObject *object = new EFLObject;
        if (object) {
            Evas_Object *window_object = (Evas_Object*)window;
            if (window_context->is_virtual) {
                window_object = static_cast<Evas_Object*>(windows->get_base_window());
            //    //window_context = windows->get_window_context(windows->get_base_window());
            }

            Evas *evas = evas_object_evas_get(window_object);
            Evas_Object *rectobj = evas_object_rectangle_add(evas);

            evas_object_size_hint_weight_set(rectobj, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
            elm_win_resize_object_add(window_object, rectobj);
            evas_object_color_set(rectobj, fill_color.r, fill_color.g, fill_color.b, fill_color.a);

            evas_object_move(rectobj, pos_x, pos_y);
            evas_object_resize(rectobj, width, height);

//            evas_object_raise(rectobj);
            evas_object_show(rectobj);

            object->extracted = FALSE;
            object->object = rectobj;
            object->type = EFLOBJECT_RECTANGLE;
            object->position.x = pos_x;
            object->position.y = pos_y;
            object->position.width = width;
            object->position.height = height;
            object->etc_info = NULL;
            object->data = NULL;

            target_window_context->etc_info =
                eina_list_append((Eina_List*)(target_window_context->etc_info), object);

            sclint window_layer = 29000;
            if (!windows->is_base_window(reinterpret_cast<sclwindow>(draw_ctx))) {
                window_layer = 29010;
            }
            evas_object_layer_set(rectobj, window_layer + 1);
        }
    }
}

SclSize
CSCLGraphicsImplEfl::get_image_size(sclchar* image_path)
{
    SCL_DEBUG();
    SclSize ret = { 0, 0 };

    CSCLWindows *windows = CSCLWindows::get_instance();

    Evas_Object *window_object = (Evas_Object*)(windows->get_base_window());
    Evas_Object *image_object = NULL;

    if (window_object) {
        Evas *evas = evas_object_evas_get(window_object);
        image_object = evas_object_image_add(evas);
    }

    if (image_object) {
        int w, h;
        evas_object_image_file_set(image_object, image_path, NULL);
        evas_object_image_size_get(image_object, &w, &h);
        evas_object_del(image_object);
        ret.width = w;
        ret.height = h;
    }

    return ret;
}

SclSize
CSCLGraphicsImplEfl::get_text_size(const SclFontInfo &fontinfo, const sclchar *str)
{
    SCL_DEBUG();
    SclSize ret = { 0, 0 };

    CSCLWindows *windows = CSCLWindows::get_instance();
    Evas_Object *winobj = NULL;
    Evas *evas = NULL;

    if (windows) {
        winobj = (Evas_Object*)(windows->get_base_window());
    }
    if (winobj) {
        evas = evas_object_evas_get(winobj);
    }

    int w, h;

    Evas_Textblock_Style *st;
    st = evas_textblock_style_new();

    Evas_Object *text_object = evas_object_textblock_add(evas);

    if (text_object && st) {
        const sclint STYLE_STR_LEN = 256;
        sclchar strStyle[STYLE_STR_LEN] = {0};
        snprintf(strStyle, STYLE_STR_LEN - 1, "DEFAULT='font=%s font_size=%d'",
            fontinfo.font_name, fontinfo.font_size);

        evas_textblock_style_set(st, strStyle);
        evas_object_textblock_style_set(text_object, st);

        evas_object_textblock_clear(text_object);
        char *markup = evas_textblock_text_utf8_to_markup(text_object, str);
        if (markup) {
            evas_object_textblock_text_markup_set(text_object, markup);
            free(markup);
        }

        evas_object_textblock_size_native_get(text_object, &w, &h);

        ret.width = w;
        ret.height = h;
    }
    if (text_object) {
        evas_object_del(text_object);
    }
    if (st) {
        evas_textblock_style_free(st);
    }

    return ret;
}
