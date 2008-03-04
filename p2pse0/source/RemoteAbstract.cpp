#include "RemoteAbstract.h"

P2PSEL0::CRemoteAbstract::CRemoteAbstract(station_c& oStation, TClientID id): m_oStation(oStation), m_id(id)
{
  m_oStation.set_callback_interface(this);
}

P2PSEL0::CRemoteAbstract::~CRemoteAbstract()
{
  msocket_c* poSocket = m_oStation.get_socket();
  if (poSocket == NULL)
    return;

  station_c* poStation = &m_oStation;
  address_c oAddress = poStation->get_remote_address();
  poSocket->delete_station(oAddress);
}

const bool P2PSEL0::CRemoteAbstract::SendMessage( buffer_c& oData, const int iStream, const bool bReliable)
{
  if (!m_oStation.is_connected())
    return false;
  return m_oStation.write(oData, iStream, bReliable) >= 0;
}

const bool P2PSEL0::CRemoteAbstract::SendPacket()
{
  return m_oStation.send_packet(NULL) == 0;
}

const bool P2PSEL0::CRemoteAbstract::ReceiveSpecificStream( buffer_c& oData, const int iStream )
{
  if (!m_oStation.is_connected())
    return false;
	oData.clear();
  return m_oStation.read(oData, iStream) == 1;
}

const bool P2PSEL0::CRemoteAbstract::ReceiveAnyStream( buffer_c& oData, int& iStream )
{
  if (!m_oStation.is_connected())
    return false;
  m_oStation.seek_first_stream();
	oData.clear();
  for (iStream = m_oStation.get_next_stream(); iStream >= 0; m_oStation.get_next_stream())
  {
    if (m_oStation.read(oData, iStream) > 0)
      return true;
  }
  return false;
}

const string P2PSEL0::CRemoteAbstract::GetAddress()
{
  return m_oStation.get_remote_address().get_address();
}

void P2PSEL0::CRemoteAbstract::SetPacketLossEmulation(double dLoss)
{
  m_oStation.set_packet_loss_emulation(dLoss);
}

void P2PSEL0::CRemoteAbstract::SetPacketLatencyEmulation(double dBaseLatency, double dDeltaLatency)
{
  m_oStation.set_packet_latency_emulation(dBaseLatency, dDeltaLatency);
}

const int P2PSEL0::CRemoteAbstract::CreateStream(int iType, policy_c oPolicy)
{
  return m_oStation.create_stream(iType, oPolicy);
}

/*
void P2PSEL0::CRemoteAbstract::station_incoming_data( buffer_c& pkt, int packet_id )
{
  if (!m_bConnected)
    return;
}

void P2PSEL0::CRemoteAbstract::station_connected( buffer_c& connection_pkt )
{
  m_bConnected = true;
}

void P2PSEL0::CRemoteAbstract::station_connection_timed_out()
{
  m_bConnected = false;
}

void P2PSEL0::CRemoteAbstract::station_connection_refused( buffer_c& reject_pkt )
{
  m_bConnected = false;
}

void P2PSEL0::CRemoteAbstract::station_disconnected( bool remote, buffer_c& reason )
{
  m_bConnected = false;
}

P2PSEL0::CRemoteClient& P2PSEL0::CRemoteClient::operator=( const CRemoteClient& oRemote )
{
  m_oStation = oRemote.m_oStation;
  m_bConnected = oRemote.m_bConnected;
  m_id = oRemote.m_id;
  return *this;
}
*/
