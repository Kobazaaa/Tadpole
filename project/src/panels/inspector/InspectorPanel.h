#ifndef TADPOLE_PANEL_INSPECTOR_H
#define TADPOLE_PANEL_INSPECTOR_H

// -- Tadpole Includes --
#include "IPanel.h"

// -- Math Includes --
#include "glm/glm.hpp"

// -- Forward Declares --
namespace kobengine
{
	class SceneObject;
	class Camera;
	class LightComponent;
	class MeshFilter;
	class MeshRenderer;
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
		// -- Sections --
		void DrawHeader() const;
		void DrawTransform();
		void DrawCamera(kobengine::Camera* pCamera);
		void DrawLight(kobengine::LightComponent* pLight) const;
		void DrawMeshFilter(kobengine::MeshFilter* pMeshFilter);
		void DrawMeshRenderer(kobengine::MeshRenderer* pMeshRenderer) const;
		void DrawAddComponent() const;

		// -- Helpers --
		static bool DrawVec3Control(const char* label, glm::vec3& values, float resetValue, float speed);
		void HandleLoadMeshDialog() const;
		void ReRegisterMeshRenderer() const;

		// -- Data --
		kobengine::SceneObject* m_pSelectedObject{};
		bool m_EditLocalSpace{ true };


		static constexpr auto LOAD_MESH_DIALOG_KEY = "InspectorLoadMesh";
	};
}

#endif // TADPOLE_PANEL_INSPECTOR_H
