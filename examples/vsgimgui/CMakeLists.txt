if(NOT ANDROID)
    find_package(Threads)
endif()

if (UNIX)
    find_library(DL_LIBRARY dl)
endif()

set(SOURCES
    vsgimgui.cpp
)

add_executable(vsgimgui ${SOURCES})

target_include_directories(vsgimgui PRIVATE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)

set_target_properties(vsgimgui PROPERTIES OUTPUT_NAME vsgimgui)

target_link_libraries(vsgimgui
    vsgImGui
    vsg::vsg
)

install(TARGETS vsgimgui
        RUNTIME DESTINATION bin
)
