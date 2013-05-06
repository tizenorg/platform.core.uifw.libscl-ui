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

#ifndef __SCLRES_TYPE_H__
#define __SCLRES_TYPE_H__

#include "sclstructs.h"
#include "sclresource.h"

#define MAX_SCL_INPUT_MODE 128
#define MAX_SCL_LAYOUT 255
#define MAX_SCL_LABEL_PROPERTIES 128
#define MAX_SCL_MODIFIER_DECORATION_NUM 128

typedef SclInputModeConfigure (*PSclInputModeConfigure);
typedef SclLayout (*PSclLayout);

typedef SclLayoutKeyCoordinate (*PSclLayoutKeyCoordinate);
typedef SclLayoutKeyCoordinate (*PSclLayoutKeyCoordinateTable)[MAX_KEY];

typedef SclLayoutKeyCoordinate (*SclLayoutKeyCoordinatePointer);
typedef SclLayoutKeyCoordinatePointer (*PSclLayoutKeyCoordinatePointerTable)[MAX_KEY];

typedef SclModifierDecoration (*PSclModifierDecoration);

typedef SclLabelProperties (*PSclLabelProperties);
typedef SclLabelProperties (*PSclLabelPropertiesTable)[MAX_SIZE_OF_LABEL_FOR_ONE];

typedef SclDefaultConfigure (*PSclDefaultConfigure);
typedef SclAutoPopupConfigure (*PSclAutoPopupConfigure);
typedef SclMagnifierWndConfigure (*PSclMagnifierWndConfigure);

#endif

