#ifndef TADPOLE_PANEL_HIERARCHY_H
#define TADPOLE_PANEL_HIERARCHY_H

// -- Tadpole Includes --
#include "IPanel.h"

// -- Kobengine Includes --
#include "Event.h"

// -- Forward Declares --
namespace kobengine
{
	class SceneObject;
}

namespace tadpole
{
	//? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//? ~~    HierarchyPanel
	//? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	class HierarchyPanel : public IPanel
	{
	public:
		//--------------------------------------------------
		//    Constructor & Destructor
		//--------------------------------------------------
		explicit HierarchyPanel();

		//--------------------------------------------------
		//    Functionality
		//--------------------------------------------------
		void OnActivate() override;
		void OnImGuiRender() override;
		void OnDeactivate() override;

		//--------------------------------------------------
		//    Event
		//--------------------------------------------------
		kobengine::Event<kobengine::SceneObject*> OnSelectedObjectChanged;

	private:
		void DrawSceneObjectNode(kobengine::SceneObject* sceneObj);
		kobengine::SceneObject* m_pSelectedObject{};
	};
}

#endif // TADPOLE_PANEL_HIERARCHY_H
