// -- Tadpole Includes --
#include "InspectorPanel.h"

#include "SceneObject.h"

//--------------------------------------------------
//    Constructor & Destructor
//--------------------------------------------------
tadpole::InspectorPanel::InspectorPanel()
{
}


//--------------------------------------------------
//    Functionality
//--------------------------------------------------
void tadpole::InspectorPanel::OnActivate()
{
}

void tadpole::InspectorPanel::OnImGuiRender()
{
	ImGui::Begin("Inspector", nullptr, ImGuiWindowFlags_None);

	if (!m_pSelectedObject)
	{
		ImGui::End();
		return;
	}
	
	ImGui::Text(m_pSelectedObject->name.c_str());

	ImGui::End();
}

void tadpole::InspectorPanel::OnDeactivate()
{
}

//--------------------------------------------------
//    Helper
//--------------------------------------------------
void tadpole::InspectorPanel::SetSelectedObject(kobengine::SceneObject* pObject)
{
	m_pSelectedObject = pObject;
}

