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

        auto imguiKey = _vsgToImGuiKeyMap[keyPress.keyBase]; 
        io.AddKeyEvent(imguiKey, true);

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

        auto imguiKey = _vsgToImGuiKeyMap[keyRelease.keyBase]; 
        io.AddKeyEvent(imguiKey, false);

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

void SendEventsToImGui::_initKeymap()
{
    _vsgToImGuiKeyMap[vsg::KEY_Undefined] = ImGuiKey_None;
    _vsgToImGuiKeyMap[vsg::KEY_Space] = ImGuiKey_Space;
    _vsgToImGuiKeyMap[vsg::KEY_0] = ImGuiKey_0;
    _vsgToImGuiKeyMap[vsg::KEY_1] = ImGuiKey_1;
    _vsgToImGuiKeyMap[vsg::KEY_2] = ImGuiKey_2;
    _vsgToImGuiKeyMap[vsg::KEY_3] = ImGuiKey_3;
    _vsgToImGuiKeyMap[vsg::KEY_4] = ImGuiKey_4;
    _vsgToImGuiKeyMap[vsg::KEY_5] = ImGuiKey_5;
    _vsgToImGuiKeyMap[vsg::KEY_6] = ImGuiKey_6;
    _vsgToImGuiKeyMap[vsg::KEY_7] = ImGuiKey_7;
    _vsgToImGuiKeyMap[vsg::KEY_8] = ImGuiKey_8;
    _vsgToImGuiKeyMap[vsg::KEY_9] = ImGuiKey_9;
    _vsgToImGuiKeyMap[vsg::KEY_a] = ImGuiKey_A;
    _vsgToImGuiKeyMap[vsg::KEY_b] = ImGuiKey_B;
    _vsgToImGuiKeyMap[vsg::KEY_c] = ImGuiKey_C;
    _vsgToImGuiKeyMap[vsg::KEY_d] = ImGuiKey_D;
    _vsgToImGuiKeyMap[vsg::KEY_e] = ImGuiKey_E;
    _vsgToImGuiKeyMap[vsg::KEY_f] = ImGuiKey_F;
    _vsgToImGuiKeyMap[vsg::KEY_g] = ImGuiKey_G;
    _vsgToImGuiKeyMap[vsg::KEY_h] = ImGuiKey_H;
    _vsgToImGuiKeyMap[vsg::KEY_i] = ImGuiKey_I;
    _vsgToImGuiKeyMap[vsg::KEY_j] = ImGuiKey_J;
    _vsgToImGuiKeyMap[vsg::KEY_k] = ImGuiKey_K;
    _vsgToImGuiKeyMap[vsg::KEY_l] = ImGuiKey_L;
    _vsgToImGuiKeyMap[vsg::KEY_m] = ImGuiKey_M;
    _vsgToImGuiKeyMap[vsg::KEY_n] = ImGuiKey_N;
    _vsgToImGuiKeyMap[vsg::KEY_o] = ImGuiKey_O;
    _vsgToImGuiKeyMap[vsg::KEY_p] = ImGuiKey_P;
    _vsgToImGuiKeyMap[vsg::KEY_q] = ImGuiKey_Q;
    _vsgToImGuiKeyMap[vsg::KEY_r] = ImGuiKey_R;
    _vsgToImGuiKeyMap[vsg::KEY_s] = ImGuiKey_S;
    _vsgToImGuiKeyMap[vsg::KEY_t] = ImGuiKey_T;
    _vsgToImGuiKeyMap[vsg::KEY_u] = ImGuiKey_U;
    _vsgToImGuiKeyMap[vsg::KEY_v] = ImGuiKey_V;
    _vsgToImGuiKeyMap[vsg::KEY_w] = ImGuiKey_W;
    _vsgToImGuiKeyMap[vsg::KEY_x] = ImGuiKey_X;
    _vsgToImGuiKeyMap[vsg::KEY_y] = ImGuiKey_Y;
    _vsgToImGuiKeyMap[vsg::KEY_z] = ImGuiKey_Z;
    _vsgToImGuiKeyMap[vsg::KEY_Quote] = ImGuiKey_Apostrophe;
    _vsgToImGuiKeyMap[vsg::KEY_Leftparen] = ImGuiKey_LeftBracket;
    _vsgToImGuiKeyMap[vsg::KEY_Rightparen] = ImGuiKey_RightBracket;
    _vsgToImGuiKeyMap[vsg::KEY_Comma] = ImGuiKey_Comma;
    _vsgToImGuiKeyMap[vsg::KEY_Minus] = ImGuiKey_Minus;
    _vsgToImGuiKeyMap[vsg::KEY_Period] = ImGuiKey_Period;
    _vsgToImGuiKeyMap[vsg::KEY_Slash] = ImGuiKey_Slash;
    _vsgToImGuiKeyMap[vsg::KEY_Semicolon] = ImGuiKey_Semicolon;
    _vsgToImGuiKeyMap[vsg::KEY_Equals] = ImGuiKey_Equal;
    _vsgToImGuiKeyMap[vsg::KEY_Backslash] = ImGuiKey_Backslash;
    _vsgToImGuiKeyMap[vsg::KEY_BackSpace] = ImGuiKey_Backspace;
    _vsgToImGuiKeyMap[vsg::KEY_Tab] = ImGuiKey_Tab;
    _vsgToImGuiKeyMap[vsg::KEY_Return] = ImGuiKey_Enter;
    _vsgToImGuiKeyMap[vsg::KEY_Pause] = ImGuiKey_Pause;
    _vsgToImGuiKeyMap[vsg::KEY_Scroll_Lock] = ImGuiKey_ScrollLock;
    _vsgToImGuiKeyMap[vsg::KEY_Escape] = ImGuiKey_Escape;
    _vsgToImGuiKeyMap[vsg::KEY_Delete] = ImGuiKey_Delete;
    _vsgToImGuiKeyMap[vsg::KEY_Home] = ImGuiKey_Home;
    _vsgToImGuiKeyMap[vsg::KEY_Left] = ImGuiKey_LeftArrow;
    _vsgToImGuiKeyMap[vsg::KEY_Up] = ImGuiKey_UpArrow;
    _vsgToImGuiKeyMap[vsg::KEY_Right] = ImGuiKey_RightArrow;
    _vsgToImGuiKeyMap[vsg::KEY_Down] = ImGuiKey_DownArrow;
    _vsgToImGuiKeyMap[vsg::KEY_Page_Up] = ImGuiKey_PageUp;
    _vsgToImGuiKeyMap[vsg::KEY_Page_Down] = ImGuiKey_PageDown;
    _vsgToImGuiKeyMap[vsg::KEY_End] = ImGuiKey_End;
    _vsgToImGuiKeyMap[vsg::KEY_Print] = ImGuiKey_PrintScreen;
    _vsgToImGuiKeyMap[vsg::KEY_Insert] = ImGuiKey_Insert;
    _vsgToImGuiKeyMap[vsg::KEY_Num_Lock] = ImGuiKey_NumLock;
    _vsgToImGuiKeyMap[vsg::KEY_KP_Enter] = ImGuiKey_KeypadEnter;
    _vsgToImGuiKeyMap[vsg::KEY_KP_Equal] = ImGuiKey_KeypadEqual;
    _vsgToImGuiKeyMap[vsg::KEY_KP_Multiply] = ImGuiKey_KeypadMultiply;
    _vsgToImGuiKeyMap[vsg::KEY_KP_Add] = ImGuiKey_KeypadAdd;
    _vsgToImGuiKeyMap[vsg::KEY_KP_Subtract] = ImGuiKey_KeypadSubtract;
    _vsgToImGuiKeyMap[vsg::KEY_KP_Decimal] = ImGuiKey_KeypadDecimal;
    _vsgToImGuiKeyMap[vsg::KEY_KP_Divide] = ImGuiKey_KeypadDivide;
    _vsgToImGuiKeyMap[vsg::KEY_KP_0] = ImGuiKey_Keypad0;
    _vsgToImGuiKeyMap[vsg::KEY_KP_1] = ImGuiKey_Keypad1;
    _vsgToImGuiKeyMap[vsg::KEY_KP_2] = ImGuiKey_Keypad2;
    _vsgToImGuiKeyMap[vsg::KEY_KP_3] = ImGuiKey_Keypad3;
    _vsgToImGuiKeyMap[vsg::KEY_KP_4] = ImGuiKey_Keypad4;
    _vsgToImGuiKeyMap[vsg::KEY_KP_5] = ImGuiKey_Keypad5;
    _vsgToImGuiKeyMap[vsg::KEY_KP_6] = ImGuiKey_Keypad6;
    _vsgToImGuiKeyMap[vsg::KEY_KP_7] = ImGuiKey_Keypad7;
    _vsgToImGuiKeyMap[vsg::KEY_KP_8] = ImGuiKey_Keypad8;
    _vsgToImGuiKeyMap[vsg::KEY_KP_9] = ImGuiKey_Keypad9;
    _vsgToImGuiKeyMap[vsg::KEY_F1] = ImGuiKey_F1;
    _vsgToImGuiKeyMap[vsg::KEY_F2] = ImGuiKey_F2;
    _vsgToImGuiKeyMap[vsg::KEY_F3] = ImGuiKey_F3;
    _vsgToImGuiKeyMap[vsg::KEY_F4] = ImGuiKey_F4;
    _vsgToImGuiKeyMap[vsg::KEY_F5] = ImGuiKey_F5;
    _vsgToImGuiKeyMap[vsg::KEY_F6] = ImGuiKey_F6;
    _vsgToImGuiKeyMap[vsg::KEY_F7] = ImGuiKey_F7;
    _vsgToImGuiKeyMap[vsg::KEY_F8] = ImGuiKey_F8;
    _vsgToImGuiKeyMap[vsg::KEY_F9] = ImGuiKey_F9;
    _vsgToImGuiKeyMap[vsg::KEY_F10] = ImGuiKey_F10;
    _vsgToImGuiKeyMap[vsg::KEY_F11] = ImGuiKey_F11;
    _vsgToImGuiKeyMap[vsg::KEY_F12] = ImGuiKey_F12;
    _vsgToImGuiKeyMap[vsg::KEY_Shift_L] = ImGuiKey_LeftShift;
    _vsgToImGuiKeyMap[vsg::KEY_Shift_R] = ImGuiKey_RightShift;
    _vsgToImGuiKeyMap[vsg::KEY_Control_L] = ImGuiKey_LeftCtrl;
    _vsgToImGuiKeyMap[vsg::KEY_Control_R] = ImGuiKey_RightCtrl;
    _vsgToImGuiKeyMap[vsg::KEY_Caps_Lock] = ImGuiKey_CapsLock;
    _vsgToImGuiKeyMap[vsg::KEY_Meta_L] = ImGuiKey_Menu;
    _vsgToImGuiKeyMap[vsg::KEY_Meta_R] = ImGuiKey_Menu;
    _vsgToImGuiKeyMap[vsg::KEY_Alt_L] = ImGuiKey_LeftAlt;
    _vsgToImGuiKeyMap[vsg::KEY_Alt_R] = ImGuiKey_RightAlt;
    _vsgToImGuiKeyMap[vsg::KEY_Super_L] = ImGuiKey_LeftSuper;
    _vsgToImGuiKeyMap[vsg::KEY_Super_R] = ImGuiKey_RightSuper;
}

