#include "KobengineEntryPoint.h"

namespace tadpole
{
	class TadpoleEditor final : public kobengine::Application
	{
	public:
		explicit TadpoleEditor(const kobengine::WindowSettings& windowSettings)
			: Application(windowSettings)
		{ }
		~TadpoleEditor() override { }
	};
}

namespace kobengine
{
	Application* CreateApplication()
	{
		return new ::tadpole::TadpoleEditor(WindowSettings("Tadpole Editor", 800, 600));
	}
}