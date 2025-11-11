#ifndef EDITOR_LAYER_H
#define EDITOR_LAYER_H

// -- Kobengine Includes --
#include "ILayer.h"

// -- Forward Declares --
namespace pompeii
{
	class IWindow;
	class Image;
	class Renderer;
}

namespace tadpole
{
	//? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//? ~~	  EditorLayer	
	//? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	class EditorLayer final : public kobengine::ILayer
	{
	public:
		//--------------------------------------------------
		//    Constructor & Destructor
		//--------------------------------------------------
		explicit EditorLayer(pompeii::IWindow* pWindow);

		//--------------------------------------------------
		//    Loop
		//--------------------------------------------------
		void OnAttach() override;
		void OnUpdate() override;
		void OnDetach() override;

		//--------------------------------------------------
		//    Events
		//--------------------------------------------------
		void HandleImageRendered(const pompeii::Image& renderedImage);

	private:
		pompeii::IWindow* m_pWindow{};
	};
}

#endif // RENDER_LAYER_H