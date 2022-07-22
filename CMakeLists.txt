cmake_minimum_required(VERSION 3.7)

project(vsgImGui
    VERSION 0.0.0
    DESCRIPTION "VulkanSceneGraph 3rd party data integration library"
    LANGUAGES CXX
)
set(VSGIMGUI_SOVERSION 0)
SET(VSGIMGUI_RELEASE_CANDIDATE 0)

set(VSGIMGUI_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE INTERNAL "Root source directory of vsgImGui")
set(VSGIMGUI_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}" CACHE INTERNAL "Root binary directory of vsgImGui")

# set the use of C++17 globally as all examples require it
set(CMAKE_CXX_STANDARD 17)

# Find Vulkan and the VSG
if (VULKAN_SDK)
    set(ENV{VULKAN_SDK} ${VULKAN_SDK})
endif()

find_package(vsg 1.0.0)

vsg_setup_dir_vars()
vsg_setup_build_vars()

if (NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/imgui/imgui.h)
    find_package(Git QUIET)

    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    RESULT_VARIABLE GIT_SUBMOD_RESULT)

    if(NOT GIT_SUBMOD_RESULT EQUAL "0")
        message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
    endif()
endif()


execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${CMAKE_CURRENT_SOURCE_DIR}/include/vsgImGui/imgui.h ${CMAKE_CURRENT_SOURCE_DIR}/src/imgui/imgui.h RESULT_VARIABLE compare_result)
if (compare_result EQUAL 0)
else()
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/imgui/imgui.h DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include/vsgImGui/)
    message("copied imgui.h")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${CMAKE_CURRENT_SOURCE_DIR}/include/vsgImGui/imconfig.h ${CMAKE_CURRENT_SOURCE_DIR}/src/imgui/imconfig.h RESULT_VARIABLE compare_result)
if (compare_result EQUAL 0)
else()
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/imgui/imconfig.h DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include/vsgImGui/)
    message("copied imconfig.h")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${CMAKE_SOURCE_DIR}/include/vsgImGui/imgui_internal.h ${CMAKE_SOURCE_DIR}/src/imgui/imgui_internal.h RESULT_VARIABLE compare_result)
if (compare_result EQUAL 0)
else()
    file(COPY ${CMAKE_SOURCE_DIR}/src/imgui/imgui_internal.h DESTINATION ${CMAKE_SOURCE_DIR}/include/vsgImGui/)
    message("copied imgui_internal.h")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${CMAKE_SOURCE_DIR}/include/vsgImGui/imstb_textedit.h ${CMAKE_SOURCE_DIR}/src/imgui/imstb_textedit.h RESULT_VARIABLE compare_result)
if (compare_result EQUAL 0)
else()
    file(COPY ${CMAKE_SOURCE_DIR}/src/imgui/imstb_textedit.h DESTINATION ${CMAKE_SOURCE_DIR}/include/vsgImGui/)
    message("copied imstb_textedit.h")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${CMAKE_SOURCE_DIR}/include/vsgImGui/imgui_stdlib.h ${CMAKE_SOURCE_DIR}/src/imgui/misc/cpp/imgui_stdlib.h RESULT_VARIABLE compare_result)
if (compare_result EQUAL 0)
else()
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/imgui/misc/cpp/imgui_stdlib.h DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include/vsgImGui/)
    message("copied imgui_stdlib.h")
endif()

vsg_add_target_clang_format(
    FILES
        include/vsgImGui/*.h
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
        PREFIX vsgImGui
        RCLEVEL VSGIMGUI_RELEASE_CANDIDATE
    )

endif()

# source directory for main vsgImGui library
add_subdirectory(src)

vsg_add_feature_summary()
