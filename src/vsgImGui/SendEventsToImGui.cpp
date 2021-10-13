/* <editor-fold desc="MIT License">

Copyright(c) 2021 Don Burns, Roland Hill and Robert Osfield.

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#include <vsgImGui/SendEventsToImGui.h>
#include <vsgImGui/imgui.h>

#include <vsg/ui/KeyEvent.h>
#include <vsg/ui/PointerEvent.h>
#include <vsg/ui/ScrollWheelEvent.h>

#include <iostream>

using namespace vsgImGui;

SendEventsToImGui::SendEventsToImGui() :
    _dragging(false)
{
    t0 = std::chrono::high_resolution_clock::now();

    _initKeymap();
}

SendEventsToImGui::~SendEventsToImGui()
{
}

uint32_t SendEventsToImGui::_convertButton(uint32_t button)
{
    return button == 1 ? 0 : button == 3 ? 1 : button;
}

void SendEventsToImGui::_assignKeyMapping(uint16_t imGuiKey, vsg::KeySymbol vsgKey, vsg::KeyModifier vsgModifier)
{
    ImGuiIO& io = ImGui::GetIO();

    uint16_t mappedKey = 257 + _vsgToIntermediateMap.size();
    _vsgToIntermediateMap[KeyAndModifier(vsgKey, vsgModifier)] = mappedKey;

    io.KeyMap[imGuiKey] = mappedKey;
}

void SendEventsToImGui::_assignKeyMapping(uint16_t imGuiKey, vsg::KeySymbol vsgKey, vsg::KeySymbol vsgKeyAlternate, vsg::KeyModifier vsgModifier)
{
    ImGuiIO& io = ImGui::GetIO();

    uint16_t mappedKey = 257 + _vsgToIntermediateMap.size();
    _vsgToIntermediateMap[KeyAndModifier(vsgKey, vsgModifier)] = mappedKey;
    _vsgToIntermediateMap[KeyAndModifier(vsgKeyAlternate, vsgModifier)] = mappedKey;

    io.KeyMap[imGuiKey] = mappedKey;
}

void SendEventsToImGui::_initKeymap()
{
    // Keyboard mapping. ImGui will use those indices to peek into the
    // io.KeyDown[] array.
    _assignKeyMapping(ImGuiKey_Tab, vsg::KeySymbol::KEY_Tab);
    _assignKeyMapping(ImGuiKey_LeftArrow, vsg::KeySymbol::KEY_Left);
    _assignKeyMapping(ImGuiKey_RightArrow, vsg::KeySymbol::KEY_Right);
    _assignKeyMapping(ImGuiKey_UpArrow, vsg::KeySymbol::KEY_Up);
    _assignKeyMapping(ImGuiKey_DownArrow, vsg::KeySymbol::KEY_Down);
    _assignKeyMapping(ImGuiKey_PageUp, vsg::KeySymbol::KEY_Page_Up);
    _assignKeyMapping(ImGuiKey_PageDown, vsg::KeySymbol::KEY_Page_Down);
    _assignKeyMapping(ImGuiKey_Home, vsg::KeySymbol::KEY_Home);
    _assignKeyMapping(ImGuiKey_End, vsg::KeySymbol::KEY_End);
    _assignKeyMapping(ImGuiKey_Insert, vsg::KeySymbol::KEY_Insert);
    _assignKeyMapping(ImGuiKey_Delete, vsg::KeySymbol::KEY_Delete);
    _assignKeyMapping(ImGuiKey_Backspace, vsg::KeySymbol::KEY_BackSpace);
    _assignKeyMapping(ImGuiKey_Enter, vsg::KeySymbol::KEY_Return);
    _assignKeyMapping(ImGuiKey_Escape, vsg::KeySymbol::KEY_Escape);
    _assignKeyMapping(ImGuiKey_KeyPadEnter, vsg::KeySymbol::KEY_KP_Enter);
    _assignKeyMapping(ImGuiKey_A, vsg::KeySymbol::KEY_A, vsg::KeySymbol::KEY_a, vsg::KeyModifier::MODKEY_Control);
    _assignKeyMapping(ImGuiKey_C, vsg::KeySymbol::KEY_C, vsg::KeySymbol::KEY_c, vsg::KeyModifier::MODKEY_Control);
    _assignKeyMapping(ImGuiKey_V, vsg::KeySymbol::KEY_V, vsg::KeySymbol::KEY_v, vsg::KeyModifier::MODKEY_Control);
    _assignKeyMapping(ImGuiKey_X, vsg::KeySymbol::KEY_X, vsg::KeySymbol::KEY_x, vsg::KeyModifier::MODKEY_Control);
    _assignKeyMapping(ImGuiKey_Y, vsg::KeySymbol::KEY_Y, vsg::KeySymbol::KEY_y, vsg::KeyModifier::MODKEY_Control);
    _assignKeyMapping(ImGuiKey_Z, vsg::KeySymbol::KEY_Z, vsg::KeySymbol::KEY_z, vsg::KeyModifier::MODKEY_Control);
}

uint16_t SendEventsToImGui::_mapToSpecialKey(const vsg::KeyEvent& keyEvent) const
{
    KeyAndModifier keyAndModifier(keyEvent.keyModified, vsg::KeyModifier(keyEvent.keyModifier & vsg::KeyModifier::MODKEY_Control));
    auto itr = _vsgToIntermediateMap.find(keyAndModifier);
    uint16_t special_key = (itr != _vsgToIntermediateMap.end()) ? itr->second : 0;

    assert(special_key < 512 && "ImGui KeysDown is an array of 512");
    assert(special_key > 256 && "ASCII stop at 127, but we use the range [257, 511]");

    return special_key;
}

void SendEventsToImGui::apply(vsg::ButtonPressEvent& buttonPress)
{
    ImGuiIO& io = ImGui::GetIO();

    if (io.WantCaptureMouse)
    {
        uint32_t button = _convertButton(buttonPress.button);
        io.MouseDown[button] = true;
        io.MousePos.x = buttonPress.x;
        io.MousePos.y = buttonPress.y;

        buttonPress.handled = true;
    }
    else
    {
        _dragging = true;
    }
}

void SendEventsToImGui::apply(vsg::ButtonReleaseEvent& buttonRelease)
{
    ImGuiIO& io = ImGui::GetIO();
    if ((!_dragging) && io.WantCaptureMouse)
    {
        uint32_t button = _convertButton(buttonRelease.button);
        io.MouseDown[button] = false;
        io.MousePos.x = buttonRelease.x;
        io.MousePos.y = buttonRelease.y;

        buttonRelease.handled = true;
    }

    _dragging = false;
}

void SendEventsToImGui::apply(vsg::MoveEvent& moveEvent)
{
    if (!_dragging)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.MousePos.x = moveEvent.x;
        io.MousePos.y = moveEvent.y;

        moveEvent.handled = io.WantCaptureMouse;
    }
}

void SendEventsToImGui::apply(vsg::ScrollWheelEvent& scrollWheel)
{
    if (!_dragging)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.MouseWheel += scrollWheel.delta[1];
        scrollWheel.handled = io.WantCaptureMouse;
    }
}

void SendEventsToImGui::apply(vsg::KeyPressEvent& keyPress)
{
    ImGuiIO& io = ImGui::GetIO();

    if (io.WantCaptureKeyboard)
    {
        io.KeyCtrl = (keyPress.keyModifier & vsg::KeyModifier::MODKEY_Control) != 0;
        io.KeyShift = (keyPress.keyModifier & vsg::KeyModifier::MODKEY_Shift) != 0;
        io.KeyAlt = (keyPress.keyModifier & vsg::KeyModifier::MODKEY_Alt) != 0;
        io.KeySuper = (keyPress.keyModifier & vsg::KeyModifier::MODKEY_Meta) != 0;

        if (uint16_t special_key = _mapToSpecialKey(keyPress); special_key > 0)
        {
            io.KeysDown[special_key] = true;
        }
        else if (uint16_t c = keyPress.keyModified; c > 0)
        {
            if (c < 512) io.KeysDown[c] = true;
            io.AddInputCharacter((unsigned short)c);
        }

        keyPress.handled = true;
    }
}

void SendEventsToImGui::apply(vsg::KeyReleaseEvent& keyRelease)
{
    ImGuiIO& io = ImGui::GetIO();

    if (io.WantCaptureKeyboard)
    {
        io.KeyCtrl = (keyRelease.keyModifier & vsg::KeyModifier::MODKEY_Control) != 0;
        io.KeyShift = (keyRelease.keyModifier & vsg::KeyModifier::MODKEY_Shift) != 0;
        io.KeyAlt = (keyRelease.keyModifier & vsg::KeyModifier::MODKEY_Alt) != 0;
        io.KeySuper = (keyRelease.keyModifier & vsg::KeyModifier::MODKEY_Meta) != 0;

        if (uint16_t special_key = _mapToSpecialKey(keyRelease); special_key > 0)
        {
            io.KeysDown[special_key] = false;
        }
        else if (uint16_t c = keyRelease.keyModified; c > 0)
        {
            if (c < 512) io.KeysDown[c] = false;
        }

        keyRelease.handled = true;
    }
}

void SendEventsToImGui::apply(vsg::ConfigureWindowEvent& configureWindow)
{
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize.x = configureWindow.width;
    io.DisplaySize.y = configureWindow.height;
}

void SendEventsToImGui::apply(vsg::FrameEvent& /*frame*/)
{
    ImGuiIO& io = ImGui::GetIO();

    auto t1 = std::chrono::high_resolution_clock::now();
    double dt = std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t0).count();
    t0 = t1;

    io.DeltaTime = dt;
}
