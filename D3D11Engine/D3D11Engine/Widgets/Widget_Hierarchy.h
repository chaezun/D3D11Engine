#pragma once
#include "IWidget.h"

class Widget_Hierarchy final : public IWidget
{
public:
	Widget_Hierarchy(class Context* context);
	~Widget_Hierarchy() = default;

	void Render() override;

private:
    void ShowHierarchy();
    void ShowMenuPopup();

private:
   void AddActor(const std::shared_ptr<class Actor>& actor);

   void ClickedItem();
   void SelectedItem(const std::shared_ptr<class Actor>& actor);

private:
    auto CreateEmptyActor() -> std::shared_ptr<class Actor>;
    void CreateStandardMesh(const MeshType& mesh_type);

private: 
    bool is_expand_to_show_actor = false;
	std::shared_ptr<class Actor> empty_actor;
};