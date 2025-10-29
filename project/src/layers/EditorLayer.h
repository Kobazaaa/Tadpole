#ifndef EDITOR_LAYER_H
#define EDITOR_LAYER_H

// -- Kobengine Includes --
#include "ILayer.h"

// -- Forward Declares --
namespace kobengine
{
	class IWindow;
}
namespace pompeii
{
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
		explicit EditorLayer(kobengine::IWindow* pWindow);

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
		kobengine::IWindow* m_pWindow{};
	};
}

#endif // RENDER_LAYER_H