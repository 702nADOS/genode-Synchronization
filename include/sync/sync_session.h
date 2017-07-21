#pragma once

#include <session/session.h>
#include <base/rpc.h>
#include <ram_session/ram_session.h>
#include <string>
#include <trace_session/connection.h>

namespace Sync {

	struct Session : Genode::Session
	{

		static const char *service_name() { return "sync"; }

		virtual void deploy(Genode::Dataspace_capability ds_cap, int type, int core) = 0;

		GENODE_RPC(Rpc_deploy, void, deploy, Genode::Dataspace_capability, int, int);

		GENODE_RPC_INTERFACE(Rpc_deploy);

	};

}
