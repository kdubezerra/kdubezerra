#pragma once

#include "ServerEventInterface.h"
#include "zig/address.h"
#include "fake_console.h"

class CServerListenerTest: public P2PSEL0::CServerEventInterface
{
public:
	void ClientConnected(const TClientID id, const buffer_c& oMessage);
	void ClientDisconnected(const TClientID id, const buffer_c& oMessage);
	void GroupConnectionFailed(const TClientID id);
	void PeerJoined(const TGroupID groupId, const TClientID clientId );
	void PeerLeft(const TGroupID groupId, const TClientID id);
	const bool ClientAccept(const buffer_c& oMessage, const address_c& oAddress);
	const bool IncomingData(const TClientID id, const buffer_c& oBuffer);

private:

};

