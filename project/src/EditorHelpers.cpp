// -- Tadpole Includes --
#include "EditorHelpers.h"

// -- Standard Library --
#include <filesystem>

// -- Kobengine Includes --
#include "AssetManager.h"
#include "Camera.h"
#include "Debug.h"
#include "LightComponent.h"
#include "MeshFilter.h"
#include "MeshRenderer.h"
#include "RenderSystem.h"
#include "SceneManager.h"
#include "SceneObject.h"
#include "ServiceLocator.h"

using namespace kobengine;

namespace
{
	Scene& ActiveScene()
	{
		return ServiceLocator::Get<SceneManager>().GetActiveScene();
	}

	SceneObject& DuplicateRecursive(SceneObject& source, Transform* pParent)
	{
		SceneObject& copy = ActiveScene().AddEmpty(source.name);
		copy.SetActive(source.IsActive());
		if (pParent)
			copy.transform->SetParent(pParent, false);
		copy.transform->SetLocalPosition(source.transform->GetLocalPosition());
		copy.transform->SetLocalEulerAngles(source.transform->GetLocalEulerAngles());
		copy.transform->SetLocalScale(source.transform->GetLocalScale());

		// -- Components --
		Camera* pCamera{};
		if (source.TryGetComponent(pCamera))
		{
			Camera* pNewCamera = copy.AddComponent<Camera>(pCamera->GetSettings(), false);
			pNewCamera->Speed = pCamera->Speed;
			pNewCamera->Sensitivity = pCamera->Sensitivity;
			pNewCamera->UseAutoExposure = pCamera->UseAutoExposure;
			pNewCamera->ManualExposureSettings = pCamera->ManualExposureSettings;
			pNewCamera->AutoExposureSettings = pCamera->AutoExposureSettings;
		}

		LightComponent* pLight{};
		if (source.TryGetComponent(pLight))
		{
			copy.AddComponent<LightComponent>(
				pLight->lightData.dirPos,
				pLight->lightData.color,
				pLight->lightData.luxLumen,
				pLight->lightData.type);
		}

		MeshFilter* pFilter{};
		if (source.TryGetComponent(pFilter))
		{
			MeshFilter* pNewFilter = copy.AddComponent<MeshFilter>();
			pNewFilter->pMesh = pFilter->pMesh;

			MeshRenderer* pRenderer{};
			if (source.TryGetComponent(pRenderer) && pNewFilter->pMesh)
				copy.AddComponent<MeshRenderer>(*pNewFilter);
		}

		// -- Children --
		for (const Transform* pChild : source.transform->GetAllChildren())
			DuplicateRecursive(*pChild->GetSceneObject(), copy.transform.get());

		return copy;
	}
}

namespace tadpole::actions
{
	//--------------------------------------------------
//    Creation
//--------------------------------------------------
	SceneObject& CreateEmpty(const std::string& name)
	{
		return ActiveScene().AddEmpty(name);
	}
	SceneObject& CreateCamera()
	{
		SceneObject& obj = ActiveScene().AddEmpty("Camera");
		obj.AddComponent<Camera>(
			CameraSettings{ .fov = 45.f, .aspectRatio = 16.f / 9.f, .nearPlane = 0.001f, .farPlane = 1000.f },
			false);
		return obj;
	}
	SceneObject& CreateLight(pompeii::LightType type)
	{
		const bool directional = (type == pompeii::LightType::Directional);
		SceneObject& obj = ActiveScene().AddEmpty(directional ? "Directional Light" : "Point Light");
		obj.AddComponent<LightComponent>(
			/* dir / pos */	directional ? glm::vec3{ 0.577f, -0.577f, 0.577f } : glm::vec3{ 0.f, 1.f, 0.f },
			/* color */		glm::vec3{ 1.f, 1.f, 1.f },
			/* lux/lumen */	directional ? 20.f : 1000.f,
			type);
		return obj;
	}
	SceneObject* ImportModel(const std::string& path)
	{
		pompeii::Mesh* pMesh{};
		try
		{
			pMesh = ServiceLocator::Get<AssetManager>().LoadMesh(path);
		}
		catch (const std::exception& e)
		{
			Debug::LogError("Failed to import model \"" + path + "\": " + e.what());
			return nullptr;
		}
		if (!pMesh)
		{
			Debug::LogError("Failed to import model \"" + path + "\"");
			return nullptr;
		}

		std::string name = std::filesystem::path(path).stem().string();
		if (name.empty())
			name = "Model";

		SceneObject& obj = ActiveScene().AddEmpty(name);
		MeshFilter* pFilter = obj.AddComponent<MeshFilter>();
		pFilter->pMesh = pMesh;
		obj.AddComponent<MeshRenderer>(*pFilter);

		Debug::Log("Imported model \"" + path + "\"");
		return &obj;
	}

	//--------------------------------------------------
	//    Duplication & Destruction
	//--------------------------------------------------
	SceneObject& Duplicate(SceneObject& source)
	{
		SceneObject& copy = DuplicateRecursive(source, source.transform->GetParent());
		copy.name += " (Copy)";
		return copy;
	}
	bool ContainsMainCamera(const SceneObject& obj)
	{
		const Camera* pMainCamera = ServiceLocator::Get<RenderSystem>().GetMainCamera();
		if (!pMainCamera)
			return false;
		if (&pMainCamera->GetSceneObject() == &obj)
			return true;
		for (const Transform* pChild : obj.transform->GetAllChildren())
			if (ContainsMainCamera(*pChild->GetSceneObject()))
				return true;
		return false;
	}
	bool DestroyObject(SceneObject& obj)
	{
		if (ContainsMainCamera(obj))
		{
			Debug::LogWarning("Cannot delete \"" + obj.name + "\": it contains the main camera.");
			return false;
		}
		// detach first so the former parent is not left with a dangling child pointer
		obj.transform->SetParent(nullptr, true);
		obj.Destroy();
		return true;
	}
}

namespace tadpole::filters
{
	//--------------------------------------------------
	//    String Filters
	//--------------------------------------------------
	bool ContainsCaseInsensitive(const std::string& haystack, const std::string& needle)
	{
		if (needle.empty()) return true;
		const auto it = std::ranges::search(haystack, needle,
			[](const char a, const char b)
			{
				return std::tolower(static_cast<unsigned char>(a)) == std::tolower(static_cast<unsigned char>(b));
			}).begin();
		return it != haystack.end();
	}
}

namespace tadpole::checks
{
	bool IsDescendantOf(const Transform* node, const Transform* potentialAncestor)
	{
		for (const Transform* parent = node->GetParent(); parent; parent = parent->GetParent())
			if (parent == potentialAncestor)
				return true;
		return false;
	}
}
