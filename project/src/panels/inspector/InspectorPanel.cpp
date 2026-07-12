// -- Tadpole Includes --
#include "InspectorPanel.h"

// -- ImGui --
#include "ImGuiFileDialog.h"

// -- Kobengine Includes --
#include "AssetManager.h"
#include "Camera.h"
#include "Debug.h"
#include "EditorHelpers.h"
#include "LightComponent.h"
#include "LightingSystem.h"
#include "MeshFilter.h"
#include "MeshRenderer.h"
#include "RenderSystem.h"
#include "Scene.h"
#include "SceneObject.h"
#include "ServiceLocator.h"

using namespace kobengine;

//--------------------------------------------------
//    Constructor & Destructor
//--------------------------------------------------
tadpole::InspectorPanel::InspectorPanel()
	: IPanel("Inspector")
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
	if (!IsOpen) return;

	ImGui::Begin(GetName().c_str(), &IsOpen, ImGuiWindowFlags_None);
	if (!m_pSelectedObject)
	{
		ImGui::TextDisabled("Select an object in the Hierarchy to inspect it.");
		ImGui::End();
		return;
	}

	DrawHeader();
	ImGui::Separator();

	DrawTransform();

	Camera* pCamera{};
	if (m_pSelectedObject->TryGetComponent(pCamera))
		DrawCamera(pCamera);

	LightComponent* pLight{};
	if (m_pSelectedObject->TryGetComponent(pLight))
		DrawLight(pLight);

	MeshFilter* pMeshFilter{};
	if (m_pSelectedObject->TryGetComponent(pMeshFilter))
		DrawMeshFilter(pMeshFilter);

	MeshRenderer* pMeshRenderer{};
	if (m_pSelectedObject->TryGetComponent(pMeshRenderer))
		DrawMeshRenderer(pMeshRenderer);

	DrawAddComponent();
	HandleLoadMeshDialog();

	ImGui::End();
}

void tadpole::InspectorPanel::OnDeactivate()
{
}

