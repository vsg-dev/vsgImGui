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

#include <vsgImGui/GuiEventHandler.h>
#include <vsgImGui/imgui.h>

using namespace vsgImGui;

//////////////////////////////////////////////////////////////////////////////
// Imporant Note: Dear ImGui expects the control Keys indices not to be	    //
// greater thant 511. It actually uses an array of 512 elements. However,   //
// OSG has indices greater than that. So here I do a conversion for special //
// keys between ImGui and OSG.						    //
//////////////////////////////////////////////////////////////////////////////

/**
 * Special keys that are usually greater than 512 in OSGga
 **/
enum ConvertedKey : int
{
    ConvertedKey_Tab = 257,
    ConvertedKey_Left,
    ConvertedKey_Right,
    ConvertedKey_Up,
    ConvertedKey_Down,
    ConvertedKey_PageUp,
    ConvertedKey_PageDown,
    ConvertedKey_Home,
    ConvertedKey_End,
    ConvertedKey_Delete,
    ConvertedKey_BackSpace,
    ConvertedKey_Enter,
    ConvertedKey_Escape,
    ConvertedKey_Space,
    // Modifiers
    ConvertedKey_LeftControl,
    ConvertedKey_RightControl,
    ConvertedKey_LeftShift,
    ConvertedKey_RightShift,
    ConvertedKey_LeftAlt,
    ConvertedKey_RightAlt,
    ConvertedKey_LeftSuper,
    ConvertedKey_RightSuper
};

/**
 * Check for a special key and return the converted code (range [257, 511]) if
 * so. Otherwise returns -1
 */
static int ConvertFromOSGKey(uint16_t key)
{
    using KEY = vsg::KeySymbol;

    switch (key)
    {
    default: // Not found
        return 0;
    case KEY::KEY_Tab:
        return ConvertedKey_Tab;
    case KEY::KEY_Left:
        return ConvertedKey_Left;
    case KEY::KEY_Right:
        return ConvertedKey_Right;
    case KEY::KEY_Up:
        return ConvertedKey_Up;
    case KEY::KEY_Down:
        return ConvertedKey_Down;
    case KEY::KEY_Page_Up:
        return ConvertedKey_PageUp;
    case KEY::KEY_Page_Down:
        return ConvertedKey_PageDown;
    case KEY::KEY_Home:
        return ConvertedKey_Home;
    case KEY::KEY_End:
        return ConvertedKey_End;
    case KEY::KEY_Delete:
        return ConvertedKey_Delete;
    case KEY::KEY_BackSpace:
        return ConvertedKey_BackSpace;
    case KEY::KEY_Return:
        return ConvertedKey_Enter;
    case KEY::KEY_Escape:
        return ConvertedKey_Escape;
    case KEY::KEY_Space:
        return ConvertedKey_Space;

    case KEY::KEY_Control_L:
        return ConvertedKey_LeftControl;
    case KEY::KEY_Control_R:
        return ConvertedKey_RightControl;
    case KEY::KEY_Shift_L:
        return ConvertedKey_LeftShift;
    case KEY::KEY_Shift_R:
        return ConvertedKey_RightShift;
    case KEY::KEY_Alt_L:
        return ConvertedKey_LeftAlt;
    case KEY::KEY_Alt_R:
        return ConvertedKey_RightAlt;
    case KEY::KEY_Super_L:
        return ConvertedKey_LeftSuper;
    case KEY::KEY_Super_R:
        return ConvertedKey_RightSuper;
    }
    assert(false && "Switch has a default case");
    return 0;
}

GuiEventHandler::GuiEventHandler() :
    _dragging(false)
{
    t0 = std::chrono::high_resolution_clock::now();

    initKeymap();
}

GuiEventHandler::~GuiEventHandler()
{
}

void GuiEventHandler::apply(vsg::ButtonPressEvent& buttonPress)
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

void GuiEventHandler::initKeymap()
{

    ImGuiIO& io = ImGui::GetIO();

    // Keyboard mapping. ImGui will use those indices to peek into the
    // io.KeyDown[] array.
    io.KeyMap[ImGuiKey_Tab] = ConvertedKey_Tab;
    io.KeyMap[ImGuiKey_LeftArrow] = ConvertedKey_Left;
    io.KeyMap[ImGuiKey_RightArrow] = ConvertedKey_Right;
    io.KeyMap[ImGuiKey_UpArrow] = ConvertedKey_Up;
    io.KeyMap[ImGuiKey_DownArrow] = ConvertedKey_Down;
    io.KeyMap[ImGuiKey_PageUp] = ConvertedKey_PageUp;
    io.KeyMap[ImGuiKey_PageDown] = ConvertedKey_PageDown;
    io.KeyMap[ImGuiKey_Home] = ConvertedKey_Home;
    io.KeyMap[ImGuiKey_End] = ConvertedKey_End;
    io.KeyMap[ImGuiKey_Delete] = ConvertedKey_Delete;
    io.KeyMap[ImGuiKey_Backspace] = ConvertedKey_BackSpace;
    io.KeyMap[ImGuiKey_Enter] = ConvertedKey_Enter;
    io.KeyMap[ImGuiKey_Escape] = ConvertedKey_Escape;
    io.KeyMap[ImGuiKey_Space] = ConvertedKey_Space;
    io.KeyMap[ImGuiKey_A] = vsg::KeySymbol::KEY_A;
    io.KeyMap[ImGuiKey_C] = vsg::KeySymbol::KEY_C;
    io.KeyMap[ImGuiKey_V] = vsg::KeySymbol::KEY_V;
    io.KeyMap[ImGuiKey_X] = vsg::KeySymbol::KEY_X;
    io.KeyMap[ImGuiKey_Y] = vsg::KeySymbol::KEY_Y;
    io.KeyMap[ImGuiKey_Z] = vsg::KeySymbol::KEY_Z;
}

