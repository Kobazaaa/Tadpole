#ifndef EDITOR_LAYER_H
#define EDITOR_LAYER_H

// -- Interface Includes --
#include "IPanel.h"
#include "ILayer.h"

// -- Rendering Includes --
#include "Renderer.h"

// -- Utilities Includes --
#include <memory>

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
		explicit EditorLayer(pompeii::Renderer* pRenderer, std::unique_ptr<ImGuiContext> pImGuiContext);

		//--------------------------------------------------
		//    Loop
		//--------------------------------------------------
		void OnAttach() override;
		void OnBegin() override;
		void OnUpdate() override;
		void OnEnd() override;
		void OnDetach() override;

	private:
		pompeii::Renderer*				m_pRenderer{};
		std::unique_ptr<ImGuiContext>	m_pImGuiContext{};

		std::vector<std::unique_ptr<IPanel>> m_vEditorPanels{};
	};
}

#endif // RENDER_LAYER_H