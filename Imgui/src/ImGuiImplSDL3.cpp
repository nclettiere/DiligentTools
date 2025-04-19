/*
 *  Copyright 2019-2023 Diligent Graphics LLC
 *  Copyright 2015-2019 Egor Yusov
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  In no event and under no legal theory, whether in tort (including negligence),
 *  contract, or otherwise, unless required by applicable law (such as deliberate
 *  and grossly negligent acts) or agreed to in writing, shall any Contributor be
 *  liable for any damages, including any direct, indirect, special, incidental,
 *  or consequential damages of any character arising as a result of this License or
 *  out of the use or inability to use the software (including but not limited to damages
 *  for loss of goodwill, work stoppage, computer failure or malfunction, or any and
 *  all other commercial damages or losses), even if such Contributor has been advised
 *  of the possibility of such damages.
 */

#include <SDL3/SDL.h>

#include "RenderDevice.h"
#include "GraphicsTypes.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "ImGuiImplSDL3.hpp"
#include "backends/imgui_impl_sdl3.h"
#include "DebugUtilities.hpp"

namespace Diligent
{

bool ImGuiImplSDL3::backend_initialized = false;

std::unique_ptr<ImGuiImplSDL3> ImGuiImplSDL3::Create(const ImGuiDiligentCreateInfo& CI, SDL_Window* window)
{
    return std::make_unique<ImGuiImplSDL3>(CI, window);
}

ImGuiImplSDL3::ImGuiImplSDL3(const ImGuiDiligentCreateInfo& CI, SDL_Window* window) :
    ImGuiImplDiligent{CI}
{
    // ImGui_ImplSDL3_XXX can only be initialized once
    // We need to check for this flag for multiple window support
    if (!backend_initialized)
    {
        switch (CI.pDevice->GetDeviceInfo().Type)
        {
            case RENDER_DEVICE_TYPE_UNDEFINED:
                LOG_ERROR_AND_THROW("Undefined device type");
                break;
            case RENDER_DEVICE_TYPE_D3D11:
            case RENDER_DEVICE_TYPE_D3D12:
                ImGui_ImplSDL3_InitForD3D(window);
                backend_initialized = true;
                break;
            case RENDER_DEVICE_TYPE_GL:
            case RENDER_DEVICE_TYPE_GLES:
                ImGui_ImplSDL3_InitForOpenGL(window, nullptr);
                backend_initialized = true;
                break;
            case RENDER_DEVICE_TYPE_VULKAN:
                ImGui_ImplSDL3_InitForVulkan(window);
                backend_initialized = true;
                break;
            case RENDER_DEVICE_TYPE_METAL:
                ImGui_ImplSDL3_InitForMetal(window);
                backend_initialized = true;
                break;
            case RENDER_DEVICE_TYPE_WEBGPU:
                LOG_ERROR_AND_THROW("WebGPU not supported");
                break;
            case RENDER_DEVICE_TYPE_COUNT:
                LOG_ERROR_AND_THROW("Unsupported device type");
                break;
        }
    }
}

ImGuiImplSDL3::~ImGuiImplSDL3()
{
}

void ImGuiImplSDL3::Shutdown()
{
    ImGui_ImplSDL3_Shutdown();
    // ImGui_ImplSDL3_CloseGamepads();
    backend_initialized = false;
}

void ImGuiImplSDL3::NewFrame(Uint32 RenderSurfaceWidth, Uint32 RenderSurfaceHeight, SURFACE_TRANSFORM SurfacePreTransform)
{
    VERIFY(SurfacePreTransform == SURFACE_TRANSFORM_IDENTITY, "Unexpected surface pre-transform");

    ImGui::SetCurrentContext(m_pImGuiCtx);
    ImGui_ImplSDL3_NewFrame();
    ImGuiImplDiligent::NewFrame(RenderSurfaceWidth, RenderSurfaceHeight, SurfacePreTransform);

#ifdef DILIGENT_DEBUG
    {
        ImGuiIO& io = ImGui::GetIO();
        VERIFY(io.DisplaySize.x == 0 || io.DisplaySize.x == static_cast<float>(RenderSurfaceWidth),
               "Render surface width (", RenderSurfaceWidth, ") does not match io.DisplaySize.x (", io.DisplaySize.x, ")");
        VERIFY(io.DisplaySize.y == 0 || io.DisplaySize.y == static_cast<float>(RenderSurfaceHeight),
               "Render surface height (", RenderSurfaceHeight, ") does not match io.DisplaySize.y (", io.DisplaySize.y, ")");
    }
#endif
}

void ImGuiImplSDL3::ProcessEvents(void* event)
{
    if (event == nullptr)
        return;

    SDL_Event* sdl_event = static_cast<SDL_Event*>(event);

    ImGui::SetCurrentContext(m_pImGuiCtx);
    ImGui_ImplSDL3_ProcessEvent(sdl_event);
}

} // namespace Diligent
