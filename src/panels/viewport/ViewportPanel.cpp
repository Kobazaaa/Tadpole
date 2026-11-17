// -- Tadpole Includes --
#include "ViewportPanel.h"

#include <algorithm>

// -- Pompeii Includes --
#include "Renderer.h"

// -- Kobengine Includes --
#include "InputManager.h"
#include "Timer.h"

//--------------------------------------------------
//    Constructor & Destructor
//--------------------------------------------------
tadpole::ViewportPanel::ViewportPanel(pompeii::Renderer* pRenderer)
	: IPanel("Viewport")
	, m_pRenderer(pRenderer)
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
	if (!IsOpen)
		return;

	m_SmoothedDeltaTime += (kobengine::Timer::GetDeltaSeconds() - m_SmoothedDeltaTime) * 0.05f;

	ImGui::SetNextWindowSize({ 800, 600 }, ImGuiCond_FirstUseEver);
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar;
	ImGui::Begin(GetName().c_str(), &IsOpen, flags);

	DrawToolbar();
	ImGui::Separator();
	DrawOutput();

	ImGui::End();
}
void tadpole::ViewportPanel::OnDeactivate()
{
	m_Sampler.Destroy(m_pRenderer->GetContext());
	for (const auto& ds : m_vDescriptorSets)
		ImGui_ImplVulkan_RemoveTexture(ds);
}



