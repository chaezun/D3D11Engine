#pragma once

class Scene final
{
public:
	Scene(class Context* context);
	~Scene();

	auto GetName() const -> const std::string& {return name;}
	void SetName(const std::string& name) { this->name=name;}

	auto GetAllActors() const ->const std::vector<std::shared_ptr<class Actor>>& {return actors;}
	auto GetActorCount() const -> const uint {return static_cast<uint>(actors.size());}
	auto GetRootActors() const -> const std::vector<std::shared_ptr<class Actor>>;

	auto CreateActor(const bool& is_active = true) -> std::shared_ptr<class Actor>;
	void AddActor(const std::shared_ptr<class Actor>& actor);
	void RemoveActor(const std::shared_ptr<class Actor>& actor);

	void Update();

private:
   auto CreateCamera() -> std::shared_ptr<class Actor>;
   auto CreateDirectionalLight() -> std::shared_ptr<class Actor>;
   auto CreateSkyBox() -> std::shared_ptr<class Actor>;

private:
   class Context* context=nullptr;
   class Renderer* renderer=nullptr;

   std::string name=NOT_ASSIGNED_STR;
   bool is_editor_mode=true;
   bool is_added=false;

   std::vector<std::shared_ptr<class Actor>> actors;

};