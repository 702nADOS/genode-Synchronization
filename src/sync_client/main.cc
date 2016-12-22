/*
 * \brief  Synchornization client to access shared mem with Rq_manager
 * \author Guru
 * \date   2016/08/24
 *
 * This component creates a new connection to the Rq_manager.
 * It then requests a dataspace capability from the Rq_manager
 * where the shared dataspace for one run queue can be found.
 */

#include <base/env.h>
#include <base/printf.h>
#include <spec/arm/cpu/atomic.h> /* atomic access to int values on arm CPUs */
#include "rq_manager_session/client.h"
#include "rq_manager_session/connection.h"
#include "rq_manager/rq_buffer.h"
#include "rq_task/rq_task.h"
#include <vector>

using namespace Genode;

#include "sync_client/sync_client.h"
#include "rq_manager/rq_manager.h"
#include <base/trace/types.h>
#include <timer_session/connection.h>

using namespace Genode;

int foo()
{
	Rq_manager::Connection rqm;
	
	/* using vectors since we dont know the size initially */
	std::vector<int *> _rqbufp;
	std::vector<int *> _lock;
	std::vector<int *> head; 
	std::vector<int *> tail; 
	std::vector<int *> window;
	
	std::vector<Dataspace_capability> dsc;
	std::vector<Rq_task::Rq_task*> buf;
	
	/* check for the number of run queues available: 
	 * This is the first call since rest variable depends on this
	 */

	int num_rqs = 1; /* Generally at least one queue should be available */
	num_rqs = rqm.get_num_rqs(); /* Actual number of run queues */
	PINF("Number of supplied run queues is: %d", num_rqs);


	/* get dataspace capability for shared dataspace of run queue of core 0 */
	/* 
	 * Get the dataspace capability for all available cores 
	 * First check how many cores are available and then get dsc for all cores
	 */

	for(int i=0; i<num_rqs; i++)
	{
		dsc.push_back(rqm.get_core_rq_ds(i));
		
		/*  Attach dataspace capability to the _rqbufp pointer, i.e.
		 * the shared memory starts at the pointer position _rqbufp 
		 */
		_rqbufp.push_back(env()->rm_session()->attach(dsc[i]));
		
		_lock.push_back( (int*) (_rqbufp[i] + (0 * sizeof(int))));
		head.push_back((int*) ( _rqbufp[i] + (1 * sizeof(int))));
		tail.push_back((int*) (_rqbufp[i] + (2 * sizeof(int))));
		window.push_back((int*) (_rqbufp[i] + (3 * sizeof(int))));
		buf.push_back((Rq_task::Rq_task*) (_rqbufp[i] + (4 * sizeof(int))));
	}

	PINF("Got Dataspace_capabilities :)");
    /*
     * TODO Get the trace and the thread data strcture
     *
     * static Genode::Trace::Connection trace(1024*4096, 64*4096, 0);
     *
     * Trace::Threads thread_info;
     */
	while(1)
	{
		printf("[Synch_client] Executing synch client");
        for(int i=0; i<num_rqs; i++){
	
		if (cmpxchg(_lock[i], false, true)) {
			//PINF("Obtained lock, now set to: %d", _lock);


            /*TODO : Notes Guru
             * Here this should pick up threads available in shared space
             * and use trace service to update them to the kernel ready queue
             *  
             * Once the function returns update the pointers head and window
             *
             * Should be extended similar to this,
             * Copy the threads into the thread_info structure created above
             * and call the deply_thread method
             *
             *
             * for(int k=0;k<number_of_threads_in_shared_space; k++)
             * {
             *      thread_info.id[k] = 
             *      thread_info.prio[k] =  
             * }
             * thread_info.n = k;
    	     * 
             * int ret = trace.deploy_thread(threads, threads[i].prio);
             *
             * update the return value to shared space
             */
	

			/* unset the lock again to enable access by other functions */
			//*_lock = false;

		} 
		else {
			PWRN("Did not obtain lock");
	  	}
	}
	}



}

