# vsgImGui
Library that integrates VulkanSceneGraph with [Dear ImGui](https://github.com/ocornut/imgui) & [ImPlot](https://github.com/epezent/implot).

## Prerequisites

* [Vulkan SDK](https://vulkan.lunarg.com/) - required to build and run VulkanSceneGraph.
* [VulkanSceneGraph](https://github.com/vsg-dev/VulkanSceneGraph) (VSG) 1.1.13 or later.

## Installing VulkanSceneGraph

vsgImGui depends on VSG (`find_package(vsg)`), so VSG must be installed before building. If you do not have it installed, build and install it from source:

    git clone https://github.com/vsg-dev/VulkanSceneGraph.git
    cd VulkanSceneGraph
    mkdir build
    cd build
    cmake ..
    cmake --build . -j 8
    cmake --install .          # prefix with sudo on Linux/macOS if needed

The install places the CMake package config (`VSGConfig.cmake`) into `<prefix>/lib/cmake/vsg`, which is what allows `find_package(vsg)` to locate the library. If VSG is installed to a non-standard prefix and CMake cannot find it, point it at the install location when configuring vsgImGui:

    cmake .. -DVSG_DIR=/path/to/install/lib/cmake/vsg
    # or
    cmake .. -DCMAKE_PREFIX_PATH=/path/to/install

## Checking out vsgImGui

Clone the repository with its submodules (ImGui and ImPlot) checked out:

    git clone --recurse-submodules https://github.com/vsg-dev/vsgImGui.git

If the repo was already cloned without submodules, fetch them with:

    cd vsgImGui
    git submodule update --init --recursive

## Building vsgImGui

Use a separate build directory (out-of-source build) instead of configuring inside the source tree:

    cd vsgImGui
    mkdir build
    cd build
    cmake ..
    cmake --build . -j 8

## Example

The [vsgExamples](https://github.com/vsg-dev/vsgExamples.git) repository provides the [vsgimgui](https://github.com/vsg-dev/vsgExamples/tree/master/examples/ui/vsgimgui_example) example.
