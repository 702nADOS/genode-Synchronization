#include <trace_session/connection.h>
#include "sched_controller/rq_buffer.h"

namespace Sync_client{

class Sync_client
{
	public:
		Sync_client();
		int deploy_thread(Genode::Trace::Threads tid,  unsigned prio);
		Genode::Dataspace_capability init_ds(int num_rqs, int num_cores);
		
	private:
		Sched_controller::Rq_buffer<Rq_task::Rq_task> *_rqs;


};
}
