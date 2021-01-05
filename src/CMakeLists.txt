
# vars used to enable subdirectories to extend the build of the vsgImGui library in a loose coupled way
set(EXTRA_DEFINES)
set(EXTRA_INCLUDES)

SET(HEADER_PATH ${CMAKE_SOURCE_DIR}/include/vsgImGui)

set(HEADERS
    ${HEADER_PATH}/imgui.h
    ${HEADER_PATH}/VSGImGuiEventHandler.h
    ${HEADER_PATH}/VSGImGui.h
    imgui/imconfig.h
    imgui/imgui_internal.h
    imgui/imstb_rectpack.h
    imgui/imstb_textedit.h
    imgui/imstb_truetype.h
 )

set(SOURCES
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_vulkan.cpp
    vsgImGui/VSGImGui.cpp
    vsgImGui/VSGImGuiEventHandler.cpp
)


add_library(vsgImGui ${HEADERS} ${SOURCES})


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

install(TARGETS vsgImGui EXPORT vsgImGuiTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
)

if (BUILD_SHARED_LIBS)
    target_compile_definitions(vsgImGui INTERFACE VSGIMGUI_SHARED_LIBRARY)
endif()


install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/vsgImGui DESTINATION include)

# [==[
install(EXPORT vsgImGuiTargets
    FILE vsgImGuiTargets.cmake
    NAMESPACE vsgImGui::
    DESTINATION lib/cmake/vsgImGui
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file("${CMAKE_BINARY_DIR}/src/vsgImGuiConfigVersion.cmake"
    VERSION ${VSGIMGUI_MAJOR_VERSION}.${VSGIMGUI_MINOR_VERSION}.${VSGIMGUI_PATCH_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_file("${CMAKE_SOURCE_DIR}/src/vsgImGuiConfig.cmake.in" "${CMAKE_BINARY_DIR}/src/vsgImGuiConfig.cmake" @ONLY)

install(FILES "${CMAKE_BINARY_DIR}/src/vsgImGuiConfig.cmake" "${CMAKE_BINARY_DIR}//vsgImGuiConfigVersion.cmake"
    DESTINATION lib/cmake/vsgImGui
)

# ]==]
