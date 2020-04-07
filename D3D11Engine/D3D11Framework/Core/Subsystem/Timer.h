#pragma once

class Timer final : public ISubsystem
{
public: 
	Timer(class Context* context);
	~Timer()=default;

	const bool Initialize() override;

	auto GetDeltaTimeMs() -> const float {return static_cast<float>(delta_time_ms);}
	auto GetDeltaTimeSec() -> const float { return static_cast<float>(delta_time_ms/1000.0);}

	void Update();
    
private:
    double delta_time_ms=0.0;
	std::chrono::high_resolution_clock::time_point previous_time;
};