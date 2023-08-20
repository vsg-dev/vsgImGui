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

#include <vsgImGui/RenderImGui.h>
#include <vsgImGui/Texture.h>

#include <vsg/state/DescriptorImage.h>

using namespace vsgImGui;

namespace
{
    auto getDefaultSampler()
    {
        auto sampler = vsg::Sampler::create();
        sampler->maxLod = 9.0; // whatever for now
        sampler->addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        sampler->addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        sampler->addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        return sampler;
    }

    vsg::ref_ptr<vsg::DescriptorSet> makeImageDescriptorSet(vsg::ref_ptr<vsg::Data> data, vsg::ref_ptr<vsg::Sampler> sampler = {})
    {
        if (!data) return {};
        // set up graphics pipeline

        vsg::DescriptorSetLayoutBindings descriptorBindings{
            {0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr} // { binding, descriptorType, descriptorCount, stageFlags, pImmutableSamplers }
        };

        auto descriptorSetLayout = vsg::DescriptorSetLayout::create(descriptorBindings);
        // create texture image and associated DescriptorSets and binding
        if (!sampler)
        {
            sampler = getDefaultSampler();
        }
        auto texture = vsg::DescriptorImage::create(sampler, data, 0, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

        auto descriptorSet = vsg::DescriptorSet::create(descriptorSetLayout, vsg::Descriptors{texture});
        return descriptorSet;
    }
} // namespace

Texture::Texture(vsg::ref_ptr<vsg::Data> data, vsg::ref_ptr<vsg::Sampler> sampler)
{
    if (data)
    {
        height = data->height();
        width = data->width();
        descriptorSet = makeImageDescriptorSet(data, sampler);
    }
}

Texture::~Texture()
{
}

void Texture::compile(vsg::Context& context)
{
    if (descriptorSet) descriptorSet->compile(context);
}

ImTextureID Texture::id(uint32_t deviceID) const
{
    return descriptorSet ? static_cast<ImTextureID>(descriptorSet->vk(deviceID)) : ImTextureID{};
}
