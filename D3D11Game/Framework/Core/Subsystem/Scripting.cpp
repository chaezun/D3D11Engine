#include "Framework.h"
#include "Scripting.h"
#include "Script/Script_Interface.h"

#pragma warning(push, 0)
#include <angelscript.h>
#include <scriptstdstring/scriptstdstring.cpp>
#pragma warning(pop)

Scripting::Scripting(Context * context)
	:ISubsystem(context)
{
}

Scripting::~Scripting()
{
	ClearContext();

	if (script_engine)
	{
		script_engine->ShutDownAndRelease();
		script_engine = nullptr;
	}
}

const bool Scripting::Initialize()
{
	script_engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
	if (!script_engine)
	{
		LOG_ERROR("Failed to create AngelScript Engine");
		return false;
	}

	RegisterStdString(script_engine);

	auto script_interface = std::make_shared<Script_Interface>();
	script_interface->Register(context, script_engine);

	script_engine->SetMessageCallback
	(
		asMETHOD(Scripting, Message_Callback),
		this,
		asCALL_THISCALL
	);

	script_engine->SetEngineProperty(asEP_BUILD_WITHOUT_LINE_CUES, true);

	return true;
}

auto Scripting::GetAngleScriptEngine() const -> class asIScriptEngine *
{
	return script_engine;
}

auto Scripting::RequestContext() -> class asIScriptContext *
{
	asIScriptContext* ctx = nullptr;
	if (ctxs.empty())
		ctx = script_engine->CreateContext();

	else
	{
		ctx = ctxs.back();
		ctxs.pop_back();
	}

	return ctx;
}

void Scripting::ReturnContext(asIScriptContext * ctx)
{
	if (!ctx)
	{
		LOG_ERROR("AngelScript Context is nullptr");
		return;
	}

	ctxs.emplace_back(ctx);
	//준비단계 종료
	ctx->Unprepare();
}

void Scripting::ClearContext()
{
	for (auto& ctx : ctxs)
		ctx->Release();

	ctxs.clear();
	ctxs.shrink_to_fit();
}

//여기서 function은 한 script 안의 하나의 함수를 의미함
auto Scripting::ExecuteCall(asIScriptFunction * function, asIScriptObject * object) -> const bool
{
	auto ctx = RequestContext();
	ctx->Prepare(function);
	ctx->SetObject(object);

	auto result = ctx->Execute();
	if (result == asEXECUTION_EXCEPTION)
	{
		LogExceptionInfo(ctx);
		ReturnContext(ctx);
		return false;
	}

	ReturnContext(ctx);
	return true;
}

//여기서 module_name은 하나의 script를 의미
void Scripting::DiscardModule(const std::string & module_name)
{
	script_engine->DiscardModule(module_name.c_str());
}

void Scripting::LogExceptionInfo(asIScriptContext * ctx)
{
	auto exception_desc = ctx->GetExceptionString();
	auto function = ctx->GetExceptionFunction();
	//함수의 선언
	auto function_decl = function->GetDeclaration();
	auto module_name = function->GetModuleName();
	auto script_path = function->GetScriptSectionName();
	auto script_file = FileSystem::GetFileNameFromPath(script_path);
	auto exception_line = std::to_string(ctx->GetExceptionLineNumber());

	LOG_ERROR_F
	(
		"%s, at line %s, in function %s, in script %s",
		exception_desc,
		exception_line,
		function_decl,
		script_file
	);
}

void Scripting::Message_Callback(const asSMessageInfo & msg)
{
	auto file_name = FileSystem::GetFileNameFromPath(msg.section);
	auto message = msg.message;

	std::string final_message;
	if (file_name != "")
		final_message = file_name + " : " + message;

	else
		final_message = message;

	LOG_ERROR(final_message);
}
