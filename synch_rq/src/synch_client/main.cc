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

/* Fiasco includes */
namespace Fiasco {
#include <l4/sys/debugger.h>
#include <l4/sys/factory.h>
#include <l4/sys/irq.h>
#include <l4/sys/scheduler.h>
#include <l4/sys/thread.h>
#include <l4/sys/types.h>
#include <l4/sys/rq_manager.h>
}

using namespace Genode;

int main()
{
	Rq_manager::Connection rqm;
	
	/* using vectors since we dont know the size initially */
	std::vector<int *> _rqbufp;
	std::vector<int *> _lock;
	std::vector<int *> head; 
	std::vector<int *> tail; 
	std::vector<int *> window;
	
	std::vector<Dataspace_capability> dsc;
	std::vector<Rq_manager::Rq_task*> buf;
	
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
		buf.push_back((Rq_manager::Rq_task*) (_rqbufp[i] + (4 * sizeof(int)))
	}

	PINF("Got Dataspace_capabilities :)");

	while(1)
	{
		for(int i=0; i<num_rqs; i++){
	
		if (cmpxchg(_lock[i], false, true)) {
			PINF("Obtained lock, now set to: %d", *_lock);


            /*TODO : Notes Guru
             * Here this should pick up threads available in shared space
             * and use trace service to update them to the kernel ready queue
             * 
             * Once the function returns update the pointers head and window
             *
             */
			/* copy content of buf[3] to variable task */

			/* unset the lock again to enable access by other functions */
			*_lock = false;

		} 
		else {
			PWRN("Did not obtain lock");
	  	}
	}
	}


	/* access content of buf[4] directly */
	PINF("Got task with task_id: %d, wcet: %d, valid: %d", buf[4].task_id, buf[4].wcet, buf[4].valid);
}
