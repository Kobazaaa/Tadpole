// -- Kobengine Includes --
#include "KobengineEntryPoint.h"

// -- Tadpole Includes --
#include "EditorLayer.h"
#include "ImGuiContext.h"

namespace tadpole
{
	class TadpoleEditor final : public kobengine::Application
	{
	public:
		explicit TadpoleEditor(const pompeii::WindowSettings& windowSettings)
			: Application(windowSettings)
		{
			m_pLayerStack->PushLayer(
				std::make_unique<EditorLayer>(
					m_pRenderLayer->GetRenderer().get(),
					std::make_unique<ImGuiContext>(
						m_pWindow.get(),
						m_pRenderLayer->GetRenderer().get()
					))
			);
		}
		~TadpoleEditor() override
		{ }
	};
}

namespace kobengine
{
	Application* CreateApplication()
	{
		return new tadpole::TadpoleEditor(pompeii::WindowSettings("Tadpole Editor - Kobengine - Pompeii", true));
	}
}