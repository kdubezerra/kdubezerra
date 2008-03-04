#pragma once

#include "GroupAbstract.h"
#include "RemotePeer.h"
#include "ClientEventInterface.h"

namespace P2PSEL0
{
  class CGroupSocialClient: public CGroupAbstract< CRemotePeer >
  {
  public:
    /// Default constructor
    CGroupSocialClient(): CGroupAbstract<CRemotePeer>::CGroupAbstract(), m_poClientEventInterface(NULL) {}
    /// Constructor
    CGroupSocialClient(TGroupID id, station_c& oServer, CClientEventInterface& oClientEventInteface): CGroupAbstract<CRemotePeer>::CGroupAbstract(id, 0, oServer), m_poClientEventInterface(&oClientEventInteface) {};
    /// Destructor
    ~CGroupSocialClient() {}
    /// Chat message
    virtual const bool Chat(const string& oMessage);
    /// Send group
    virtual const bool SendMessage(buffer_c& oData, const int iStream = -1, const bool bReliable = true);
    /// Send group
    virtual const bool SendPacket();

  protected:
    /// Callback Interface
    CClientEventInterface* m_poClientEventInterface;
  };
}

