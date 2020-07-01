#pragma once

class Script_Module final
{

public:
	Script_Module(class Scripting* scripting, const std::string module_name);
	~Script_Module();

	auto LoadScript(const std::string& path) -> const bool;
	auto GetAsIScriptModule() -> class asIScriptModule*;

private:
	class Scripting* scripting = nullptr;
    std::unique_ptr<class CScriptBuilder> builder;
	std::string module_name;
};