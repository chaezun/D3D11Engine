#include "Framework.h"
#include "Script_Instance.h"
#include "Script_Module.h"
#include "Scene/Actor.h"

#pragma warning(push, 0)
#include <angelscript.h>
#pragma warning(pop)

Script_Instance::~Script_Instance()
{
	if (script_object)
	{
		script_object->Release();
		script_object = nullptr;
	}

	constructor_function = nullptr;
	start_function = nullptr;
	update_function = nullptr;
	scripting = nullptr;
	is_instantiated = false;
}

auto Script_Instance::Instantiate(Scripting * scripting, Actor * actor, const std::string & path) -> const bool
{
	if (!actor || !scripting)
	{
		LOG_ERROR("Invalid parameter");
		return false;
	}

	this->scripting = scripting;
	this->actor = actor;
	this->script_path = path;
	this->class_name = FileSystem::GetIntactFileNameFromPath(path);
	this->module_name = class_name + std::to_string(actor->GetID());
	this->constructor_decl = class_name + "@" + class_name + "(Actor@)";

	is_instantiated = CreateScriptObject();

	return is_instantiated;
}

void Script_Instance::ExecuteStart()
{
	if (!scripting)
	{
		LOG_ERROR("Invalid parameter, Scripting is nullptr");
		return;
	}

	scripting->ExecuteCall(start_function, script_object);
}

void Script_Instance::ExecuteUpdate()
{
	if (!scripting)
	{
		LOG_ERROR("Invalid parameter, Scripting is nullptr");
		return;
	}

	scripting->ExecuteCall(update_function, script_object);
}

auto Script_Instance::CreateScriptObject() -> const bool
{
	if (!scripting)
	{
		LOG_ERROR("Invalid parameter, Scripting is nullptr");
		return false;
	}

	//Create Module
	script_module = std::make_shared<Script_Module>(scripting, module_name);
	auto result = script_module->LoadScript(script_path);
	if (!result)
		return false;

	//Get Type
	auto type_id = script_module->GetAsIScriptModule()->GetTypeIdByDecl(class_name.c_str());
	auto type = scripting->GetAngleScriptEngine()->GetTypeInfoById(type_id);
	if (!type)
	{
		LOG_ERROR("The type is incorrect");
		return false;
	}

	//Get Functions in the script
	start_function = type->GetMethodByDecl("void Start()");
	update_function = type->GetMethodByDecl("void Update()");
	constructor_function = type->GetFactoryByDecl(constructor_decl.c_str());

	if (!constructor_function)
	{
		LOG_ERROR("Couldn't find the appropriate factory for the type '" + class_name + "'");
		return false;
	}

	auto ctx = scripting->RequestContext();
	{
	   auto r = ctx->Prepare(constructor_function);
	   if(r > 0)
	      return false;

       r = ctx->SetArgObject(0, actor);
	   if (r > 0)
		   return false;

	   r = ctx->Execute();
	   if (r > 0)
		   return false;
       
	   script_object = *static_cast<asIScriptObject**>(ctx->GetAddressOfReturnValue());
	   script_object->AddRef();
	}
	scripting->ReturnContext(ctx);

	return true;
}
