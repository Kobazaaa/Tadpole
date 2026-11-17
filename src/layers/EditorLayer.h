#ifndef EDITOR_LAYER_H
#define EDITOR_LAYER_H

// -- Interface Includes --
#include "IPanel.h"
#include "ILayer.h"

// -- Rendering Includes --
#include "Renderer.h"

// -- Utilities Includes --
#include <functional>
#include <memory>
#include <string>

// -- Forward Declares --
namespace tadpole
{
	class ImGuiContext;
}
namespace pompeii
{
	class IWindow;
	class Image;
}

namespace tadpole
{
	//? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//? ~~	  EditorLayer
	//? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	class EditorLayer final : public kobengine::ILayer
	{
	public:
		//--------------------------------------------------
		//    Constructor & Destructor
		//--------------------------------------------------
		explicit EditorLayer(pompeii::IWindow* pWindow, pompeii::Renderer* pRenderer, std::unique_ptr<ImGuiContext> pImGuiContext);

		//--------------------------------------------------
		//    Loop
		//--------------------------------------------------
		void OnAttach() override;
		void OnBegin() override;
		void OnUpdate() override;
		void OnEnd() override;
		void OnDetach() override;

	private:
		//--------------------------------------------------
		//    Helpers
		//--------------------------------------------------
		void DrawMenuBar();
		void BuildDefaultDockLayout(ImGuiID dockSpaceID) const;
		void HandleFileDialogs();
		void HandleFileDialog(const std::string& key, const std::function<void(const std::string&)>& func) const;
		void DrawAboutPopup();

		pompeii::IWindow*				m_pWindow{};
		pompeii::Renderer*				m_pRenderer{};
		std::unique_ptr<ImGuiContext>	m_pImGuiContext{};

		std::vector<std::unique_ptr<IPanel>> m_vEditorPanels{};

		bool m_ResetDockLayout{ false };
		bool m_OpenAboutPopup{ false };
	};
}

#endif // EDITOR_LAYER_H
