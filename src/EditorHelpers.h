#ifndef EDITOR_ACTIONS_H
#define EDITOR_ACTIONS_H

// -- Standard Library --
#include <string>

// -- Kobengine Includes --
#include "Light.h"

// -- Forward Declarations --
namespace kobengine
{
	class Transform;
	class SceneObject;
}

namespace tadpole::actions
{
	//? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//? ~~	  Editor Actions
	//? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// -- Creation --
	kobengine::SceneObject& CreateEmpty(const std::string& name = "Empty");
	kobengine::SceneObject& CreateCamera();
	kobengine::SceneObject& CreateLight(pompeii::LightType type);
	kobengine::SceneObject* ImportModel(const std::string& path);

	// -- Duplication & Destruction --
	kobengine::SceneObject& Duplicate(kobengine::SceneObject& source);
	bool ContainsMainCamera(const kobengine::SceneObject& obj);
	bool DestroyObject(kobengine::SceneObject& obj);
}

namespace tadpole::filters
{
	//? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//? ~~	  Editor Filters
	//? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	constexpr const char* MODEL_FILE_FILTERS = "3D Models (*.gltf *.glb *.obj *.fbx){.gltf,.glb,.obj,.fbx},All Files{.*}";

	bool ContainsCaseInsensitive(const std::string& haystack, const std::string& needle);
}

namespace tadpole::checks
{
	//? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//? ~~	  Editor Checks
	//? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	bool IsDescendantOf(const kobengine::Transform* node, const kobengine::Transform* potentialAncestor);
}

#endif // EDITOR_ACTIONS_H
