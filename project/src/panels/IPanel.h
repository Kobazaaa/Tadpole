#ifndef TADPOLE_INTERFACE_PANEL_H
#define TADPOLE_INTERFACE_PANEL_H

// -- Standard Library --
#include <string>

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
        explicit IPanel(std::string name)
	        : m_Name(std::move(name))
        {}
        virtual ~IPanel() = default;

		//--------------------------------------------------
		//    Functionality
		//--------------------------------------------------
        virtual void OnActivate() = 0;
        virtual void OnImGuiRender() = 0;
        virtual void OnDeactivate() = 0;

		//--------------------------------------------------
		//    Accessors
		//--------------------------------------------------
        const std::string& GetName() const { return m_Name; }
        bool IsOpen = true;

    private:
        std::string m_Name{};
    };
}

#endif // TADPOLE_INTERFACE_PANEL_H
