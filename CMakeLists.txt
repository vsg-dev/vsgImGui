cmake_minimum_required(VERSION 3.7)

project(vsgImGui
    VERSION 0.0.0
    DESCRIPTION "VulkanSceneGraph 3rd party data integration library"
    LANGUAGES CXX
)
set(VSGIMGUI_SOVERSION 0)
SET(VSGIMGUI_RELEASE_CANDIDATE 0)

# set the use of C++17 globally as all examples require it
set(CMAKE_CXX_STANDARD 17)

# Find Vulkan and the VSG
if (VULKAN_SDK)
    set(ENV{VULKAN_SDK} ${VULKAN_SDK})
endif()

find_package(vsg 0.1.6 REQUIRED)

# find the optional vsgXchange that can be used for reading and range of image and 3d model formats and shader compilation
find_package(vsgXchange QUIET)

if (NOT EXISTS ${CMAKE_SOURCE_DIR}/src/imgui/imgui.h)
    find_package(Git QUIET)

    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    RESULT_VARIABLE GIT_SUBMOD_RESULT)

    if(NOT GIT_SUBMOD_RESULT EQUAL "0")
        message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
    endif()
endif()


execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${CMAKE_SOURCE_DIR}/include/vsgImGui/imgui.h ${CMAKE_SOURCE_DIR}/src/imgui/imgui.h RESULT_VARIABLE compare_result)
if (compare_result EQUAL 0)
else()
    file(COPY ${CMAKE_SOURCE_DIR}/src/imgui/imgui.h DESTINATION ${CMAKE_SOURCE_DIR}/include/vsgImGui/)
    message("copied imgui.h")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${CMAKE_SOURCE_DIR}/include/vsgImGui/imconfig.h ${CMAKE_SOURCE_DIR}/src/imgui/imconfig.h RESULT_VARIABLE compare_result)
if (compare_result EQUAL 0)
else()
    file(COPY ${CMAKE_SOURCE_DIR}/src/imgui/imconfig.h DESTINATION ${CMAKE_SOURCE_DIR}/include/vsgImGui/)
    message("copied imconfig.h")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${CMAKE_SOURCE_DIR}/include/vsgImGui/imgui_stdlib.h ${CMAKE_SOURCE_DIR}/src/imgui/misc/cpp/imgui_stdlib.h RESULT_VARIABLE compare_result)
if (compare_result EQUAL 0)
else()
    file(COPY ${CMAKE_SOURCE_DIR}/src/imgui/misc/cpp/imgui_stdlib.h DESTINATION ${CMAKE_SOURCE_DIR}/include/vsgImGui/)
    message("copied imgui_stdlib.h")
endif()

vsg_setup_build_vars()
vsg_setup_dir_vars()

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

vsg_add_option_maintainer(
    PREFIX vsgImGui
    RCLEVEL VSGIMGUI_RELEASE_CANDIDATE
)

# source directory for main vsgXchange library
add_subdirectory(src)

vsg_add_feature_summary()
