#pragma once

#include <base/printf.h>
#include <base/rpc_client.h>
#include <sync/sync_session.h>

namespace Sync {
struct Session_client : Genode::Rpc_client<Session>
{
	Session_client(Genode::Capability<Session> cap):
		Genode::Rpc_client<Session>(cap) {}

	void deploy(Genode::Dataspace_capability sync_ds_cap, int type, int core)
	{
		call<Rpc_deploy>(sync_ds_cap, type, core);
	}
};
}
