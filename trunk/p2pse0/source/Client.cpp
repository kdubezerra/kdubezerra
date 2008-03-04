#include <iostream>
#include <set>
#include <algorithm>

#include "Client.h"
using namespace P2PSEL0;

//-------------------------------------------------------------------------
//Facade for client interface
//-------------------------------------------------------------------------
CClientInterface* CClientInterface::GetNewClient(CClientEventInterface& oListener)
{
	return new CClient(oListener);
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
CClient::CClient(CClientEventInterface& oEventInterface): m_sGroup(*(CGroupLimboClient*)CreateGroup< CGroupLimboClient > (0, station_c::get_invalid_station()))
{
	m_poStation = NULL;
	m_poConsole = &console_c::get_default_console();
	
	m_iStreamIDGenerator = 0;

	m_poInterface = &oEventInterface;
	///I am in Limbo
	m_iGroupID = GetLimboGroup().GetID();

	m_oSocket.set_callback_interface(this);
	m_oSocket.set_console(m_poConsole);
}
//-------------------------------------------------------------------------
CClient::~CClient()
{
	//Delete interface
	m_poInterface = NULL;

	Stop();
}
//-------------------------------------------------------------------------
const bool CClient::Start(const unsigned short startPort, const unsigned short endPort)
{
	m_poConsole->printf("CCLient::Start\n");
	//Não permitir que essa função seja chamada mais de uma vez
	if (m_oSocket.is_open())
		return false;
	// abre o socket utilizando a faixa de portas especificada
	int oppenedPort = m_oSocket.open_socket(startPort, endPort);
	if (oppenedPort == 0)
		return false;
	return true;
}
//-------------------------------------------------------------------------
const bool CClient::Stop()
{
	if (!m_oSocket.is_open())
		return false;

	Disconnect();

	m_oSocket.close_socket();
	
	return true;
}
//-------------------------------------------------------------------------
const bool CClient::Connect(address_c& oAddress)
{
	if (!m_oSocket.is_open())
		return false;

	/// Internal state?
	m_poStation = m_oSocket.create_station(oAddress);
	m_poStation->set_callback_interface(this);
	m_poStation->connect();

	return true;
}
//-------------------------------------------------------------------------
const bool CClient::Disconnect()
{
	if (!m_oSocket.is_open())
		return false;
	/// Disconnection protocol
	if (IsConnected())
	{
		m_poStation->disconnect();
	}
	if (m_poStation != NULL)
	{
		address_c oAddress = m_poStation->get_remote_address();
		m_oSocket.delete_station(oAddress);
		m_poStation = NULL;
	}

	///TODO: Now the destructor of RemotePeers DOESN'T closes the station, we must
	///manually destroy all connection with remote peers here!
	SAFEDELETE(m_sGroup.m_poActual);
	///I am in Limbo
	m_iGroupID = GetLimboGroup().GetID();
	m_iClientID = INVALID_ID;

	return true;
}
//-------------------------------------------------------------------------
const bool CClient::ProcessNonBlocking()
{
	m_oSocket.process_nonblocking();
	GetGroup()->ProcessNonBlocking();

	if (IsGroupSocial() == false)
	{
		list<CRemotePeer*> oList;

		for (int i = 0; i < GetNumberOfRemotePeers(); i++)
		{
			TClientID iRemoteID = GetRemotePeerID(i);
			CRemotePeer* poPeer = GetGroup()->GetClientPointer(iRemoteID);
			oList.push_back(poPeer);
		}

		for (int i = 0; i < m_sGroup.m_oLimbo.GetNumberOfRemoteClients(); i++)
		{
			TClientID iRemoteID = m_sGroup.m_oLimbo.GetClientID(i);
			CRemotePeer* poPeer = m_sGroup.m_oLimbo.GetClientPointer(iRemoteID);
			oList.push_back(poPeer);
		}

		for (list<CRemotePeer*>::iterator it = oList.begin(); it != oList.end(); it++)
		{
			CRemotePeer* poPeer = *it;
			TClientID iRemoteID = poPeer->GetID();

			if (poPeer->GetInformServerStatus() == true)
			{
				buffer_c oMessage;
				oMessage << (NLubyte) P2PSEL0::PROTOCOL_MAN_GROUP_CONNECTED;
				oMessage << iRemoteID;

				address_c oAddr(poPeer->GetAddress());

				map<address_c, TClientID>::iterator it2 = m_oAddressIDMap.find(oAddr);
				if (it2 != m_oAddressIDMap.end())
				{
					m_oAddressIDMap.erase(it2);
				}

				m_poConsole->xprintf(2, "Client Connected: %d - %d\n", GetID(), iRemoteID);

				m_poStation->write(oMessage, P2PSEL0::CGlobalInformation::m_iGroupEventProtocolStream);

				poPeer->ResetInformServerStatus();
			}
		}
	}

	return true;
}
//-------------------------------------------------------------------------
const bool CClient::SendPacket()
{
	if (m_poStation == NULL)
		return false;
	return (m_poStation->send_packet(NULL) == SENDPACKET_OK) && GetGroup()->SendPacket();
}
//-------------------------------------------------------------------------
CGroupAbstract< CRemotePeer >* CClient::GetGroup()
{
	if (m_sGroup.m_oLimbo.GetID() == GetGroupID())
		return &m_sGroup.m_oLimbo;
	return m_sGroup.m_poActual;
}
//-------------------------------------------------------------------------
template < class T >
CGroupAbstract< CRemotePeer >* CClient::CreateGroup( const TGroupID id, station_c& oServer )
{
	T* poGroup = new T(id, oServer, *m_poInterface);
	return poGroup;
}
//-------------------------------------------------------------------------
CGroupLimboClient& CClient::GetLimboGroup()
{
	return m_sGroup.m_oLimbo;
}
//-------------------------------------------------------------------------
const int CClient::GetNumberOfRemotePeers()
{
	return GetGroup()->GetNumberOfRemoteClients();
}
//-------------------------------------------------------------------------
const TClientID CClient::GetRemotePeerID(const int iIndex)
{
	return GetGroup()->GetClientID(iIndex);
}
const bool CClient::GetRemotePeerAddress(const TClientID iRemotePeerID, address_c& oAddress)
{
	CRemotePeer* poPeer = GetGroup()->GetClientPointer(iRemotePeerID);
	if (poPeer == NULL)
		return false;

	oAddress.set_address(poPeer->GetAddress());
	return true;
}

//-------------------------------------------------------------------------
void CClient::SetPacketLossEmulation(double dLoss) 
{
	m_oSocket.set_packet_loss_emulation(dLoss);
}
//-------------------------------------------------------------------------
void CClient::SetPacketLatencyEmulation(double dBaseLatency, double dDeltaLatency) 
{
  m_oSocket.set_packet_delay_emulation(dBaseLatency, dDeltaLatency);
}
//-------------------------------------------------------------------------
void CClient::SetRemotePacketLossEmulation(const TClientID id, double dLoss)
{
  CRemotePeer* poRemote = GetGroup()->GetClientPointer(id);
  if (poRemote != NULL)
    poRemote->SetPacketLossEmulation(dLoss);
}
//-------------------------------------------------------------------------
void CClient::SetRemotePacketLatencyEmulation(const TClientID id, double dBaseLatency, double dDeltaLatency)
{
  CRemotePeer* poRemote = GetGroup()->GetClientPointer(id);
  if (poRemote != NULL)
    poRemote->SetPacketLatencyEmulation(dBaseLatency, dDeltaLatency);
}
//-------------------------------------------------------------------------
const double CClient::GetServerRoundTripTimeEstimative() 
{
  if (IsConnected())
  {
    return m_poStation->get_delay_estimative();
  }
  else return -1.0;
}
//-------------------------------------------------------------------------

const double CClient::GetRemotePeerRoundTripTimeEstimative(const TClientID id) 
{
  if (IsConnected() == false) return -1.0;

	if (GetID() == id) return 0.0;

	CRemotePeer* poRemotePeer = GetGroup()->GetClientPointer(id);

	if (poRemotePeer == NULL) return -1.0;
	else return poRemotePeer->GetRoundTripTimeEstimative();
}

//-------------------------------------------------------------------------
const bool CClient::SendUnicastMessage(TClientID id, buffer_c& oBuffer, const int iStream, const bool bReliable)
{
	if (id == GetID())
	{
		SendOwnMessage(oBuffer, iStream);
		m_poInterface->IncomingPeerData(id, buffer_c::getEmptyBuffer());
		return true;
	}
	CRemotePeer* poRemotePeer = GetGroup()->GetClientPointer(id);
	if (poRemotePeer == NULL) 
		return false;
	else 
		return poRemotePeer->SendMessage(oBuffer, iStream);
}
//-------------------------------------------------------------------------
const bool CClient::SendBroadcastMessage(buffer_c& oBuffer, const int iStream, const bool bReliable)
{
	const bool bBroad = GetGroup()->SendMessage(oBuffer, iStream);
	return SendOwnMessage(oBuffer, iStream) && bBroad;
}
//-------------------------------------------------------------------------
const bool CClient::Chat(const string& oMessage)
{
	return GetGroup()->Chat(oMessage);
}
// TODO: Different logics to receive a chat in a action or social group may be placed in the Group classes, not in client!
//-------------------------------------------------------------------------
const bool CClient::ReceiveChat(string& oChatMessage, TClientID& iClientID)
{
	buffer_c oBuffer;

	if (IsGroupAction() == true)
	{
		if (!ReceiveFromAnyPeer(oBuffer, iClientID, P2PSEL0::CGlobalInformation::m_iChatStream))
			return false;
	}
	else if (IsGroupSocial() == true)
	{
		if (!ReceiveFromServer(oBuffer, P2PSEL0::CGlobalInformation::m_iChatStream))
			return false;
		oBuffer >> iClientID;
	}
	else return false;

	oBuffer >> oChatMessage;
	return true;
}
//-------------------------------------------------------------------------
const bool CClient::IsConnected() const
{
	if (m_poStation == NULL)
		return false;
	return m_poStation->is_connected();
}
//-------------------------------------------------------------------------
const bool CClient::SendMessageToServer(buffer_c& oBuffer, const int iStream, const bool bReliable)
{
	if (!IsConnected())
		return false;
	return (m_poStation->write(oBuffer, iStream, bReliable) > 0);
}
//-------------------------------------------------------------------------
const bool CClient::SendOwnMessage(buffer_c& oBuffer, const int iStream)
{
	map< int, list < buffer_c > >::iterator it = m_oOwnMessagesMap.find(iStream);
	if (it == m_oOwnMessagesMap.end())
	{
		list< buffer_c > oList;
		m_oOwnMessagesMap[iStream] = oList;
		it = m_oOwnMessagesMap.find(iStream);
	}
	it->second.push_back(oBuffer);
	buffer_c oUnreliable;
	m_poInterface->IncomingPeerData(GetID(), oUnreliable);
	return true;
}

//-------------------------------------------------------------------------
const bool CClient::ReceiveFromServer(buffer_c& oBuffer, const int iStream)
{
	if (!IsConnected())
		return false;
	return (m_poStation->read(oBuffer, iStream) > 0);
}
//-------------------------------------------------------------------------
const bool CClient::ReceiveFromServerAnyStream(buffer_c& oBuffer, int& iStream)
{
	if (!IsConnected())
		return false;

	m_poStation->seek_first_stream();
	while ((iStream = m_poStation->get_next_stream()) >= 0)
	{
		if (m_poStation->read(oBuffer, iStream))
			return true;
	}
	return false;
}
const bool CClient::ReceiveOwn(buffer_c& oBuffer, const int iStream)
{
	map< int, list < buffer_c > >::iterator it = m_oOwnMessagesMap.find(iStream);
	if (it == m_oOwnMessagesMap.end())
		return false;
	if (it->second.empty() == false)
	{
		oBuffer = it->second.front();
		it->second.pop_front();
		return true;
	}
	else return false;
}

const bool CClient::ReceiveOwnAnyStream(buffer_c& oBuffer, int& iStream)
{
	for (map<int, list<buffer_c> >::iterator it = m_oOwnMessagesMap.begin(); it != m_oOwnMessagesMap.end(); it++)
	{
		iStream = it->first;
		if (ReceiveOwn(oBuffer, iStream) == true) 
			return true;
	}

	return false;
}

//-------------------------------------------------------------------------
const bool CClient::ReceiveFromPeer(buffer_c& oBuffer, const TClientID id, const int iStream)
{
	if (!IsConnected())
		return false;

	if (id == GetID())
	{
		return ReceiveOwn(oBuffer, iStream);
	}

	CRemotePeer* pPeer = GetGroup()->GetClientPointer(id);
	if (pPeer == NULL)
		return false;
	return pPeer->ReceiveSpecificStream(oBuffer, iStream);
}
//-------------------------------------------------------------------------
const bool CClient::ReceiveFromPeerAnyStream(buffer_c& oBuffer, const TClientID id, int& iStream)
{
	if (!IsConnected())
		return false;

	if (id == GetID())
	{
		return ReceiveOwnAnyStream(oBuffer, iStream);
	}

	CRemotePeer* pPeer = GetGroup()->GetClientPointer(id);
	if (pPeer == NULL)
		return false;
	return pPeer->ReceiveAnyStream(oBuffer, iStream);
}
//-------------------------------------------------------------------------
const bool CClient::ReceiveFromAnyPeer(buffer_c& oBuffer, TClientID& id, const int iStream)
{
	if (!IsConnected())
		return false;

	CGroupAbstract< CRemotePeer >* pGroup = GetGroup();
	for (int i = 0; i < pGroup->GetNumberOfRemoteClients(); i++)
	{
		id = pGroup->GetClientID(i);
		CRemotePeer* pPeer = GetGroup()->GetClientPointer(id);
		if (pPeer->ReceiveSpecificStream(oBuffer, iStream))
			return true;
	}

	return ReceiveOwn(oBuffer, iStream);
}
//-------------------------------------------------------------------------
const bool CClient::ReceiveFromAnyPeerAnyStream(buffer_c& oBuffer, TClientID& id, int& iStream)
{
	if (!IsConnected())
		return false;

	CGroupAbstract< CRemotePeer >* pGroup = GetGroup();
	for (int i = 0; i < pGroup->GetNumberOfRemoteClients(); i++)
	{
		id = pGroup->GetClientID(i);
		CRemotePeer* pPeer = GetGroup()->GetClientPointer(id);
		if (pPeer->ReceiveAnyStream(oBuffer, iStream))
			return true;
	}

	return ReceiveOwnAnyStream(oBuffer, iStream);
}
//-------------------------------------------------------------------------
const bool CClient::IsValidStream(int iStream)
{
	if (!IsConnected())
		return false;

	//Not implemented!
	return true;
}
//-------------------------------------------------------------------------
const bool CClient::GetServerAddress(address_c& oAddress)
{
	if (!IsConnected())
		return false;

	oAddress = m_poStation->get_remote_address();
	return true;
}
//-------------------------------------------------------------------------
void CClient::SetDebuggingConsole(console_c& oConsole)
{
	m_oSocket.set_console(&oConsole);
	m_poConsole = &oConsole;
}
//-------------------------------------------------------------------------
const bool CClient::IsGroupSocial()
{
	return GetGroupType(GetGroupID()) == GROUP_SOCIAL_TYPE;
}
//-------------------------------------------------------------------------
const bool CClient::IsGroupAction()
{
	return GetGroupType(GetGroupID()) == GROUP_ACTION_TYPE;
}
//-------------------------------------------------------------------------
const bool CClient::HasSomethingToSend()
{
  if (IsConnected() && (!m_poStation->nothing_to_send()))
    return true;
  return GetGroup()->HasSomethingToSend();
}
//-------------------------------------------------------------------------
const double CClient::GetServerLossEstimative() const
{
	if (IsConnected())
	{
		return m_poStation->get_loss_estimative();
	}
	else return -1.0;
}

const double CClient::GetRemotePeerLossEstimative(const TClientID iClientId) /*const*/
{
	if (IsConnected() == false) return -1.0;

	if (GetID() == iClientId) return 0.0;

	CRemotePeer* poRemotePeer = GetGroup()->GetClientPointer(iClientId);

	if (poRemotePeer == NULL) return -1.0;
	else return poRemotePeer->GetLossEstimative();
}

/*! \brief Utility service to get an estimative of how packets arrive "out of order" from the server
\return an estimative of how packets arrive out of order from the server (0.0 means "all packets arrive properly (e. g., 1, 2, 3, etc)",
1.0 means "all packets arrive in backwards (e.g. 10, 9, 8, etc)". This measure is proportional to the standard deviation of the packet order
*/
const double CClient::GetServerOutOfOrderEstimative() const
{
	if (IsConnected())
	{
		return m_poStation->get_delay_estimative();
	}
	else return -1.0;

}

/*! \brief Utility service to get an estimative of how packets arrive "out of order" from the server
\param id the unique identifier of the peer we are interested to estimate the out-of-order factor
\return an estimative of how packets arrive out of order from the server (0.0 means "all packets arrive properly (e. g., 1, 2, 3, etc)",
1.0 means "all packets arrive in backwards (e.g. 10, 9, 8, etc)". This measure is proportional to the standard deviation of the packet order
*/
const double CClient::GetRemotePeerOutOfOrderEstimative(const TClientID iClientId) /*const*/
{
	if (IsConnected() == false) return -1.0;

	if (GetID() == iClientId) return 0.0;

	CRemotePeer* poRemotePeer = GetGroup()->GetClientPointer(iClientId);

	if (poRemotePeer == NULL) return -1.0;
	else return poRemotePeer->GetOutOfOrderEstimative();
}
//-------------------------------------------------------------------------
void CClient::msocket_incoming_unreg(address_c &addr, const buffer_c &pkt) 
{
	///Nothing to do
	return;
}
//-------------------------------------------------------------------------
void CClient::msocket_read_error(NLsocket &sock)
{
	///Critical failure!
	buffer_c oMessage;
	m_poInterface->ConnectionFailed(oMessage);
}
//-------------------------------------------------------------------------
bool CClient::msocket_accept_station(address_c &addr, const buffer_c& connection_data, buffer_c& answer)
{
//FIXME: Peers soh se conectam sob ordem do servidor, entao NUNCA vai chegar pedido de conexao
//de um endereco sem station associada (se chegar pode dropar, se for pra ti conectar com ele
//certamente o servidor vai te dar um GroupSync e dai tu cria a station)
	return false;
/*
	/// Check list of valid address for group peers.
	map< address_c, TClientID >::iterator it = m_oAddressIDMap.find(addr);
	if (it == m_oAddressIDMap.end())
		return false;

	return true;
*/
}
//-------------------------------------------------------------------------
void CClient::msocket_station_connected(station_c &st, const buffer_c& oConnectionData)
{
// FIXME Never happens
/*
	address_c oAddress = st.get_remote_address();
	map< address_c, TClientID >::iterator it = m_oAddressIDMap.find(oAddress);
	if (it == m_oAddressIDMap.end())
		return;

	if (!forced_accept)
	{
		/// Put in limbo
		// TODO: set_debug, set_compression(), set_connect_data(), etc
		m_sGroup.m_oLimbo.CreateClient(st, it->second);

		/// ADD VOID* IN GROUPABSTRACT TO REFERENCE INTERFACE AND ADD CONSTRUCTOR PARAMETER FOR REMOTEABSTRACT
		m_sGroup.m_oLimbo.GetClientPointer(it->second)->SetCallbackInterface(*m_poInterface);

		char vDebugString[64];
		sprintf(vDebugString, "RemotePeer %d", st.get_remote_port());
		st.set_debug(vDebugString);

		///Peer-to-Peer Stream
		CGlobalInformation::m_iChatStream = st.create_stream(STREAM_INDIVIDUAL_ACK);
	} 
	else 
	{
		m_sGroup.m_oLimbo.ForceClientConnected(it->second);
	}

	buffer_c oMessage;
	oMessage << (NLubyte)P2PSEL0::PROTOCOL_MAN_GROUP_CONNECTED;
	oMessage << it->second;
	
	m_oAddressIDMap.erase(it);
	m_poStation->write(oMessage, P2PSEL0::CGlobalInformation::m_iGroupEventProtocolStream);
*/
}
//-------------------------------------------------------------------------
void CClient::msocket_closed()
{
}
//-------------------------------------------------------------------------
void CClient::station_incoming_data(buffer_c& pkt, int packet_id)
{
	/// TO DO Message demultiplex message (Specific stream - Unicast for client-server-client?)
	buffer_c oBuffer;

	//Management messages!
	while (m_poStation->read(oBuffer, CGlobalInformation::m_iGroupEventProtocolStream) > 0)
	{
		unsigned char iType;
		oBuffer >> iType;
		switch (iType)
		{
		case PROTOCOL_MAN_GROUP_SYNC:
			ProcessGroupSync(oBuffer);
			break;
		case PROTOCOL_MAN_GROUP_CONNECTED:
			/// IMPOSSIBLE! ONLY SERVER RECEIVE THIS!
			std::cerr << "CClient::station_incoming_data - PROTOCOL_MAN_GROUP_CONNECTED shouldn't be receive\n";
			break;
		case PROTOCOL_EVENT_PEER_JOINED:
			ProcessPeerJoin(oBuffer);
			break;
		case PROTOCOL_EVENT_PEER_LEFT:
			ProcessPeerLeft(oBuffer);
			break;
		}
	}

	m_poInterface->IncomingServerData(pkt);
}

void CClient::ProcessGroupSync(buffer_c& oBuffer)
{
	std::set < TClientID > oIncomingClientList;

	m_poConsole->xprintf(2, "%d - received GROUP_SYNC from server (", GetID());

	/// Sync group here. Compare with my current group.
	unsigned short iCount;
	oBuffer >> iCount;
	m_poConsole->xprintf(2, "%d peers)\n", iCount);

	for (int i = 0; i < iCount; i++)
	{
		TClientID iClientID;
		oBuffer >> iClientID;
		string oStringAddress;
		oBuffer >> oStringAddress;
		address_c oAddress(oStringAddress);
		/// If it's myself then continue!
		if (iClientID == GetID())
			continue;

		m_poConsole->xprintf(2, "Adding ID=%d, address=%s\n", iClientID, oStringAddress.c_str());

		/// Find if it's in my group
		CGroupAbstract< CRemotePeer >* poMyGroup = GetGroup();

		/// All in sync
		oIncomingClientList.insert(iClientID);

		/// Check if it's in my group
		if (poMyGroup->IsReferenced(iClientID))
			continue;
		/// Check if it's in limbo
		if (m_sGroup.m_oLimbo.IsReferenced(iClientID))
			continue;

		/// Check if client exist in my list.
		map< address_c, TClientID >::iterator it = m_oAddressIDMap.find(oAddress);
		if (it == m_oAddressIDMap.end())
		{
			m_oAddressIDMap[oAddress] = iClientID;
			/// Create station to connect with him (symetric mode)
			station_c* poStation = m_oSocket.create_station(oAddress);

			// TODO: set_debug, set_connect_data, set_compression, set_max_outgoing_packet_size, etc....
			char vDebugString[64];
			sprintf(vDebugString, "RemotePeer %d", poStation->get_remote_port());
			poStation->set_debug(vDebugString);

			m_sGroup.m_oLimbo.CreateClient(*poStation, iClientID);
			/// ADD VOID* IN GROUPABSTRACT TO REFERENCE INTERFACE AND ADD CONSTRUCTOR PARAMETER FOR REMOTEABSTRACT
			m_sGroup.m_oLimbo.GetClientPointer(iClientID)->SetCallbackInterface(*m_poInterface);

			CGlobalInformation::m_iChatStream = poStation->create_stream(STREAM_INDIVIDUAL_ACK);

			poStation->connect();
		}
	}
	///Remove all clients from group & pending to enter in group
	CGroupAbstract< CRemotePeer >* poMyGroup = GetGroup();
	std::set < TClientID > oCurrentClientList;
	poMyGroup->GetClientSet(oCurrentClientList);

	std::vector < TClientID > oClientDiff(oCurrentClientList.size());
	std::vector < TClientID >::iterator itLast = std::set_difference(oCurrentClientList.begin(), oCurrentClientList.end(), oIncomingClientList.begin(), oIncomingClientList.end(), oClientDiff.begin());
	for (std::vector < TClientID >::iterator it = oClientDiff.begin(); it != itLast; it++)
	{
		poMyGroup->RemoveClient(*it);
	}

	/// Remove clients from limbo too
	oClientDiff.clear();
	oCurrentClientList.clear();
	m_sGroup.m_oLimbo.GetClientSet(oCurrentClientList);
	oClientDiff.resize(oCurrentClientList.size());

	itLast = std::set_difference(oCurrentClientList.begin(), oCurrentClientList.end(), oIncomingClientList.begin(), oIncomingClientList.end(), oClientDiff.begin());
	for (std::vector < TClientID >::iterator it = oClientDiff.begin(); it != itLast; it++)
	{
		poMyGroup->RemoveClient(*it);
	}

	///Remove all clients from incoming address list
	std::set< TClientID > oAddressMapSet;
	for (std::map< address_c, TClientID >::iterator it = m_oAddressIDMap.begin(); it != m_oAddressIDMap.end(); it++) 
	{
		oAddressMapSet.insert(it->second);
	}
	std::vector < TClientID > oAddressMapDiff(oAddressMapSet.size());
	itLast = std::set_difference(oAddressMapSet.begin(), oAddressMapSet.end(), oIncomingClientList.begin(), oIncomingClientList.end(), oAddressMapDiff.begin());
	for (std::vector < TClientID >::iterator it = oAddressMapDiff.begin(); it != itLast; it++)
	{
		for (std::map< address_c, TClientID >::iterator itMap = m_oAddressIDMap.begin(); itMap != m_oAddressIDMap.end(); itMap++) 
		{
			if (*it == itMap->second)
			{
				m_poConsole->xprintf(2, "Removing ID=%d, address=%s\n", GetID(), itMap->first.get_address().c_str());
				m_oAddressIDMap.erase(itMap);
				break;
			}
		}
	}
}

void CClient::ProcessPeerJoin(buffer_c& oBuffer)
{
	TGroupID iGroup;
	oBuffer >> iGroup;

	TClientID id;
	oBuffer >> id;

	/// Check if I need to destroy my own group
	m_poConsole->xprintf(2, "%d - received PEER JOINED from server (group %d, peer %d)\n", GetID(), iGroup, id);

	if (id == GetID())
	{
		if (iGroup != GetGroupID()) {
			if (m_sGroup.m_poActual != NULL) 
			{
				SAFEDELETE(m_sGroup.m_poActual);
			}
		}
		/// My new group
		m_iGroupID = iGroup;

		/// PRIMEIRO PEER JOINED TEM QUE SER EU!!!
		/// I need to move to another group (Slave method)
		if (GetGroupType(iGroup) == GROUP_SOCIAL_TYPE)
		{
			m_sGroup.m_poActual = CreateGroup< CGroupSocialClient >(iGroup, *m_poStation);
		}
		if (GetGroupType(iGroup) == GROUP_ACTION_TYPE)
		{
			m_sGroup.m_poActual = CreateGroup< CGroupActionClient >(iGroup, *m_poStation);
		}
		m_poInterface->GroupJoined(iGroup);
	}
	else
	{
		if (GetGroupType(iGroup) == GROUP_SOCIAL_TYPE)
		{
			//Just add client to group
			GetGroup()->CreateClient(station_c::get_invalid_station(), id);
		}
		if (GetGroupType(iGroup) == GROUP_ACTION_TYPE)
		{
			//Move from limbo to group
			m_sGroup.m_oLimbo.ChangeToGroup(id, *GetGroup());
		}
		m_poInterface->PeerJoined(id);
	}
}

void CClient::ProcessPeerLeft(buffer_c& oBuffer)
{
	TGroupID iGroup;
	oBuffer >> iGroup;

	TClientID id;
	oBuffer >> id;

	m_poConsole->xprintf(2, "%d - received PEER LEFT from server (group %d, peer %d)\n", GetID(), iGroup, id);

	if (id == GetID())
	{
		/// Disconnect everyone!
		m_sGroup.m_poActual->Clear();
		const TGroupID gid = m_sGroup.m_poActual->GetID();
		SAFEDELETE(m_sGroup.m_poActual);
		m_iGroupID = m_sGroup.m_oLimbo.GetID();
		// Add group left
		m_poInterface->GroupLeft(gid);
	}
	else
	{
		/// Someone left the group
		GetGroup()->RemoveClient(id);

		m_poInterface->PeerLeft(id);
	}
}

//-------------------------------------------------------------------------
const bool CClient::IsClientPresent(const TClientID id)
{
  for (int i = 0; i < GetNumberOfRemotePeers(); i++)
  {
    if (id == GetRemotePeerID(i))
      return true;
  }
  return false;
}
//-------------------------------------------------------------------------
const int CClient::CreateStream(const TClientID id, int iType, policy_c oPolicy)
{
	if (id == GetID())
	{
			m_iStreamIDGenerator++;
			return m_iStreamIDGenerator;
	}

  CRemotePeer* pPeer = GetGroup()->GetClientPointer(id);
  return pPeer->CreateStream(iType, oPolicy);
}
//-------------------------------------------------------------------------
const int CClient::CreateStreamToServer(int iType, policy_c oPolicy)
{
  return m_poStation->create_stream(iType, oPolicy);
}
//-------------------------------------------------------------------------
void CClient::station_connected(buffer_c& connection_pkt, const bool forced)
{
	///Receive id
	connection_pkt >> m_iClientID;

	///Reserved streams...
	CGlobalInformation::m_iChatStream = m_poStation->create_stream(STREAM_INDIVIDUAL_ACK);
	CGlobalInformation::m_iGroupEventProtocolStream = m_poStation->create_stream(STREAM_INDIVIDUAL_ACK); //Default policy

	// Create stream with myself for chat
	CGlobalInformation::m_iChatStream = CreateStream(GetID(), STREAM_INDIVIDUAL_ACK, DefaultPolicy);

	m_poInterface->Connected(m_iClientID, connection_pkt);
}
//-------------------------------------------------------------------------
void CClient::station_connection_timed_out()
{
	buffer_c oEmpty;
	m_poInterface->ConnectionFailed(oEmpty);
}
//-------------------------------------------------------------------------
void CClient::station_connection_refused(buffer_c& reject_pkt)
{
	m_poInterface->ConnectionFailed(reject_pkt);
}
//-------------------------------------------------------------------------
void CClient::station_disconnected(bool remote, buffer_c& reason)
{
	if (remote) 
    Disconnect();

	m_poInterface->Disconnected(reason);
}
//-------------------------------------------------------------------------
