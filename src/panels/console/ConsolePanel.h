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
		//--------------------------------------------------
		//    Helpers
		//--------------------------------------------------
		struct ConsoleItem
		{
			std::string message;
			kobengine::LogSeverity severity;
			float time;
		};
		struct DisplayItem
		{
			const ConsoleItem* pItem;
			uint32_t count;
		};

		void LogMessage(const std::string& msg, kobengine::LogSeverity severity);
		void DrawToolbar();
		void DrawLogItems(const std::vector<DisplayItem>& items) const;
		std::vector<DisplayItem> BuildDisplayList() const;
		bool PassesFilter(const ConsoleItem& item) const;
		void CopyToClipboard(const std::vector<DisplayItem>& items) const;

		static const char* SeverityTag(kobengine::LogSeverity severity);
		static ImVec4 SeverityColor(kobengine::LogSeverity severity);

		// -- Options --
		bool m_AutoScroll{ true };
		bool m_Collapse{ true };
		bool m_ShowErrors{ true };
		bool m_ShowWarnings{ true };
		bool m_ShowInfo{ true };
		char m_SearchBuffer[128]{};

		// -- Log Storage --
		std::deque<ConsoleItem> m_vLogItems{};
		static constexpr size_t MAX_LOG_COUNT{ 999 };
	};
}
#endif // TADPOLE_PANEL_CONSOLE_H
