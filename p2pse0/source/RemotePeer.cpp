#include "RemotePeer.h"

void P2PSEL0::CRemotePeer::station_incoming_data( buffer_c& pkt, int packet_id )
{
  if (!m_oStation.is_connected())
    return;
  m_poInterface->IncomingPeerData(m_id, pkt);
}

void P2PSEL0::CRemotePeer::station_connected( buffer_c& connection_pkt, const bool forced )
{
	m_bInformServer = true;
}

void P2PSEL0::CRemotePeer::station_connection_timed_out()
{
	// Aqui sim deu pra bola...
}
void P2PSEL0::CRemotePeer::station_connection_refused( buffer_c& reject_pkt )
{
	m_oStation.connect();
}

void P2PSEL0::CRemotePeer::station_disconnected( bool remote, buffer_c& reason )
{
  /// FUTURE RECOVERY PROTOCOL!
  // Algo como m_oStation.connect() ???
}

P2PSEL0::CRemotePeer& P2PSEL0::CRemotePeer::operator=( const CRemotePeer& oRemote )
{
  m_oStation = oRemote.m_oStation;
  m_oStation.set_callback_interface(this);
  m_id = oRemote.m_id;
  m_poInterface = oRemote.m_poInterface;
  m_bInformServer = oRemote.m_bInformServer;
  return *this;
}
