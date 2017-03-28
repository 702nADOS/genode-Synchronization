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
	
	Sync_client::Sync_client() {}

	int Sync_client::deploy_thread(int *list) //gmc
	{
		int ds_size = ((1+2*list[0])*sizeof(int));
		Genode::Dataspace_capability _ds=Genode::env()->ram_session()->alloc(ds_size);
		int *newlist = Genode::env()->rm_session()->attach(_ds);
		for(int i=0;i<(1+2*list[0]);i++)
		{
			newlist[i]=list[i];
		}
		Genode::env()->cpu_session()->deploy_queue(_ds);		
		
		return 1;
	}

	Genode::Dataspace_capability Sync_client::init_ds(int num_rqs, int num_cores)
	{
		int ds_size = num_cores*(4 * sizeof(int)) + (num_rqs * sizeof(Rq_task::Rq_task));
		Genode::Dataspace_capability _ds=Genode::env()->ram_session()->alloc(ds_size);
		_rqs = Genode::env()->rm_session()->attach(_ds);
		return _ds;
	}
}
