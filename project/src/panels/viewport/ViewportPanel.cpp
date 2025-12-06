// -- Tadpole Includes --
#include "ViewportPanel.h"

// -- Pompeii Includes --
#include "Renderer.h"

// -- Kobengine Includes --
#include "InputManager.h"

//--------------------------------------------------
//    Constructor & Destructor
//--------------------------------------------------
tadpole::ViewportPanel::ViewportPanel(pompeii::Renderer* pRenderer)
	: m_pRenderer(pRenderer)
{}


//--------------------------------------------------
//    Functionality
//--------------------------------------------------
void tadpole::ViewportPanel::OnActivate()
{
	auto& context = m_pRenderer->GetContext();
	pompeii::SamplerBuilder builder{};
	builder
		.SetFilters(VK_FILTER_NEAREST, VK_FILTER_NEAREST)
		.SetAddressMode(VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER)
		.SetMipmapMode(VK_SAMPLER_MIPMAP_MODE_NEAREST)
		.SetMipLevels(0.f, 0.f, 0.f)
		.SetBorderColor(VK_BORDER_COLOR_INT_OPAQUE_BLACK)
		.Build(context, m_Sampler);

	int i = 0;
	m_vDescriptorSets.resize(context.maxFramesInFlight);
	for (auto& ds : m_vDescriptorSets)
	{
		ds = ImGui_ImplVulkan_AddTexture(m_Sampler.GetHandle(), m_pRenderer->GetOutputImages()[i].GetView().GetHandle(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		i++;
	}
}

void tadpole::ViewportPanel::OnImGuiRender()
{
	ImGui::SetNextWindowSize({ 800, 600 }, ImGuiCond_FirstUseEver);
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
	ImGui::Begin("Viewport", nullptr, flags);

	const ImVec2 availSize = ImGui::GetContentRegionAvail();
	const auto imageSize = m_pRenderer->GetCurrentOutputImage().GetExtent3D();
	const float imageWidth = static_cast<float>(imageSize.width);
	const float imageHeight = static_cast<float>(imageSize.height);

	const float availRatio = availSize.x / availSize.y;
	const float imageRatio = imageWidth / imageHeight;

	ImVec2 drawSize;
	if (imageRatio > availRatio)
	{
		drawSize.x = availSize.x;
		drawSize.y = availSize.x / imageRatio;
	}
	else
	{
		drawSize.y = availSize.y;
		drawSize.x = availSize.y * imageRatio;
	}

	const ImVec2 offset = { (availSize.x - drawSize.x) * 0.5f, (availSize.y - drawSize.y) * 0.5f };
	ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + offset.x, ImGui::GetCursorPos().y + offset.y));
	ImGui::Image(m_vDescriptorSets[m_pRenderer->GetContext().currentFrame], drawSize);

	kobengine::InputManager::SetInputActive(ImGui::IsWindowFocused());

	ImGui::End();
}

void tadpole::ViewportPanel::OnDeactivate()
{
	m_Sampler.Destroy(m_pRenderer->GetContext());
}

