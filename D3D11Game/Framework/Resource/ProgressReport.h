#pragma once
#include "Framework.h"

struct Progress final
{
	Progress()
	{
		Clear();
	}

	void Clear()
	{
		status.clear();
		jobs_done = 0;
		job_count = 0;
		is_loading = false;
	}

	std::string status;
	int jobs_done;
	int job_count;
	bool is_loading;
};

enum class ProgressType :uint
{
	Scene,
	Model,
};

class ProgressReport final
{
public:
	static ProgressReport& Get()
	{
		static ProgressReport instance;
		return instance;
	}

	//Status
	auto GetStatus(const ProgressType& type) ->const std::string
	{
		return reports[type].status;
	}

	void SetStatus(const ProgressType& type, const std::string& status)
	{
		reports[type].status = status;
	}

	//Loading
	auto IsLoading(const ProgressType& type) ->const bool
	{
		return reports[type].is_loading;
	}

	void SetLoading(const ProgressType& type, const bool& is_loading)
	{
		reports[type].is_loading = is_loading;
	}

	//Precentage
	auto GetPrecentage(const ProgressType& type) ->const float
	{
		float jobs_done = static_cast<float>(reports[type].jobs_done);
		float job_count = static_cast<float>(reports[type].job_count);

		return jobs_done / job_count;
	}

	//JobsDone
	void SetJobsDone(const ProgressType& type, const int& jobs_done)
	{
		auto& report = reports[type];

		report.jobs_done = jobs_done;

		if (report.jobs_done >= report.job_count)
			report.is_loading = false;
	}

	//JobCount
	void SetJobCount(const ProgressType& type, const int& job_count)
	{
		auto& report = reports[type];

		report.job_count = job_count;

		report.is_loading = true;
	}

	//JobsDone +1¾¿ Áõ°¡
	void IncrementJobsDone(const ProgressType& type)
	{
		auto& report = reports[type];

		report.jobs_done++;

		if (report.jobs_done >= report.job_count)
			report.is_loading = false;
	}

	void Reset(const ProgressType& type)
	{
		reports[type].Clear();
	}

private:
	ProgressReport()
	{
		reports[ProgressType::Scene] = Progress();
		reports[ProgressType::Model] = Progress();
	}

private:
	std::map<ProgressType, Progress> reports;
};