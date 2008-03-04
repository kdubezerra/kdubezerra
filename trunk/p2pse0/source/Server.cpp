#include "Server.h"
#include "GroupAbstract.h"
#include "GroupActionServer.h"
#include "GroupSocialServer.h"
#include "GroupLimboServer.h"
#include "RemoteClient.h"

using namespace P2PSEL0;

//----------------------------------------------------------------
//Facade
//----------------------------------------------------------------
CServerInterface* CServerInterface::GetNewServer(CServerEventInterface& oServerListener)
{
	return new CServer(oServerListener);
}
//----------------------------------------------------------------
CServer::CServer(CServerEventInterface& oEventInterface)
: m_oGroupIDSocialGenerator(1, 1023), m_oGroupIDActionGenerator(1024, 16384), m_oClientIDGenerator(50000, 60000), m_poInterface(&oEventInterface)
{
	m_oSocket.set_callback_interface(this);
	m_poConsole = &console_c::get_default_console();
	m_oSocket.set_console(m_poConsole);

	m_iLimboGroup = CreateGroup< CGroupLimboServer > (m_oClientIDGenerator.GetSize());
	m_iSocialGroup = CreateGroup< CGroupSocialServer > (m_oClientIDGenerator.GetSize());

	/// EXTREMALLY THIGH PATCH------------------------------------------------------------------------------------
	((CGroupLimboServer*)GetGroup(m_iLimboGroup))->SetGroupDefault(*(CGroupSocialServer*)GetGroup(m_iSocialGroup));
	/// ----------------------------------------------------------------------------------------------------------
}

CServer::~CServer()
{
	/// Destroy all groups!
	while (GetNumberOfGroups() > 0)
		DestroyGroup(GetGroupIDByIndex(0));

	m_oSocket.close_socket();
}

//-------------------------------------------------------------------------------------------------------
//SERVICES
//-------------------------------------------------------------------------------------------------------
const bool CServer::Start(const int iMinPort, const int iMaxPort)
{
	//Não permitir que essa função seja chamada mais de uma vez
	if (m_oSocket.is_open())
		return false;
	// abre o socket utilizando a faixa de portas especificada
	int oppenedPort = m_oSocket.open_socket(iMinPort, iMaxPort);
	if (oppenedPort == 0)
		return false;
	return true;
}

const bool CServer::Stop()
{
	if (!m_oSocket.is_open())
		return false;

	/// Destroy all groups!
	while (GetNumberOfGroups() > 2) //First and second group MUST be ignored
		DestroyGroup(GetGroupIDByIndex(2));

	m_oSocket.close_socket();

	return true;
}

const TGroupID CServer::CreateSocialGroup(const int nMaxPeer)
{
	return CreateGroup< CGroupSocialServer >(nMaxPeer);
}

const TGroupID CServer::CreateActionGroup(const int nMaxPeer)
{
	return CreateGroup< CGroupActionServer >(nMaxPeer);
}

const bool CServer::DestroyGroup(const TGroupID iGroupID)
{
	/// Only non-reserved groups can be destroyed! 
	if (m_iLimboGroup == iGroupID || m_iSocialGroup == iGroupID)
		return false;

	std::map< TGroupID, CGroupAbstract< CRemoteClient >* >::iterator itGroup = m_oGroupMap.find(iGroupID);
	if (itGroup == m_oGroupMap.end())
		return false;

	CGroupAbstract< CRemoteClient >* poGroupDying = itGroup->second;
	CGroupAbstract< CRemoteClient >* poGroupDest = GetGroup(m_iSocialGroup);
	CGroupLimboServer* poLimbo = (CGroupLimboServer*)GetGroup(m_iLimboGroup);
	while (poGroupDying->GetNumberOfRemoteClients() > 0)
		poLimbo->PullToLimbo(poGroupDying->GetClientID(0), *poGroupDying, *poGroupDest);

	m_oGroupMap.erase(itGroup);
	return true;
}

