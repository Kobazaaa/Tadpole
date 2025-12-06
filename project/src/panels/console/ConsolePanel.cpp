// -- Tadpole Includes --
#include "ConsolePanel.h"

// -- Kobengine Includes --
#include "Event.h"
#include "Timer.h"

//--------------------------------------------------
//    Constructor & Destructor
//--------------------------------------------------
tadpole::ConsolePanel::ConsolePanel()
{ }

//--------------------------------------------------
//    Functionality
//--------------------------------------------------
void tadpole::ConsolePanel::OnActivate()
{
    kobengine::Debug::OnMessageLogged.AddListener(this, &ConsolePanel::LogMessage);
	kobengine::Debug::Log("Welcome to the Console!");
}

void tadpole::ConsolePanel::OnImGuiRender()
{
	ImGui::Begin("Console");

    if (ImGui::Button("Clear"))
		m_vLogItems.clear();

	ImGui::SameLine();
	ImGui::Checkbox("Auto Scroll", &m_AutoScroll);
	ImGui::SameLine();
	ImGui::Checkbox("Info", &m_ShowInfo);
	ImGui::SameLine();
	ImGui::Checkbox("Warnings", &m_ShowWarnings);
	ImGui::SameLine();
	ImGui::Checkbox("Errors", &m_ShowErrors);

    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true);
	for (const auto& [message, severity, count] : m_vLogItems)
	{
		// filter
		if ((severity == kobengine::LogSeverity::Error && !m_ShowErrors) ||
			(severity == kobengine::LogSeverity::Warning && !m_ShowWarnings) ||
			(severity == kobengine::LogSeverity::Normal && !m_ShowInfo))
			continue;

		// color
		ImVec4 color;
		switch (severity)
		{
		case kobengine::LogSeverity::Error:		color = ImVec4(1, 0.3f, 0.3f, 1); break;
		case kobengine::LogSeverity::Warning:	color = ImVec4(1, 1, 0.4f, 1);    break;
		default:								color = ImVec4(1, 1, 1, 1);       break;
		}

		// txt
		float maxTextWidth = ImGui::GetWindowWidth() - 50;
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + maxTextWidth);
		ImGui::TextUnformatted(message.c_str());
		ImGui::PopTextWrapPos();
		if (count > 0)
		{
			ImGui::SameLine(maxTextWidth);
			ImGui::Text("[%d]", count + 1);
		}
		ImGui::PopStyleColor();
	}

	bool scrollAtBottom = (ImGui::GetScrollY() >= ImGui::GetScrollMaxY());
	if (m_AutoScroll && scrollAtBottom)
		ImGui::SetScrollHereY(1.0f);
	ImGui::EndChild();

    ImGui::End();
}

void tadpole::ConsolePanel::OnDeactivate()
{
	kobengine::Debug::OnMessageLogged.RemoveListener(this, &ConsolePanel::LogMessage);
}


//--------------------------------------------------
//    Helper
//--------------------------------------------------
void tadpole::ConsolePanel::LogMessage(const std::string& msg, kobengine::LogSeverity severity)
{
	const char* prefix;
	switch (severity)
	{
	case kobengine::LogSeverity::Normal:
		prefix = "[INFO]  ";
		break;
	case kobengine::LogSeverity::Warning:
		prefix = "[WARN]  ";
		break;
	case kobengine::LogSeverity::Error:
		prefix = "[ERROR] ";
		break;
	default:
		prefix = "[INFO]  ";
		break;
	}

	ConsoleItem newItem = ConsoleItem{ prefix + msg, severity, 0 };
	auto it = std::find_if(m_vLogItems.begin(), m_vLogItems.end(), [&newItem](const ConsoleItem& item)
	{
			return item.message == newItem.message && item.severity == newItem.severity;
	});
	if (it != m_vLogItems.end()) ++it->count;
	else m_vLogItems.push_back(newItem);

	if (m_vLogItems.size() >= m_MaxLogCount)
		m_vLogItems.pop_front();
}

