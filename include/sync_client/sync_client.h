#include <trace_session/connection.h>

namespace Sync_client{

class Sync_client
{
	public:
		Sync_client();
		int deploy_thread(Genode::Trace::Threads tid,  unsigned prio);
		
	protected:


};
}
