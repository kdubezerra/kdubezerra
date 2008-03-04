#pragma once

// ZIG include files.
#include <zig/station.h>
#include <zig/msocket.h>
#include <zig/buffer.h>
#include <zig/leetnet.h>
//#include <zig/typereg.h>
#include <zig/address.h>

#include <string>
using std::string;

#include <map>
#include <set>
#include "config.h"

namespace P2PSEL0
{
	template< class T >
		class CGroupAbstract
		{
		public:
			/// Default constructor
			CGroupAbstract(): m_iMaxClients(0), m_oServer(station_c::get_invalid_station()) {
				m_id = INVALID_ID;
				m_dBannedTimeout = CGlobalInformation::m_dBannedTimeout;
			}

			/// Constructor
			CGroupAbstract(TGroupID id, const unsigned int iMaxClients, station_c& oServer): m_iMaxClients(iMaxClients), m_id(id), m_oServer(oServer) {
				m_dBannedTimeout = CGlobalInformation::m_dBannedTimeout;
			}

			/// Virtual destructor (important to abstract classes)
			virtual ~CGroupAbstract() 
			{
				while (GetNumberOfRemoteClients() > 0)
					RemoveClient(GetClientID(0));
			}

			/// Chat message
			virtual const bool Chat(const string& oMessage) = 0;
			/// Send group
			virtual const bool SendMessage(buffer_c& oData, const int iStream, const bool bReliable = true) = 0;
			/// Send group
			virtual const bool SendPacket()
			{
				for (typename std::map< TClientID, T* >::iterator it = m_oRemoteMap.begin(); it != m_oRemoteMap.end(); it++)
					it->second->SendPacket();
				return true;
			}
			/// Remove from this group and put in limbo
			virtual const bool ChangeToGroup(const TClientID iClientID, CGroupAbstract< T >& oGroup)
			{
				typename std::map< TClientID, T* >::iterator it = m_oRemoteMap.find(iClientID);
				T* oValue(it->second);
				m_oRemoteMap.erase(it);
				oGroup.m_oRemoteMap[iClientID] = oValue;
				return true;
			}

			/// Get ID
			virtual const TGroupID GetID() const { return m_id; }

			/// Add client
			virtual const bool CreateClient(station_c& oStation, TClientID id)
			{
				T* poClient = new T(oStation, id);
				m_oRemoteMap[id] = poClient;
				return true;
			}

			/// Force connected state
			virtual const bool ForceClientConnected(TClientID id)
			{
				typename std::map< TClientID, T* >::iterator it = m_oRemoteMap.find(id);
				if (it != m_oRemoteMap.end())
				{
					it->second->ForceConnectedState();
					return true;
				}
				return false;
			}
			/// Get number of remote clients
			virtual const int GetNumberOfRemoteClients() { return (int)m_oRemoteMap.size();}

			/// Get max number of clients in the group
			virtual const unsigned int GetMaxNumberOfClients() const { return m_iMaxClients; }

			/// Get CRemoteClient reference
			virtual const TClientID GetClientID(const unsigned int iIndex)
			{
				typename std::map< TClientID, T* >::const_iterator it = m_oRemoteMap.begin();
				if (iIndex >= m_oRemoteMap.size())
					return INVALID_ID;
				for (unsigned int i = 0; i < iIndex; i++, it++) ;
				return it->first;
			}

			virtual const bool IsReferenced(const TClientID id)
			{
				/// Check if it's referenced by any internal map
				return (m_oRemoteMap.find(id) != m_oRemoteMap.end());
			}

			virtual void GetClientSet(std::set< TClientID >& oSet)
			{
				for (typename std::map< TClientID, T* >::iterator it = m_oRemoteMap.begin(); it != m_oRemoteMap.end(); it++)
					oSet.insert(it->first);
			}


			virtual const bool RemoveClient(const TClientID id)
			{
				typename std::map< TClientID, T* >::iterator it = m_oRemoteMap.find(id);
				if (it != m_oRemoteMap.end())
				{
					SAFEDELETE(it->second);
					m_oRemoteMap.erase(it);
					return true;
				}
				return false;
			}

			virtual void Clear()
			{
				while (GetNumberOfRemoteClients() > 0)
					RemoveClient(GetClientID(0));
			}

			virtual void OverrideStation(station_c& oStation) {}

			virtual void ProcessNonBlocking()
			{
				// Check remote timeouts (ATTENTION, only server can detect client timeouts!!)
				vector< TClientID > oClient;
				for (typename std::map< TClientID, T* >::iterator it = m_oRemoteMap.begin(); it != m_oRemoteMap.end(); it++)
					if (it->second->Timeout())
					{
						oClient.push_back(it->first);
						/// KILL HIM NOW!
					}
				for (int i = 0; i < (int) oClient.size(); i++)
					RemoveClient(oClient[i]);

				// Remove banned clients
				vector< TClientID > oRemovedBanned;
				double now = get_time();
				for (std::map< TClientID, double >::iterator it = m_oBannedClients.begin(); it != m_oBannedClients.end(); it++) {
					if ( (it->second != 0.0) && (it->second + m_dBannedTimeout < now) )
					{
						oRemovedBanned.push_back(it->first);
					}
				}

				for (int i = 0; i < (int) oRemovedBanned.size(); i++) {
					std::map< TClientID, double >::iterator it = m_oBannedClients.find(oRemovedBanned[i]);
					m_oBannedClients.erase(it);
				}
			}

			/// Set limbo if group needs to know it.
			virtual void SetGroupLimbo(CGroupAbstract< T >& oGroup) {}

			/// Get client by id
			virtual T* GetClientPointer(const TClientID id)
			{
				typename std::map< TClientID, T* >::iterator it = m_oRemoteMap.find(id);
				if (it == m_oRemoteMap.end())
					return NULL;
				return it->second;
			}

			virtual const bool IsBanned(const TClientID id) {

				std::map< TClientID, double >::iterator it = m_oBannedClients.find(id);
				if (it == m_oBannedClients.end())
					return false;
        else {printf("agora eh %f, ID %d tah banido ate %f do grupo %d\n", get_time(), id, it->second + m_dBannedTimeout, GetID() ); return true; }
			}

			// TODO TODO TODO: add API in CServerInterface to ban clients from a group
			virtual const bool BanClient(const TClientID id) {
				
				if (!IsReferenced(id))
					return false;
		
				std::map< TClientID, double >::iterator it = m_oBannedClients.find(id);
				if (it != m_oBannedClients.end()) {
					return false;
				}

				m_oBannedClients[id] = get_time();
				return true;
			}

			virtual const bool HasSomethingToSend()
			{
				for (typename std::map< TClientID, T* >::iterator it = m_oRemoteMap.begin(); it != m_oRemoteMap.end(); it++)
					if (it->second->HasSomethingToSend())
					{
						printf("Group %d has something to send (%d)\n", m_id, it->first);
						return true;
					}
				return false;
			}

		protected:
			/// Max number of clients in this group
			const unsigned int m_iMaxClients;
			/// Map from client id to remote client
			std::map< TClientID, T* > m_oRemoteMap;
			/// Group ID
			TGroupID m_id;
			/// Client-to-Server station
			station_c& m_oServer;
			/// Banned clients (waiting timeout to reconnect or banned forever)
			std::map< TClientID, double > m_oBannedClients;
			/// Banned timeout
			double m_dBannedTimeout;
		};
}

