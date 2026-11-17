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
		// -- Drawing --
		void DrawSceneObjectNode(kobengine::SceneObject* sceneObj);
		void DrawObjectContextMenu(kobengine::SceneObject* sceneObj);
		void DrawCreateContextMenu();

		// -- Actions --
		void SelectObject(kobengine::SceneObject* sceneObj);
		void DeleteObject(kobengine::SceneObject* sceneObj);
		void HandleShortcuts();

		// -- Selection --
		kobengine::SceneObject* m_pSelectedObject{};
	};
}

#endif // TADPOLE_PANEL_HIERARCHY_H
