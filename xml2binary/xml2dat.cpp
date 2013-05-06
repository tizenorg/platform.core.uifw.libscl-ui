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

#include <limits.h>
#include <unistd.h>
#include "xml2dat.h"
#include "string_encoder.h"
#include "metadata_handler.h"
#include "encode_layout.h"
#include "encode_input_mode_configure.h"
#include "encode_key_coordinate_frame.h"
#include "encode_label_properties_frame.h"
#include "encode_modifier_decoration.h"
#include "encode_default_configure.h"
#include "encode_magnifier_configure.h"
#include "encode_autopopup_configure.h"
#include "encode_nine_patch.h"
#include "xmlresource.h"
using namespace xmlresource;
using namespace std;

String_Encoder actual_string_encoder;
IString_Encoder& string_encoder = actual_string_encoder;

static inline void show_usage(){
    static const char* message = {  "xml2binary: missing folder operand\n"
                                    "-------------------------------------------------------\n"
                                    "|  Usage: xml2binary operand1 operand2                |\n"
                                    "|  operand1: the folder where xml text files located  |\n"
                                    "|  operand2: the folder you want to place the bin file|\n"
                                    "------------------------------------------------------- \n"};
    printf("%s", message);
}

int main(const int argc, char* argv[]) {
    if (argc < 2) {
        show_usage();
        return -1;
    }

    char* xml_text_dir = argv[1];
    if ( 0 != access(xml_text_dir, R_OK)) {
        perror(xml_text_dir);
        return -1;
    }

    char* xml_bin_dir = NULL;
    if (argc < 3) {
        xml_bin_dir = argv[1];
    } else {
        xml_bin_dir = argv[2];
    }

    if ( 0 != access(xml_bin_dir, W_OK)) {
        perror(xml_bin_dir);
        return -1;
    }

    XMLResource *xmlresource = XMLResource::get_instance();
    if (!xmlresource) {
        printf("Failed build xmlresource instance.\n");
        return -1;
    }


    xmlresource->set_resource_directory(xml_text_dir);
    xmlresource->init("main_entry.xml");

    static const char* metadata_path = "/usr/share/libscl-ui/metadata.xml";
    if ( 0 != access(metadata_path, R_OK)) {
        perror(metadata_path);
        return -1;
    }
    MetaData_Handler md_handler(metadata_path);

    char bin_file[_POSIX_PATH_MAX] = {0};
    snprintf(bin_file, _POSIX_PATH_MAX, "%s/%s", xml_bin_dir, "sclresource.bin");

    FILE *fp = fopen(bin_file, "wb");
    if (!fp) {
        perror(bin_file);
        return -1;
    }
    fclose(fp);

    enum{
        ALLDATA = 0,
        METADATA,
        INPUT_MODE_CONFIGURE,
        LAYOUT,
        KEY_COORDINATE_FRAME,
        LABEL_PROPERTIES_FRAME,
        MODIFIER_DECORATION,
        DEFAULT_CONFIGURE,
        AUTOPOPUP_CONFIGURE,
        MAGNIFIER_CONFIGURE,
        NINE_PATCH,
        STRING,
        MAX_DATATYPE
    };

    int size_table[MAX_DATATYPE] = {0};
    int offset_table[MAX_DATATYPE] = {0};
    const int SIZE_WIDTH = 8;
    const int OFFSET_WIDTH = 8;
    int offset = 0;
    {
        //Reserve for size_table
        ResourceStorage rs;
        rs.reserve(MAX_DATATYPE * (SIZE_WIDTH + OFFSET_WIDTH));
        int ret = rs.toFile(bin_file, offset);
        if (ret < 0) {
            printf("error\n");
            return -1;
        }
    }

    offset_table[METADATA] = offset;
    size_table[METADATA] = md_handler.encode(bin_file, offset);
    offset_table[INPUT_MODE_CONFIGURE] = offset;
    size_table[INPUT_MODE_CONFIGURE] = encode_input_mode_configure_file(bin_file, offset, md_handler);

    offset_table[LAYOUT] = offset;
    size_table[LAYOUT] = encode_layout_file(bin_file, offset, md_handler);

    offset_table[KEY_COORDINATE_FRAME] = offset;
    size_table[KEY_COORDINATE_FRAME] = encode_key_coordinate_frame_file(bin_file, offset, md_handler);

    offset_table[LABEL_PROPERTIES_FRAME] = offset;
    size_table[LABEL_PROPERTIES_FRAME] = encode_label_properties_frame_file(bin_file, offset, md_handler);

    offset_table[MODIFIER_DECORATION] = offset;
    size_table[MODIFIER_DECORATION] = encode_modifier_decoration_file(bin_file, offset, md_handler);

    offset_table[DEFAULT_CONFIGURE] = offset;
    size_table[DEFAULT_CONFIGURE] = encode_default_configure_file(bin_file, offset, md_handler);

    offset_table[AUTOPOPUP_CONFIGURE] = offset;
    size_table[AUTOPOPUP_CONFIGURE] = encode_autopopup_configure_file(bin_file, offset, md_handler);

    offset_table[MAGNIFIER_CONFIGURE] = offset;
    size_table[MAGNIFIER_CONFIGURE] = encode_magnifier_configure_file(bin_file, offset, md_handler);

    offset_table[NINE_PATCH] = offset;
    size_table[NINE_PATCH] = encode_nine_patch_file(bin_file, offset, md_handler);

    offset_table[STRING] = offset;
    actual_string_encoder.set_path(bin_file);
    size_table[STRING] = string_encoder.encode(offset);

    offset_table[ALLDATA] = 0;
    size_table[ALLDATA]= offset;

    {
        ResourceStorage rs;
        for (int i = 0; i < MAX_DATATYPE; ++i) {
            rs.put<sint_t>(size_table[i], SIZE_WIDTH);
            rs.put<sint_t>(offset_table[i], OFFSET_WIDTH);
        }

        int offset = 0;
        int ret = rs.toFile(bin_file, offset);
        if (ret < 0) {
            printf("error\n");
            return -1;
        }
    }

    printf("xml2binary succesfully.\n");

    return 0;
}
