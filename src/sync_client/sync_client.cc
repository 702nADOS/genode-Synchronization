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

#include <trace_session/connection.h>
#include <timer_session/connection.h>

/* Fiasco includes */
namespace Fiasco {
#include <l4/sys/debugger.h>
#include <l4/sys/factory.h>
#include <l4/sys/irq.h>
#include <l4/sys/scheduler.h>
#include <l4/sys/thread.h>
#include <l4/sys/types.h>
#include <l4/sys/consts.h>
}

using namespace Genode;

/* local includes */
#include "sync_client/sync_client.h"

namespace Sync_client{
	
	Sync_client::Sync_client() {
		Genode::Dataspace_capability ds_cap=init_ds(32,1);

    sched.set_sync_ds(ds_cap);

    the_cycle();
}

	int Sync_client::deploy_thread(int *list) //gmc
	{
		int ds_size = ((1+2*list[0])*sizeof(int));
		Genode::Ram_dataspace_capability _ds=Genode::env()->ram_session()->alloc(ds_size);
		int *newlist = Genode::env()->rm_session()->attach(_ds);
		for(int i=0;i<(1+2*list[0]);i++)
		{
			newlist[i]=list[i];
		}
		Genode::env()->cpu_session()->deploy_queue(_ds);		
		Genode::env()->ram_session()->free(_ds);
		return 1;
	}

	Genode::Dataspace_capability Sync_client::init_ds(int num_rqs, int num_cores)
	{
		int ds_size = num_cores*(4 * sizeof(int)) + (num_rqs * sizeof(Rq_task::Rq_task));
		Genode::Dataspace_capability _ds=Genode::env()->ram_session()->alloc(ds_size);
		_rqs = new Sched_controller::Rq_buffer<Rq_task::Rq_task>[num_cores];
		for (int i = 0; i < num_cores; i++) {
			_rqs[i].init_w_shared_ds(_ds);
		}
		return _ds;
	}

	void Sync_client::the_cycle()
	{
		
		    while(1){
			int list[100];
			int counter=1;
			sched.are_you_ready();
			Rq_task::Rq_task *dequeued_task;
			while(1)
			{
				_rqs->deq(&dequeued_task);
				if(dequeued_task==nullptr) break;
				list[2*counter-1]=(*dequeued_task).task_id;
				list[2*counter]=(*dequeued_task).prio;
				counter++;
			}
			list[0]=counter-1;
			deploy_thread(list);
    		    }
	}
}
