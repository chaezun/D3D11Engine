#include "Framework.h"
#include "Thread.h"

Thread::Thread(Context * context)
	:ISubsystem(context)
{
	thread_count_max = std::thread::hardware_concurrency();

	//기존 프로그램의 스레드를 1개 제외한 나머지를 받아옴
	thread_count= thread_count_max-1;
}

Thread::~Thread()
{
	std::unique_lock<std::mutex> lock(task_mutex);

	is_stopping = true;

	lock.unlock();

	//해당 조건 변수를 기다리고 있는 모든 스레드를 깨움
	condition_var.notify_all();

	for (auto& thread : threads)
		//해당 스레드가 종료되기까지 기다렸다가 다음으로 넘어감
		thread.join();

	threads.clear();
	//벡터의 capacity를 데이터가 손실되지 않는 한 최소한으로 만듦
	threads.shrink_to_fit();
}

const bool Thread::Initialize()
{
	for (uint i = 0; i < thread_count; i++)
	{
		threads.emplace_back(std::thread(&Thread::Invoke, this));
	}

	LOG_INFO_F("%d thread have been created", thread_count);

	return true;
}

auto Thread::GetThreadAvailable() -> const uint
{
    uint invalid_threads = 0;
	for(const auto& task : tasks)
		invalid_threads += task->IsExecuting() ? 1 : 0;

	//현재 사용가능한 스레드의 개수를 반환
	return thread_count - invalid_threads;
}

void Thread::Invoke()
{
	//작업 목록을 담을 임시변수
	std::shared_ptr<Task> task;

	while (true)
	{
		//락을 담당할 객체 생성
		//여기서는 생성과 동시에 lock을 걺.
		std::unique_lock<std::mutex> lock(task_mutex);

		//여기서 lock_guard를 사용할 수 없는 이유는 wait함수가 내부적으로 락을 풀었다가 다시 잠궈주는데
		//lock_guard은 정의와 동시에 락이 걸리고 파괴될 때만 락을 풀 수 있기 때문이다.

		//pred함수(2번째 인자)의 반환값이 (거짓)일 경우 계속 락이 걸리고 (참)일 경우 락을 해제한다.
		/* 
		   condition_variable.wait의 정의부분
			while (!_Pred())
			  wait(lock);
		*/

		//condition_var.wait를 사용하는 이유는 unique_lock객체를 사용하여 
		//락 조건을 판별하는 이유는 조건에 따른 락의 유무를 결정하기 위함이다.(교착상태 방지)

		//tasks.empty()가 참을 반환하면(작업목록이 비어있음을 의미) 계속 락이 걸린다. 하지만 is_stopping이 true인 경우 락을 푼다.
		//tasks.empty()가 거짓을 반환하면(작업목록에 작업이 있음) 락이 풀린다.

		//즉, 작업목록에 작업이 있으면 항상 lock을 풀고(is_stopping의 flag값과 상관없이) 
		//작업 목록에 작업이 없으면 is_stopping의 flag값에 따라 락을 풀지 말지 결정함.
		condition_var.wait(lock, [this]() {return !tasks.empty() || is_stopping; });

		//condition_var.wait에서 락이 풀렸어도 조건을 통해 while문 탈출
		//is_stopping이 true이고 tasks의 queue에 작업 목록이 없을 경우
		if (is_stopping&&tasks.empty())
			return;

		//먼저 들어온 작업을 임시변수에 저장
		task = tasks.front();

		//작업 목록을 임시변수에 저장했으니 해당 작업 목록을 삭제
		tasks.pop_front();

		//락을 품
		lock.unlock();

		//작업 수행
		task->Execute();
	}
}

