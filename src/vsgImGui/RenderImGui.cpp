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

#include <vsgImGui/RenderImGui.h>

#include "../imgui/backends/imgui_impl_vulkan.h"

#include <vsg/vk/SubmitCommands.h>
#include <vsg/io/Logger.h>

using namespace vsgImGui;

namespace
{
    void check_vk_result(VkResult err)
    {
        if (err == 0) return;

        vsg::error("[vulkan] Error: VkResult = ", err);
    }
} // namespace

RenderImGui::RenderImGui(const vsg::ref_ptr<vsg::Window>& window, bool useClearAttachments, bool uploadFonts)
{
    _init(window);
    if(uploadFonts) _uploadFonts();

    if (useClearAttachments)
    {
        // clear the depth buffer before view2 gets rendered
        VkClearValue clearValue{};
        clearValue.depthStencil = {1.0f, 0};
        VkClearAttachment attachment{VK_IMAGE_ASPECT_DEPTH_BIT, 1, clearValue};
        VkClearRect rect{VkRect2D{VkOffset2D{0, 0}, VkExtent2D{window->extent2D().width, window->extent2D().height}}, 0, 1};
        _clearAttachments = vsg::ClearAttachments::create(vsg::ClearAttachments::Attachments{attachment}, vsg::ClearAttachments::Rects{rect});
    }
}

RenderImGui::~RenderImGui()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui::DestroyContext();
}

void RenderImGui::_init(const vsg::ref_ptr<vsg::Window>& window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // ImGuiIO& io = ImGui::GetIO();

    ImGui::StyleColorsDark();

    _device = window->getOrCreateDevice();

    std::tie(_queueFamily, std::ignore) = _device->getPhysicalDevice()->getQueueFamily(window->traits()->queueFlags, window->getSurface());
    _queue = _device->getQueue(_queueFamily);

    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = *(_device->getInstance());
    init_info.PhysicalDevice = *(_device->getPhysicalDevice());
    init_info.Device = *(_device);
    init_info.QueueFamily = _queueFamily;
    init_info.Queue = *(_queue);  // ImGui doesn't use the queue so we shouldn't need to assign it, but it has an IM_ASSERT requiring it during debug build.
    init_info.PipelineCache = VK_NULL_HANDLE;

    // Create Descriptor Pool
    vsg::DescriptorPoolSizes pool_sizes = {
        {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
        {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
        {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};

    uint32_t maxSets = 1000 * pool_sizes.size();
    _descriptorPool = vsg::DescriptorPool::create(_device, maxSets, pool_sizes);

    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(init_info.PhysicalDevice,
                                              *(window->getSurface()),
                                              &capabilities);
    uint32_t imageCount = 3;
    imageCount =
        std::max(imageCount,
                 capabilities.minImageCount); // Vulkan spec requires
                                              // minImageCount to be 1 or greater
    if (capabilities.maxImageCount > 0)
        imageCount = std::min(
            imageCount,
            capabilities.maxImageCount); // Vulkan spec specifies 0 as being
                                         // unlimited number of images

    init_info.DescriptorPool = *(_descriptorPool);
    init_info.Allocator = nullptr;
    init_info.MinImageCount = capabilities.minImageCount;
    init_info.ImageCount = imageCount;
    init_info.CheckVkResultFn = check_vk_result;

    ImGui_ImplVulkan_Init(&init_info, *window->getOrCreateRenderPass());
}

void RenderImGui::_uploadFonts()
{
    auto commandPool = vsg::CommandPool::create(_device, _queueFamily, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
    auto fence = vsg::Fence::create(_device);

    uint64_t timeout = 1000000000;
    vsg::submitCommandsToQueue(commandPool, fence, timeout, _queue, [&](vsg::CommandBuffer& commandBuffer)
    {
        ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
    });

    VkResult result = fence->status();
    while(result == VK_NOT_READY)
    {
        result = fence->wait(timeout);
    }

    if (result != VK_SUCCESS)
    {
        vsg::error("RenderImGui::_uploadFonts(), fence->state() = ", result);
    }

    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void RenderImGui::add(const Component& component)
{
    _components.push_back(component);
}

bool RenderImGui::renderComponents() const
{
    ImGui_ImplVulkan_NewFrame();
    ImGui::NewFrame();

    bool visibleComponents = false;
    for (auto& component : _components)
    {
        if (component()) visibleComponents = true;
    }

    ImGui::EndFrame();
    ImGui::Render();

    return visibleComponents;
}

void RenderImGui::record(vsg::CommandBuffer& commandBuffer) const
{
    bool visibleComponents = renderComponents();

    if (visibleComponents)
    {
        if (_clearAttachments) _clearAttachments->record(commandBuffer);

        ImDrawData* draw_data = ImGui::GetDrawData();
        if (draw_data)
            ImGui_ImplVulkan_RenderDrawData(draw_data, &(*commandBuffer));
    }
}
