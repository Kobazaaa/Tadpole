// -- Tadpole Includes --
#include "ViewportPanel.h"

#include <algorithm>

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
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar;
	ImGui::Begin("Viewport", nullptr, flags);

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

		ImGui::EndMenuBar();
	}
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

	ImGui::Image(m_vDescriptorSets[m_pRenderer->GetContext().currentFrame], imageSize);
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
