// -- Tadpole Includes --
#include "ImGuiContext.h"
#include "EditorLayer.h"

#include "ConsolePanel.h"
#include "HierarchyPanel.h"
#include "InspectorPanel.h"
#include "ViewportPanel.h"

// -- Kobengine Includes --
#include "IWindow.h"

//--------------------------------------------------
//    Constructor & Destructor
//--------------------------------------------------
tadpole::EditorLayer::EditorLayer(pompeii::Renderer* pRenderer, std::unique_ptr<ImGuiContext> pImGuiContext)
	: m_pRenderer(pRenderer)
	, m_pImGuiContext(std::move(pImGuiContext))
{
	// -- Panels --
	{
		m_vEditorPanels.reserve(4);

		// console
		m_vEditorPanels.push_back(std::make_unique<ConsolePanel>());

		// subscribe inspector to hierarchy event
		auto hierarchyPanel = std::make_unique<HierarchyPanel>();
		auto inspectorPanel = std::make_unique<InspectorPanel>();
		hierarchyPanel->OnSelectedObjectChanged.AddListener(inspectorPanel.get(), &InspectorPanel::SetSelectedObject);

		m_vEditorPanels.push_back(std::move(hierarchyPanel));
		m_vEditorPanels.push_back(std::move(inspectorPanel));

		// viewport
		m_vEditorPanels.push_back(std::make_unique<ViewportPanel>(pRenderer));

		m_vEditorPanels.shrink_to_fit();
	}
}


//--------------------------------------------------
//    Loop
//--------------------------------------------------
void tadpole::EditorLayer::OnAttach()
{
	for (const auto& panel : m_vEditorPanels)
		panel->OnActivate();
}
void tadpole::EditorLayer::OnBegin()
{
	m_pImGuiContext->BeginFrame();
}
void tadpole::EditorLayer::OnUpdate()
{
	// -- Acquire Images --
	auto& outputImage = m_pRenderer->GetCurrentOutputImage();
	auto& presentImage = m_pRenderer->GetCurrentSwapChainImage();
	const auto& context = m_pRenderer->GetContext();
	const auto& cmd = context.commandPool->GetBuffer(context.currentFrame);

	// -- Transition Output to Render To --
	outputImage.TransitionLayout(cmd,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_ACCESS_2_SHADER_SAMPLED_READ_BIT, VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT,
		0, outputImage.GetMipLevels(), 0, outputImage.GetLayerCount());

	presentImage.TransitionLayout(cmd,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VK_ACCESS_2_NONE, VK_PIPELINE_STAGE_2_NONE,
		VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		0, presentImage.GetMipLevels(), 0, presentImage.GetLayerCount());

	// -- Setup Global Dock Space --
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y));
	ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y));
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::Begin("##DockSpaceHost", nullptr, flags);
	ImGui::PopStyleVar(2);

	ImGui::DockSpace(ImGui::GetID("MyDockspace"));

	for (const auto& panel : m_vEditorPanels)
		panel->OnImGuiRender();

	ImGui::End();
}
void tadpole::EditorLayer::OnEnd()
{
	m_pImGuiContext->EndFrame();
}

void tadpole::EditorLayer::OnDetach()
{
	for (const auto& panel : m_vEditorPanels)
		panel->OnDeactivate();
	m_pImGuiContext->Destroy();
}
