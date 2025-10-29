// -- Tadpole Includes --
#include "EditorLayer.h"

// -- Kobengine Includes --
#include <iostream>

#include "IWindow.h"

//--------------------------------------------------
//    Constructor & Destructor
//--------------------------------------------------
tadpole::EditorLayer::EditorLayer(kobengine::IWindow* pWindow)
	: m_pWindow(pWindow)
{ }


//--------------------------------------------------
//    Loop
//--------------------------------------------------
void tadpole::EditorLayer::OnAttach()
{

}

void tadpole::EditorLayer::OnUpdate()
{

}

void tadpole::EditorLayer::OnDetach()
{

}

//--------------------------------------------------
//    Events
//--------------------------------------------------
void tadpole::EditorLayer::HandleImageRendered(const pompeii::Image& renderedImage)
{
	renderedImage;
	std::cout << "HELO";
}
