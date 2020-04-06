#include "Framework.h"
#include "SceneManager.h"
#include "Scene/Scene.h"

SceneManager::SceneManager(Context * context)
:ISubsystem(context)
{
   SUBSCRIBE_TO_EVENT(EventType::Update, EVENT_HANDLER(Update));
}

const bool SceneManager::Initialize()
{
    RegisterScene("First");
	SetCurrentScene("First");

	return true;
}

auto SceneManager::GetCurrentScene() const -> class Scene *
{
	return current_scene.expired() ? nullptr : current_scene.lock().get();
}

void SceneManager::SetCurrentScene(const std::string & scene_name)
{
	if (scenes.find(scene_name) == scenes.end())
	{
	   LOG_ERROR("Do not have a scene with that name");
	   return;
    }

   current_scene=scenes[scene_name];
}

auto SceneManager::RegisterScene(const std::string & scene_name) -> class Scene*
{
    auto pair_ib=scenes.insert(std::make_pair(scene_name, std::make_shared<Scene>(context)));
	//삽입이 성공하면 true, 실패하면 false
	//실패했다는건 이미 자료가 들어있기 때문
	if (!pair_ib.second)
	{
	   LOG_ERROR("Already have a scene with that name");
	}

	return pair_ib.first->second.get();
}

void SceneManager::Update()
{
   if(!current_scene.expired())
	   current_scene.lock()->Update();
}
