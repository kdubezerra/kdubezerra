#include "GroupSocialServer.h"
#include "GroupLimboServer.h"
#include <stdio.h>
const bool P2PSEL0::CGroupSocialServer::Chat( const string& oMessage )
{
	buffer_c oBuffer;
	oBuffer << oMessage;

	return SendMessage(oBuffer, CGlobalInformation::m_iChatStream, true);
}

const bool P2PSEL0::CGroupSocialServer::SendMessage( buffer_c& oData, const int iStream /*= -1*/, const bool bReliable /*= true*/ )
{
	for (std::map< TClientID, CRemoteClient* >::iterator it = m_oRemoteMap.begin(); it != m_oRemoteMap.end(); it++)
	{
		it->second->SendMessage(oData, iStream, bReliable);
	}
	return true;
}

const bool P2PSEL0::CGroupSocialServer::CreateClient(station_c& oStation, TClientID id)
{
	CGroupAbstract<CRemoteClient>::CreateClient(oStation, id);
	/// Set event interface for remote client (ONLY NEED TO SET HERE)
	GetClientPointer(id)->SetCallbackInterface(*m_poInterface);

	/// Send its peer joined
	buffer_c oMessage;
	oMessage << (unsigned char)PROTOCOL_EVENT_PEER_JOINED;
	oMessage << GetID();
	oMessage << id;

	SendMessage(oMessage, CGlobalInformation::m_iGroupEventProtocolStream, true);

	std::map< TClientID, CRemoteClient* >::iterator it = m_oRemoteMap.find(id);

	/// Send peer joined from other peers
	for (int i = 0; i < GetNumberOfRemoteClients(); i++)
	{
		oMessage.clear();
		const TClientID clientID = GetClientID(i);
		if (clientID != id)
		{
			buffer_c oMessage;
			oMessage << (unsigned char)PROTOCOL_EVENT_PEER_JOINED;
			oMessage << GetID();
			oMessage << GetClientID(i);
			it->second->SendMessage(oMessage, CGlobalInformation::m_iGroupEventProtocolStream, true);
		}
	}

	return true;
}

const bool P2PSEL0::CGroupSocialServer::RemoveClient(const TClientID id)
{
	bool ret = CGroupAbstract< CRemoteClient >::RemoveClient(id);
	if (!ret)
		return false;
	/// Send its peer left
	buffer_c oMessage;
	oMessage << (unsigned char)PROTOCOL_EVENT_PEER_LEFT;
	oMessage << GetID();
	oMessage << id;

	ret = SendMessage(oMessage, CGlobalInformation::m_iGroupEventProtocolStream, true);
	m_poInterface->PeerLeft(GetID(), id);
	oMessage.clear();
	m_poInterface->ClientDisconnected(id, oMessage);
	return ret;
}

const bool P2PSEL0::CGroupSocialServer::ChangeToGroup(const TClientID iClientID, CGroupAbstract< CRemoteClient >& oGroup)
{
	CRemoteClient* pClient = GetClientPointer(iClientID);
	if (pClient == NULL)
		return false;

	bool ret = CGroupAbstract< CRemoteClient >::ChangeToGroup(iClientID, *(CGroupAbstract< CRemoteClient >*)m_poGroupLimbo);
	if (!ret)
		return false;

	/// Send its peer left
	buffer_c oMessage;
	oMessage << (unsigned char)PROTOCOL_EVENT_PEER_LEFT;
	oMessage << GetID();
	oMessage << iClientID;

	ret = pClient->SendMessage(oMessage, CGlobalInformation::m_iGroupEventProtocolStream, true);
	ret &= SendMessage(oMessage, CGlobalInformation::m_iGroupEventProtocolStream, true);
	m_poInterface->PeerLeft(GetID(), iClientID);
	return ret;
}

void P2PSEL0::CGroupSocialServer::ProcessNonBlocking() 
{
	CGroupAbstract<CRemoteClient>::ProcessNonBlocking();

	/// Check if someone is capable too enter in this group
	vector< TClientID > oClients;
	m_poGroupLimbo->GetWishList(GetID(), oClients);

	for (unsigned int i = 0; i < oClients.size(); i++)
	{
		const TClientID iClientCandidate = oClients[i];
		/// Client is always capable for SOCIAL GROUP!
		m_poGroupLimbo->ChangeToGroup(iClientCandidate, *(CGroupAbstract< CRemoteClient >*)this);
	}

	/// Chat relay
	buffer_c oChatRequest;
	buffer_c oChatReply;
	string oMessage;

	for (int j = 0; j < GetNumberOfRemoteClients(); j++)
	{
		const TClientID id = GetClientID(j);
		CRemoteClient* poClient = GetClientPointer(id);
		while (poClient->ReceiveSpecificStream(oChatRequest, CGlobalInformation::m_iChatStream))
		{
			printf("Recebi chat do client %d\n", id);
			oChatReply.clear();
			oChatReply << id;
			oChatRequest >> oMessage;
			oChatReply << oMessage;
			SendMessage(oChatReply, CGlobalInformation::m_iChatStream, true);
		}
	}
}
