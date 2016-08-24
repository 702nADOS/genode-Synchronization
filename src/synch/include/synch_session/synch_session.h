/*
 * \brief  Connection to Synch service
 * \author Guru
 * \date   08/22/2016
 */

#ifndef INCLUDE_SYNCH_SESSION_H
#define INCLUDE_SYNCH_SESSION_H

#include <session/session.h>
#include <base/rpc.h>

/* Fiasco includes */
/*namespace Fiasco {
#include <l4/sys/debugger.h>
#include <l4/sys/factory.h>
#include <l4/sys/irq.h>
#include <l4/sys/scheduler.h>
#include <l4/sys/thread.h>
#include <l4/sys/types.h>
}
*/
namespace Synch { struct Session; }

Struct Synch::Session : Genode::Session
{
      static const char *service_name() { return "SYNCH component"; }

        /*
         * Method to deploy a thread
         *
         * Take a thread from the shared memory and deploy to the ready queue
         */
        virtual bool deploy_thread( ) = 0;
        /*
         * Method to deploy more than one thread
         * No list will be provided from the controller as it is already exists in SM
         * */
        virtual bool deploy_threadlist() = 0;

        GENODE_RPC(Rpc_deploy_thread, bool, deploy_thread);
        GENODE_RPC(deploy_list, bool, deploy_list);
        GENODE_RPC_INTERFACE(Rpc_deploy_thread, deploy_list);
};
