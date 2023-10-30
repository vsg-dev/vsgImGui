# vsgImGui
Library that integrates VulkanSceneGraph with [Dear ImGui](https://github.com/ocornut/imgui) & [ImPlot](https://github.com/epezent/implot).

## Checking out vsgImGui

    git clone https://github.com/vsg-dev/vsgImGui.git

## Building vsgImGui

The first run of cmake will automatically checkout ImGui and ImPlot as submodules when required.

    cd vsgImGui
    cmake .
    make -j 8

## Example

The [vsgExamples](https://github.com/vsg-dev/vsgExamples.git) repository provides the [vsgimgui](https://github.com/vsg-dev/vsgExamples/tree/master/examples/ui/vsgimgui_example) example.
