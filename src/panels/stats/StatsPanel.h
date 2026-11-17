#ifndef TADPOLE_PANEL_STATS_H
#define TADPOLE_PANEL_STATS_H

// -- Standard Library --
#include <array>

// -- Tadpole Includes --
#include "IPanel.h"

// -- Forward Declarations --
namespace pompeii
{
	class Renderer;
}

namespace tadpole
{
	//? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//? ~~    StatsPanel
	//? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	class StatsPanel : public IPanel
	{
	public:
		//--------------------------------------------------
		//    Constructor & Destructor
		//--------------------------------------------------
		explicit StatsPanel(pompeii::Renderer* pRenderer);

		//--------------------------------------------------
		//    Functionality
		//--------------------------------------------------
		void OnActivate() override;
		void OnImGuiRender() override;
		void OnDeactivate() override;

	private:
		// -- Sections --
		void DrawPerformance() const;
		void DrawSceneStats() const;
		void DrawVideoMemory() const;

		// -- Frame Time History --
		static constexpr int FRAME_HISTORY_SIZE{ 200 };
		std::array<float, FRAME_HISTORY_SIZE> m_vFrameTimesMs{};
		int m_FrameTimeOffset{ 0 };
		int m_FrameTimeCount{ 0 };

		// -- Renderer --
		pompeii::Renderer* m_pRenderer{};
	};
}

#endif // TADPOLE_PANEL_STATS_H
