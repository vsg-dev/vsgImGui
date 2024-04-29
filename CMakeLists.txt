cmake_minimum_required(VERSION 3.7)

project(vsgImGui
    VERSION 0.3.0
    DESCRIPTION "VulkanSceneGraph, ImGui and ImPlot integration library"
    LANGUAGES CXX
)
set(VSGIMGUI_SOVERSION 0)
SET(VSGIMGUI_RELEASE_CANDIDATE 0)

set(VSGIMGUI_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE INTERNAL "Root source directory of vsgImGui")
set(VSGIMGUI_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}" CACHE INTERNAL "Root binary directory of vsgImGui")

# set the use of C++17 globally as all examples require it
set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(vsgImGuiMacros)

# Find Vulkan and the VSG
if (VULKAN_SDK)
    set(ENV{VULKAN_SDK} ${VULKAN_SDK})
endif()

find_package(vsg 1.0.5)

vsg_setup_dir_vars()
vsg_setup_build_vars()

if ( (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/imgui/imgui.h) OR
     (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/implot/implot.h) )
    find_package(Git QUIET)

    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    RESULT_VARIABLE GIT_SUBMOD_RESULT)

    if(NOT GIT_SUBMOD_RESULT EQUAL "0")
        message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
    endif()
endif()

vsg_copy_imgui_headers(
    FILES
        ${VSGIMGUI_SOURCE_DIR}/src/imgui/imgui.h
        ${VSGIMGUI_SOURCE_DIR}/src/imgui/imconfig.h
        ${VSGIMGUI_SOURCE_DIR}/src/imgui/imgui_internal.h
        ${VSGIMGUI_SOURCE_DIR}/src/imgui/imstb_textedit.h
        ${VSGIMGUI_SOURCE_DIR}/src/imgui//misc/cpp/imgui_stdlib.h
        ${VSGIMGUI_SOURCE_DIR}/src/implot/implot.h
        ${VSGIMGUI_SOURCE_DIR}/src/implot/implot_internal.h
)

vsg_add_target_clang_format(
    FILES
        include/vsgImGui/RenderImGui.h
        include/vsgImGui/SendEventsToImGui.h
        include/vsgImGui/Texture.h
        src/vsgImGui/*.cpp
)
vsg_add_target_clobber()
vsg_add_target_cppcheck(
    FILES
        include/vsgImGui/*.h
        src/vsgImGui/*.cpp
)
vsg_add_target_docs(
    FILES
        include
)
vsg_add_target_uninstall()


if (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})

    vsg_add_option_maintainer(
        PREFIX v
        RCLEVEL ${VSGIMGUI_RELEASE_CANDIDATE}
    )

endif()

# source directory for main vsgImGui library
add_subdirectory(src)

vsg_add_feature_summary()
