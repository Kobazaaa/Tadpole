#ifndef TADPOLE_PANEL_CONSOLE_H
#define TADPOLE_PANEL_CONSOLE_H

// -- Tadpole Includes --
#include "IPanel.h"

// -- Standard Library --
#include <string>
#include <deque>

// -- Kobengine Includes --
#include "Debug.h"

namespace tadpole
{
	//? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//? ~~    ConsolePanel
	//? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	class ConsolePanel : public IPanel
	{
	public:
		//--------------------------------------------------
		//    Constructor & Destructor
		//--------------------------------------------------
		explicit ConsolePanel();

		//--------------------------------------------------
		//    Functionality
		//--------------------------------------------------
		void OnActivate() override;
		void OnImGuiRender() override;
		void OnDeactivate() override;

	private:
		void LogMessage(const std::string& msg, kobengine::LogSeverity severity);

		bool m_AutoScroll{ true };
		bool m_ShowErrors{ true };
		bool m_ShowWarnings{ true };
		bool m_ShowInfo{ true };

		struct ConsoleItem
		{
			std::string message;
			kobengine::LogSeverity severity;
			uint32_t count;
		};
		std::deque<ConsoleItem> m_vLogItems;
		uint32_t m_MaxLogCount{ 64 };
	};
}
#endif // TADPOLE_PANEL_CONSOLE_H
