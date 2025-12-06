#ifndef TADPOLE_PANEL_INSPECTOR_H
#define TADPOLE_PANEL_INSPECTOR_H

// -- Tadpole Includes --
#include "IPanel.h"

// -- Forward Declares --
namespace kobengine
{
	class SceneObject;
}

namespace tadpole
{
	//? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//? ~~    InspectorPanel
	//? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	class InspectorPanel : public IPanel
	{
	public:
		//--------------------------------------------------
		//    Constructor & Destructor
		//--------------------------------------------------
		explicit InspectorPanel();

		//--------------------------------------------------
		//    Functionality
		//--------------------------------------------------
		void OnActivate() override;
		void OnImGuiRender() override;
		void OnDeactivate() override;

		void SetSelectedObject(kobengine::SceneObject* pObject);

	private:
		kobengine::SceneObject* m_pSelectedObject{};
	};
}

#endif // TADPOLE_PANEL_INSPECTOR_H
