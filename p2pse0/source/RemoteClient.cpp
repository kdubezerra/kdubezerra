#include <iostream>
#include "RemoteClient.h"

void P2PSEL0::CRemoteClient::station_incoming_data( buffer_c& pkt, int packet_id )
{
  if (!m_oStation.is_connected())
    return;
	
  //m_oStation.get_console()->xprintf(2, "CRemoteClient::station_incoming_data: %d\n", m_id);

  m_dLastPacketReceiveTime = get_time();

  buffer_c oBuffer;

  while (m_oStation.read(oBuffer, CGlobalInformation::m_iGroupEventProtocolStream) > 0)
  {
    unsigned char iType;
    oBuffer >> iType;
    switch (iType)
    {
    case PROTOCOL_MAN_GROUP_SYNC:
      {
        /// IMPOSSIBLE! ONLY SERVER RECEIVE THIS!
        std::cerr << "CRemoteClient::station_incoming_data - PROTOCOL_MAN_GROUP_SYNC shouldn't be receive\n";
      }
      break;
    case PROTOCOL_EVENT_PEER_LEFT:
      {
        /// IMPOSSIBLE! ONLY SERVER RECEIVE THIS!
        std::cerr << "CRemoteClient::station_incoming_data - PROTOCOL_EVENT_PEER_LEFT shouldn't be receive\n";
      }
      break;
    case PROTOCOL_EVENT_PEER_JOINED:
      {
        /// IMPOSSIBLE! ONLY SERVER RECEIVE THIS!
        std::cerr << "CRemoteClient::station_incoming_data - PROTOCOL_EVENT_PEER_JOINED shouldn't be receive\n";
      }
      break;
    case PROTOCOL_MAN_GROUP_CONNECTED:
      {
        TClientID id;
        oBuffer >> id;
        oConnectedClient.insert(id);
        m_oStation.get_console()->xprintf(2, "Remote Client %s Connected to %d\n", m_oStation.get_remote_address().get_address().c_str(), id);
      }
      break;
    }
  }

  /// ADD CALLBACK HERE!
  m_poInterface->IncomingData(m_id, pkt);
}

const bool P2PSEL0::CRemoteClient::IsConnected(TClientID id)
{
  std::set< TClientID >::iterator it = oConnectedClient.find(id);
  bool ret = (it != oConnectedClient.end());
  return ret;
}

void P2PSEL0::CRemoteClient::station_connected( buffer_c& connection_pkt, const bool forced )
{
  m_dLastPacketReceiveTime = get_time();
}

void P2PSEL0::CRemoteClient::station_connection_timed_out()
{
  //NEVER HAPPENS!
}

void P2PSEL0::CRemoteClient::station_connection_refused( buffer_c& reject_pkt )
{
  //NEVER HAPPENS!
}

void P2PSEL0::CRemoteClient::station_disconnected( bool remote, buffer_c& reason )
{
  //Schedule disconnect!
  m_dLastPacketReceiveTime -= CGlobalInformation::m_dTimeoutValue;
}

P2PSEL0::CRemoteClient& P2PSEL0::CRemoteClient::operator=( const CRemoteClient& oRemote )
{
  m_oStation = oRemote.m_oStation;
  m_oStation.set_callback_interface(this);
  m_id = oRemote.m_id;
  return *this;
}

const bool P2PSEL0::CRemoteClient::Timeout()
{
  return (get_time() - m_dLastPacketReceiveTime) > CGlobalInformation::m_dTimeoutValue;
}

const bool P2PSEL0::CRemoteClient::GroupTimeout()
{
  return (get_time() - m_dGroupJoinAttemptTime) > CGlobalInformation::m_dTimeoutValue;
}
