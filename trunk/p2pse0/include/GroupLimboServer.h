#pragma once

#include <vector>
#include "GroupAbstract.h"
#include "RemoteClient.h"
#include "ServerEventInterface.h"

namespace P2PSEL0
{
  ///Foward declaration
  class CGroupSocialServer;

  class CGroupLimboServer: public CGroupAbstract< CRemoteClient >
  {
  public:
    /// Default constructor
    CGroupLimboServer(): CGroupAbstract<CRemoteClient>::CGroupAbstract(), m_poInterface(NULL) {}
    /// Constructor
    CGroupLimboServer(TGroupID id, const unsigned int iMaxClients, CServerEventInterface& oEventInterface): CGroupAbstract<CRemoteClient>::CGroupAbstract(id, iMaxClients, station_c::get_invalid_station()), m_poInterface(&oEventInterface) {};
    /// Remove from this group and put in limbo
    virtual const bool PullToLimbo(const TClientID iClientID, CGroupAbstract< CRemoteClient >& oGroupSource, CGroupAbstract< CRemoteClient >& oGroupDest);
    /// Remove this guy from list
    virtual const bool RemoveClient(const TClientID id);
    /// Get wish list to group
    virtual void GetWishList(TGroupID id, std::vector< TClientID >& oList);
    /// Notify that someone is leaving limbo
    virtual const bool NotifyChange(const TClientID id, const bool bIn);
    /// Remove from this group and put in limbo
    virtual const bool ChangeToGroup(const TClientID iClientID, CGroupAbstract< CRemoteClient >& oGroup);

    static const int GetTypeEnum() { return GROUP_LIMBO_TYPE; }

    /// Chat message
    virtual const bool Chat(const string& oMessage) { return false; }
    /// Send group
    virtual const bool SendMessage(buffer_c& oData, const int iStream, const bool bReliable = true) { return false; }

    virtual void ProcessNonBlocking();

    /// Set limbo group
    virtual void SetGroupDefault(CGroupSocialServer& oGroup) {m_poGroupDefault = &oGroup;}

  protected:
    /// Callback interface
    CServerEventInterface* m_poInterface;
    /// Default group
    CGroupSocialServer* m_poGroupDefault;
  };
}
