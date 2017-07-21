#include <trace_session/connection.h>
#include "sched_controller/rq_buffer.h"
#include "sched_controller_session/connection.h"

namespace Sync{

class Sync
{
	public:
		Sync();
		void deploy(Genode::Dataspace_capability ds_cap, int type, int core);
		int deploy_thread(int *list);
		Genode::Dataspace_capability init_ds(int num_rqs, int num_cores);
		
	private:
		void the_cycle();
		Sched_controller::Connection sched;
		Sched_controller::Rq_buffer<Rq_task::Rq_task> *_rqs;


};
}