const bool CServer::MoveToGroup(const TClientID iClientID, const TGroupID iGroupDestID)
{
	/// Check if destination group is valid
	std::map< TGroupID, CGroupAbstract< CRemoteClient >* >::iterator itGroup = m_oGroupMap.find(iGroupDestID);
	if (itGroup == m_oGroupMap.end())
		return false;

	/// Check if client id is valid
	CGroupAbstract< CRemoteClient >* poGroupSource = GetClientGroup(iClientID);
	if (poGroupSource == NULL)
		return false;

	/// It's already in group
	if (poGroupSource->GetID() == iGroupDestID)
		return true;
	
	CGroupAbstract< CRemoteClient >* poGroupDest = itGroup->second;

	if (poGroupDest->IsBanned(iClientID)) {
	printf("Client %d can't enter in group %d, because he is banned\n", iClientID, iGroupDestID);
	return false;
	}

	if (GetGroupType(poGroupSource->GetID()) == GROUP_ACTION_TYPE)
		poGroupSource->BanClient(iClientID);

	CGroupLimboServer* poLimbo = (CGroupLimboServer*)GetGroup(m_iLimboGroup);

	return poLimbo->PullToLimbo(iClientID, *poGroupSource, *poGroupDest);
}

const int CServer::GetNumberOfGroups()
{
	return (int)m_oGroupMap.size();
}

const TGroupID CServer::GetGroupIDByIndex(const unsigned int iIndex)
{
	std::map< TGroupID, CGroupAbstract< CRemoteClient >* >::iterator it = m_oGroupMap.begin();
	for (unsigned int i = 0; i < iIndex; i++, it++)
	{
		if (it == m_oGroupMap.end())
			return INVALID_ID;
	}
	return it->first;
}

const bool CServer::DisconnectClient(const TClientID id)
{
	/// Check if client id is valid
	CGroupAbstract< CRemoteClient >* poGroupSource = GetClientGroup(id);
	if (poGroupSource == NULL)
		return false;

	return poGroupSource->RemoveClient(id);
}

const unsigned int CServer::GetNumberOfClients(const TGroupID gid)
{
	CGroupAbstract< CRemoteClient >* poGroup = GetGroup(gid);
	if (poGroup == NULL)
		return 0;
	return poGroup->GetNumberOfRemoteClients();
}

/// Get max number of clients in the specified group
const unsigned int CServer::GetMaxNumberOfClients(const TGroupID gid)
{
	CGroupAbstract< CRemoteClient >* poGroup = GetGroup(gid);
	if (poGroup == NULL)
		return 0;
	return poGroup->GetMaxNumberOfClients();
}

const TClientID CServer::GetClientByIndex(const TGroupID gid, const unsigned int iIndex)
{
	CGroupAbstract< CRemoteClient >* poGroup = GetGroup(gid);
	if (poGroup == NULL)
		return INVALID_ID;
	return poGroup->GetClientID(iIndex);
}

//verifica se existe cliente com o id do parâmetro
const bool CServer::HasClient(const TClientID clientId)
{
	const int iGroupCount = GetNumberOfGroups();
	for (int i = 0; i < iGroupCount; i++)
	{
		CGroupAbstract< CRemoteClient >* poGroup = GetGroup(GetGroupIDByIndex(i));
		if (poGroup->IsReferenced(clientId) == true)
			return true;
	}

	return false;
}

//verifica se existe grupo com o id do parâmetro
const bool CServer::HasGroup(const TGroupID& groupId)
{
	const int iGroupCount = GetNumberOfGroups();
	for (int i = 0; i < iGroupCount; i++)
	{
		if (groupId == GetGroupIDByIndex(i))
			return true;
	}

	return false;
}
const bool CServer::SendBroadcast(const buffer_c& oBuffer, const TGroupID gid, const int iStream, const bool bReliable)
{
	CGroupAbstract< CRemoteClient >* poGroup = GetGroup(gid);
	if (poGroup == NULL)
		return false;
	// TODO: remove const_cast
	return poGroup->SendMessage(const_cast<buffer_c&>(oBuffer), iStream, bReliable);
}

const bool CServer::SendUnicast(const buffer_c& oBuffer, const TClientID id, const int iStream, const bool bReliable)
{
	CGroupAbstract< CRemoteClient >* poGroup = GetClientGroup(id);
	if (poGroup == NULL)
		return false;
	CRemoteClient* poClient = poGroup->GetClientPointer(id);
	// TODO: remove const_cast
	return poClient->SendMessage(const_cast<buffer_c&>(oBuffer), iStream, bReliable);
}

