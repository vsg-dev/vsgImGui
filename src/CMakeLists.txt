
# vars used to enable subdirectories to extend the build of the vsgImGui library in a loosely coupled way
set(EXTRA_DEFINES)
set(EXTRA_INCLUDES)

SET(HEADER_PATH ${VSGIMGUI_SOURCE_DIR}/include/vsgImGui)

set(HEADERS
    ${HEADER_PATH}/SendEventsToImGui.h
    ${HEADER_PATH}/RenderImGui.h
    ${HEADER_PATH}/Texture.h
)

set(SOURCES
    vsgImGui/RenderImGui.cpp
    vsgImGui/SendEventsToImGui.cpp
    vsgImGui/Texture.cpp
)

if(NOT VSG_IMGUI_USE_SYSTEM_IMGUI)
    set(HEADERS ${HEADERS}
        imgui/imgui.h
        imgui/imconfig.h
        imgui/imgui_internal.h
        imgui/imstb_rectpack.h
        imgui/imstb_textedit.h
        imgui/imstb_truetype.h
        imgui/misc/cpp/imgui_stdlib.h
    )

    set(SOURCES ${SOURCES}
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/backends/imgui_impl_vulkan.cpp
        imgui/misc/cpp/imgui_stdlib.cpp
    )
endif()

if(NOT VSG_IMGUI_USE_SYSTEM_IMPLOT)
    set(HEADERS ${HEADERS}
        implot/implot.h
        implot/implot_internal.h
    )

    set(SOURCES ${SOURCES}
        implot/implot.cpp
        implot/implot_items.cpp
    )
endif()

OPTION(SHOW_DEMO_WINDOW "Toggle the build of the ImGui::ShowDemoWindow(bool*) and ImPlot::ShadowDemoWindow(bool*)" ON)

if (SHOW_DEMO_WINDOW)
    if(NOT VSG_IMGUI_USE_SYSTEM_IMGUI)
        set(HEADERS ${HEADERS} imgui/imgui_demo.cpp)
    endif()

    if(NOT VSG_IMGUI_USE_SYSTEM_IMPLOT)
        set(SOURCES ${SOURCES} implot/implot_demo.cpp)
    endif()
else()
    set(SOURCES ${SOURCES}
        vsgImGui/fallback_demo.cpp
    )
endif()

add_library(vsgImGui ${HEADERS} ${SOURCES})

# add definitions to enable building vsgImGui as part of submodule
add_library(vsgImGui::vsgImGui ALIAS vsgImGui)
set(vsgImGui_FOUND TRUE CACHE INTERNAL "vsgImGui found.")
set(CMAKE_DISABLE_FIND_PACKAGE_vsgImGui TRUE CACHE INTERNAL "Disable find_package(vsgImGui) as it's not necessary.")

set(EXTRA_DEFINES "IMGUI_USER_CONFIG=<vsgImGui/Export.h>")

set_property(TARGET vsgImGui PROPERTY VERSION ${VSGIMGUI_VERSION_MAJOR}.${VSGIMGUI_VERSION_MINOR}.${VSGIMGUI_VERSION_PATCH})
set_property(TARGET vsgImGui PROPERTY SOVERSION ${VSGIMGUI_SOVERSION})
set_property(TARGET vsgImGui PROPERTY POSITION_INDEPENDENT_CODE ON)
set_property(TARGET vsgImGui PROPERTY CXX_STANDARD 17)

target_compile_definitions(vsgImGui PRIVATE ${EXTRA_DEFINES})

target_include_directories(vsgImGui PUBLIC
    $<BUILD_INTERFACE:${VSGIMGUI_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${VSGIMGUI_SOURCE_DIR}/include/vsgImGui>
    $<INSTALL_INTERFACE:include>
    ${EXTRA_INCLUDES}
)
if(NOT VSG_IMGUI_USE_SYSTEM_IMGUI)
    target_include_directories(vsgImGui PRIVATE
        $<BUILD_INTERFACE:${VSGIMGUI_SOURCE_DIR}/src/imgui/backends>
    )
endif()

target_link_libraries(vsgImGui
    PUBLIC
        vsg::vsg
    PRIVATE
        ${EXTRA_LIBRARIES}
)

if(VSG_IMGUI_USE_SYSTEM_IMGUI)
    target_link_libraries(vsgImGui PUBLIC imgui::imgui)
endif()

if(VSG_IMGUI_USE_SYSTEM_IMPLOT)
    target_link_libraries(vsgImGui PUBLIC implot::implot)
endif()

install(TARGETS vsgImGui ${INSTALL_TARGETS_DEFAULT_FLAGS})

if (BUILD_SHARED_LIBS)
    target_compile_definitions(vsgImGui INTERFACE VSGIMGUI_SHARED_LIBRARY)
endif()


install(DIRECTORY ${VSGIMGUI_SOURCE_DIR}/include/vsgImGui DESTINATION include)

vsg_add_cmake_support_files(
    CONFIG_TEMPLATE
        vsgImGuiConfig.cmake.in
)
