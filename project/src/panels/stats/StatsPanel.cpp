// -- Tadpole Includes --
#include "StatsPanel.h"

// -- Standard Library --
#include <algorithm>

// -- Pompeii Includes --
#include "Renderer.h"

// -- Kobengine Includes --
#include "AssetManager.h"
#include "Camera.h"
#include "LightComponent.h"
#include "MeshRenderer.h"
#include "SceneManager.h"
#include "SceneObject.h"
#include "ServiceLocator.h"
#include "Timer.h"

using namespace kobengine;

//--------------------------------------------------
//    Constructor & Destructor
//--------------------------------------------------
tadpole::StatsPanel::StatsPanel(pompeii::Renderer* pRenderer)
	: IPanel("Stats")
	, m_pRenderer(pRenderer)
{}


//--------------------------------------------------
//    Functionality
//--------------------------------------------------
void tadpole::StatsPanel::OnActivate()
{
}
void tadpole::StatsPanel::OnImGuiRender()
{
	m_vFrameTimesMs[static_cast<size_t>(m_FrameTimeOffset)] = Timer::GetDeltaSeconds() * 1000.f;
	m_FrameTimeOffset = (m_FrameTimeOffset + 1) % FRAME_HISTORY_SIZE;
	m_FrameTimeCount = std::min(m_FrameTimeCount + 1, FRAME_HISTORY_SIZE);

	if (!IsOpen) return;
	ImGui::Begin(GetName().c_str(), &IsOpen);

	DrawPerformance();
	DrawSceneStats();
	DrawVideoMemory();

	ImGui::End();
}
void tadpole::StatsPanel::OnDeactivate()
{
}


//--------------------------------------------------
//    Sections
//--------------------------------------------------
void tadpole::StatsPanel::DrawPerformance() const
{
	ImGui::SeparatorText("Performance");

	float averageMs = 0.f;
	float maxMs = 0.f;
	for (int i = 0; i < m_FrameTimeCount; ++i)
	{
		averageMs += m_vFrameTimesMs[static_cast<size_t>(i)];
		maxMs = std::max(maxMs, m_vFrameTimesMs[static_cast<size_t>(i)]);
	}
	if (m_FrameTimeCount > 0)
		averageMs /= static_cast<float>(m_FrameTimeCount);

	ImGui::Text("FPS:        %.1f", averageMs > 0.f ? 1000.f / averageMs : 0.f);
	ImGui::Text("Frame Time: %.2f ms (max %.2f ms)", averageMs, maxMs);

	char overlay[32];
	snprintf(overlay, sizeof(overlay), "%.2f ms", averageMs);
	ImGui::PlotLines("##FrameTimes",
		m_vFrameTimesMs.data(), m_FrameTimeCount, m_FrameTimeOffset,
		overlay, 0.f, std::max(maxMs * 1.25f, 1.f),
		ImVec2(-FLT_MIN, 60.f));
}
void tadpole::StatsPanel::DrawSceneStats() const
{
	ImGui::SeparatorText("Scene");

	int totalObjects = 0;
	int activeObjects = 0;
	int lightCount = 0;
	int rendererCount = 0;
	int cameraCount = 0;
	for (const SceneObject* obj : ServiceLocator::Get<SceneManager>().GetActiveScene().GetAllObjects())
	{
		++totalObjects;
		if (obj->IsActive()) ++activeObjects;
		if (obj->HasComponent<LightComponent>()) ++lightCount;
		if (obj->HasComponent<MeshRenderer>()) ++rendererCount;
		if (obj->HasComponent<Camera>()) ++cameraCount;
	}

	const std::vector<pompeii::Mesh*> meshes = ServiceLocator::Get<AssetManager>().GetAllMeshes();
	const int meshCount = static_cast<int>(std::ranges::count_if(meshes, [](const pompeii::Mesh* pMesh) { return pMesh != nullptr; }));

	ImGui::Text("Objects:        %d (%d active)", totalObjects, activeObjects);
	ImGui::Text("Cameras:        %d", cameraCount);
	ImGui::Text("Lights:         %d", lightCount);
	ImGui::Text("Mesh Renderers: %d", rendererCount);
	ImGui::Text("Loaded Meshes:  %d", meshCount);
}
void tadpole::StatsPanel::DrawVideoMemory() const
{
	ImGui::SeparatorText("Video Memory");

	const pompeii::Context& context = m_pRenderer->GetContext();

	VmaBudget budgets[VK_MAX_MEMORY_HEAPS]{};
	vmaGetHeapBudgets(context.allocator, budgets);

	VkPhysicalDeviceMemoryProperties memProps{};
	vkGetPhysicalDeviceMemoryProperties(context.physicalDevice.GetHandle(), &memProps);

	for (uint32_t i = 0; i < memProps.memoryHeapCount; ++i)
	{
		const VmaBudget& budget = budgets[i];
		if (budget.budget == 0)
			continue;

		const double usedMB = static_cast<double>(budget.usage) / (1024.0 * 1024.0);
		const double budgetMB = static_cast<double>(budget.budget) / (1024.0 * 1024.0);
		const bool deviceLocal = (memProps.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) != 0;

		char label[128];
		snprintf(label, sizeof(label), "Heap %u%s: %.1f / %.1f MB", i, deviceLocal ? " (Device Local)" : "", usedMB, budgetMB);

		const float fraction = budget.budget > 0 ? static_cast<float>(static_cast<double>(budget.usage) / static_cast<double>(budget.budget)) : 0.f;
		ImGui::ProgressBar(fraction, ImVec2(-FLT_MIN, 0.f), label);
	}
}
