#include "GroupActionServer.h"

const bool P2PSEL0::CGroupActionServer::Chat( const string& oMessage )
{
  buffer_c oBuffer;
  oBuffer << oMessage;

  return SendMessage(oBuffer, CGlobalInformation::m_iChatStream, true);
}

const bool P2PSEL0::CGroupActionServer::SendMessage( buffer_c& oData, const int iStream /*= -1*/, const bool bReliable /*= true*/ )
{
  for (std::map< TClientID, CRemoteClient* >::iterator it = m_oRemoteMap.begin(); it != m_oRemoteMap.end(); it++)
  {
    it->second->SendMessage(oData, iStream, bReliable);
  }
  return true;
}

const bool P2PSEL0::CGroupActionServer::ChangeToGroup(const TClientID iClientID, CGroupAbstract<CRemoteClient>& oGroup)
{
  buffer_c oMessage;
  oMessage << (unsigned char)PROTOCOL_EVENT_PEER_LEFT;
  oMessage << GetID();
  oMessage << iClientID;

  CRemoteClient* poClient = GetClientPointer(iClientID);
  poClient->SendMessage(oMessage, CGlobalInformation::m_iGroupEventProtocolStream, true);

  /*
  oMessage.clear();
  /// Send group sync to everyone
  oMessage << (unsigned char)PROTOCOL_MAN_GROUP_SYNC;
  oMessage << (unsigned short)0;

  poClient->SendMessage(oMessage, CGlobalInformation::m_iGroupEventProtocolStream, true);
  */

  const bool ret = CGroupAbstract< CRemoteClient >::ChangeToGroup(iClientID, *m_poGroupLimbo);
  if (!ret)
    return false;
  return (NotifyChange(iClientID));
}

const bool P2PSEL0::CGroupActionServer::RemoveClient(const TClientID id)
{
  bool ret = CGroupAbstract< CRemoteClient >::RemoveClient(id);
  if (!ret)
    return false;
  ret = NotifyChange(id);
  buffer_c oMessage;
  m_poInterface->ClientDisconnected(id, oMessage);
  return ret;
}

const bool P2PSEL0::CGroupActionServer::NotifyChange(const TClientID id)
{
  /// Send message to everyone in group and everyone trying to enter in group that someone inside
  /// the group has quit!

  buffer_c oMessage;
  /// Send group sync to everyone
  oMessage << (unsigned char)PROTOCOL_MAN_GROUP_SYNC;

  vector< TClientID > oClients;
  m_poGroupLimbo->GetWishList(GetID(), oClients);
  vector< TClientID > oClientsWishing = oClients;

  oMessage << (unsigned short)(oClients.size() + m_oRemoteMap.size());

  for (int i = 0; i < (int)oClients.size(); i++)
  {
    oMessage << oClients[i];
    oMessage << m_poGroupLimbo->GetClientPointer(oClients[i])->GetAddress();
  }

  for (std::map< TClientID, CRemoteClient* >::iterator it = m_oRemoteMap.begin(); it != m_oRemoteMap.end(); it++)
  {
    oMessage << it->first;
    oMessage << it->second->GetAddress();
  }

  /// Send sync to limbo TOO!
  for (unsigned int i = 0; i < oClientsWishing.size(); i++)
  {
    CRemoteClient* poClient = m_poGroupLimbo->GetClientPointer(oClientsWishing[i]);
    poClient->SendMessage(oMessage, CGlobalInformation::m_iGroupEventProtocolStream, true);
  }

  /// Broadcast message
  SendMessage(oMessage, CGlobalInformation::m_iGroupEventProtocolStream, true);

  oMessage.clear();
  /// Send its peer left

  oMessage << (unsigned char)PROTOCOL_EVENT_PEER_LEFT;
  oMessage << GetID();
  oMessage << id;

  /// Broadcast message
  SendMessage(oMessage, CGlobalInformation::m_iGroupEventProtocolStream, true);
  m_poInterface->PeerLeft(GetID(), id);
  return true;
}

void P2PSEL0::CGroupActionServer::ProcessNonBlocking()
{
  CGroupAbstract<CRemoteClient>::ProcessNonBlocking();

  /// Check if someone is capable to enter in this group
  vector< TClientID > oClients;
  m_poGroupLimbo->GetWishList(GetID(), oClients);
  //vector< TClientID > oCandidatesApproved;

  for (unsigned int i = 0; i < oClients.size(); i++)
  {

    TClientID iClientCandidate = oClients[i];
    bool bOk = true;
    for (int j = 0; j < GetNumberOfRemoteClients(); j++)
    {
      const TClientID iGroupClientID = GetClientID(j);
      if (!GetClientPointer(iGroupClientID)->IsConnected(iClientCandidate))
      {
        bOk = false;
        break;
      }
      if (!m_poGroupLimbo->GetClientPointer(iClientCandidate)->IsConnected(iGroupClientID))
      {
        bOk = false;
        break;
      }
    }
    if (bOk)
      m_poGroupLimbo->ChangeToGroup(iClientCandidate, *(CGroupAbstract< CRemoteClient >*)this);
      //oCandidatesApproved.push_back(iClientCandidate);
  }

  /*
  for (int i = 0; i < (int)oCandidatesApproved.size(); i++)
    m_poGroupLimbo->ChangeToGroup(oCandidatesApproved[i], *(CGroupAbstract< CRemoteClient >*)this);
  */
}
