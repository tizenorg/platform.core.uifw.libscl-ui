CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(libscl-ui CXX)

SET(SRCS
    sclres/sclres_manager.cpp
    sclres/sclres.cpp
    xmlresource/main_entry_parser.cpp
    xmlresource/input_mode_configure_parser.cpp
    xmlresource/layout_parser.cpp
    xmlresource/layout_parser_helper.cpp
    xmlresource/modifier_decoration_parser.cpp
    xmlresource/label_properties_parser.cpp
    xmlresource/default_configure_parser.cpp
    xmlresource/autopopup_configure_parser.cpp
    xmlresource/magnifier_configure_parser.cpp
    xmlresource/nine_patch_file_list_parser.cpp
    xmlresource/xmlresource.cpp
    xmlresource/xml_parser_utils.cpp
    binary_xmlresource/string_bin_parser.cpp
    binary_xmlresource/metadata_bin_parser.cpp
    binary_xmlresource/input_mode_configure_bin_parser.cpp
    binary_xmlresource/layout_bin_parser.cpp
    binary_xmlresource/key_coordinate_frame_bin_parser.cpp
    binary_xmlresource/modifier_decoration_bin_parser.cpp
    binary_xmlresource/label_properties_bin_parser.cpp
    binary_xmlresource/default_configure_bin_parser.cpp
    binary_xmlresource/autopopup_configure_bin_parser.cpp
    binary_xmlresource/magnifier_configure_bin_parser.cpp
    binary_xmlresource/nine_patch_file_list_bin_parser.cpp
    binary_xmlresource/binary_xmlresource.cpp
    binary_xmlresource/file_storage.cpp
    scl/sclerroradjustment.cpp
    scl/utils/win32/sclutils-win32.cpp
    scl/utils/linux/sclutils-linux.cpp
    scl/scldebug.cpp
    scl/sclutils.cpp
    scl/sclcontroller.cpp
    scl/sclgwes.cpp
    scl/sclevents.cpp
    scl/gwes/win32/sclgraphics-win32.cpp
    scl/gwes/efl/sclwindows-efl.cpp
    scl/gwes/efl/sclgraphics-efl.cpp
    scl/gwes/efl/sclevents-efl.cpp
    scl/gwes/win32/sclevents-win32.cpp
    scl/sclfeedback.cpp
    scl/scluibuilder.cpp
    scl/scluiimpl.cpp
    scl/sclui.cpp
    scl/scleffect.cpp
    scl/scleventhandler.cpp
    scl/sclwindows.cpp
    scl/sclgraphics.cpp
    scl/sclimageproxy.cpp
    scl/sclfontproxy.cpp
    scl/sclcontext.cpp
    scl/sclactionstate.cpp
    scl/sclresourcecache.cpp
    res/sclresource.cpp
    res/simple_debug.cpp
)

SET(PACKAGE ${PROJECT_NAME})
SET(PKGNAME ${PACKAGE})
SET(PREFIX ${CMAKE_INSTALL_PREFIX})
SET(LIBDIR "${PREFIX}/lib")
SET(INCLUDEDIR "\${PREFIX}/scl/include")

IF("${CMAKE_BUILD_TYPE}" STREQUAL "")
    SET(CMAKE_BUILD_TYPE "Release")
ENDIF()
MESSAGE(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src
                    ${CMAKE_SOURCE_DIR}/src/include
                    ${CMAKE_SOURCE_DIR}/scl
                    ${CMAKE_SOURCE_DIR}/scl/include
                    ${CMAKE_SOURCE_DIR}/xmlresource/include
                    ${CMAKE_SOURCE_DIR}/binary_xmlresource/include
                    ${CMAKE_SOURCE_DIR}/binary_xmlresource/include/interface
                    ${CMAKE_SOURCE_DIR}/sclres
                    ${CMAKE_SOURCE_DIR}/scl/gwes
                    ${CMAKE_SOURCE_DIR}/scl/gwes/gtk
                    ${CMAKE_SOURCE_DIR}/scl/gwes/win32
                    ${CMAKE_SOURCE_DIR}/scl/gwes/efl
                    ${CMAKE_SOURCE_DIR}/res )

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs REQUIRED
        elementary
        feedback
        sensor
        dlog
        ecore-imf
        libxml-2.0
        utilX
        tts
        vconf
        )

FOREACH(flag ${pkgs_CFLAGS})
    SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CFLAGS} -fPIC -Wall")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -g")

ADD_DEFINITIONS("-DPACKAGE=\"${PACKAGE}\"")
ADD_DEFINITIONS("-DPACKAGE_NAME=\"${PKGNAME}\"")
ADD_DEFINITIONS("-DLOG_TAG=\"LIBSCL_UI\"")
ADD_DEFINITIONS("-DDEFAULT_THEME=\"default\"")
ADD_DEFINITIONS("-DIMG_PATH_PREFIX=\"image\"")
ADD_DEFINITIONS("-D__EFL__")

ADD_LIBRARY(${PROJECT_NAME} SHARED ${SRCS})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${pkgs_LDFLAGS})

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${LIBDIR})

INSTALL(FILES "${CMAKE_SOURCE_DIR}/scl/include/scl.h" DESTINATION include/libscl-ui)
INSTALL(FILES "${CMAKE_SOURCE_DIR}/scl/include/sclui.h" DESTINATION include/libscl-ui)
INSTALL(FILES "${CMAKE_SOURCE_DIR}/scl/include/scltypes.h" DESTINATION include/libscl-ui)
INSTALL(FILES "${CMAKE_SOURCE_DIR}/scl/include/sclconfig.h" DESTINATION include/libscl-ui)
INSTALL(FILES "${CMAKE_SOURCE_DIR}/scl/include/sclstructs.h" DESTINATION include/libscl-ui)
INSTALL(FILES "${CMAKE_SOURCE_DIR}/scl/include/scleventcallback.h" DESTINATION include/libscl-ui)
INSTALL(FILES "${CMAKE_SOURCE_DIR}/res/simple_debug.h" DESTINATION include/libscl-ui)

CONFIGURE_FILE(${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)
INSTALL(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION lib/pkgconfig)

ADD_SUBDIRECTORY("xml2binary")