void GuiEventHandler::apply(vsg::ButtonReleaseEvent& buttonRelease)
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

void GuiEventHandler::apply(vsg::MoveEvent& moveEvent)
{
    if (!_dragging)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.MousePos.x = moveEvent.x;
        io.MousePos.y = moveEvent.y;

        moveEvent.handled = io.WantCaptureMouse;
    }
}

void GuiEventHandler::apply(vsg::ScrollWheelEvent& scrollWheel)
{
    if (!_dragging)
    {
        ImGuiIO& io = ImGui::GetIO();
        io.MouseWheel += scrollWheel.delta[1];
        scrollWheel.handled = io.WantCaptureMouse;
    }
}

void GuiEventHandler::apply(vsg::KeyPressEvent& keyPress)
{
    ImGuiIO& io = ImGui::GetIO();

    if (io.WantCaptureKeyboard)
    {
        const uint16_t c = keyPress.keyModified;
        const uint16_t special_key = ConvertFromOSGKey(c);
        if (special_key > 0)
        {
            assert(special_key < 512 && "ImGui KeysDown is an array of 512");
            assert(special_key > 256 &&
                   "ASCII stop at 127, but we use the range [257, 511]");

            io.KeysDown[special_key] = true;

            io.KeyCtrl = io.KeysDown[ConvertedKey_LeftControl] ||
                         io.KeysDown[ConvertedKey_RightControl];
            io.KeyShift = io.KeysDown[ConvertedKey_LeftShift] ||
                          io.KeysDown[ConvertedKey_RightShift];
            io.KeyAlt = io.KeysDown[ConvertedKey_LeftAlt] ||
                        io.KeysDown[ConvertedKey_RightAlt];
            io.KeySuper = io.KeysDown[ConvertedKey_LeftSuper] ||
                          io.KeysDown[ConvertedKey_RightSuper];
        }
        else if (c > 0)
        {
            io.AddInputCharacter((unsigned short)c);
        }

        keyPress.handled = true;
    }
}

void GuiEventHandler::apply(vsg::KeyReleaseEvent& keyRelease)
{
    ImGuiIO& io = ImGui::GetIO();

    if (io.WantCaptureKeyboard)
    {
        const uint16_t special_key = ConvertFromOSGKey(keyRelease.keyBase);
        if (special_key > 0)
        {
            assert(special_key < 512 && "ImGui KeysDown is an array of 512");
            assert(special_key > 256 &&
                   "ASCII stop at 127, but we use the range [257, 511]");

            io.KeysDown[special_key] = false;

            io.KeyCtrl = io.KeysDown[ConvertedKey_LeftControl] ||
                         io.KeysDown[ConvertedKey_RightControl];
            io.KeyShift = io.KeysDown[ConvertedKey_LeftShift] ||
                          io.KeysDown[ConvertedKey_RightShift];
            io.KeyAlt = io.KeysDown[ConvertedKey_LeftAlt] ||
                        io.KeysDown[ConvertedKey_RightAlt];
            io.KeySuper = io.KeysDown[ConvertedKey_LeftSuper] ||
                          io.KeysDown[ConvertedKey_RightSuper];
        }

        keyRelease.handled = true;
    }
}

void GuiEventHandler::apply(vsg::ConfigureWindowEvent& configureWindow)
{
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize.x = configureWindow.width;
    io.DisplaySize.y = configureWindow.height;
}

void GuiEventHandler::apply(vsg::FrameEvent& /*frame*/)
{
    ImGuiIO& io = ImGui::GetIO();

    auto t1 = std::chrono::high_resolution_clock::now();
    double dt = std::chrono::duration_cast<std::chrono::duration<double>>(t1 - t0)
                    .count();
    t0 = t1;

    io.DeltaTime = dt;
}

uint32_t GuiEventHandler::_convertButton(uint32_t button)
{
    return button == 1 ? 0 : button == 3 ? 1 : button;
}
