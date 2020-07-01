#pragma once

enum EngineFlags : uint
{
	EngineFlags_Update = 1U << 0,
	EngineFlags_Render = 1U << 1,
	EngineFlags_Game = 1U << 2,
};

//프로그램의 시작점 -> D3D11Engine와 D3D11Framework를 연결해주는 역할
class Engine final
{
public: 
   static auto GetEngineFlags() -> const uint& {return engine_flags;}
   static void SetEngineFlags(const uint& flags) {engine_flags=flags;}
   static void FlagEnable(const EngineFlags& flag) {engine_flags |= flag;}
   static void FlagDisable(const EngineFlags& flag) { engine_flags &= ~flag; }
   static void FlagToggle(const EngineFlags& flag)
   {
	   IsFlagEnabled(flag) ? FlagDisable(flag) : FlagEnable(flag);
   }
   static auto IsFlagEnabled(const EngineFlags& flag) -> const bool
   {
      return (engine_flags & flag) >0U;
   } 

public:
	Engine();
	~Engine();

	auto GetContext() const -> class Context* { return context; }

	void Update();
	
private:
    static uint engine_flags;

	class Context* context;

};