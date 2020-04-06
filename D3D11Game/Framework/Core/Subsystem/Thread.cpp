#include "Framework.h"
#include "Thread.h"

Thread::Thread(Context * context)
	:ISubsystem(context)
{
	thread_count_max = std::thread::hardware_concurrency();

	//���� ���α׷��� �����带 1�� ������ �������� �޾ƿ�
	thread_count= thread_count_max-1;
}

Thread::~Thread()
{
	std::unique_lock<std::mutex> lock(task_mutex);

	is_stopping = true;

	lock.unlock();

	//�ش� ���� ������ ��ٸ��� �ִ� ��� �����带 ����
	condition_var.notify_all();

	for (auto& thread : threads)
		//�ش� �����尡 ����Ǳ���� ��ٷȴٰ� �������� �Ѿ
		thread.join();

	threads.clear();
	//������ capacity�� �����Ͱ� �սǵ��� �ʴ� �� �ּ������� ����
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

	//���� ��밡���� �������� ������ ��ȯ
	return thread_count - invalid_threads;
}

void Thread::Invoke()
{
	//�۾� ����� ���� �ӽú���
	std::shared_ptr<Task> task;

	while (true)
	{
		//���� ����� ��ü ����
		//���⼭�� ������ ���ÿ� lock�� ��.
		std::unique_lock<std::mutex> lock(task_mutex);

		//���⼭ lock_guard�� ����� �� ���� ������ wait�Լ��� ���������� ���� Ǯ���ٰ� �ٽ� ����ִµ�
		//lock_guard�� ���ǿ� ���ÿ� ���� �ɸ��� �ı��� ���� ���� Ǯ �� �ֱ� �����̴�.

		//pred�Լ�(2��° ����)�� ��ȯ���� (����)�� ��� ��� ���� �ɸ��� (��)�� ��� ���� �����Ѵ�.
		/* 
		   condition_variable.wait�� ���Ǻκ�
			while (!_Pred())
			  wait(lock);
		*/

		//condition_var.wait�� ����ϴ� ������ unique_lock��ü�� ����Ͽ� 
		//�� ������ �Ǻ��ϴ� ������ ���ǿ� ���� ���� ������ �����ϱ� �����̴�.(�������� ����)

		//tasks.empty()�� ���� ��ȯ�ϸ�(�۾������ ��������� �ǹ�) ��� ���� �ɸ���. ������ is_stopping�� true�� ��� ���� Ǭ��.
		//tasks.empty()�� ������ ��ȯ�ϸ�(�۾���Ͽ� �۾��� ����) ���� Ǯ����.

		//��, �۾���Ͽ� �۾��� ������ �׻� lock�� Ǯ��(is_stopping�� flag���� �������) 
		//�۾� ��Ͽ� �۾��� ������ is_stopping�� flag���� ���� ���� Ǯ�� ���� ������.
		condition_var.wait(lock, [this]() {return !tasks.empty() || is_stopping; });

		//condition_var.wait���� ���� Ǯ�Ⱦ ������ ���� while�� Ż��
		//is_stopping�� true�̰� tasks�� queue�� �۾� ����� ���� ���
		if (is_stopping&&tasks.empty())
			return;

		//���� ���� �۾��� �ӽú����� ����
		task = tasks.front();

		//�۾� ����� �ӽú����� ���������� �ش� �۾� ����� ����
		tasks.pop_front();

		//���� ǰ
		lock.unlock();

		//�۾� ����
		task->Execute();
	}
}

