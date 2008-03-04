#include <string>
using std::string;

#include "ClientListenerTest.h"
#include "fake_console.h"

extern fake_console_c oConsole;

void CClientListenerTest::Connected(const TClientID id, buffer_c& oMessage)
{
  m_id = id;
  oConsole.xprintf(3, "%d - Connected\n", m_id);
}

void CClientListenerTest::Disconnected(buffer_c& oMessage)
{
  oConsole.xprintf(3, "CClientListenerTest::Disconnected -  My: %d\n", m_id);
}

void CClientListenerTest::GroupJoined(const TGroupID gid)
{
  oConsole.xprintf(3, "CClientListenerTest::GroupJoined - My: %d, Group: %d\n", m_id, gid);
}

void CClientListenerTest::GroupLeft(const TGroupID gid)
{
  oConsole.xprintf(3, "CClientListenerTest::GroupLeft - My: %d, Group: %d\n", m_id, gid);
}

void CClientListenerTest::PeerJoined(const TClientID id)
{
  oConsole.xprintf(3, "CClientListenerTest::PeerJoined - My: %d, Other: %d\n", m_id, id);
}

void CClientListenerTest::PeerLeft(const TClientID id)
{
  oConsole.xprintf(3, "CClientListenerTest::PeerLeft - My: %d, Other: %d\n", m_id, id);
}

void CClientListenerTest::ConnectionFailed(buffer_c& oMessage)
{
  oConsole.xprintf(3, "CClientListenerTest::ConnectionFailed - My: %d\n", m_id);
}

void CClientListenerTest::IncomingPeerData(const TClientID id, buffer_c& oUnreliable)
{
	string oMessage;
	TClientID iSenderID = INVALID_ID;

	// Receive chat messages
	while (m_poClient0->ReceiveChat(oMessage, iSenderID))
	{
		oConsole.printf("IncomingPeerData(%d) >>> %d says \"%s\"\n", id, iSenderID, oMessage.c_str());		
	}
}

void CClientListenerTest::IncomingServerData(buffer_c& oUnreliable)
{
	string oMessage;
	TClientID iSenderID = INVALID_ID;

	// Receive chat messages
	while (m_poClient0->ReceiveChat(oMessage, iSenderID))
	{
		oConsole.printf("IncomingServerData >>> %d says \"%s\"\n", iSenderID, oMessage.c_str());		
	}

}