//--------------------------------------------------
//    Sections
//--------------------------------------------------
void tadpole::InspectorPanel::DrawHeader() const
{
	// --- Enabled toggle ---
	bool enabled = m_pSelectedObject->IsActive();
	if (ImGui::Checkbox("##Enabled", &enabled))
		m_pSelectedObject->SetActive(enabled);
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("Active");

	// --- Object name ---
	ImGui::SameLine();
	char nameBuffer[128];
	strcpy_s(nameBuffer, m_pSelectedObject->name.c_str());
	ImGui::SetNextItemWidth(-FLT_MIN);
	if (ImGui::InputText("##Name", nameBuffer, sizeof(nameBuffer)))
		m_pSelectedObject->name = nameBuffer;
}
void tadpole::InspectorPanel::DrawTransform()
{
	if (!ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		return;

	auto& transform = m_pSelectedObject->transform;
	const bool hasParent = transform->GetParent() != nullptr;
	const bool useLocal = m_EditLocalSpace || !hasParent;

	glm::vec3 pos = useLocal ? transform->GetLocalPosition() : transform->GetPosition();
	glm::vec3 rot = useLocal ? transform->GetLocalEulerAngles() : transform->GetEulerAngles();
	glm::vec3 scale = useLocal ? transform->GetLocalScale() : transform->GetScale();

	if (DrawVec3Control("Position", pos, 0.f, 0.1f))
		useLocal ? transform->SetLocalPosition(pos) : transform->SetPosition(pos);
	if (DrawVec3Control("Rotation", rot, 0.f, 0.1f))
		useLocal ? transform->SetLocalEulerAngles(rot) : transform->SetEulerAngles(rot);
	if (DrawVec3Control("Scale", scale, 1.f, 0.05f))
		useLocal ? transform->SetLocalScale(scale) : transform->SetScale(scale);

	if (hasParent)
		ImGui::Checkbox("Edit Local Space", &m_EditLocalSpace);

	if (scale.x <= 0.f || scale.y <= 0.f || scale.z <= 0.f)
		ImGui::TextColored(ImVec4(1.f, 0.8f, 0.2f, 1.f), "Warning: zero or negative scale is undefined.");
}
void tadpole::InspectorPanel::DrawCamera(Camera* pCamera)
{
	ImGui::PushID(pCamera);
	if (ImGui::CollapsingHeader(pCamera->name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ServiceLocator::Get<RenderSystem>().GetMainCamera() == pCamera)
			ImGui::TextDisabled("Main Camera");

		// --- Movement ---
		ImGui::SeparatorText("Movement");
		ImGui::DragFloat("Speed", &pCamera->Speed, 0.05f, 0.001f, 1000.f);
		ImGui::DragFloat("Sensitivity", &pCamera->Sensitivity, 0.005f, 0.001f, 10.f);

		// --- Projection ---
		auto setting = pCamera->GetSettings();
		ImGui::SeparatorText("Projection");
		if (ImGui::DragFloat("Field Of View", &setting.fov, 0.1f, 1.f, 179.f))
			pCamera->ChangeSettings(setting);
		if (ImGui::DragFloat2("View Planes", &setting.nearPlane, 0.01f, 0.0001f, 10000.f))
			pCamera->ChangeSettings(setting);

		// --- Exposure ---
		ImGui::SeparatorText("Exposure");
		ImGui::Checkbox("Auto-Exposure", &pCamera->UseAutoExposure);
		if (pCamera->UseAutoExposure)
		{
			ImGui::DragFloat("Min Log Lum", &pCamera->AutoExposureSettings.minLogLum, 0.01f);
			ImGui::DragFloat("Log Lum Range", &pCamera->AutoExposureSettings.logLumRange, 0.01f);
		}
		else
		{
			ImGui::DragFloat("Aperture", &pCamera->ManualExposureSettings.aperture, 0.01f, 0.1f, 32.0f);
			ImGui::DragFloat("ISO", &pCamera->ManualExposureSettings.iso, 1.0f, 10.f, 12800.f);
			ImGui::DragFloat("Shutter Speed", &pCamera->ManualExposureSettings.shutterSpeed, 0.001f, 0.0001f, 10.f);
		}
	}
	ImGui::PopID();
}
void tadpole::InspectorPanel::DrawLight(LightComponent* pLight) const
{
	ImGui::PushID(pLight);
	if (ImGui::CollapsingHeader(pLight->name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		auto& lightingSystem = ServiceLocator::Get<LightingSystem>();
		pompeii::Light& light = pLight->lightData;
		const bool isDirectional = light.type == pompeii::LightType::Directional;

		// --- Type ---
		int type = static_cast<int>(light.type);
		if (ImGui::Combo("Type", &type, "Directional\0Point\0"))
		{
			light.type = static_cast<pompeii::LightType>(type);
			if (light.type == pompeii::LightType::Point) light.dirPos = pLight->GetTransform().GetPosition();
			else light.dirPos = glm::vec3{ 0.577f, -0.577f, 0.577f };
			light.CalculateLightMatrices(pLight->GetSceneObject().GetScene().GetAABB());
			lightingSystem.UpdateLight(*pLight);
		}

		// --- Direction / Position ---
		if (isDirectional)
		{
			if (ImGui::DragFloat3("Direction", &light.dirPos.x, 0.01f))
				lightingSystem.UpdateLight(*pLight);
		}
		else
		{
			if (ImGui::DragFloat3("Position", &light.dirPos.x, 0.1f))
			{
				pLight->GetTransform().SetPosition(light.dirPos);
				lightingSystem.UpdateLight(*pLight);
			}
		}

		// --- Intensity & Color ---
		if (ImGui::DragFloat(isDirectional ? "Intensity (lux)" : "Intensity (lumen)", &light.luxLumen, 1.f, 0.f, 1000000.f))
			lightingSystem.UpdateLight(*pLight);
		if (ImGui::ColorEdit3("Color", &light.color.r))
			lightingSystem.UpdateLight(*pLight);

		// --- Shadows ---
		if (ImGui::Button("Recalculate Shadow Matrices"))
		{
			light.CalculateLightMatrices(pLight->GetSceneObject().GetScene().GetAABB());
			lightingSystem.UpdateLight(*pLight);
		}
	}
	ImGui::PopID();
}
void tadpole::InspectorPanel::DrawMeshFilter(MeshFilter* pMeshFilter)
{
	ImGui::PushID(pMeshFilter);
	if (ImGui::CollapsingHeader(pMeshFilter->name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (const pompeii::Mesh* pMesh = pMeshFilter->pMesh)
		{
			ImGui::Text("Vertices:  %u", static_cast<uint32_t>(pMesh->vertices.size()));
			ImGui::Text("Indices:   %u", static_cast<uint32_t>(pMesh->indices.size()));
			ImGui::Text("Submeshes: %u", static_cast<uint32_t>(pMesh->vSubMeshes.size()));
			ImGui::Text("Textures:  %u", static_cast<uint32_t>(pMesh->images.size()));
		}
		else
		{
			ImGui::TextDisabled("No mesh assigned.");
		}

		if (ImGui::Button("Load Mesh..."))
		{
			IGFD::FileDialogConfig config{};
			config.path = ".";
			ImGuiFileDialog::Instance()->OpenDialog(LOAD_MESH_DIALOG_KEY, "Load Mesh", filters::MODEL_FILE_FILTERS, config);
		}
	}
	ImGui::PopID();
}
void tadpole::InspectorPanel::DrawMeshRenderer(MeshRenderer* pMeshRenderer) const
{
	ImGui::PushID(pMeshRenderer);
	if (ImGui::CollapsingHeader(pMeshRenderer->name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		MeshFilter* pOwnFilter = m_pSelectedObject->GetComponent<MeshFilter>();

		if (!pMeshRenderer->pMeshFilter)
			ImGui::TextColored(ImVec4(1.f, 0.4f, 0.4f, 1.f), "No MeshFilter linked.");
		else if (&pMeshRenderer->pMeshFilter->GetSceneObject() != m_pSelectedObject)
			ImGui::Text("Linked to MeshFilter on \"%s\"", pMeshRenderer->pMeshFilter->GetSceneObject().name.c_str());
		else if (!pMeshRenderer->pMeshFilter->pMesh)
			ImGui::TextColored(ImVec4(1.f, 0.8f, 0.2f, 1.f), "Linked MeshFilter has no mesh.");
		else
			ImGui::TextDisabled("Linked to this object's MeshFilter.");

		if (pOwnFilter && pMeshRenderer->pMeshFilter != pOwnFilter)
		{
			if (ImGui::Button("Link MeshFilter On This Object"))
			{
				pMeshRenderer->pMeshFilter = pOwnFilter;
				ReRegisterMeshRenderer();
			}
		}
	}
	ImGui::PopID();
}
void tadpole::InspectorPanel::DrawAddComponent() const
{
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	constexpr float buttonWidth = 200.f;
	ImGui::SetCursorPosX(std::max(0.f, (ImGui::GetContentRegionAvail().x - buttonWidth) * 0.5f));
	if (ImGui::Button("Add Component", ImVec2(buttonWidth, 0)))
		ImGui::OpenPopup("AddComponentPopup");

	if (!ImGui::BeginPopup("AddComponentPopup"))
		return;

	const bool hasCamera = m_pSelectedObject->HasComponent<Camera>();
	const bool hasLight = m_pSelectedObject->HasComponent<LightComponent>();
	const bool hasFilter = m_pSelectedObject->HasComponent<MeshFilter>();
	const bool hasRenderer = m_pSelectedObject->HasComponent<MeshRenderer>();
	MeshFilter* pFilter = m_pSelectedObject->GetComponent<MeshFilter>();

	if (ImGui::MenuItem("Camera", nullptr, false, !hasCamera))
	{
		m_pSelectedObject->AddComponent<Camera>(
			CameraSettings{ .fov = 45.f, .aspectRatio = 16.f / 9.f, .nearPlane = 0.001f, .farPlane = 1000.f },
			false);
	}
	if (ImGui::MenuItem("Light (Directional)", nullptr, false, !hasLight))
	{
		m_pSelectedObject->AddComponent<LightComponent>(
			glm::vec3{ 0.577f, -0.577f, 0.577f }, glm::vec3{ 1.f, 1.f, 1.f }, 20.f, pompeii::LightType::Directional);
	}
	if (ImGui::MenuItem("Light (Point)", nullptr, false, !hasLight))
	{
		m_pSelectedObject->AddComponent<LightComponent>(
			m_pSelectedObject->transform->GetPosition(), glm::vec3{ 1.f, 1.f, 1.f }, 1000.f, pompeii::LightType::Point);
	}
	if (ImGui::MenuItem("Mesh Filter", nullptr, false, !hasFilter))
	{
		m_pSelectedObject->AddComponent<MeshFilter>();
	}

	// MeshRenderer requires a MeshFilter with a loaded mesh; adding one without would crash the engine
	const bool canAddRenderer = !hasRenderer && pFilter && pFilter->pMesh;
	if (ImGui::MenuItem("Mesh Renderer", nullptr, false, canAddRenderer))
	{
		m_pSelectedObject->AddComponent<MeshRenderer>(*pFilter);
	}
	if (!hasRenderer && (!pFilter || !pFilter->pMesh))
		ImGui::TextDisabled("Mesh Renderer requires a Mesh Filter\nwith a loaded mesh.");

	ImGui::EndPopup();
}

//--------------------------------------------------
//    Helpers
//--------------------------------------------------
bool tadpole::InspectorPanel::DrawVec3Control(const char* label, glm::vec3& values, float resetValue, float speed)
{
	bool changed = false;
	constexpr float labelWidth = 90.f;

	ImGui::PushID(label);

	ImGui::AlignTextToFramePadding();
	ImGui::Text("%s", label);
	ImGui::SameLine(labelWidth);

	const float lineHeight = ImGui::GetFrameHeight();
	const ImVec2 buttonSize = { lineHeight, lineHeight };
	const float innerSpacing = ImGui::GetStyle().ItemInnerSpacing.x;
	const float itemSpacing = ImGui::GetStyle().ItemSpacing.x;
	const float fieldWidth = std::max(1.f,
		(ImGui::GetContentRegionAvail().x - 3.f * (buttonSize.x + innerSpacing) - 2.f * itemSpacing) / 3.f);

	struct Axis
	{
		const char* name;
		float* value;
		ImVec4 color;
	};
	const Axis axes[3] = {
		{ .name = "X", .value = &values.x, .color = ImVec4(0.75f, 0.20f, 0.20f, 1.f) },
		{ .name = "Y", .value = &values.y, .color = ImVec4(0.25f, 0.55f, 0.25f, 1.f) },
		{ .name = "Z", .value = &values.z, .color = ImVec4(0.20f, 0.35f, 0.75f, 1.f) },
	};

	for (const Axis& axis : axes)
	{
		if (axis.name != axes[0].name)
			ImGui::SameLine(0.f, itemSpacing);

		ImGui::PushID(axis.name);
		ImGui::PushStyleColor(ImGuiCol_Button, axis.color);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(axis.color.x + 0.1f, axis.color.y + 0.1f, axis.color.z + 0.1f, 1.f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, axis.color);
		if (ImGui::Button(axis.name, buttonSize))
		{
			*axis.value = resetValue;
			changed = true;
		}
		ImGui::PopStyleColor(3);

		ImGui::SameLine(0.f, innerSpacing);
		ImGui::SetNextItemWidth(fieldWidth);
		if (ImGui::DragFloat("##Value", axis.value, speed, 0.f, 0.f, "%.2f"))
			changed = true;
		ImGui::PopID();
	}

	ImGui::PopID();
	return changed;
}
void tadpole::InspectorPanel::HandleLoadMeshDialog() const
{
	constexpr ImGuiWindowFlags dialogFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking;
	if (!ImGuiFileDialog::Instance()->Display(LOAD_MESH_DIALOG_KEY, dialogFlags, ImVec2(800, 500)))
		return;

	if (ImGuiFileDialog::Instance()->IsOk() && m_pSelectedObject)
	{
		const std::string path = ImGuiFileDialog::Instance()->GetFilePathName();

		MeshFilter* pMeshFilter{};
		if (m_pSelectedObject->TryGetComponent(pMeshFilter))
		{
			pompeii::Mesh* pMesh{};
			try
			{
				pMesh = ServiceLocator::Get<AssetManager>().LoadMesh(path);
			}
			catch (const std::exception& e)
			{
				Debug::LogError("Failed to load mesh \"" + path + "\": " + e.what());
			}

			if (pMesh)
			{
				pMeshFilter->pMesh = pMesh;
				m_pSelectedObject->GetScene().GrowAABB(pMesh->aabb);
				ReRegisterMeshRenderer();
				Debug::Log("Loaded mesh \"" + path + "\"");
			}
		}
	}
	ImGuiFileDialog::Instance()->Close();
}
void tadpole::InspectorPanel::ReRegisterMeshRenderer() const
{
	MeshRenderer* pMeshRenderer = m_pSelectedObject->GetComponent<MeshRenderer>();
	if (!pMeshRenderer) return;
	auto& renderSystem = ServiceLocator::Get<RenderSystem>();
	renderSystem.UnregisterMeshRenderer(*pMeshRenderer);
	renderSystem.RegisterMeshRenderer(*pMeshRenderer);
}

//--------------------------------------------------
//    Helper
//--------------------------------------------------
void tadpole::InspectorPanel::SetSelectedObject(SceneObject* pObject)
{
	m_pSelectedObject = pObject;
}
