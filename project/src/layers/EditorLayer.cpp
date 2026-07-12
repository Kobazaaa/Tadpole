// -- Tadpole Includes --
#include "ImGuiContext.h"
#include "EditorLayer.h"
#include "EditorHelpers.h"

#include "ConsolePanel.h"
#include "HierarchyPanel.h"
#include "InspectorPanel.h"
#include "StatsPanel.h"
#include "ViewportPanel.h"

// -- ImGui --
#include "imgui_internal.h"
#include "ImGuiFileDialog.h"

// -- Kobengine Includes --
#include "IWindow.h"

//--------------------------------------------------
//    Constructor & Destructor
//--------------------------------------------------
tadpole::EditorLayer::EditorLayer(pompeii::IWindow* pWindow, pompeii::Renderer* pRenderer, std::unique_ptr<ImGuiContext> pImGuiContext)
	: m_pWindow(pWindow)
	, m_pRenderer(pRenderer)
	, m_pImGuiContext(std::move(pImGuiContext))
{
	// -- Panels --
	{
		m_vEditorPanels.reserve(5);

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

		// stats
		m_vEditorPanels.push_back(std::make_unique<StatsPanel>(pRenderer));

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

	// -- Setup Global Dock Space Host Window --
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y));
	ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y));
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::Begin("##DockSpaceHost", nullptr, flags);
	ImGui::PopStyleVar(2);

	DrawMenuBar();

	// -- Dock Space --
	const ImGuiID dockSpaceID = ImGui::GetID("MyDockspace");
	const bool layoutMissing = ImGui::DockBuilderGetNode(dockSpaceID) == nullptr;
	if (layoutMissing || m_ResetDockLayout)
	{
		BuildDefaultDockLayout(dockSpaceID);
		m_ResetDockLayout = false;
	}
	ImGui::DockSpace(dockSpaceID);

	for (const auto& panel : m_vEditorPanels)
		panel->OnImGuiRender();

	HandleFileDialogs();
	DrawAboutPopup();

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


//--------------------------------------------------
//    Helpers
//--------------------------------------------------
void tadpole::EditorLayer::DrawMenuBar()
{
	if (!ImGui::BeginMenuBar())
		return;

	// -- File --
	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Import Model..."))
		{
			IGFD::FileDialogConfig config{};
			config.path = ".";
			ImGuiFileDialog::Instance()->OpenDialog("ImportModel", "Import Model", filters::MODEL_FILE_FILTERS, config);
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Save Layout As..."))
		{
			IGFD::FileDialogConfig config{};
			config.path = ".";
			config.fileName = "layout.ini";
			config.flags = ImGuiFileDialogFlags_ConfirmOverwrite;
			ImGuiFileDialog::Instance()->OpenDialog("SaveLayout", "Save Layout", ".ini", config);
		}
		if (ImGui::MenuItem("Load Layout..."))
		{
			IGFD::FileDialogConfig config{};
			config.path = ".";
			ImGuiFileDialog::Instance()->OpenDialog("LoadLayout", "Load Layout", ".ini", config);
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Exit", "Alt+F4"))
			m_pWindow->Close();

		ImGui::EndMenu();
	}

	// -- Scene --
	if (ImGui::BeginMenu("Scene"))
	{
		if (ImGui::MenuItem("Create Empty Object"))			actions::CreateEmpty();
		if (ImGui::MenuItem("Create Camera"))				actions::CreateCamera();
		if (ImGui::MenuItem("Create Directional Light"))	actions::CreateLight(pompeii::LightType::Directional);
		if (ImGui::MenuItem("Create Point Light"))			actions::CreateLight(pompeii::LightType::Point);

		ImGui::EndMenu();
	}

	// -- Window --
	if (ImGui::BeginMenu("Window"))
	{
		for (const auto& panel : m_vEditorPanels)
			ImGui::MenuItem(panel->GetName().c_str(), nullptr, &panel->IsOpen);

		ImGui::Separator();

		if (ImGui::MenuItem("Reset Layout"))
		{
			m_ResetDockLayout = true;
			for (const auto& panel : m_vEditorPanels)
				panel->IsOpen = true;
		}

		ImGui::EndMenu();
	}

	// -- Help --
	if (ImGui::BeginMenu("Help"))
	{
		if (ImGui::MenuItem("About Tadpole"))
			m_OpenAboutPopup = true;
		ImGui::EndMenu();
	}

	ImGui::EndMenuBar();
}
void tadpole::EditorLayer::BuildDefaultDockLayout(ImGuiID dockSpaceID) const
{
	ImGui::DockBuilderRemoveNode(dockSpaceID);
	ImGui::DockBuilderAddNode(dockSpaceID, ImGuiDockNodeFlags_DockSpace);
	ImGui::DockBuilderSetNodeSize(dockSpaceID, ImGui::GetMainViewport()->Size);

	ImGuiID centerID = dockSpaceID;
	const ImGuiID leftID = ImGui::DockBuilderSplitNode(centerID, ImGuiDir_Left, 0.20f, nullptr, &centerID);
	const ImGuiID rightID = ImGui::DockBuilderSplitNode(centerID, ImGuiDir_Right, 0.30f, nullptr, &centerID);
	const ImGuiID bottomID = ImGui::DockBuilderSplitNode(centerID, ImGuiDir_Down, 0.30f, nullptr, &centerID);

	ImGui::DockBuilderDockWindow("Hierarchy", leftID);
	ImGui::DockBuilderDockWindow("Inspector", rightID);
	ImGui::DockBuilderDockWindow("Console", bottomID);
	ImGui::DockBuilderDockWindow("Stats", bottomID);
	ImGui::DockBuilderDockWindow("Viewport", centerID);

	ImGui::DockBuilderFinish(dockSpaceID);
}
void tadpole::EditorLayer::HandleFileDialogs()
{
	HandleFileDialog("ImportModel", [](const std::string& path) { actions::ImportModel(path); });
	HandleFileDialog("SaveLayout", [](const std::string& path) { ImGui::SaveIniSettingsToDisk(path.c_str()); });
	HandleFileDialog("LoadLayout", [](const std::string& path) { ImGui::LoadIniSettingsFromDisk(path.c_str()); });
}
void tadpole::EditorLayer::HandleFileDialog(const std::string& key, const std::function<void(const std::string&)>& func) const
{
	constexpr ImGuiWindowFlags fileDialogFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking;
	constexpr ImVec2 fileDialogSize = ImVec2(800, 500);
	if (ImGuiFileDialog::Instance()->Display(key, fileDialogFlags, fileDialogSize))
	{
		if (ImGuiFileDialog::Instance()->IsOk())
		{
			const std::string path = ImGuiFileDialog::Instance()->GetFilePathName();
			func(path);
		}
		ImGuiFileDialog::Instance()->Close();
	}
}
void tadpole::EditorLayer::DrawAboutPopup()
{
	if (m_OpenAboutPopup)
	{
		ImGui::OpenPopup("About Tadpole");
		m_OpenAboutPopup = false;
	}

	const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("About Tadpole", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Tadpole Editor");
		ImGui::TextDisabled("Kobengine v0.0.0 + Pompeii (Vulkan) v0.0.0");
		ImGui::Separator();
		ImGui::Text("Dear ImGui %s", ImGui::GetVersion());
		ImGui::Spacing();
		if (ImGui::Button("Close", ImVec2(120, 0)))
			ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
}
