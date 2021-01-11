#pragma once

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

#include <chrono>

#include <vsg/core/Visitor.h>
#include <vsg/ui/KeyEvent.h>

#include <vsgImGui/RenderImGui.h>

namespace vsgImGui
{
    class VSGIMGUI_DECLSPEC SendEventsToImGui : public vsg::Inherit<vsg::Visitor, SendEventsToImGui>
    {
    public:
        SendEventsToImGui();

        void apply(vsg::ButtonPressEvent& buttonPress) override;
        void apply(vsg::ButtonReleaseEvent& buttonRelease) override;
        void apply(vsg::MoveEvent& moveEvent) override;
        void apply(vsg::ScrollWheelEvent& scrollWheel) override;
        void apply(vsg::KeyPressEvent& keyPress) override;
        void apply(vsg::KeyReleaseEvent& keyRelease) override;
        void apply(vsg::ConfigureWindowEvent& configureWindow) override;
        void apply(vsg::FrameEvent& frame) override;

    protected:
        ~SendEventsToImGui();

        uint32_t _convertButton(uint32_t button);
        void _assignKeyMapping(uint16_t imGuiKey, vsg::KeySymbol vsgKey, vsg::KeyModifier vsgModifier = {});
        void _assignKeyMapping(uint16_t imGuiKey, vsg::KeySymbol vsgKey, vsg::KeySymbol vsgKeyAlternate, vsg::KeyModifier vsgModifier = {});
        void _initKeymap();
        uint16_t _mapToSpecialKey(const vsg::KeyEvent& keyEvent) const;

        std::chrono::high_resolution_clock::time_point t0;
        bool _dragging;

        using KeyAndModifier = std::pair<vsg::KeySymbol, vsg::KeyModifier>;
        std::map<KeyAndModifier, uint16_t> _vsgToIntermediateMap;
    };
} // namespace vsgImGui

EVSG_type_name(vsgImGui::SendEventsToImGui);
