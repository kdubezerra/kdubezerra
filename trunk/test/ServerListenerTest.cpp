#include "ServerListenerTest.h"
#include "fake_console.h"
#include "zig/buffer.h"

extern fake_console_c oConsole;

void CServerListenerTest::ClientConnected(const TClientID id, const buffer_c& oMessage)
{
  oConsole.printf("CServerListenerTest::ClientConnected - %d\n", id);
}

void CServerListenerTest::ClientDisconnected(const TClientID id, const buffer_c& oMessage)
{
  oConsole.printf("CServerListenerTest::ClientDisconnected - %d\n", id);
}

void CServerListenerTest::PeerJoined(const TGroupID groupId, const TClientID clientId )
{
  oConsole.printf("CServerListenerTest::PeerJoined - group %d client %d\n", groupId, clientId);
}

void CServerListenerTest::PeerLeft(const TGroupID groupId, const TClientID id)
{
  oConsole.printf("CServerListenerTest::PeerLeft - group %d client %d\n", groupId, id);
}

const bool CServerListenerTest::ClientAccept(const buffer_c& oMessage, const address_c& oAddress)
{
  oConsole.printf("CServerListenerTest::ClientAccept - %s\n", oAddress.get_address().c_str());
  return true;
}

const bool CServerListenerTest::IncomingData(const TClientID id, const buffer_c& oBuffer)
{
  //oConsole.printf("CServerListenerTest::IncomingData - client: %d packet size: %d\n", id, oBuffer.size());
  return true;
}

void CServerListenerTest::GroupConnectionFailed(const TClientID id)
{
  oConsole.printf("CServerListenerTest::GroupConnectionFailed - client: %d\n", id);
}
