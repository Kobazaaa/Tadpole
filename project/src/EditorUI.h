#ifndef EDITOR_UI_H
#define EDITOR_UI_H

// -- Standard Library --
#include <functional>
#include <string>

// -- ImGui --
#include "imgui.h"

// -- Forward Declarations --
namespace kobengine
{
	class SceneObject;
}

namespace tadpole
{
	//? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//? ~~	  Interface	
	//? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	class IEditorUI
	{
	public:
		virtual ~IEditorUI() = default;
		virtual void Draw(ImGuiID dockID = -1) = 0;
	protected:
		void HandleFileDialog(const std::string& key, const std::function<void(const std::string&)>& func);
	};

	//? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//? ~~	  Scene Hierarchy	
	//? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	class SceneHierarchy final : IEditorUI
	{
	public:
		void Draw(ImGuiID dockID = -1) override;
		kobengine::SceneObject* GetSelectedObject() const;
	private:
		void DrawSceneObjectNode(kobengine::SceneObject* sceneObj);
		void DrawGeneralContextMenu();
		kobengine::SceneObject* m_pSelectedObject{};
	};

	//? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//? ~~	  Menu Bar	
	//? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	class MenuBar final : IEditorUI
	{
	public:
		void Draw(ImGuiID dockID = -1) override;
	};

	//? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//? ~~	  Utilities	
	//? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	class Utilities final : IEditorUI
	{
	public:
		void Draw(ImGuiID dockID = -1) override;
	};

	//? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//? ~~	  Inspector	
	//? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	class Inspector final : IEditorUI
	{
	public:
		void Draw(kobengine::SceneObject* pSelectedObject, ImGuiID dockID = -1);
	private:
		void Draw(ImGuiID dockID = -1) override;
		void DrawGeneralContextMenu() const;
		kobengine::SceneObject* m_pSelectedObject{};
	};
}

#endif // EDITOR_UI_H

