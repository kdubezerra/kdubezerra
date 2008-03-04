#pragma once

#include "ClientEventInterface.h"
#include "ClientInterface.h"

class CClientListenerTest: public P2PSEL0::CClientEventInterface
{
public:

	CClientListenerTest()
	{
		m_poClient0 = NULL;
		m_id = INVALID_ID;
	}

	void SetClient0(P2PSEL0::CClientInterface* poClient0)
	{
		m_poClient0 = poClient0;
	}

  TClientID m_id;
  P2PSEL0::CClientInterface* m_poClient0;

  void Connected(const TClientID id, buffer_c& oMessage);
  void Disconnected(buffer_c& oMessage);
  void GroupJoined(const TGroupID gid);
  void GroupLeft(const TGroupID gid);
  void PeerJoined(const TClientID id);
  void PeerLeft(const TClientID id);
  void ConnectionFailed(buffer_c& oMessage);
  void IncomingPeerData(const TClientID id, buffer_c& oUnreliable);
  void IncomingServerData(buffer_c& oUnreliable);
};

