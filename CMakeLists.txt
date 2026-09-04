cmake_minimum_required(VERSION 3.10)

project(vsgImGui
    VERSION 0.8.0
    DESCRIPTION "VulkanSceneGraph, ImGui and ImPlot integration library"
    LANGUAGES CXX
)

option(VSG_IMGUI_USE_SYSTEM_IMGUI "Use system installed ImGui" OFF)
option(VSG_IMGUI_USE_SYSTEM_IMPLOT "Use system installed ImPlot" OFF)

if(VSG_IMGUI_USE_SYSTEM_IMPLOT AND NOT VSG_IMGUI_USE_SYSTEM_IMGUI)
    message(FATAL_ERROR "System ImPlot requires system ImGui")
endif()

set(VSGIMGUI_SOVERSION 0)
SET(VSGIMGUI_RELEASE_CANDIDATE 0)

set(VSGIMGUI_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE INTERNAL "Root source directory of vsgImGui")
set(VSGIMGUI_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}" CACHE INTERNAL "Root binary directory of vsgImGui")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(vsgImGuiMacros)

# Find Vulkan and the VSG
if (VULKAN_SDK)
    set(ENV{VULKAN_SDK} ${VULKAN_SDK})
endif()

find_package(vsg 1.1.13)

vsg_setup_dir_vars()
vsg_setup_build_vars()

set(VSG_IMGUI_NEEDS_SUBMODULES OFF)
if(NOT VSG_IMGUI_USE_SYSTEM_IMGUI AND NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/imgui/imgui.h)
    set(VSG_IMGUI_NEEDS_SUBMODULES ON)
endif()
if(NOT VSG_IMGUI_USE_SYSTEM_IMPLOT AND NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/implot/implot.h)
    set(VSG_IMGUI_NEEDS_SUBMODULES ON)
endif()

if(VSG_IMGUI_NEEDS_SUBMODULES)
    find_package(Git REQUIRED)
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    RESULT_VARIABLE GIT_SUBMOD_RESULT)

    if(NOT GIT_SUBMOD_RESULT EQUAL "0")
        message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
    endif()
endif()

if(VSG_IMGUI_USE_SYSTEM_IMGUI)
    find_package(imgui CONFIG REQUIRED)
    vsg_create_dependency_header(imgui.h imgui.h)
else()
    vsg_copy_imgui_headers(
        FILES
            ${VSGIMGUI_SOURCE_DIR}/src/imgui/imgui.h
            ${VSGIMGUI_SOURCE_DIR}/src/imgui/imconfig.h
            ${VSGIMGUI_SOURCE_DIR}/src/imgui/imgui_internal.h
            ${VSGIMGUI_SOURCE_DIR}/src/imgui/imstb_textedit.h
            ${VSGIMGUI_SOURCE_DIR}/src/imgui//misc/cpp/imgui_stdlib.h
    )
endif()

if(VSG_IMGUI_USE_SYSTEM_IMPLOT)
    find_package(implot CONFIG REQUIRED)
    vsg_create_dependency_header(implot.h implot.h)
else()
    vsg_copy_imgui_headers(
        FILES
            ${VSGIMGUI_SOURCE_DIR}/src/implot/implot.h
            ${VSGIMGUI_SOURCE_DIR}/src/implot/implot_internal.h
    )
endif()

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
