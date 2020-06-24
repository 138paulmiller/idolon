#include "pch.h"
#include "pool.h"
namespace
{
	uint32_t s_count;
	
	std::mutex s_queuemutex;
	std::queue<Job> s_jobqueue;
	
	std::mutex s_poolmutex;
	std::vector<std::thread> s_threads;
	
	std::condition_variable s_jobcondition;


	bool s_running;
}

namespace Pool
{

	void Runner()
	{
		for (;;)
		{
			std::unique_lock<std::mutex> lock(s_queuemutex);
				//wait until job queue is not empty, or 
				s_jobcondition.wait(lock, [] {return !s_jobqueue.empty() || !s_running; });
				Job job = s_jobqueue.front();
				s_jobqueue.pop();
			lock.unlock();			
			
			job();
		}
	}

	void Startup()
	{
		{
			std::unique_lock<std::mutex> lock(s_poolmutex);
			s_running = true;
		}
		const int s_count =  std::thread::hardware_concurrency();
		s_threads.reserve(s_count);
		for (int i = 0; i < s_count; i++)
		{
			s_threads.push_back(std::thread(Runner));
		}
	}

	void Shutdown()
	{
		{
			std::unique_lock<std::mutex> lock(s_poolmutex);
			s_running = false;
		}
		//wakeup all threads
		s_jobcondition.notify_all();
	}
	
	void Run(const Job & job)
	{
		std::unique_lock<std::mutex> lock(s_queuemutex);
		s_jobqueue.push(job);
		lock.unlock();	
		//wakeup thread
		s_jobcondition.notify_one();
	}
}