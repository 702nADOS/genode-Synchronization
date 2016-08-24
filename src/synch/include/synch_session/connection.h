/*
 * \brief  Connection to Synch service
 * \author Guru
 * \date   08/22/2016
 */

#ifndef _INCLUDE__SYNCH_SESSION__CONNECTION_H_
#define _INCLUDE__SYNCH_SESSION__CONNECTION_H_

#include <hello_session/client.h>
#include <base/connection.h>

namespace Synch {

	struct Connection : Genode::Connection<Session>, Session_client
	{
		Connection()
		:
			/* create session */
			Genode::Connection<Synch::Session>(session("foo, ram_quota=4K")),

			/* initialize RPC interface */
			Session_client(cap()) { }
	};
}

#endif /* _INCLUDE__SYNCH_SESSION__CONNECTION_H_ */
