#ifndef TADPOLE_PANEL_VIEWPORT_H
#define TADPOLE_PANEL_VIEWPORT_H

// -- Standard Library --
#include <string>

// -- Tadpole Includes --
#include "IPanel.h"
#include "glm/vec2.hpp"

// -- Pompeii Includes --
#include "DescriptorSet.h"
#include "Sampler.h"

// -- Forward Declarations --
namespace pompeii
{
	class Renderer;
}

namespace tadpole
{
	//? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//? ~~    AspectPreset
	//? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	struct AspectPreset
	{
		std::string name;
		uint32_t width;
		uint32_t height;
		bool isFree;
	};

	//? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//? ~~    ViewportPanel
	//? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	class ViewportPanel : public IPanel
	{
	public:
		//--------------------------------------------------
		//    Constructor & Destructor
		//--------------------------------------------------
		explicit ViewportPanel(pompeii::Renderer* pRenderer);

		//--------------------------------------------------
		//    Functionality
		//--------------------------------------------------
		void OnActivate() override;
		void OnImGuiRender() override;
		void OnDeactivate() override;

	private:
		// helpers
		void DrawToolbar();
		void DrawBuiltInPresets();
		void DrawCustomPresets();
		void DrawAddPreset();
		void DrawStatsOverlay(const ImVec2& imageScreenPos) const;

		void DrawOutput();
		void ResizeOutput(uint32_t w, uint32_t h);

		// stats
		bool m_ShowStatsOverlay{ false };
		float m_SmoothedDeltaTime{ 1.f / 60.f };

		// aspect
		std::vector<AspectPreset> m_vPresets = {
			{ .name = "Free Aspect",	.width = 1920, .height = 1920,  .isFree = true  },
			{ .name = "Full HD",		.width = 1920, .height = 1080,	.isFree = false },
			{ .name = "QHD",			.width = 2560, .height = 1440,	.isFree = false },
			{ .name = "4K UHD",			.width = 3840, .height = 2160,	.isFree = false },
			{ .name = "WFHD",			.width = 2560, .height = 1080,	.isFree = false },
			{ .name = "Square",			.width = 1080, .height = 1080,	.isFree = false }
		};
		static constexpr int BUILTIN_COUNT = 6;
		int m_SelectedPreset = 0;


		// image
		std::vector<VkDescriptorSet> m_vDescriptorSets{};
		pompeii::Sampler m_Sampler{};
		pompeii::Renderer* m_pRenderer{};
	};
}

#endif // TADPOLE_PANEL_VIEWPORT_H
