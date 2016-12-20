/*
 * \brief  Create threads and schedule them using kernel synchronization module
 * \author Guru Chandrasekhara
 * \date   2016-06-16
 *
 * The threads are created and added using Trace service
 */


/* Genode includes */

#include <util/list.h>
#include <util/string.h>

#include <base/printf.h>
#include <base/thread.h>
#include <base/env.h>
#include <base/rpc_client.h>
#include <cpu_session/cpu_session.h>
#include <cpu_session/client.h>
#include <parent/parent.h>

#include <ram_session/connection.h>
#include <cpu_session/connection.h>
#include <trace_session/connection.h>
#include <timer_session/connection.h>

#include <foc_cpu_session/connection.h>

#include <os/config.h>
#include <base/sleep.h>

/*
 * Structure used send the thread information to Genode API
 */
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
        int value = trace1.deploy_thread(ttid, threads[i].prio);
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