struct thread_info {
    unsigned long id;
    char name[32];
    unsigned prio;  
}threads[15];


using namespace Genode;
using namespace Fiasco;

/*
 * Thread class
 */
class Mythread : public Genode::Thread<2*4096>
{
    public:

        Mythread() : Thread("MyThread") { }

        void func()
        {
            PINF("I am a thread!\n\n");
        }

        void entry()
        {
            func();
        }

};

/* 
 * Helper class to create the threads
 */
class Thread_creator
{
    Mythread myt;
    public:
    int run_thread(int);
    int create_thread(int);
};

/*
 * Thread is run from here
 * The entry function of Mythread class will run
 */
int Thread_creator::run_thread(int thread_num)
{
    myt.start();

    Genode::Thread_capability mycap = myt.cap();
    PINF("Got Thread capability information. %lx\n", myt.tid());

    myt.join();
}

int Thread_creator::create_thread(int thread_num)
{
    printf("Now we will create thread %i.\n", thread_num);

    Genode::Thread_capability mycap = myt.cap();
    PINF("Got Thread capability information. %lx\n", myt.tid());

    return 0;
}

/*
 * Trace function called from main which returns the thread information
 */
int trace()
{
    using namespace Genode;

    Sync_client::Sync_client sync;

    static Genode::Trace::Connection trace1(1024*4096, 64*4096, 0);

    static Timer::Connection timer;

    Trace::CPU_info last_step[32];


    int timestamp = 1000;

    Trace::Threads ttid;

//    unsigned long tid[5];
    int k = 0;	

    timer.msleep(timestamp);

    Trace::Subject_id subjects[32];
    size_t num_subjects = trace1.subjects(subjects, 32);

    unsigned long long idle;

    printf("%zd tracing subjects present\n", num_subjects);

    for (size_t i = 0; i < num_subjects; i++) {
        Trace::CPU_info info = trace1.cpu_info(subjects[i]);
        //Trace::RAM_info ram_info = trace1.ram_info(subjects[i]);
        if(strcmp(info.session_label().string(), "init")!=0&&strcmp(info.session_label().string(), "init -> idle")!=0) {

            threads[i].id = info.id();
            threads[i].prio = info.prio();
            strncpy(threads[i].name, info.thread_name().string(), 9);

            if( (strcmp(threads[i].name, "MyThread", 8) == 0)) // Only the MyThread thread ids are being extracted
            {
              //  tid[k] = threads[i].id;
                ttid.id[k] = info.id();; // This is the id of the  threads in the trace file
                ttid.prio[k] = info.prio();
                k++;
            } 
            ttid.n = 5; // Number of threads to be sent
            printf("ID:%d %lld prio:%d  thread_id: %lx %s name:%s\n",
                    subjects[i].id,
                    (info.execution_time().value-last_step[i].execution_time().value)/(timestamp*10),
                    info.prio(),
                    info.id(),
                    info.session_label().string(),
                    info.thread_name().string());
        }

        if(strcmp(info.thread_name().string(), "idle")==0) {
            idle=100-((info.execution_time().value-last_step[i].execution_time().value)/(timestamp*10));
        }

        last_step[i]=info;
}


for(int i=0;i <num_subjects; i++){
    if( (strcmp(threads[i].name, "MyThread", 8) == 0)) // If MyThread id is obtained then a call to deploy thread 
    {
        int value = sync.deploy_thread(ttid, threads[i].prio);
        break;
    }
}

printf("\n--- test-trace finished ---\n\n");
return 0;
}

int main()
{
    PDBG("Hello World! I am a process\n");
    printf("And this is how normal printf looks like...\n");

    Thread_creator t[5];

    trace();

    for (int i = 0; i < 5; i++) {
        t[i].run_thread(i);
    }

    printf("\n Trace Finished: Back to Main function \n");

    return 0;
}
