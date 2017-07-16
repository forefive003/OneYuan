
#include <stdio.h>
#include <signal.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#include <process.h>
#else
#include <unistd.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <pthread.h>
#endif

#include "etpLib.h"
#include "ThreadPool.h"

CThreadPool::CWorker::CWorker(CThreadPool * pThreadPool) :
	m_pThreadPool(pThreadPool)
{
	m_workerStatus = WORKER_INIT;
	m_run_cnt = 0;
	m_update_time = time(NULL);
}

CThreadPool::CWorker::~CWorker()
{
}

void CThreadPool::CWorker::TellExit()
{
	m_workerStatus = WORKER_STOPPING;

#ifndef _WIN32
	pthread_mutex_lock(&m_pThreadPool->m_job_lock);
	pthread_cond_broadcast(&m_pThreadPool->m_job_notify);
	pthread_mutex_unlock(&m_pThreadPool->m_job_lock);
#else
	SetEvent(m_pThreadPool->m_Event);
#endif
}

bool CThreadPool::CWorker::Aged(unsigned long long agetime)
{
	if (!this->m_isBusy)
	{
		if (this->m_update_time <= (long int)agetime)
		{
			EL_LOG_INFO("aged thread %lu, updatime %lu, agedtim %lu.",
							this->GetThreadID(),
							this->m_update_time,
							agetime);

			/*aged*/
			this->TellExit();
			return true;
		}
	}

	return false;
}