//--------------------------------------------------
//    Helper
//--------------------------------------------------
void tadpole::ViewportPanel::DrawToolbar()
{
	if (ImGui::BeginMenuBar())
	{
		const AspectPreset& preset = m_vPresets[m_SelectedPreset];
		const std::string displayName = preset.name + " (" + std::to_string(preset.width) + "x" + std::to_string(preset.height) + ")";
		if (ImGui::BeginMenu(displayName.c_str()))
		{
			DrawBuiltInPresets();
			DrawCustomPresets();
			DrawAddPreset();
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			ImGui::MenuItem("Stats Overlay", nullptr, &m_ShowStatsOverlay);
			ImGui::EndMenu();
		}

		// -- Right-Aligned Info --
		const VkExtent2D extent = m_pRenderer->GetOutputImages()[m_pRenderer->GetContext().currentFrame].GetExtent2D();
		char info[64];
		snprintf(info, sizeof(info), "%ux%u  |  %.0f FPS", extent.width, extent.height, 1.f / m_SmoothedDeltaTime);
		const float infoWidth = ImGui::CalcTextSize(info).x;
		ImGui::SameLine(ImGui::GetWindowWidth() - infoWidth - ImGui::GetStyle().FramePadding.x * 2.f - ImGui::GetStyle().ItemSpacing.x);
		ImGui::TextDisabled("%s", info);

		ImGui::EndMenuBar();
	}
}
void tadpole::ViewportPanel::DrawStatsOverlay(const ImVec2& imageScreenPos) const
{
	const VkExtent2D extent = m_pRenderer->GetOutputImages()[m_pRenderer->GetContext().currentFrame].GetExtent2D();
	char text[128];
	snprintf(text, sizeof(text), "FPS:        %.1f\nFrame Time: %.2f ms\nResolution: %ux%u",
		1.f / m_SmoothedDeltaTime, m_SmoothedDeltaTime * 1000.f, extent.width, extent.height);

	ImDrawList* pDrawList = ImGui::GetWindowDrawList();
	const ImVec2 padding{ 8.f, 6.f };
	const ImVec2 textPos{ imageScreenPos.x + 10.f, imageScreenPos.y + 10.f };
	const ImVec2 textSize = ImGui::CalcTextSize(text);

	pDrawList->AddRectFilled(
		{ textPos.x - padding.x, textPos.y - padding.y },
		{ textPos.x + textSize.x + padding.x, textPos.y + textSize.y + padding.y },
		IM_COL32(0, 0, 0, 150), 4.f);
	pDrawList->AddText(textPos, IM_COL32(255, 255, 255, 220), text);
}
void tadpole::ViewportPanel::DrawBuiltInPresets()
{
	ImGui::TextDisabled("Built-In");
	if (ImGui::BeginTable("##AspectTableBuiltIn", 1, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_NoPadOuterX))
	{
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
		for (int i = 0; i < BUILTIN_COUNT; i++)
		{
			const AspectPreset& preset = m_vPresets[i];
			const bool selected = (i == m_SelectedPreset);
			std::string displayName = preset.name + " ";
			std::string resolution = "(" + std::to_string(preset.width) + "x" + std::to_string(preset.height) + ")";

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			if (ImGui::Selectable(displayName.c_str(), selected))
			{
				m_SelectedPreset = i;
				ResizeOutput(preset.width, preset.height);
			}

			ImGui::SameLine();
			ImGui::TextDisabled(resolution.c_str());

			if (selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndTable();
	}
}
void tadpole::ViewportPanel::DrawCustomPresets()
{
	if (m_vPresets.size() <= BUILTIN_COUNT)
		return;

	ImGui::Separator();
	ImGui::TextDisabled("Custom");
	if (ImGui::BeginTable("##AspectTableCustom", 2, ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_NoPadOuterX))
	{
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
		ImGui::TableSetupColumn("Delete", ImGuiTableColumnFlags_WidthFixed, 50.0f);

		for (int i = BUILTIN_COUNT; i < m_vPresets.size(); i++)
		{
			const AspectPreset& preset = m_vPresets[i];
			const bool selected = (i == m_SelectedPreset);
			std::string displayName = preset.name + " ";
			std::string resolution = "(" + std::to_string(preset.width) + "x" + std::to_string(preset.height) + ")";

			ImGui::PushID(i);
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			if (ImGui::Selectable(displayName.c_str(), selected))
			{
				m_SelectedPreset = i;
				ResizeOutput(preset.width, preset.height);
			}

			ImGui::SameLine();
			ImGui::TextDisabled(resolution.c_str());

			ImGui::TableSetColumnIndex(1);
			ImGui::PushItemWidth(ImGui::GetColumnWidth());
			if (ImGui::SmallButton("X"))
			{
				m_vPresets.erase(m_vPresets.begin() + i);

				if (m_SelectedPreset == i)
				{
					m_SelectedPreset = 0;
					const AspectPreset& p = m_vPresets[0];
					ResizeOutput(p.width, p.height);
				}
				else if (m_SelectedPreset > i)
				{
					m_SelectedPreset--;
					const AspectPreset& p = m_vPresets[m_SelectedPreset];
					ResizeOutput(p.width, p.height);
				}

				ImGui::PopID();
				break;
			}
			ImGui::PopItemWidth();
			ImGui::PopID();
		}
		ImGui::EndTable();
	}
}
void tadpole::ViewportPanel::DrawAddPreset()
{
	const float availWidth = ImGui::GetContentRegionAvail().x;
	const auto text = "+";
	const float buttonWidth = ImGui::CalcTextSize(text).x;
	ImGui::SetCursorPosX((availWidth - buttonWidth) * 0.5f);
	if (ImGui::Button(text))
		ImGui::OpenPopup("AddPresetPopup");
	if (ImGui::BeginPopupModal("AddPresetPopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static char nameBuf[64] = "";
		static int w = 1920;
		static int h = 1080;

		ImGui::Text("Create Custom Aspect Ratio");
		ImGui::Separator();

		ImGui::InputText("Name", nameBuf, sizeof(nameBuf));
		if (ImGui::InputInt("Width", &w)) w = std::max(w, 0);
		if (ImGui::InputInt("Height", &h)) h = std::max(h, 0);

		ImGui::Separator();

		if (ImGui::Button("Add", ImVec2(120, 0)))
		{
			if (w > 0 && h > 0 && strlen(nameBuf) > 0)
				m_vPresets.push_back({ .name = nameBuf,
									  .width = static_cast<uint32_t>(w), .height = static_cast<uint32_t>(h),
									  .isFree = false });

			nameBuf[0] = '\0';
			w = 1920;
			h = 1080;

			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(120, 0)))
			ImGui::CloseCurrentPopup();

		ImGui::EndPopup();
	}
}

void tadpole::ViewportPanel::DrawOutput()
{
	AspectPreset& preset = m_vPresets[m_SelectedPreset];
	ImVec2 childSize = ImGui::GetContentRegionAvail();

	ImGui::BeginChild("##ViewportChild", childSize, true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground);

	ImVec2 imageSize = childSize;
	if (!preset.isFree)
	{
		float ratio = static_cast<float>(preset.width) / static_cast<float>(preset.height);
		imageSize.y = imageSize.x / ratio;
		if (imageSize.y > childSize.y)
		{
			imageSize.y = childSize.y;
			imageSize.x = imageSize.y * ratio;
		}
	}

	ImGui::SetCursorPos(ImVec2(
		(childSize.x - imageSize.x) * 0.5f,
		(childSize.y - imageSize.y) * 0.5f
	));

	const ImVec2 imageScreenPos = ImGui::GetCursorScreenPos();
	ImGui::Image(m_vDescriptorSets[m_pRenderer->GetContext().currentFrame], imageSize);
	if (m_ShowStatsOverlay)
		DrawStatsOverlay(imageScreenPos);
	kobengine::InputManager::SetInputActive(ImGui::IsWindowFocused());
	ImGui::EndChild();
}
void tadpole::ViewportPanel::ResizeOutput(uint32_t w, uint32_t h)
{
	m_pRenderer->ExecuteAfterCommandBuffer([this, w, h]
	{
		m_pRenderer->ResizeOutput(w, h);

		int i = 0;
		m_vDescriptorSets.resize(m_pRenderer->GetContext().maxFramesInFlight);
		for (auto& ds : m_vDescriptorSets)
		{
			ImGui_ImplVulkan_RemoveTexture(ds);
			ds = ImGui_ImplVulkan_AddTexture(m_Sampler.GetHandle(), m_pRenderer->GetOutputImages()[i].GetView().GetHandle(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			i++;
		}
	});
}
