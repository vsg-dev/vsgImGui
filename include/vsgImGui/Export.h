#pragma once

/* <editor-fold desc="MIT License">

Copyright(c) 2021 Robert Osfield

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

#if (defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__))
#    if defined(vsgImGui_EXPORTS)
#        define VSGIMGUI_DECLSPEC __declspec(dllexport)
#    elif defined(VSGIMGUI_SHARED_LIBRARY)
#        define VSGIMGUI_DECLSPEC __declspec(dllimport)
#    else
#        define VSGIMGUI_DECLSPEC
#    endif
#else
#    define VSGIMGUI_DECLSPEC
#endif

#ifndef VSG_IMGUI_USE_SYSTEM_IMGUI
#    ifdef IMGUI_API
#        undef IMGUI_API
#    endif
#    define IMGUI_API VSGIMGUI_DECLSPEC
#endif
#ifndef VSG_IMGUI_USE_SYSTEM_IMPLOT
#    ifdef IMPLOT_API
#        undef IMPLOT_API
#    endif
#    define IMPLOT_API VSGIMGUI_DECLSPEC
#endif

#include <vulkan/vulkan.h>
#ifndef VSG_IMGUI_USE_SYSTEM_IMGUI
#    define ImTextureID VkDescriptorSet
#endif

#include <vsg/maths/vec2.h>
#include <vsg/maths/vec4.h>

#define IM_VEC2_CLASS_EXTRA                               \
    constexpr ImVec2(const vsg::vec2& v) : x(v.x), y(v.y) \
    {                                                     \
    }                                                     \
    operator vsg::vec2() const                            \
    {                                                     \
        return vsg::vec2(x, y);                           \
    }

#define IM_VEC4_CLASS_EXTRA                                               \
    constexpr ImVec4(const vsg::vec4& v) : x(v.x), y(v.y), z(v.z), w(v.w) \
    {                                                                     \
    }                                                                     \
    operator vsg::vec4() const                                            \
    {                                                                     \
        return vsg::vec4(x, y, z, w);                                     \
    }
