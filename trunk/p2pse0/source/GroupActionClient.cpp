#include "GroupActionClient.h"
using namespace P2PSEL0;

const bool CGroupActionClient::Chat( const string& oMessage )
{
	buffer_c oBuffer;
	oBuffer << oMessage;

	for (std::map< TClientID, CRemotePeer* >::iterator it = m_oRemoteMap.begin();
		it != m_oRemoteMap.end(); it++)
	{
		it->second->SendMessage(oBuffer, CGlobalInformation::m_iChatStream);
	}

	return true;
}

const bool CGroupActionClient::SendMessage( buffer_c& oData, const int iStream /*= -1*/, const bool bReliable /*= true*/ )
{
	for (std::map< TClientID, CRemotePeer* >::iterator it = m_oRemoteMap.begin();
		it != m_oRemoteMap.end(); it++)
	{
		it->second->SendMessage(oData, iStream, bReliable);
	}
	return true;
}
