// -- Tadpole Includes --
#include "HierarchyPanel.h"
#include "EditorHelpers.h"

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
	: IPanel("Hierarchy")
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
    if (!IsOpen) return;

    ImGui::Begin(GetName().c_str(), &IsOpen, ImGuiWindowFlags_None);

    const std::vector<SceneObject*> allObjects = ServiceLocator::Get<SceneManager>().GetActiveScene().GetAllObjects();
    for (SceneObject* obj : allObjects)
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

    // -- Click Empty Space to Deselect --
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered())
        SelectObject(nullptr);

    DrawCreateContextMenu();
    HandleShortcuts();

    // -- Footer --
    ImGui::Separator();
    ImGui::TextDisabled("%d object%s", static_cast<int>(allObjects.size()), allObjects.size() == 1 ? "" : "s");

    ImGui::End();
}

void tadpole::HierarchyPanel::OnDeactivate()
{
}

//--------------------------------------------------
//    Drawing
//--------------------------------------------------
void tadpole::HierarchyPanel::DrawSceneObjectNode(SceneObject* sceneObj)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen |
        ImGuiTreeNodeFlags_OpenOnArrow |
        ImGuiTreeNodeFlags_FramePadding |
        ImGuiTreeNodeFlags_SpanAvailWidth;
    if (m_pSelectedObject == sceneObj) flags |= ImGuiTreeNodeFlags_Selected;
    if (sceneObj->transform->GetAllChildren().empty()) flags |= ImGuiTreeNodeFlags_Leaf;

    ImGui::PushID(sceneObj);
    const bool isActive = sceneObj->IsActive();

    if (!isActive) ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
	const bool nodeOpen = ImGui::TreeNodeEx(sceneObj->name.c_str(), flags, "%s", sceneObj->name.c_str());
    if (!isActive) ImGui::PopStyleColor();

    if (ImGui::IsItemClicked())
        SelectObject(sceneObj);

    // -- Right Click Context Menu --
    DrawObjectContextMenu(sceneObj);

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
            if (dropped && dropped != sceneObj && !checks::IsDescendantOf(sceneObj->transform.get(), dropped->transform.get()))
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
void tadpole::HierarchyPanel::DrawObjectContextMenu(SceneObject* sceneObj)
{
    if (!ImGui::BeginPopupContextItem())
        return;

    if (ImGui::MenuItem("Duplicate", "Ctrl+D")) SelectObject(&actions::Duplicate(*sceneObj));
    if (ImGui::MenuItem("Delete", "Del")) DeleteObject(sceneObj);

    ImGui::Separator();

    bool active = sceneObj->IsActive();
    if (ImGui::MenuItem("Active", nullptr, &active))
        sceneObj->SetActive(active);

    ImGui::Separator();

    if (ImGui::MenuItem("Create Empty Child"))
    {
        SceneObject& child = actions::CreateEmpty();
        child.transform->SetParent(sceneObj->transform.get(), false);
        SelectObject(&child);
    }

    ImGui::EndPopup();
}
void tadpole::HierarchyPanel::DrawCreateContextMenu()
{
    if (!ImGui::BeginPopupContextWindow("HierarchyCreate", ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
        return;

    ImGui::TextDisabled("Create");
    ImGui::Separator();

    if (ImGui::MenuItem("Empty Object"))
        SelectObject(&actions::CreateEmpty());
    if (ImGui::MenuItem("Camera"))
        SelectObject(&actions::CreateCamera());
    if (ImGui::MenuItem("Directional Light"))
        SelectObject(&actions::CreateLight(pompeii::LightType::Directional));
    if (ImGui::MenuItem("Point Light"))
        SelectObject(&actions::CreateLight(pompeii::LightType::Point));

    ImGui::EndPopup();
}

//--------------------------------------------------
//    Actions
//--------------------------------------------------
void tadpole::HierarchyPanel::SelectObject(SceneObject* sceneObj)
{
    m_pSelectedObject = sceneObj;
    OnSelectedObjectChanged.Invoke(m_pSelectedObject);
}
void tadpole::HierarchyPanel::DeleteObject(SceneObject* sceneObj)
{
    if (!sceneObj) return;
    if (!actions::DestroyObject(*sceneObj)) return;
    if (sceneObj == m_pSelectedObject) SelectObject(nullptr);
}
void tadpole::HierarchyPanel::HandleShortcuts()
{
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) return;
    if (!m_pSelectedObject) return;

    if (ImGui::IsKeyPressed(ImGuiKey_Delete)) DeleteObject(m_pSelectedObject);
    else if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_D)) SelectObject(&actions::Duplicate(*m_pSelectedObject));
}