const bool CServer::ReceiveMessageAnyClientAnyStream(buffer_c& oBuffer, TClientID& id, int& iStream)
{
	const int iGroupCount = GetNumberOfGroups();
	for (int i = 0; i < iGroupCount; i++)
	{
		CGroupAbstract< CRemoteClient >* poGroup = GetGroup(GetGroupIDByIndex(i));
		for (int j = 0; j < poGroup->GetNumberOfRemoteClients(); j++)
		{
			id = poGroup->GetClientID(j);
			CRemoteClient* poClient = poGroup->GetClientPointer(id);
			if (poClient != NULL)
			{
				if (poClient->ReceiveAnyStream(oBuffer, iStream))
					return true;
			}
		}
	}
	return false;
}

const bool CServer::ReceiveMessageAnyClient(buffer_c& oBuffer, TClientID& id, const int iStream)
{
	const int iGroupCount = GetNumberOfGroups();
	for (int i = 0; i < iGroupCount; i++)
	{
		CGroupAbstract< CRemoteClient >* poGroup = GetGroup(GetGroupIDByIndex(i));
		for (int j = 0; j < poGroup->GetNumberOfRemoteClients(); j++)
		{
			id = poGroup->GetClientID(j);
			CRemoteClient* poClient = poGroup->GetClientPointer(id);
			if (poClient != NULL)
				if (poClient->ReceiveSpecificStream(oBuffer, iStream))
					return true;
		}
	}
	return false;
}

const bool CServer::ReceiveMessageFromClientAnyStream(buffer_c& oBuffer, const TClientID id, int& iStream)
{
	CGroupAbstract< CRemoteClient >* poGroup = GetClientGroup(id);
	CRemoteClient* poClient = poGroup->GetClientPointer(id);
	return (poClient->ReceiveAnyStream(oBuffer, iStream));
}

const bool CServer::ReceiveMessageFromClient(buffer_c& oBuffer, const TClientID id, const int iStream)
{
	CGroupAbstract< CRemoteClient >* poGroup = GetClientGroup(id);
	CRemoteClient* poClient = poGroup->GetClientPointer(id);
	return (poClient->ReceiveSpecificStream(oBuffer, iStream));
}

const bool CServer::ReceiveMessageFromGroupAnyStream(buffer_c& oBuffer, const TGroupID gid, TClientID& id, int& iStream)
{
	CGroupAbstract< CRemoteClient >* poGroup = GetGroup(gid);
	for (int i = 0; i < poGroup->GetNumberOfRemoteClients(); i++)
	{
		id = poGroup->GetClientID(i);
		CRemoteClient* poClient = poGroup->GetClientPointer(id);
		if (poClient->ReceiveAnyStream(oBuffer, iStream))
			return true;
	}
	return false;
}

const bool CServer::ReceiveMessageFromGroup(buffer_c& oBuffer, const TGroupID gid, TClientID& id, const int iStream)
{
	CGroupAbstract< CRemoteClient >* poGroup = GetGroup(gid);
	for (int i = 0; i < poGroup->GetNumberOfRemoteClients(); i++)
	{
		id = poGroup->GetClientID(i);
		CRemoteClient* poClient = poGroup->GetClientPointer(id);
		if (poClient->ReceiveSpecificStream(oBuffer, iStream))
			return true;
	}
	return false;
}

const bool CServer::HasSomethingToSend()
{
	const int iGroupCount = GetNumberOfGroups();
	for (int i = 0; i < iGroupCount; i++)
	{
		CGroupAbstract< CRemoteClient >* poGroup = GetGroup(GetGroupIDByIndex(i));
		if (poGroup->HasSomethingToSend() == true) return true;
	}
	return false;
}

const bool CServer::Dispatch()
{
	const int iGroupCount = GetNumberOfGroups();
	for (int i = 0; i < iGroupCount; i++)
	{
		CGroupAbstract< CRemoteClient >* poGroup = GetGroup(GetGroupIDByIndex(i));
		poGroup->SendPacket();
	}
	return true;
}

void CServer::ProcessNonBlocking()
{
	m_oSocket.process_nonblocking();
	for (std::map< TGroupID, CGroupAbstract< CRemoteClient >* >::iterator it = m_oGroupMap.begin(); it != m_oGroupMap.end(); it++)
		it->second->ProcessNonBlocking();
}

