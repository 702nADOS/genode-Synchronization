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
#include "sync/sync.h"

namespace Sync{
	
Sync::Sync()
{
	//get num cores from monitor
	num_cores=mon_manager.get_num_cores();
}

	void Sync::deploy(Genode::Dataspace_capability sync_ds_cap, int type, int core)
	{
		Genode::printf("deploy on core %d with strategy %d\n", core, type);
		Genode::env()->cpu_session()->deploy_queue(sync_ds_cap);		
	}
}
