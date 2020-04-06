#pragma once

class Script_Instance final
{
public:
	Script_Instance() = default;
	~Script_Instance();

	auto Instantiate(class Scripting* scripting, class Actor* actor, const std::string& path) -> const bool;

	auto GetScriptPath() const -> const std::string& { return script_path; }
	auto IsInstantiated() const -> const bool& { return is_instantiated; }

	void ExecuteStart();
	void ExecuteUpdate();

private:
   auto CreateScriptObject() -> const bool;

private:
   class Scripting* scripting					 = nullptr;
   class asIScriptObject* script_object			 = nullptr;
   class asIScriptFunction* constructor_function = nullptr;
   class asIScriptFunction* start_function		 = nullptr;
   class asIScriptFunction* update_function		 = nullptr;
   class Actor* actor                            = nullptr;
   std::string script_path						 = "";
   std::string class_name						 = "";
   std::string constructor_decl					 = "";
   std::string module_name						 = "";
   bool is_instantiated							 = false;
   std::shared_ptr<class Script_Module> script_module;
};