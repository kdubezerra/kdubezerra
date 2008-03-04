#pragma once

// ZIG include files.
#include <zig/station.h>
#include <zig/msocket.h>
#include <zig/buffer.h>
#include <zig/leetnet.h>
//#include <zig/typereg.h>
#include <zig/address.h>
#include "config.h"

namespace P2PSEL0
{
  class CRemoteAbstract: protected station_callback_i
  {
    /// Private callbacks implementation
    friend class station_c;

  public:
    /// Empty constructor for STL
    CRemoteAbstract():m_oStation(station_c::get_invalid_station()) { m_oStation.set_callback_interface(this); }
    /// Constructor
    /// Não responsável pela construção e destruição de stations
    CRemoteAbstract(station_c& oStation, TClientID id);
    /// Destructor
    ~CRemoteAbstract();
    /// Send
    virtual const bool SendMessage(buffer_c& oData, const int iStream = -1, const bool bReliable = true);
    /// Send
    virtual const bool SendPacket();
    /// Receive from one stream
    virtual const bool ReceiveSpecificStream(buffer_c& oData, const int iStream);
    /// Receive from any stream
    virtual const bool ReceiveAnyStream(buffer_c& oData, int& iStream);

    /// Get id
    virtual const TClientID GetID() const { return m_id; }

    /// Operator =
    //CRemoteAbstract& operator=(const CRemoteClient& oRemote);

    /// Connected
    virtual const bool IsConnected() const { return m_oStation.is_connected(); }

    /// Force to connected state
    virtual const bool ForceConnectedState() { return false; /*DUMMY!!!*/}
    /// Get Address
    virtual const string GetAddress();

    virtual const bool Timeout() { return false; }

    virtual void SetPacketLossEmulation(double dLoss);

    virtual void SetPacketLatencyEmulation(double dBaseLatency, double dDeltaLatency);

		virtual const double GetRoundTripTimeEstimative() { return m_oStation.get_rtt_estimative(); }

		virtual const double GetLossEstimative() { return m_oStation.get_loss_estimative(); }

		virtual const double GetOutOfOrderEstimative() { return m_oStation.get_delay_estimative(); }

    virtual const bool HasSomethingToSend() { return !m_oStation.nothing_to_send(); }

    /// Create Stream
    virtual const int CreateStream(int iType, policy_c oPolicy);

  protected:

    /// station
    station_c& m_oStation;

    /// Client Id
    TClientID m_id;
  };
}

