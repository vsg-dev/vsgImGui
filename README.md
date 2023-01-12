# vsgImGui
Library that integrates VulkanSceneGraph with ImGui

## Checking out vsgImGui

    git clone https://github.com/vsg-dev/vsgImGui.git

## Building vsgImGui

The first run of cmake will automatically checkout imgui as a submodule when required.

    cd vsgImGui
    cmake .
    make -j 8

## Example

The [vsgExamples](https://github.com/vsg-dev/vsgExamples.git) repository provides the [vsgimgui](https://github.com/vsg-dev/vsgExamples/tree/master/examples/ui/vsgimgui) example.

## Recent Changes
- Fixed issue (#34) with v1.88 and assertion failures when running on Windows and Mac by:
    - Upgrading to ImGui 1.89.2.
    - Using io.AddKeyEvent within SendEventsToImGui.
    - ImGui's io.AddKeyEventAPI is better because it removes dependence on platform-specific constants as array indices to index into ImGui arrays keyboard arrays. This resulted in assertion failures for keycodes depending on platforms where vsgImGui was not fully tested.