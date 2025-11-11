// -- Tadpole Includes --
#include "EditorLayer.h"

// -- Kobengine Includes --
#include "IWindow.h"

// -- Standard Library --
#include <iostream>

// -- ImGui --
#include "Image.h"
#include "imgui.h"
#include "backends/imgui_impl_vulkan.h"
#include "backends/imgui_impl_glfw.h"

//--------------------------------------------------
//    Constructor & Destructor
//--------------------------------------------------
tadpole::EditorLayer::EditorLayer(pompeii::IWindow* pWindow)
	: m_pWindow(pWindow)
{ }


//--------------------------------------------------
//    Loop
//--------------------------------------------------
void tadpole::EditorLayer::OnAttach()
{
	//// Setup ImGui Context
	//IMGUI_CHECKVERSION();
	//ImGui::CreateContext();
	//ImGuiIO& io = ImGui::GetIO();
	//io.IniFilename = nullptr;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

}

void tadpole::EditorLayer::OnUpdate()
{

}

void tadpole::EditorLayer::OnDetach()
{
	//ImGui_ImplVulkan_Shutdown();
	//ImGui_ImplGlfw_Shutdown();
	//ImGui::DestroyContext();
}

//--------------------------------------------------
//    Events
//--------------------------------------------------
void tadpole::EditorLayer::HandleImageRendered(const pompeii::Image& renderedImage)
{
	renderedImage;
	std::cout << renderedImage.GetCurrentLayout() << std::endl;
}
