#pragma once

#include <sync/sync_client.h>
#include <base/connection.h>

namespace Sync {

	struct Connection : Genode::Connection<Session>, Session_client
	{
		Connection() : Genode::Connection<Sync::Session>(session("sync, ram_quota=4096")),
		               Session_client(cap()) { }
	};


}
