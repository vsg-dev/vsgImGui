/* <editor-fold desc="MIT License">

Copyright(c) 2021 Don Burns, Roland Hill and Robert Osfield.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#include <vsgImGui/GuiCommand.h>

#include "../imgui/backends/imgui_impl_vulkan.h"

#include <iostream>

using namespace vsgImGui;

namespace
{
    void check_vk_result(VkResult err)
    {
        if (err == 0)
            return;

        std::cerr << "[vulkan] Error: VkResult = " << err << std::endl;
        if (err < 0)
            abort();
    }
}

GuiCommand::GuiCommand( const vsg::ref_ptr<vsg::Window> &window )
{
    _init(window);
    _uploadFonts(window);
}

GuiCommand::~GuiCommand()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui::DestroyContext();
    vkDestroyDescriptorPool(_device, _descriptorPool, nullptr);
    vkDestroyCommandPool(_device, _commandPool, nullptr);
}

void GuiCommand::add(const Component& component)
{
    _components.push_back(component);
}

void GuiCommand::renderComponents() const
{
    ImGui_ImplVulkan_NewFrame();
    ImGui::NewFrame();

    for(auto& component : _components)
    {
        component();
    }

    ImGui::Render();
}

void GuiCommand::record(vsg::CommandBuffer& commandBuffer) const
{
    // render();

    ImDrawData* draw_data = ImGui::GetDrawData();
    if( draw_data )
        ImGui_ImplVulkan_RenderDrawData(draw_data, &(*commandBuffer));
}

void GuiCommand::_init( const vsg::ref_ptr<vsg::Window> &window )
{
    VkResult err;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    //ImGuiIO& io = ImGui::GetIO();

    ImGui::StyleColorsDark();

    auto [physicalDevice, queueFamily] = window->getInstance()->getPhysicalDeviceAndQueueFamily( VK_QUEUE_GRAPHICS_BIT);
    _queueFamily = queueFamily;
    _queue = window->getDevice()->getQueue( _queueFamily )->queue();
    _device = window->getDevice()->getDevice();

    ImGui_ImplVulkan_InitInfo init_info = {};

    init_info.Instance       = window->getInstance()->getInstance();
    init_info.PhysicalDevice = physicalDevice->getPhysicalDevice();
    init_info.Device         = _device;
    init_info.QueueFamily    = _queueFamily;
    init_info.Queue          = _queue;
    init_info.PipelineCache  = VK_NULL_HANDLE;

    // Create Descriptor Pool
    _descriptorPool = VK_NULL_HANDLE;
    {
        VkDescriptorPoolSize pool_sizes[] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };
        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
        pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;
        err = vkCreateDescriptorPool(window->getDevice()->getDevice(), &pool_info, nullptr, &_descriptorPool);
        check_vk_result(err);
    }

    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice->getPhysicalDevice(), *(window->getSurface()), &capabilities);
    uint32_t imageCount = 3;
    imageCount = std::max(imageCount, capabilities.minImageCount);                        // Vulkan spec requires minImageCount to be 1 or greater
    if (capabilities.maxImageCount > 0) imageCount = std::min(imageCount, capabilities.maxImageCount); // Vulkan spec specifies 0 as being unlimited number of images

    init_info.DescriptorPool  = _descriptorPool;
    init_info.Allocator       = nullptr;
    init_info.MinImageCount   = capabilities.minImageCount;
    init_info.ImageCount      = imageCount;
    init_info.CheckVkResultFn = check_vk_result;

    ImGui_ImplVulkan_Init(&init_info, *window->getOrCreateRenderPass());
}

void GuiCommand::_uploadFonts( const vsg::ref_ptr<vsg::Window> &window )
{
    VkResult err;

    // VkcommandPool commandPool;
    {
        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = _queueFamily;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        poolInfo.pNext = nullptr;

        err = vkCreateCommandPool(_device, &poolInfo, window->getDevice()->getAllocationCallbacks(), &_commandPool);
        check_vk_result(err);
    }

    VkCommandBuffer commandBuffer;
    {
        VkCommandBufferAllocateInfo allocateInfo = {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocateInfo.commandPool = _commandPool;
        allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocateInfo.commandBufferCount = 1;
        err = vkAllocateCommandBuffers(_device, &allocateInfo, &commandBuffer);
        check_vk_result(err);
    }

    err = vkResetCommandPool(_device, _commandPool, 0);
    check_vk_result(err);

    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    err = vkBeginCommandBuffer(commandBuffer, &begin_info);
    check_vk_result(err);

    ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);

    VkSubmitInfo end_info = {};
    end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    end_info.commandBufferCount = 1;
    end_info.pCommandBuffers = &commandBuffer;
    err = vkEndCommandBuffer(commandBuffer);
    check_vk_result(err);
    err = vkQueueSubmit(_queue, 1, &end_info, VK_NULL_HANDLE);
    check_vk_result(err);

    err = vkDeviceWaitIdle(_device);
    check_vk_result(err);
    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

