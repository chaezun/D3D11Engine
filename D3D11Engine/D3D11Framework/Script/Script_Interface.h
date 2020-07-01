#pragma once

class Script_Interface final
{
public:
	Script_Interface() = default;
	~Script_Interface() = default;

	void Register(class Context* context, class asIScriptEngine* script_engine);

private:
	void RegisterEnumerations();
	void RegisterTypes();
	void RegisterVector2();
	void RegisterVector3();
	void RegisterQuaternion();
	void RegisterTimer();
	void RegisterInput();
	void RegisterTransform();
	void RegisterRigidBody();
	void RegisterActor();

private:
	class Context* context;
	class asIScriptEngine* script_engine;
};