void CThreadPool::CWorker::Run()
{
    Runnable * pTask = NULL;

    m_workerStatus = WORKER_STARTED;

    while(m_workerStatus == WORKER_STARTED)
    {
		pTask = m_pThreadPool->GetOneTask();

        if(NULL == pTask)
        {
            /*wait*/
#ifndef _WIN32
            struct timespec beattime;
			struct timeval now;

			gettimeofday(&now, NULL);
			beattime.tv_sec = now.tv_sec + 2;
			beattime.tv_nsec = now.tv_usec * 1000;

			pthread_mutex_lock(&m_pThreadPool->m_job_lock);
			pthread_cond_timedwait(&m_pThreadPool->m_job_notify, &m_pThreadPool->m_job_lock, &beattime);
			pthread_mutex_unlock(&m_pThreadPool->m_job_lock);
#else
			DWORD waittime = 2000;
			DWORD dReturn = WaitForSingleObject(m_pThreadPool->m_Event, waittime);
#endif
        }
        else
        {
        	m_isBusy = true;

            pTask->Run();
            delete pTask;
            pTask = NULL;

            m_run_cnt++;

            m_update_time = time(NULL);
            m_isBusy = false;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////

CThreadPool::CThreadPool(void) :
	m_bRun(false),
	m_bEnableInsertTask(false)
{
	MUTEX_SETUP(m_csTasksLock);
	MUTEX_SETUP(m_csThreadPoolLock);
}

CThreadPool::~CThreadPool(void)
{
    Terminate();

    MUTEX_CLEANUP(m_csTasksLock);
    MUTEX_CLEANUP(m_csThreadPoolLock);
}

bool CThreadPool::Init(unsigned int minThreads, unsigned int maxThreads,
							const char *name,
							unsigned int startCore,
							unsigned int coreCnt)
{
    if(minThreads == 0)
    {
        return false;
    }
    if(maxThreads < minThreads)
    {
        return false;
    }

    m_bRun = true;
	m_bEnableInsertTask = true;

#ifndef _WIN32
	pthread_mutex_init(&m_job_lock, NULL);
	pthread_cond_init(&m_job_notify, NULL);
#else
	m_Event = CreateEvent(NULL,FALSE,FALSE,NULL);
#endif

    m_name = name;
    m_start_core_id = startCore;
    m_cur_core_id = m_start_core_id;
    m_use_core_cnt = coreCnt;

    m_minThreads = minThreads;
    m_maxThreads = maxThreads;
    unsigned int i = m_ThreadPool.size();

    unsigned int core_id = 0;

    for(; i< minThreads; i++)
    {
        CWorker * pWorker = new CWorker(this);
        if(NULL == pWorker)
        {
            return false;
        }

        MUTEX_LOCK(m_csThreadPoolLock);
		m_ThreadPool.insert(pWorker);
		core_id = get_new_core();
		MUTEX_UNLOCK(m_csThreadPoolLock);

        pWorker->Start(core_id);
    }

    return true;
}

bool CThreadPool::IsAllWorkerBusy()
{
	ThreadPoolItr itr;
	CWorker *work;

	for (itr = m_ThreadPool.begin();
			itr != m_ThreadPool.end();
			itr++)
	{
		work = *itr;
		if (!work->m_isBusy)
		{
			return false;
		}
	}

	return true;
}


void CThreadPool::TellAllWorkerExit()
{
	CWorker *work;
	ThreadPool trashThread;
	int totalAgedCnt = 0;

	ThreadPoolItr itr = m_ThreadPool.begin();
	ThreadPoolItr itr_tmp;
	while(itr != m_ThreadPool.end())
	{
		itr_tmp = itr++;

		work = *itr_tmp;
		work->TellExit();
		m_ThreadPool.erase(itr_tmp);
		trashThread.insert(work);

		totalAgedCnt++;
	}

	ThreadPoolItr itr1 = trashThread.begin();
	while(itr1 != trashThread.end())
	{
		(*itr1)->Join(6);
		trashThread.erase(itr1);
		delete (*itr1);

		itr1 = trashThread.begin();
	}

	if (totalAgedCnt)
	{
		EL_LOG_DEBUG("delete %d threads.", totalAgedCnt);
	}
}

bool CThreadPool::Execute(Runnable * pRunnable)
{
    if(!m_bEnableInsertTask)
    {
        return false;
    }

    if(NULL == pRunnable)
    {
        return false;
    }

	if(IsAllWorkerBusy())
    {
        if(m_ThreadPool.size() < m_maxThreads)
        {
        	unsigned int core_id = 0;

            CWorker * pWorker = new CWorker(this);
            if(NULL == pWorker)
            {
                return false;
            }

			MUTEX_LOCK(m_csThreadPoolLock);
			m_ThreadPool.insert(pWorker);
			core_id = get_new_core();
			MUTEX_UNLOCK(m_csThreadPoolLock);

            pWorker->Start(core_id);
        }
    }

	MUTEX_LOCK(m_csTasksLock);
	m_Tasks.push_back(pRunnable);
	MUTEX_UNLOCK(m_csTasksLock);

	/*set to trigged*/
#ifndef _WIN32
	pthread_mutex_lock(&m_job_lock);
	pthread_cond_broadcast(&m_job_notify);
	pthread_mutex_unlock(&m_job_lock);
#else
	SetEvent(m_Event);
#endif
    return true;
}

Runnable * CThreadPool::GetOneTask()
{
    Runnable * Task = NULL;
	MUTEX_LOCK(m_csTasksLock);
	if(!m_Tasks.empty())
	{
		Task = m_Tasks.front();
		m_Tasks.pop_front();
	}
	MUTEX_UNLOCK(m_csTasksLock);

    return Task;
}

unsigned int CThreadPool::GetThreadPoolSize()
{
    return m_ThreadPool.size();
}

unsigned int CThreadPool::GetTaskSize()
{
	unsigned int ret = 0;

	MUTEX_LOCK(m_csTasksLock);
	ret = m_Tasks.size();
	MUTEX_UNLOCK(m_csTasksLock);

	return ret;
}

void CThreadPool::aged_worker(unsigned long long agetime)
{
	ThreadPool trashThread;
	int totalAgedCnt = 0;
	time_t now = time(NULL);
	CWorker *work = NULL;

	MUTEX_LOCK(m_csThreadPoolLock);

	ThreadPoolItr itr = m_ThreadPool.begin();
	ThreadPoolItr itr_tmp;
	while(itr != m_ThreadPool.end())
	{
		if (GetThreadPoolSize() <= this->m_minThreads)
		{
			break;
		}

		itr_tmp = itr++;

		work = *itr_tmp;
		if( work->Aged(agetime))
		{
			m_ThreadPool.erase(itr_tmp);
			trashThread.insert(work);

			totalAgedCnt++;
		}
	}

	ThreadPoolItr itr1 = trashThread.begin();
	while(itr1 != trashThread.end())
	{
		(*itr1)->Join(6);
		trashThread.erase(itr1);
		delete (*itr1);

		itr1 = trashThread.begin();
	}

	MUTEX_UNLOCK(m_csThreadPoolLock);

	if (totalAgedCnt)
	{
		EL_LOG_INFO("Aged %d threads, spend %lu s.", totalAgedCnt, time(NULL)-now);
	}
}

unsigned int CThreadPool::get_new_core()
{
	if (m_use_core_cnt != 0)
	{
		m_cur_core_id = m_start_core_id + (m_cur_core_id - m_start_core_id) % m_use_core_cnt;

		return m_cur_core_id++;
	}

	return NO_SPEC_CORE;
}

void CThreadPool::print_info()
{
	CWorker *work = NULL;
	ThreadPoolItr itr = m_ThreadPool.begin();

	int total = 0;
	int busyCnt = 0, totalThrd = 0;

	while(itr != m_ThreadPool.end())
	{
		work = *itr;
		total += work->m_run_cnt;

		if (work->m_isBusy)
		{
			busyCnt++;
		}

		totalThrd++;
		itr++;
	}

	EL_LOG_DEBUG("%s: Total run task %d, spare %lu, Total run thread %d. busy %d.",
			 	 this->m_name, total, m_Tasks.size(),totalThrd, busyCnt);
}

void CThreadPool::Terminate()
{
    m_bEnableInsertTask = false;
    while(m_Tasks.size() > 0)
    {
    	sleep_s(1);
    }
    m_bRun = false;
    m_minThreads = 0;
    m_maxThreads = 0;
    m_maxPendingTasks = 0;

    TellAllWorkerExit();
}

