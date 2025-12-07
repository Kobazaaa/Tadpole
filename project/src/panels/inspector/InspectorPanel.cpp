// -- Tadpole Includes --
#include "InspectorPanel.h"

#include "Debug.h"

#include "Camera.h"
#include "LightComponent.h"
#include "LightingSystem.h"
#include "MeshFilter.h"
#include "MeshRenderer.h"

#include "Scene.h"
#include "SceneObject.h"
#include "ServiceLocator.h"

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
	
	// --- Enabled toggle ---
	bool enabled = m_pSelectedObject->IsActive();
	if (ImGui::Checkbox("##Enabled", &enabled))
		m_pSelectedObject->SetActive(enabled);

	// --- Object name ---
	ImGui::SameLine();
	char nameBuffer[128];
	strcpy_s(nameBuffer, m_pSelectedObject->name.c_str());
	if (ImGui::InputText("##Name", nameBuffer, sizeof(nameBuffer)))
		m_pSelectedObject->name = nameBuffer;

	ImGui::Separator();

	// --- Hardcoded components ---

	// -- Transform --
	{
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			auto& transform = m_pSelectedObject->transform;

			glm::vec3 pos = transform->GetPosition();
			glm::vec3 rot = transform->GetEulerAngles();
			glm::vec3 scale = transform->GetScale();

			constexpr float offset = 150.f;

			ImGui::Text("Position");
			ImGui::SameLine(offset);
			if (ImGui::DragFloat3("##Position", &pos.x, 0.1f)) transform->SetPosition(pos);

			ImGui::Text("Rotation");
			ImGui::SameLine(offset);
			if (ImGui::DragFloat3("##Rotation", &rot.x, 0.1f)) transform->SetEulerAngles(rot);

			ImGui::Text("Scale");
			ImGui::SameLine(offset);
			if (ImGui::DragFloat3("##Scale", &scale.x, 0.1f))  transform->SetScale(scale);

			if (scale.x <= 0 || scale.y <= 0 || scale.z <= 0)
			{
				kobengine::Debug::LogWarning("Negative or Zero-Scale is Undefined on object: " + m_pSelectedObject->name);
			}
		}
	}

	// -- Camera --
	{
		kobengine::Camera* pCameraComponent;
		if (m_pSelectedObject->TryGetComponent(pCameraComponent))
		{
			ImGui::PushID(pCameraComponent);
			if (ImGui::CollapsingHeader(pCameraComponent->name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				// --- Movement ---
				ImGui::SeparatorText("Movement");
				ImGui::DragFloat("Speed", &pCameraComponent->Speed);
				ImGui::DragFloat("Sensitivity", &pCameraComponent->Sensitivity);

				// --- Projection ---
				auto setting = pCameraComponent->GetSettings();
				ImGui::SeparatorText("Projection");
				if (ImGui::DragFloat("Field Of View", &setting.fov, 0.1f, 1.f, 179.f))
					pCameraComponent->ChangeSettings(setting);
				if (ImGui::DragFloat2("View Plane", &setting.nearPlane, 0.01f, 0.0001f))
					pCameraComponent->ChangeSettings(setting);

				// --- Exposure ---
				ImGui::SeparatorText("Exposure");
				ImGui::Checkbox("Auto-Exposure", &pCameraComponent->UseAutoExposure);
				if (pCameraComponent->UseAutoExposure)
				{
					ImGui::DragFloat("Min Log Lum", &pCameraComponent->AutoExposureSettings.minLogLum, 0.01f);
					ImGui::DragFloat("Log Lum Range", &pCameraComponent->AutoExposureSettings.logLumRange, 0.01f);
				}
				else
				{
					ImGui::DragFloat("Aperture", &pCameraComponent->ManualExposureSettings.aperture, 0.01f, 0.1f, 32.0f);
					ImGui::DragFloat("ISO", &pCameraComponent->ManualExposureSettings.iso, 1.0f, 10.f, 12800.f);
					ImGui::DragFloat("ShutterSpeed", &pCameraComponent->ManualExposureSettings.shutterSpeed, 0.001f, 0.0001f, 10.f);
				}
			}
			ImGui::PopID();
		}
	}

	// -- Mesh Filter --
	{
		kobengine::MeshFilter* pMeshFilter;
		if (m_pSelectedObject->TryGetComponent(pMeshFilter))
		{
			ImGui::PushID(pMeshFilter);
			if (ImGui::CollapsingHeader(pMeshFilter->name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				if (ImGui::BeginDragDropSource())
				{
					ImGui::SetDragDropPayload(pMeshFilter->name.c_str(), pMeshFilter, sizeof(*pMeshFilter));
					ImGui::Text("%s", pMeshFilter->name.c_str());
					ImGui::EndDragDropSource();
				}
			}
			ImGui::PopID();
		}
	}

	// -- Mesh Renderer --
	{
		kobengine::MeshRenderer* pMeshRenderer;
		if (m_pSelectedObject->TryGetComponent(pMeshRenderer))
		{
			ImGui::PushID(pMeshRenderer);
			if (ImGui::CollapsingHeader(pMeshRenderer->name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MeshFilter"))
					{
						kobengine::Component* dropped = *static_cast<kobengine::Component**>(payload->Data);
						if (kobengine::MeshFilter* mf = dynamic_cast<kobengine::MeshFilter*>(dropped))
							if (mf != pMeshRenderer->pMeshFilter)
								pMeshRenderer->pMeshFilter = mf;
					}
					ImGui::EndDragDropTarget();
				}
			}
			ImGui::PopID();
		}
	}

	// -- Light --
	{
		kobengine::LightComponent* pLightComponent;
		if (m_pSelectedObject->TryGetComponent(pLightComponent))
		{
			ImGui::PushID(pLightComponent);
			if (ImGui::CollapsingHeader(pLightComponent->name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Text("Type"); ImGui::SameLine();
				if (ImGui::Combo("##Type", reinterpret_cast<int*>(&pLightComponent->lightData.type), "Directional\0Point"))
				{
					kobengine::ServiceLocator::Get<kobengine::LightingSystem>().UpdateLight(*pLightComponent);
				}

				ImGui::Text("Intensity"); ImGui::SameLine();
				if (ImGui::DragFloat("##Intensity", &pLightComponent->lightData.luxLumen))
					kobengine::ServiceLocator::Get<kobengine::LightingSystem>().UpdateLight(*pLightComponent);
				ImGui::Text("Color"); ImGui::SameLine();
				if (ImGui::ColorEdit3("##Color", &pLightComponent->lightData.color.r))
					kobengine::ServiceLocator::Get<kobengine::LightingSystem>().UpdateLight(*pLightComponent);

				if (ImGui::Button("Recalculate Matrices"))
					pLightComponent->lightData.CalculateLightMatrices(pLightComponent->GetSceneObject().GetScene().GetAABB());
			}
			ImGui::PopID();
		}
	}


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

