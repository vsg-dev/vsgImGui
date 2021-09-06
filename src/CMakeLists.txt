
# vars used to enable subdirectories to extend the build of the vsgImGui library in a loose coupled way
set(EXTRA_DEFINES)
set(EXTRA_INCLUDES)

SET(HEADER_PATH ${CMAKE_SOURCE_DIR}/include/vsgImGui)

set(HEADERS
    ${HEADER_PATH}/imgui.h
    ${HEADER_PATH}/SendEventsToImGui.h
    ${HEADER_PATH}/RenderImGui.h
    imgui/imconfig.h
    imgui/imgui_internal.h
    imgui/imstb_rectpack.h
    imgui/imstb_textedit.h
    imgui/imstb_truetype.h
    imgui/misc/cpp/imgui_stdlib.h
)

set(SOURCES
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_vulkan.cpp
    vsgImGui/RenderImGui.cpp
    vsgImGui/SendEventsToImGui.cpp
    imgui/misc/cpp/imgui_stdlib.cpp
)


add_library(vsgImGui ${HEADERS} ${SOURCES})

set(EXTRA_DEFINES "IMGUI_USER_CONFIG=<vsgImGui/Export.h>")

set_property(TARGET vsgImGui PROPERTY VERSION ${VSGIMGUI_VERSION_MAJOR}.${VSGIMGUI_VERSION_MINOR}.${VSGIMGUI_VERSION_PATCH})
set_property(TARGET vsgImGui PROPERTY SOVERSION ${VSGIMGUI_SOVERSION})
set_property(TARGET vsgImGui PROPERTY POSITION_INDEPENDENT_CODE ON)
set_property(TARGET vsgImGui PROPERTY CXX_STANDARD 17)

target_compile_definitions(vsgImGui PRIVATE ${EXTRA_DEFINES})

target_include_directories(vsgImGui PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/vsgImGui>
    $<INSTALL_INTERFACE:include>
    ${EXTRA_INCLUDES}
)

target_link_libraries(vsgImGui
    PUBLIC
        vsg::vsg
    PRIVATE
        ${EXTRA_LIBRARIES}
)

install(TARGETS vsgImGui ${INSTALL_TARGETS_DEFAULT_FLAGS})

if (BUILD_SHARED_LIBS)
    target_compile_definitions(vsgImGui INTERFACE VSGIMGUI_SHARED_LIBRARY)
endif()


install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/vsgImGui DESTINATION include)

vsg_add_cmake_support_files(
    CONFIG_TEMPLATE
        vsgImGuiConfig.cmake.in
)
