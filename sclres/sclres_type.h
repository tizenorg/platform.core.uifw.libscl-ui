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

#ifndef __SCLRES_TYPE_H__
#define __SCLRES_TYPE_H__

#include "sclstructs.h"
#include "sclresource.h"

#define MAX_SCL_INPUT_MODE 64
#define MAX_SCL_LAYOUT 128
#define MAX_SCL_LABEL_PROPERTIES 128
#define MAX_SCL_MODIFIER_DECORATION_NUM 64

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

