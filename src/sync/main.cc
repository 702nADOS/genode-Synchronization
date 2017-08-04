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
#include "sched_controller/rq_buffer.h"
#include "rq_task/rq_task.h"
#include <vector>

#include "sched_controller_session/connection.h"

//using namespace Genode;

#include "sync/sync.h"
#include "sync/sync_session.h"
#include <base/trace/types.h>
#include <timer_session/connection.h>
#include <cap_session/connection.h>
#include <os/server.h>
#include <root/component.h>
#include <base/signal.h>
#include <base/thread_state.h>
#include <base/sleep.h>

using namespace Genode;

namespace Sync
{

	struct Session_component : Genode::Rpc_object<Session>
	{

		private:

			Sync *_sync = nullptr;

		public:

			void deploy(Genode::Dataspace_capability sync_ds_cap, int type, int core)
			{
				_sync->deploy(sync_ds_cap,type,core);
			}

			Session_component(Sync *sync)
			: Genode::Rpc_object<Session>()
			{
				_sync = sync;
			}

	};

	class Root_component : public Genode::Root_component<Session_component>
	{

		private:

			Sync *_sync = nullptr;

		protected:

			Session_component *_create_session(const char *args)
			{
				return new (md_alloc()) Session_component(_sync);
			}

		public:

			Root_component(Genode::Rpc_entrypoint *ep,
			               Genode::Allocator *allocator,
			               Sync *sync)
			: Genode::Root_component<Session_component>(ep, allocator)
			{
				_sync = sync;
			}
	};

}

int foo()
{
	//Rq_manager::Connection rqm;
	
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
	//num_rqs = rqm.get_num_rqs(); /* Actual number of run queues */
	PINF("Number of supplied run queues is: %d", num_rqs);


	/* get dataspace capability for shared dataspace of run queue of core 0 */
	/* 
	 * Get the dataspace capability for all available cores 
	 * First check how many cores are available and then get dsc for all cores
	 */

	for(int i=0; i<num_rqs; i++)
	{
		//dsc.push_back(rqm.get_core_rq_ds(i));
		
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
		Genode::printf("[Synch_client] Executing synch client");
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



using namespace Genode;
using namespace Fiasco;

int main()
{
    Sync::Sync sync;

	Cap_connection cap;

	static Sliced_heap sliced_heap(env()->ram_session(),
	                               env()->rm_session());

	enum { STACK_SIZE = 4096 };
	static Rpc_entrypoint ep(&cap, STACK_SIZE, "sync_ep");

	static Sync::Root_component sync_root(&ep, &sliced_heap, &sync);
	env()->parent()->announce(ep.manage(&sync_root)); 

	Genode::sleep_forever();

	return 0;
}
