#ifndef TADPOLE_PANEL_VIEWPORT_H
#define TADPOLE_PANEL_VIEWPORT_H

// -- Tadpole Includes --
#include "IPanel.h"

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
		std::vector<VkDescriptorSet> m_vDescriptorSets{};
		pompeii::Sampler m_Sampler{};
		pompeii::Renderer* m_pRenderer{};
	};
}

#endif // TADPOLE_PANEL_VIEWPORT_H
