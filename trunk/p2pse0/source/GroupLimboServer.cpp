#include "GroupLimboServer.h"
using namespace P2PSEL0;

const bool CGroupLimboServer::PullToLimbo(const TClientID iClientID, CGroupAbstract< CRemoteClient >& oGroupSource, CGroupAbstract< CRemoteClient >& oGroupDest)
{
  CRemoteClient* pClient = oGroupSource.GetClientPointer(iClientID);
  //Isn't supposed to be here, PATCH!
  pClient->ClearConnections();
  pClient->RestartGroupTimeout();
  //---------------------------------
  pClient->SetWishGroup< CGroupAbstract< CRemoteClient > >(&oGroupDest);

  //Pull to limbo
  if (oGroupSource.GetID() != GetID())
  {
    if (!oGroupSource.ChangeToGroup(iClientID, *this))
      return false;
  }

  NotifyChange(iClientID, true);
  return true;
}

void CGroupLimboServer::GetWishList(TGroupID id, std::vector< TClientID >& oList)
{
  for (std::map< TClientID, CRemoteClient* >::iterator it = m_oRemoteMap.begin(); it != m_oRemoteMap.end(); it++)
  {
    CGroupAbstract< CRemoteClient >* poGroup = it->second->GetWishGroup< CGroupAbstract< CRemoteClient > >(false);
    if (poGroup->GetID() == id)
    {
      oList.push_back(it->first);
    }
  }
}

const bool CGroupLimboServer::RemoveClient(const TClientID id)
{
  bool ret = CGroupAbstract<CRemoteClient>::RemoveClient(id);
  if (!ret)
    return false;
  ret = NotifyChange(id, false);
  buffer_c oMessage;
  m_poInterface->ClientDisconnected(id, oMessage);
  return ret;
}

const bool CGroupLimboServer::NotifyChange(const TClientID id, const bool bIn)
{
  /// Send message to everyone in the wishing group and everyone that was attempting to enter in the wishing group
  /// that someone aborted it's connection to that group (bIn = false) or is trying to enter in group (bIn = true)

  CRemoteClient* poClient = GetClientPointer(id);
  CGroupAbstract< CRemoteClient >* poGroup = poClient->GetWishGroup< CGroupAbstract< CRemoteClient > >(!bIn);

  /// If it's social group, just leave!
  if (GetGroupType(poGroup->GetID()) == GROUP_SOCIAL_TYPE)
    return true;

  buffer_c oMessage;
  /// Send group sync to everyone
  oMessage << (unsigned char)PROTOCOL_MAN_GROUP_SYNC;

  vector< TClientID > oClientsWishing;
  GetWishList(poGroup->GetID(), oClientsWishing);

  vector< TClientID > oClientsAlreadyInGroup;
  set< TClientID > oClientsAlreadyInGroupSet;
  poGroup->GetClientSet(oClientsAlreadyInGroupSet);
  for (std::set< TClientID >::iterator it = oClientsAlreadyInGroupSet.begin(); it != oClientsAlreadyInGroupSet.end(); it++)
    oClientsAlreadyInGroup.push_back(*it);

  oMessage << (unsigned short) (oClientsWishing.size() + oClientsAlreadyInGroup.size());
  for (int i = 0; i < (int)oClientsAlreadyInGroup.size(); i++)
  {
    TClientID idToConnect = oClientsAlreadyInGroup[i];
    oMessage << idToConnect;
    oMessage << poGroup->GetClientPointer(idToConnect)->GetAddress();
  }
  for (int i = 0; i < (int)oClientsWishing.size(); i++)
  {
    TClientID idToConnect = oClientsWishing[i];
    oMessage << idToConnect;
    oMessage << GetClientPointer(idToConnect)->GetAddress();
  }

  /// Send sync to limbo TOO!
  for (unsigned int i = 0; i < oClientsWishing.size(); i++)
  {
    CRemoteClient* poClient = GetClientPointer(oClientsWishing[i]);
    poClient->SendMessage(oMessage, CGlobalInformation::m_iGroupEventProtocolStream, true);
  }

  /// Broadcast message
  poGroup->SendMessage(oMessage, CGlobalInformation::m_iGroupEventProtocolStream, true);
  return true;
}

const bool CGroupLimboServer::ChangeToGroup(const TClientID iClientID, CGroupAbstract< CRemoteClient >& oGroup)
{
  CRemoteClient* poClient = GetClientPointer(iClientID);
  CGroupAbstract< CRemoteClient >* poGroup = poClient->GetWishGroup< CGroupAbstract< CRemoteClient > >(true);

  CGroupAbstract<CRemoteClient>::ChangeToGroup(iClientID, *poGroup);

  /// Send its peer joined
  buffer_c oMessage;
  oMessage << (unsigned char)PROTOCOL_EVENT_PEER_JOINED;
  oMessage << oGroup.GetID();
  oMessage << iClientID;

  poGroup->SendMessage(oMessage, CGlobalInformation::m_iGroupEventProtocolStream, true);

  /// Send to incoming peer a list of peers that are already in group
  for (int i = 0; i < oGroup.GetNumberOfRemoteClients(); i++)
  {
    oMessage.clear();
    const TClientID clientID = oGroup.GetClientID(i);
    if (clientID != iClientID)
    {
      buffer_c oMessage;
      oMessage << (unsigned char)PROTOCOL_EVENT_PEER_JOINED;
      oMessage << oGroup.GetID();
      oMessage << oGroup.GetClientID(i);
      poClient->SendMessage(oMessage, CGlobalInformation::m_iGroupEventProtocolStream, true);
    }
  }
  m_poInterface->PeerJoined(oGroup.GetID(), iClientID);
  return true;
}

void CGroupLimboServer::ProcessNonBlocking()
{
  CGroupAbstract< CRemoteClient >::ProcessNonBlocking();

  /// Check first if someone give up
  for (int i = 0; i < GetNumberOfRemoteClients(); i++)
  {
    TClientID iClientCandidate = GetClientID(i);
    if (GetClientPointer(iClientCandidate)->GroupTimeout())
    {
      PullToLimbo(iClientCandidate, *this, *(CGroupAbstract< CRemoteClient >*)m_poGroupDefault);
      m_poInterface->GroupConnectionFailed(iClientCandidate);
    }
  }
}
