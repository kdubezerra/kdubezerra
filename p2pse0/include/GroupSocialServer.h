#pragma once

#include "GroupAbstract.h"
#include "RemoteClient.h"
#include "ServerEventInterface.h"

namespace P2PSEL0
{
	class CGroupLimboServer;

	class CGroupSocialServer: public CGroupAbstract< CRemoteClient >
	{
	public:
		/// Default constructor
		CGroupSocialServer(): CGroupAbstract<CRemoteClient>::CGroupAbstract(), m_poGroupLimbo(NULL) {}
		/// Constructor
		CGroupSocialServer(TGroupID id, const unsigned int iMaxClients, CServerEventInterface& oEventInterface): CGroupAbstract<CRemoteClient>::CGroupAbstract(id, iMaxClients, station_c::get_invalid_station()), m_poInterface(&oEventInterface) {};
		/// Chat message
		virtual const bool Chat(const string& oMessage);
		/// Send group
		virtual const bool SendMessage(buffer_c& oData, const int iStream = -1, const bool bReliable = true);
		/// Create client
		virtual const bool CreateClient(station_c& oStation, TClientID id);
		/// Set limbo group
		virtual void SetGroupLimbo(CGroupAbstract< CRemoteClient >& oGroup) {m_poGroupLimbo = (CGroupLimboServer*)&oGroup;}
		/// Remove using protocol
		virtual const bool RemoveClient(const TClientID id);
		/// Remove from this group and put in limbo
		virtual const bool ChangeToGroup(const TClientID iClientID, CGroupAbstract< CRemoteClient >& oGroup);
		/// Process capable clients
		virtual void ProcessNonBlocking();

		static const int GetTypeEnum() { return GROUP_SOCIAL_TYPE; }

	private:
		/// Limbo group
		CGroupLimboServer* m_poGroupLimbo;
		/// Callback interface
		CServerEventInterface* m_poInterface;
	};
}

