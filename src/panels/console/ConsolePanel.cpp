// -- Tadpole Includes --
#include "ConsolePanel.h"

// -- Standard Library --
#include <algorithm>

// -- ImGui --
#include "imgui_internal.h"

// -- Kobengine Includes --
#include "EditorHelpers.h"
#include "Event.h"
#include "Timer.h"

//--------------------------------------------------
//    Constructor & Destructor
//--------------------------------------------------
tadpole::ConsolePanel::ConsolePanel()
	: IPanel("Console")
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
	if (!IsOpen)
		return;

	ImGui::Begin(GetName().c_str(), &IsOpen);

	DrawToolbar();

	const std::vector<DisplayItem> displayItems = BuildDisplayList();

    ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), ImGuiChildFlags_Borders);
	DrawLogItems(displayItems);

	const bool scrollAtBottom = (ImGui::GetScrollY() >= ImGui::GetScrollMaxY());
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
//    Helpers
//--------------------------------------------------
void tadpole::ConsolePanel::LogMessage(const std::string& msg, kobengine::LogSeverity severity)
{
	m_vLogItems.push_back(ConsoleItem{ msg, severity, kobengine::Timer::GetTotalTimeSeconds() });
	if (m_vLogItems.size() > MAX_LOG_COUNT)
		m_vLogItems.pop_front();
}
void tadpole::ConsolePanel::DrawToolbar()
{
	if (ImGui::Button("Clear")) m_vLogItems.clear();
	ImGui::SameLine();
	if (ImGui::Button("Copy")) CopyToClipboard(BuildDisplayList());

	ImGui::SameLine();
	ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
	ImGui::SameLine();

	ImGui::Checkbox("Collapse", &m_Collapse);
	ImGui::SameLine();
	ImGui::Checkbox("Auto Scroll", &m_AutoScroll);

	ImGui::SameLine();
	ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
	ImGui::SameLine();

	// -- Severity Filters (with counts) --
	uint32_t infoCount = 0, warningCount = 0, errorCount = 0;
	for (const ConsoleItem& item : m_vLogItems)
	{
		switch (item.severity)
		{
		case kobengine::LogSeverity::Error:		++errorCount;	break;
		case kobengine::LogSeverity::Warning:	++warningCount;	break;
		default:								++infoCount;	break;
		}
	}

	char label[32];
	snprintf(label, sizeof(label), "Info (%u)###Info", infoCount);
	ImGui::Checkbox(label, &m_ShowInfo);
	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Text, SeverityColor(kobengine::LogSeverity::Warning));
	snprintf(label, sizeof(label), "Warnings (%u)###Warnings", warningCount);
	ImGui::Checkbox(label, &m_ShowWarnings);
	ImGui::PopStyleColor();
	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Text, SeverityColor(kobengine::LogSeverity::Error));
	snprintf(label, sizeof(label), "Errors (%u)###Errors", errorCount);
	ImGui::Checkbox(label, &m_ShowErrors);
	ImGui::PopStyleColor();

	// -- Search --
	ImGui::SameLine();
	ImGui::SetNextItemWidth(-FLT_MIN);
	ImGui::InputTextWithHint("##ConsoleSearch", "Search...", m_SearchBuffer, sizeof(m_SearchBuffer));
}
void tadpole::ConsolePanel::DrawLogItems(const std::vector<DisplayItem>& items) const
{
	for (const DisplayItem& displayItem : items)
	{
		const ConsoleItem& item = *displayItem.pItem;

		// timestamp
		ImGui::TextDisabled("[%8.2f]", item.time);
		ImGui::SameLine();

		// message
		const float maxTextWidth = ImGui::GetWindowWidth() - 60.f;
		ImGui::PushStyleColor(ImGuiCol_Text, SeverityColor(item.severity));
		ImGui::PushTextWrapPos(maxTextWidth);
		ImGui::TextUnformatted(item.message.c_str());
		ImGui::PopTextWrapPos();
		ImGui::PopStyleColor();

		// collapse count badge
		if (displayItem.count > 1)
		{
			ImGui::SameLine(maxTextWidth);
			ImGui::TextDisabled("[%u]", displayItem.count);
		}
	}

	if (items.empty())
		ImGui::TextDisabled(m_vLogItems.empty() ? "No messages." : "No messages match the current filters.");
}
std::vector<tadpole::ConsolePanel::DisplayItem> tadpole::ConsolePanel::BuildDisplayList() const
{
	std::vector<DisplayItem> result;
	result.reserve(m_vLogItems.size());

	if (!m_Collapse)
	{
		for (const ConsoleItem& item : m_vLogItems)
			if (PassesFilter(item))
				result.push_back(DisplayItem{ &item, 1 });
		return result;
	}

	std::unordered_map<std::string, size_t> indexLookup;
	for (const ConsoleItem& item : m_vLogItems)
	{
		if (!PassesFilter(item))
			continue;

		std::string key = std::to_string(static_cast<int>(item.severity)) + item.message;
		if (const auto it = indexLookup.find(key); it != indexLookup.end())
		{
			++result[it->second].count;
		}
		else
		{
			indexLookup.emplace(std::move(key), result.size());
			result.push_back(DisplayItem
				{
					.pItem = &item,
					.count = 1
				});
		}
	}
	return result;
}
bool tadpole::ConsolePanel::PassesFilter(const ConsoleItem& item) const
{
	if ((item.severity == kobengine::LogSeverity::Error && !m_ShowErrors) ||
		(item.severity == kobengine::LogSeverity::Warning && !m_ShowWarnings) ||
		(item.severity == kobengine::LogSeverity::Normal && !m_ShowInfo))
		return false;
	return filters::ContainsCaseInsensitive(item.message, m_SearchBuffer);
}
void tadpole::ConsolePanel::CopyToClipboard(const std::vector<DisplayItem>& items) const
{
	std::string text;
	for (const DisplayItem& displayItem : items)
	{
		text += SeverityTag(displayItem.pItem->severity);
		text += displayItem.pItem->message;
		if (displayItem.count > 1)
			text += " [x" + std::to_string(displayItem.count) + "]";
		text += '\n';
	}
	ImGui::SetClipboardText(text.c_str());
}

const char* tadpole::ConsolePanel::SeverityTag(kobengine::LogSeverity severity)
{
	switch (severity)
	{
	case kobengine::LogSeverity::Error:		return "[ERROR] ";
	case kobengine::LogSeverity::Warning:	return "[WARN]  ";
	case kobengine::LogSeverity::Normal:	return "[INFO]  ";
	default:								return "[INFO]  ";
	}
}
ImVec4 tadpole::ConsolePanel::SeverityColor(kobengine::LogSeverity severity)
{
	switch (severity)
	{
	case kobengine::LogSeverity::Error:		return { 1.f, 0.35f, 0.35f, 1.f };
	case kobengine::LogSeverity::Warning:	return { 1.f, 0.85f, 0.35f, 1.f };
	case kobengine::LogSeverity::Normal:	return { 0.85f, 0.85f, 0.85f, 1.f };
	default:								return { 0.85f, 0.85f, 0.85f, 1.f };
	}
}
