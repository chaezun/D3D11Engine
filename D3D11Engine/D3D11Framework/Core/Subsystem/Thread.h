#pragma once
#include "ISubsystem.h"

class Task final
{
public:
	typedef std::function<void()> Process;

public:
	Task(Process&& process)
	{
		this->process=std::forward<Process>(process);
	}
	~Task()=default;

	void Execute()
	{
		is_executing =true;
		process();
		is_executing =false;
	}

	auto IsExecuting() const -> const bool& { return is_executing;}

private:
   Process process;
   bool is_executing = false;
};

class Thread final :public ISubsystem
{
public:
	Thread(class Context* context);
	~Thread();

	const bool Initialize() override;

	auto GetThreadCount() const -> const uint& { return thread_count; }
	auto GetThreadCountMax() const -> const uint& { return thread_count_max; }
	auto GetThreadAvailable() -> const uint;

	void Invoke();

	template<typename Process>
	void AddTask(Process&& process);

	template<typename Process>
	void Loop(Process&& process, const uint& range);

private:
    std::vector<std::thread> threads;
	std::deque<std::shared_ptr<Task>> tasks;
	std::mutex task_mutex;
	//condition_variable: 스레드간 통신을 가능하게 해줌
	std::condition_variable condition_var;
	uint thread_count_max = 0;
	uint thread_count     = 0;
	bool is_stopping      = false;
};

template<typename Process>
inline void Thread::AddTask(Process && process)
{
	if (threads.empty())
	{
		LOG_WARNING("No available threads, function will execute in the same thread");
		process();
		return;
	}

	std::unique_lock<std::mutex> lock(task_mutex);

	tasks.push_back(std::make_shared<Task>(process));
    
	lock.unlock();

	//해당 조건 변수를 기다리고 있는 스레드 중 한 개의 스레드를 깨움
	condition_var.notify_one();
}

template<typename Process>
inline void Thread::Loop(Process && process, const uint & range)
{
   uint available_threads = GetThreadAvailable();
   uint task_count        = available_threads + 1;

   std::vector<bool> task_dones(available_threads, false);

   uint start = 0;
   uint end = 0;
   for (uint i = 0; i < available_threads; i++)
   {
	   start = (range / task_count) * i;
	   end=start + (range/ task_count);

	   AddTask([&process, &task_dones, i, start, end]() { process(start, end); task_dones[i] = true; });
   }

   process(end, range);

   uint complete_tasks = 0;
   while (complete_tasks != task_dones.size())
   {
	   complete_tasks = 0;
	   for(const bool job_done : task_dones)
	      complete_tasks += job_done ? 1 : 0;
   }
}

//Task -> process -> thread 
//     -> process 

//process ----------------------------------------------------------
//                  thread      thread
//                  -stack      -stack
//- heap
//- data
//- code
//------------------------------------------------------------------
