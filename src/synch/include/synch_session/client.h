/*
 * \brief  Connection to Synch service
 * \author Guru
 * \date   08/22/2016
 */
#ifndef _INCLUDE_SYCH_SESSION_H__CLIENT_H_
#define _INCLUDE__SYNCH_SESSION_H__CLIENT_H_

#include <hello_session/hello_session.h>
#include <base/rpc_client.h>
#include <base/printf.h>

namespace Synch {

	struct Session_client : Genode::Rpc_client<Session>
	{
		Session_client(Genode::Capability<Session> cap)
		: Genode::Rpc_client<Session>(cap) { }

		void deploy_thread()
		{
			PDBG("Deploying a thread from user RQ.");
			call<Rpc_deplo_thread>();
		}

		int deploy_rqlist()
		{
			return call<Rpc_deploy_rqlist>();
		}
	};
}

#endif /* _INCLUDE__SYNCH_SESSION_H__CLIENT_H_ */
