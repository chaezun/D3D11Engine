#pragma once
#include "ISubsystem.h"

class SceneManager final :public ISubsystem
{
public: 
	SceneManager(class Context* context);
	~SceneManager()=default;

	const bool Initialize() override;

	auto GetScenes() -> const std::map<std::string, std::shared_ptr<class Scene>>& {return scenes;}

	auto GetCurrentScene() const -> class Scene*;
	void SetCurrentScene(const std::string& scene_name);

	auto RegisterScene(const std::string& scene_name) -> class Scene*;

	void Update();

private:
    std::weak_ptr<class Scene> current_scene;
    std::map<std::string, std::shared_ptr<class Scene>> scenes;

};