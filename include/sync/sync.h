#include <base/heap.h>
#include <trace_session/connection.h>
#include "sched_controller/rq_buffer.h"
#include "sched_controller_session/connection.h"
#include "mon_manager/mon_manager_connection.h"

namespace Sync{

class Sync
{
	public:
		Sync();
		void deploy(Genode::Dataspace_capability sync_ds_cap, int type, int core);
		
	private:
		int num_cores;
		Mon_manager::Connection mon_manager;
};
}
