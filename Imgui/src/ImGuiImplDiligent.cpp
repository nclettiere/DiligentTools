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

#include <cstddef>
#include "imgui.h"
#include "ImGuiImplDiligent.hpp"
#include "ImGuiDiligentRenderer.hpp"
#include "RenderDevice.h"
#include "DeviceContext.h"
#include "RefCntAutoPtr.hpp"
#include "BasicMath.hpp"
#include "MapHelper.hpp"

namespace Diligent
{

ImGuiDiligentCreateInfo::ImGuiDiligentCreateInfo(IRenderDevice* _pDevice,
                                                 TEXTURE_FORMAT _BackBufferFmt,
                                                 TEXTURE_FORMAT _DepthBufferFmt) noexcept :
    pDevice{_pDevice},
    BackBufferFmt{_BackBufferFmt},
    DepthBufferFmt{_DepthBufferFmt}
{}

ImGuiDiligentCreateInfo::ImGuiDiligentCreateInfo(IRenderDevice*       _pDevice,
                                                 const SwapChainDesc& _SCDesc) noexcept :
    ImGuiDiligentCreateInfo{_pDevice, _SCDesc.ColorBufferFormat, _SCDesc.DepthBufferFormat}
{}


ImGuiImplDiligent::ImGuiImplDiligent(const ImGuiDiligentCreateInfo& CI, bool secondaryWindow)
{
    m_pImGuiCtx = ImGui::CreateContext();

    //if (secondaryWindow)
    ImGuiIO& io    = ImGui::GetIO();
    io.IniFilename = nullptr;

    ImGui::SetCurrentContext(m_pImGuiCtx);

    m_pRenderer = std::make_unique<ImGuiDiligentRenderer>(CI);
}

ImGuiImplDiligent::~ImGuiImplDiligent()
{
    ImGui::DestroyContext(m_pImGuiCtx);
}

void ImGuiImplDiligent::NewFrame(Uint32 RenderSurfaceWidth, Uint32 RenderSurfaceHeight, SURFACE_TRANSFORM SurfacePreTransform)
{
    ImGui::SetCurrentContext(m_pImGuiCtx);

    //ImGuiIO& io = ImGui::GetIO();
    //io.DisplaySize.x = static_cast<float>(RenderSurfaceWidth);
    //io.DisplaySize.y = static_cast<float>(RenderSurfaceHeight);

    m_pRenderer->NewFrame(RenderSurfaceWidth, RenderSurfaceHeight, SurfacePreTransform);
    ImGui::NewFrame();
}

void ImGuiImplDiligent::EndFrame()
{
    ImGui::EndFrame();
}

void ImGuiImplDiligent::Render(IDeviceContext* pCtx)
{
    // No need to call ImGui::EndFrame as ImGui::Render calls it automatically
    //ImGui::SetCurrentContext(m_pImGuiCtx);
    ImGui::Render();
    m_pRenderer->RenderDrawData(pCtx, ImGui::GetDrawData());
}

// Use if you want to reset your rendering device without losing ImGui state.
void ImGuiImplDiligent::InvalidateDeviceObjects()
{
    m_pRenderer->InvalidateDeviceObjects();
}

void ImGuiImplDiligent::CreateDeviceObjects()
{
    m_pRenderer->CreateDeviceObjects();
}

void ImGuiImplDiligent::UpdateFontsTexture()
{
    m_pRenderer->CreateFontsTexture();
}

} // namespace Diligent
