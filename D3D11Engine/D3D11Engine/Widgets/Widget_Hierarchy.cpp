#include "stdafx.h"
#include "Widget_Hierarchy.h"
#include "Scene/Scene.h"
#include "Scene/Actor.h"
#include "Scene/Component/Transform.h"
#include "Scene/Component/Renderable.h"
#include "Scene/Component/Terrain.h"

namespace Hierarchy_Data
{
	static Scene* current_scene = nullptr;
	static Actor* copied_actor = nullptr;
	static Actor* hovered_actor = nullptr;
	static Actor* clicked_actor = nullptr;
	static bool is_show_rename_popup = false;
}

Widget_Hierarchy::Widget_Hierarchy(Context * context)
	:IWidget(context)
{
	title = "Hierarchy";
}

void Widget_Hierarchy::Render()
{
    if(!is_visible)
	  return;

    ShowHierarchy();

	if (ImGui::IsMouseReleased(0) && Hierarchy_Data::clicked_actor)
	{
		if(Hierarchy_Data::hovered_actor && Hierarchy_Data::hovered_actor->GetID() == Hierarchy_Data::clicked_actor->GetID())
		   SelectedItem(Hierarchy_Data::clicked_actor->shared_from_this());

		Hierarchy_Data::clicked_actor=nullptr;
	}
}

void Widget_Hierarchy::ShowHierarchy()
{
    Hierarchy_Data::current_scene = Editor_Helper::Get().scene_manager->GetCurrentScene();

	//ImGuiTreeNodeFlags_DefaultOpen : 시작부터 하위노드를 다 보여주는 옵션
	if (ImGui::CollapsingHeader(Hierarchy_Data::current_scene->GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
	   auto roots = Hierarchy_Data::current_scene->GetRootActors();
	   for(const auto& root : roots)
	     AddActor(root);
	}
    
	ClickedItem();
	ShowMenuPopup();
}

void Widget_Hierarchy::ShowMenuPopup()
{
	if (!ImGui::BeginPopup("Hierarchy Menu Popup"))
		return;

	if (ImGui::BeginMenu("Create 3D"))
	{
		if (ImGui::MenuItem("Cube")) CreateStandardMesh(MeshType::Cube);
		if (ImGui::MenuItem("Sphere")) CreateStandardMesh(MeshType::Sphere);
		if (ImGui::MenuItem("Plane")) CreateStandardMesh(MeshType::Plane);
		if (ImGui::MenuItem("Capsule")) CreateStandardMesh(MeshType::Capsule);

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Environment"))
	{
		if (ImGui::MenuItem("Terrain"))
			CreateTerrain();

		ImGui::EndMenu();
	}

	ImGui::EndPopup();
}

void Widget_Hierarchy::AddActor(const std::shared_ptr<class Actor>& actor)
{
   if(!actor)
     return;

   auto childs = actor->GetTransform()->GetChilds();
   //ImGuiTreeNodeFlags_AllowItemOverlap: 마우스를 끌어다가 놓으면 자동으로 자식으로 만들어주는 플래그
   ImGuiTreeNodeFlags flags= ImGuiTreeNodeFlags_AllowItemOverlap;
   //ImGuiTreeNodeFlags_OpenOnArrow : 화살표를 on했을때만 open하는 플래그
   flags |= childs.empty() ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_OpenOnArrow;

   if (const auto selected_actor = Editor_Helper::Get().selected_actor.lock())
   {
	   const auto is_selected = selected_actor ->GetID() == actor->GetID();
	   flags |= is_selected ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None;
   }

   //노드 추가해서 화면에 보여주는 역할
   const auto is_node_open = ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<intptr_t>(actor->GetID())), flags, actor->GetName().c_str());

   if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
      Hierarchy_Data::hovered_actor=actor.get();

   if (is_node_open)
   {
       for(const auto& child : childs)
	     AddActor(child->GetActor()->shared_from_this());
	   //노드 추가를 중지함
	   ImGui::TreePop();
   }
}

//Mouse Event를 추가하려면 여기에
void Widget_Hierarchy::ClickedItem()
{
   //hierarchy key에 마우스가 올라가 있는지 확인하는 변수
   const auto is_window_hovered  = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem|ImGuiHoveredFlags_AllowWhenBlockedByPopup);
   const auto is_left_click      = ImGui::IsMouseClicked(0);
   const auto is_right_click     = ImGui::IsMouseClicked(1);

   if (!is_window_hovered)
     return;

   if(is_left_click&&Hierarchy_Data::hovered_actor)
	   Hierarchy_Data::clicked_actor= Hierarchy_Data::hovered_actor;

    if(is_right_click)
	{ 
	    if(Hierarchy_Data::hovered_actor)
		  //shared_from_this : Row Pointer->Shared Pointer 변환
		  SelectedItem(Hierarchy_Data::hovered_actor->shared_from_this());

         ImGui::OpenPopup("Hierarchy Menu Popup");
	}

	if((is_left_click||is_right_click) && !Hierarchy_Data::hovered_actor)
	   SelectedItem(empty_actor);
}

void Widget_Hierarchy::SelectedItem(const std::shared_ptr<class Actor>& actor)
{
    is_expand_to_show_actor=true;
	Editor_Helper::Get().selected_actor=actor;
}

auto Widget_Hierarchy::CreateEmptyActor() -> std::shared_ptr<class Actor>
{
	if (auto scene = Hierarchy_Data::current_scene)
	{
		auto actor=scene->CreateActor();
		actor->SetName("Empty");

		return actor;
	}
	
	return nullptr;
}

void Widget_Hierarchy::CreateStandardMesh(const MeshType & mesh_type)
{
	const auto directory = Editor_Helper::Get().resource_manager->GetAssetDirectory(AssetType::Model);

	switch (mesh_type)
	{
	case MeshType::Cube:
	{
		Editor_Helper::Get().LoadModel(directory + "Cube.fbx");
		break;
	}
	case MeshType::Sphere: 
	{
		Editor_Helper::Get().LoadModel(directory + "Sphere.fbx");
		break;
	}
	case MeshType::Plane:
	{
		Editor_Helper::Get().LoadModel(directory + "Plane.fbx");
		break;
	}
	case MeshType::Capsule:
	{
		Editor_Helper::Get().LoadModel(directory + "Capsule.fbx");
		break;
	}
	}
}

void Widget_Hierarchy::CreateTerrain()
{
	if (auto actor = CreateEmptyActor())
	{
		actor->AddComponent<Terrain>();
		actor->SetName("Terrain");
	}
}

