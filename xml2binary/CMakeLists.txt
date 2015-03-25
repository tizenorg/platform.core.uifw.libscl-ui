CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(xml2binary CXX)

SET(SRCS
    ../xmlresource/autopopup_configure_parser.cpp
    ../xmlresource/default_configure_parser.cpp
    ../xmlresource/input_mode_configure_parser.cpp
    ../xmlresource/label_properties_parser.cpp
    ../xmlresource/layout_parser.cpp
    ../xmlresource/layout_parser_helper.cpp
    ../xmlresource/magnifier_configure_parser.cpp
    ../xmlresource/main_entry_parser.cpp
    ../xmlresource/modifier_decoration_parser.cpp
    ../xmlresource/nine_patch_file_list_parser.cpp
    ../xmlresource/xml_parser_utils.cpp
    ../xmlresource/xmlresource.cpp
    encode_metadata.cpp
    encode_input_mode_configure.cpp
    encode_layout.cpp
    encode_key_coordinate_frame.cpp
    encode_label_properties_frame.cpp
    encode_default_configure.cpp
    encode_autopopup_configure.cpp
    encode_magnifier_configure.cpp
    encode_modifier_decoration.cpp
    encode_nine_patch.cpp
    metadata_handler.cpp
    xml2dat.cpp
    ../sclres/sclres.cpp
    ../res/simple_debug.cpp
)

SET(PREFIX ${CMAKE_INSTALL_PREFIX})
SET(BINDIR "${PREFIX}/bin")
SET(METADATADIR "${PREFIX}/share/libscl-ui")

IF("${CMAKE_BUILD_TYPE}" STREQUAL "")
    SET(CMAKE_BUILD_TYPE "Release")
ENDIF()
MESSAGE(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/xml2binary/include
                    ${CMAKE_SOURCE_DIR}/xmlresource/include
                    ${CMAKE_SOURCE_DIR}/scl/include
                    ${CMAKE_SOURCE_DIR}/res
                    ${CMAKE_SOURCE_DIR}/sclres )

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs REQUIRED
        elementary
        libxml-2.0
        )

FOREACH(flag ${pkgs_CFLAGS})
    SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CFLAGS} -fpie -fPIE -Wall")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -fPIE")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -g -fPIE")

ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${pkgs_LDFLAGS})

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${BINDIR})
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/data/metadata.xml DESTINATION ${METADATADIR})
