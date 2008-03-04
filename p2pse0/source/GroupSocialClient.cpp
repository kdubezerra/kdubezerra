#include "GroupSocialClient.h"

const bool P2PSEL0::CGroupSocialClient::Chat( const string& oMessage )
{
	buffer_c oBuffer;

	oBuffer << oMessage;
	SendMessage(oBuffer, CGlobalInformation::m_iChatStream, true);

	return true;
}

const bool P2PSEL0::CGroupSocialClient::SendMessage( buffer_c& oData, const int iStream /*= -1*/, const bool bReliable /*= true*/ )
{
	return m_oServer.write(oData, iStream, bReliable, NULL) == 1;
}

const bool P2PSEL0::CGroupSocialClient::SendPacket()
{
	// TODO
	return true;
}

