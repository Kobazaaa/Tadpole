#ifndef TADPOLE_IMGUI_CONTEXT_H
#define TADPOLE_IMGUI_CONTEXT_H

// -- ImGui --
#include <imgui.h>

// -- Pompeii Includes --
#include "DescriptorPool.h"

// -- Forward Declarations --
namespace pompeii
{
	class IWindow;
	class Renderer;
}

namespace tadpole
{
    //? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //? ~~    ImGuiContext
    //? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    class ImGuiContext final
    {
    public:
        //--------------------------------------------------
        //    Constructor & Destructor
        //--------------------------------------------------
        explicit ImGuiContext(const pompeii::IWindow* pWindow, pompeii::Renderer* pRenderer);
        ~ImGuiContext();

		ImGuiContext(const ImGuiContext& other)					= delete;
		ImGuiContext(ImGuiContext&& other) noexcept				= delete;
		ImGuiContext& operator=(const ImGuiContext& other)		= delete;
		ImGuiContext& operator=(ImGuiContext&& other) noexcept	= delete;

		//--------------------------------------------------
		//    Functionality
		//--------------------------------------------------
		void BeginFrame() const;
		void EndFrame() const;

		void Destroy() const;

    private:
		void SetupImGuiStyle() const;

		pompeii::Renderer* m_pRenderer{};
		pompeii::DescriptorPool m_DescriptorPool{};
		mutable bool m_BeenDestroyed{ false };
    };
}

#endif // TADPOLE_IMGUI_CONTEXT_H
