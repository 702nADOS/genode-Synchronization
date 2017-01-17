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
}

using namespace Genode;

/* local includes */
#include "sync_client/sync_client.h"

namespace Sync_client{
	
	Sync_client::Sync_client() {}

	int Sync_client::deploy_thread(Genode::Trace::Threads tid,  unsigned prio) //gmc
	{
		PWRN("Deploying threads");

		Fiasco::l4_sched_thread_list tlist;

		/*
		 * Fill the thread list to the kernel list
		 * Have to make the same structure on both sides for transparency
		 */

		for(int i = 0; i< tid.n; i++)
		{
			tlist.list[i] = tid.id[i];
			tlist.prio[i] = tid.prio[i];
		}
		tlist.n = tid.n;
		Fiasco::l4_sched_param_t params = Fiasco::l4_sched_param(prio);

		Fiasco::l4_msgtag_t tag = Fiasco::l4_scheduler_deploy_thread(Fiasco::L4_BASE_SCHEDULER_CAP, tlist, &params);
		if (Fiasco::l4_error(tag)){
			PWRN("Scheduling thread has failed %lx %lx %d", tid.id[0], tid.id[1], tlist.n);
			return 0;
		}
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
