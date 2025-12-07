// -- Tadpole Includes --
#include "HierarchyPanel.h"

// -- Kobengine Includes --
#include "imgui_internal.h"
#include "SceneManager.h"
#include "SceneObject.h"
#include "ServiceLocator.h"
using namespace kobengine;

//--------------------------------------------------
//    Constructor & Destructor
//--------------------------------------------------
tadpole::HierarchyPanel::HierarchyPanel()
{
}


//--------------------------------------------------
//    Functionality
//--------------------------------------------------
void tadpole::HierarchyPanel::OnActivate()
{
}

void tadpole::HierarchyPanel::OnImGuiRender()
{
    ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_None);

    for (SceneObject* obj : ServiceLocator::Get<SceneManager>().GetActiveScene().GetAllObjects())
        if (!obj->transform->GetParent())
            DrawSceneObjectNode(obj);

    // -- Drag & Drop Unparenting --
    if (ImGui::BeginDragDropTargetCustom(ImGui::GetCurrentWindow()->ContentRegionRect, ImGui::GetCurrentWindow()->ID))
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PAYLOAD_SCENE_OBJECT"))
            if (const SceneObject* dropped = *static_cast<SceneObject**>(payload->Data))
                dropped->transform->SetParent(nullptr, true);
        ImGui::EndDragDropTarget();
    }

    ImGui::End();
}

void tadpole::HierarchyPanel::OnDeactivate()
{
}

//--------------------------------------------------
//    Helpers
//--------------------------------------------------
void tadpole::HierarchyPanel::DrawSceneObjectNode(SceneObject* sceneObj)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen |
        ImGuiTreeNodeFlags_OpenOnArrow |
        ImGuiTreeNodeFlags_FramePadding |
        ImGuiTreeNodeFlags_SpanAvailWidth;
    if (m_pSelectedObject == sceneObj)
        flags |= ImGuiTreeNodeFlags_Selected;
    if (sceneObj->transform->GetAllChildren().empty())
        flags |= ImGuiTreeNodeFlags_Leaf;

    ImGui::PushID(sceneObj);
    const bool isActive = sceneObj->IsActive();

    if (!isActive) ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
	const bool nodeOpen = ImGui::TreeNodeEx(sceneObj->name.c_str(), flags, "%s", sceneObj->name.c_str());
    if (!isActive) ImGui::PopStyleColor();


    if (ImGui::IsItemClicked())
    {
        m_pSelectedObject = sceneObj;
        OnSelectedObjectChanged.Invoke(m_pSelectedObject);
    }

    // -- Right Click Context Menu --
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Delete"))
        {
            if (sceneObj == m_pSelectedObject)
            {
                m_pSelectedObject = nullptr;
                OnSelectedObjectChanged.Invoke(m_pSelectedObject);
            }
            sceneObj->Destroy();
        }
        ImGui::EndPopup();
    }

    // -- Drag & Drop Source --
    if (ImGui::BeginDragDropSource())
    {
        SceneObject* payload = sceneObj;
        ImGui::SetDragDropPayload("PAYLOAD_SCENE_OBJECT", &payload, sizeof(SceneObject*));
        ImGui::Text("%s", sceneObj->name.c_str());
        ImGui::EndDragDropSource();
    }

    // -- Drag & Drop Parenting --
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("PAYLOAD_SCENE_OBJECT"))
        {
            SceneObject* dropped = *static_cast<SceneObject**>(payload->Data);
            if (dropped && dropped != sceneObj)
                dropped->transform->SetParent(sceneObj->transform.get(), true);
        }
        ImGui::EndDragDropTarget();
    }

    // -- Draw Children --
    if (nodeOpen)
    {
        for (const Transform* child : sceneObj->transform->GetAllChildren())
            DrawSceneObjectNode(child->GetSceneObject());
        ImGui::TreePop();
    }
    ImGui::PopID();
}
