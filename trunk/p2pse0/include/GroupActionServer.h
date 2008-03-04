#pragma once

#include <string>
#include "GroupAbstract.h"
#include "GroupLimboServer.h"
#include "RemoteClient.h"
#include "ServerEventInterface.h"

namespace P2PSEL0
{
	class CGroupActionServer: public CGroupAbstract< CRemoteClient >
	{
	public:
		/// Default constructor
		CGroupActionServer(): CGroupAbstract<CRemoteClient>::CGroupAbstract(), m_poInterface(NULL) {}
		/// Constructor
		CGroupActionServer(TGroupID id, const unsigned int iMaxClients, CServerEventInterface& oEventInterface): CGroupAbstract<CRemoteClient>::CGroupAbstract(id, iMaxClients, station_c::get_invalid_station()), m_poInterface(&oEventInterface) {}
		/// Chat message
		virtual const bool Chat(const string& oMessage);
		/// Send group
		virtual const bool SendMessage(buffer_c& oData, const int iStream, const bool bReliable = true);
		/// Remove from this group and put in limbo
		virtual const bool ChangeToGroup(const TClientID iClientID, CGroupAbstract<CRemoteClient>::CGroupAbstract& oGroup);
		/// Set limbo group
		virtual void SetGroupLimbo(CGroupAbstract< CRemoteClient >& oGroup) {m_poGroupLimbo = (CGroupLimboServer*)&oGroup;}
		/// Remove using protocol
		virtual const bool RemoveClient(const TClientID id);
		/// Process timeout, connects & etc...
		virtual void ProcessNonBlocking();

		static const int GetTypeEnum() { return GROUP_ACTION_TYPE; }

	protected:
		/// Limbo group
		CGroupLimboServer* m_poGroupLimbo;
	private:
		/// Notify incoming & group
		const bool NotifyChange(const TClientID id);
		/// Callback interface
		CServerEventInterface* m_poInterface;
	};
}

