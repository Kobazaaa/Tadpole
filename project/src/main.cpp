// -- Kobengine Includes --
#include "KobengineEntryPoint.h"

// -- Tadpole Includes --
#include "EditorLayer.h"

namespace tadpole
{
	class TadpoleEditor final : public kobengine::Application
	{
	public:
		explicit TadpoleEditor(const kobengine::WindowSettings& windowSettings)
			: Application(windowSettings)
		{
			auto iLayer = m_pLayerStack->PushLayer(std::make_unique<EditorLayer>(m_pWindow.get()));
			auto editorLayer = dynamic_cast<EditorLayer*>(iLayer);
			m_pRenderLayer->OnImageRendered.AddListener(editorLayer, &EditorLayer::HandleImageRendered);
		}
		~TadpoleEditor() override { }
	};
}

namespace kobengine
{
	Application* CreateApplication()
	{
		return new tadpole::TadpoleEditor(WindowSettings("Tadpole Editor - Kobengine - Pompeii", 800, 600));
	}
}