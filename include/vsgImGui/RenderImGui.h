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

#include <functional>

#include <vsg/commands/ClearAttachments.h>
#include <vsg/app/Window.h>
#include <vsg/vk/DescriptorPool.h>

#include <vsgImGui/Export.h>
#include <vsgImGui/imgui.h>

namespace vsgImGui
{

    class VSGIMGUI_DECLSPEC RenderImGui : public vsg::Inherit<vsg::Command, RenderImGui>
    {
    public:
        RenderImGui(const vsg::ref_ptr<vsg::Window>& window, bool useClearAttachments = false, bool uploadFonts = true);

        template<typename... Args>
        RenderImGui(const vsg::ref_ptr<vsg::Window>& window, Args&... args) :
            RenderImGui(window, false)
        {
            (add(args), ...);
        }

        template<typename... Args>
        RenderImGui(const vsg::ref_ptr<vsg::Window>& window, Args&... args, bool useClearAttachments) :
            RenderImGui(window, useClearAttachments)
        {
            (add(args), ...);
        }

        using Component = std::function<bool()>;
        using Components = std::list<Component>;

        /// add a GUI rendering component that provides the ImGui calls to render the
        /// required GUI elements.
        void add(const Component& component);

        Components& getComponents() { return _components; }
        const Components& getComponents() const { return _components; }

        bool renderComponents() const;

        void record(vsg::CommandBuffer& commandBuffer) const override;

        void _init(const vsg::ref_ptr<vsg::Window>& window);
        void _uploadFonts();

    protected:
        virtual ~RenderImGui();

        vsg::ref_ptr<vsg::Device> _device;
        uint32_t _queueFamily;
        vsg::ref_ptr<vsg::Queue> _queue;
        vsg::ref_ptr<vsg::DescriptorPool> _descriptorPool;
        Components _components;

        vsg::ref_ptr<vsg::ClearAttachments> _clearAttachments;

    };

} // namespace vsgImGui

EVSG_type_name(vsgImGui::RenderImGui);