const TGroupID CServer::GetClientGroupID(const TClientID id)
{
	CGroupAbstract< CRemoteClient >* poGroup = GetClientGroup(id);
	if (poGroup == NULL)
		return INVALID_ID;
	return poGroup->GetID();
}

const int CServer::CreateStream(const TClientID id, const int iType, const policy_c& oPolicy)
{
	CGroupAbstract< CRemoteClient >* poGroup = GetClientGroup(id);
	if (poGroup == NULL)
		return -1;
	return poGroup->GetClientPointer(id)->CreateStream(iType, oPolicy);
}

void CServer::SetDebuggingConsole(console_c& oConsole)
{
	m_oSocket.set_console(&oConsole);
	m_poConsole = &oConsole;
}

const TGroupID CServer::GetDefaultSocialGroupID()
{
	return m_iSocialGroup;
}


///////////////////////////
///										 ///
///	 PRIVATE METHODS	 ///
///										 ///
///////////////////////////

template < class T >
	const TGroupID CServer::CreateGroup( const int nMaxPeer )
{
	TGroupID id(0);
	switch (T::GetTypeEnum())
	{
	case GROUP_LIMBO_TYPE:
		/// Do nothing...
		break;
	case GROUP_SOCIAL_TYPE:
		{
			id = m_oGroupIDSocialGenerator.GetID();
			if (!m_oGroupIDSocialGenerator.IsValid(id))
				return id;
		}
		break;
	case GROUP_ACTION_TYPE:
		{
			id = m_oGroupIDActionGenerator.GetID();
			if (!m_oGroupIDActionGenerator.IsValid(id))
				return id;
		}
		break;
	}
	T* poGroup =	new T(id, nMaxPeer, *m_poInterface);
	m_oGroupMap[id] = poGroup;
	if (GetGroupType(id) != GROUP_LIMBO_TYPE)
		poGroup->SetGroupLimbo(*GetGroup(m_iLimboGroup));
	return id;
}

CGroupAbstract< CRemoteClient >* CServer::GetGroup( const TGroupID id )
{
	std::map< TGroupID, CGroupAbstract< CRemoteClient >* >::const_iterator it = m_oGroupMap.find(id);
	if (it == m_oGroupMap.end())
		return NULL;
	return (it->second);
}

CGroupAbstract< CRemoteClient >* CServer::GetClientGroup(const TClientID id)
{
	for (std::map< TGroupID, CGroupAbstract< CRemoteClient >* >::iterator it = m_oGroupMap.begin(); it != m_oGroupMap.end(); it++)
	{
		CRemoteClient* poClient = it->second->GetClientPointer(id);
		if (poClient != NULL)
			return it->second;
	}
	return NULL;
}

bool CServer::msocket_accept_station( address_c &addr, const buffer_c& connection_data, buffer_c& answer )
{
	const TClientID id = m_oClientIDGenerator.GetID();
	if (!m_oClientIDGenerator.IsValid(id))
		return false;

	//Add callback with connection addr, id, answer...
	//if callback return true, client accepted, otherwise client rejected!

	const bool bAppCallbackReturn = m_poInterface->ClientAccept(connection_data, addr);
	if (!bAppCallbackReturn)
		m_oClientIDGenerator.FreeID(id);
	else
		answer << id;

	return bAppCallbackReturn;
}

void CServer::msocket_station_connected( station_c &st, const buffer_c& oConnectionData )
{
	CGlobalInformation::m_iChatStream = st.create_stream(STREAM_INDIVIDUAL_ACK);
	CGlobalInformation::m_iGroupEventProtocolStream = st.create_stream(STREAM_INDIVIDUAL_ACK);

	TClientID id = m_oClientIDGenerator.GetLastCreatedID();
	CGroupAbstract< CRemoteClient >* poGroup = GetGroup(m_iSocialGroup);

	char vDebugString[64];
	sprintf(vDebugString, "RemoteClient %d", id);
	st.set_debug(vDebugString);

	/// update client group
	if (poGroup->CreateClient(st, id) == true)
	{ 
		m_poInterface->ClientConnected(id, oConnectionData);
		m_poInterface->PeerJoined(poGroup->GetID(), id);
	}
	else
	{
		m_poConsole->xprintf(1, "Ops, CServer can't create CRemoteClient instance\n");
	}
}

