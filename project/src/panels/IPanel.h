#ifndef TADPOLE_INTERFACE_PANEL_H
#define TADPOLE_INTERFACE_PANEL_H

// -- ImGui --
#include "imgui.h"
#include "imgui_impl_vulkan.h"

namespace tadpole
{
    //? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //? ~~    IPanel
    //? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    class IPanel
    {
    public:
        //--------------------------------------------------
        //    Constructor & Destructor
        //--------------------------------------------------
        virtual ~IPanel() = default;

		//--------------------------------------------------
		//    Functionality
		//--------------------------------------------------
        virtual void OnActivate() = 0;
        virtual void OnImGuiRender() = 0;
        virtual void OnDeactivate() = 0;
    };
}

#endif // TADPOLE_INTERFACE_PANEL_H
