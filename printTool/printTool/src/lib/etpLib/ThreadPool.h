
#ifndef __THREAD_POOL_EXECUTOR__
#define __THREAD_POOL_EXECUTOR__

#include <set>
#include <list>

#include "Thread.h"

enum
{
	WORKER_INIT = 0,
	WORKER_STARTED,
	WORKER_STOPPING,
	WORKER_STOPPED
};


class CThreadPool
{
public:
    CThreadPool(void);
    ~CThreadPool(void);

    bool Init(unsigned int minThreads, unsigned int maxThreads,
    		const char *name,
			unsigned int startCore,
			unsigned int coreCnt);

    bool Execute(Runnable * pRunnable);
    void Terminate();

    unsigned int GetThreadPoolSize();
    unsigned int GetTaskSize();
    Runnable * GetOneTask();

    bool IsAllWorkerBusy();
    void TellAllWorkerExit();

#ifndef _WIN32
    pthread_mutex_t m_job_lock;
	pthread_cond_t m_job_notify;
#else
	HANDLE m_Event;
#endif

    unsigned int get_new_core();
    void print_info();
    void aged_worker(unsigned long long agetime);

private:
    class CWorker : public CThread
    {
    public:
        CWorker(CThreadPool * pThreadPool);
        ~CWorker();
        void Run();
        void TellExit();

        bool Aged(unsigned long long agetime);

        volatile bool m_isBusy;
        unsigned long m_run_cnt;

    private:
        CThreadPool * m_pThreadPool;

        unsigned long long m_update_time;
        volatile int m_workerStatus;
    };

    typedef std::set<CWorker *> ThreadPool;
    typedef std::list<Runnable *> Tasks;
    typedef Tasks::iterator TasksItr;
    typedef ThreadPool::iterator ThreadPoolItr;

    ThreadPool m_ThreadPool;
    ThreadPool m_TrashThread;
    Tasks m_Tasks;

    MUTEX_TYPE m_csTasksLock;
    MUTEX_TYPE m_csThreadPoolLock;

    volatile bool m_bRun;
    volatile bool m_bEnableInsertTask;
    volatile unsigned int m_minThreads;
    volatile unsigned int m_maxThreads;
    volatile unsigned int m_maxPendingTasks;

    const char *m_name;

    unsigned int m_cur_core_id;
    unsigned int m_start_core_id;
    unsigned int m_use_core_cnt;
};

#endif

