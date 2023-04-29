/* <editor-fold desc="MIT License">

Copyright(c) 2023 Timothy Moore

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

</editor-fold> */

#pragma once

#include <vsg/nodes/Compilable.h>
#include <vsg/state/Sampler.h>

#include <vsgImGui/RenderImGui.h>

namespace vsgImGui
{
    /// Texture adapter that uses a DescriptorSet/DescriptorImage to hold a texture image on the GPU in a form that ImGui can use.
    class VSGIMGUI_DECLSPEC Texture : public vsg::Inherit<vsg::Compilable, Texture>
    {
    public:
        Texture(vsg::ref_ptr<vsg::Data> data = {}, vsg::ref_ptr<vsg::Sampler> sampler = {});

        void compile(vsg::Context& context) override;

        /// get the ImTextureID used with ImGui::Image(..) calls
        ImTextureID id(uint32_t deviceID) const;

        vsg::ref_ptr<vsg::DescriptorSet> descriptorSet;
        uint32_t height = 0;
        uint32_t width = 0;

    protected:
        virtual ~Texture();
    };
} // namespace vsgImGui
