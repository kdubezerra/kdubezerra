#pragma once

#if !defined (__P2PSE0_COMPILATION__)
#error "Bad, bad BigNose, no donut for you. You cannot include directly \"Client.h\"."
#endif

// ZIG include files.
#include <zig/station.h>
#include <zig/msocket.h>
#include <zig/buffer.h>
#include <zig/leetnet.h>
//#include <zig/typereg.h>
#include <zig/address.h>
#include <zig/console.h>
#include "idgen.h"
#include "config.h"
#include <map>
#include <list>
#include "GroupAbstract.h"
#include "GroupSocialClient.h"
#include "GroupLimboClient.h"
#include "GroupActionClient.h"
#include "RemotePeer.h"
#include "ClientEventInterface.h"
#include "ClientInterface.h"

namespace P2PSEL0
{
	class CClient: private msocket_callback_i, private station_callback_i, public CClientInterface
	{
		/// Hack against BigNose ;)
		friend class CClientInterface;
	private:
		CClient(CClientEventInterface& oEventInterface);
		/// Destructor
		~CClient();

	public:

		/// Start
		virtual const bool Start(const unsigned short startPort, const unsigned short endPort);
		/// Stop
		virtual const bool Stop();
		/// Connect
		virtual const bool Connect(address_c& oAddress);
		/// Disconnect
		virtual const bool Disconnect();
		/// Get id
		virtual const TClientID GetID() const { return m_iClientID; }
		/// Get group id
		virtual const TGroupID GetGroupID() const { return m_iGroupID; }
		/// Process non-blocking
		virtual const bool ProcessNonBlocking(); 
		/// Dispatch
		virtual const bool SendPacket();
		/// Get number of remote peers
		virtual const int GetNumberOfRemotePeers();
		/// Get remote peer id by index
		virtual const TClientID GetRemotePeerID(const int iIndex);

		virtual const bool GetRemotePeerAddress(const TClientID iRemotePeerID, address_c& oAddress);

		/// Send message unicast
		virtual const bool SendUnicastMessage(TClientID id, buffer_c& oBuffer, const int iStream, const bool bReliable);
		/// Send message broadcast
		virtual const bool SendBroadcastMessage(buffer_c& oBuffer, const int iStream, const bool bReliable);
		/// Chat
		virtual const bool Chat(const string& oMessage);
		/// Chat
		virtual const bool ReceiveChat(string& oMessage, TClientID& iClientID);
		/// Is connected
		virtual const bool IsConnected() const;
		/// Send to server
		virtual const bool SendMessageToServer(buffer_c& oBuffer, const int iStream, const bool bReliable);
		/// Send message to himself!
		virtual const bool SendOwnMessage(buffer_c& oBuffer, const int iStream);
		/// Receive from server
		virtual const bool ReceiveFromServer(buffer_c& oBuffer, const int iStream);
		/// Receive from server
		virtual const bool ReceiveFromServerAnyStream(buffer_c& oBuffer, int& iStream);
		/// Receive own message
		virtual const bool ReceiveOwn(buffer_c& oBuffer, const int iStream);
		/// Receive own message
		virtual const bool ReceiveOwnAnyStream(buffer_c& oBuffer, int& iStream);
		/// Receive from server
		virtual const bool ReceiveFromPeer(buffer_c& oBuffer, const TClientID id, const int iStream);
		/// Receive from server
		virtual const bool ReceiveFromPeerAnyStream(buffer_c& oBuffer, const TClientID id, int& iStream);
		/// Receive from server
		virtual const bool ReceiveFromAnyPeer(buffer_c& oBuffer, TClientID& id, const int iStream);
		/// Receive from server
		virtual const bool ReceiveFromAnyPeerAnyStream(buffer_c& oBuffer, TClientID& id, int& iStream);
		/// Test stream
		virtual const bool IsValidStream(int iStream);
		/// Get server address
		virtual const bool GetServerAddress(address_c& oAddress);

		void SetDebuggingConsole(console_c& oConsole);

		virtual const bool IsGroupSocial();
		/// Is group action
		virtual const bool IsGroupAction();

		virtual const bool IsClientPresent(const TClientID id);

		virtual void SetPacketLossEmulation(double dLoss);

		virtual void SetPacketLatencyEmulation(double dBaseLatency, double dDeltaLatency);

		virtual const double GetServerRoundTripTimeEstimative();

		virtual const double GetRemotePeerRoundTripTimeEstimative(const TClientID id);

		virtual void SetRemotePacketLossEmulation(const TClientID id, double dLoss);

		virtual void SetRemotePacketLatencyEmulation(const TClientID id, double dBaseLatency, double dDeltaLatency);

		virtual const bool HasSomethingToSend();

		virtual const int CreateStream(const TClientID id, int iType, policy_c oPolicy);

		virtual const int CreateStreamToServer(int iType, policy_c oPolicy);

		/*! \brief Utility service to get an estimative of the packet loss from the server to the client
		\return an estimative of probability of packet loss from the server to the client (0.0 means "no packet loss", 1.0 means "all packets were lost")
		*/
		virtual const double GetServerLossEstimative() const;

		/*! \brief Utility service to get an estimative of the packet loss between the client and a specific peer
		\param id the unique identifier of the peer we are interested to estimate the packet loss
		\return an estimative of probability of packet loss from the specified remote peer (0.0 means "no packet loss", 1.0 means "all packets were lost")
		1.0 means "all packets were lost")
		*/
		virtual const double GetRemotePeerLossEstimative(const TClientID iClientId) /*const*/;

		/*! \brief Utility service to get an estimative of how packets arrive "out of order" from the server
		\return an estimative of how packets arrive out of order from the server (0.0 means "all packets arrive properly (e. g., 1, 2, 3, etc)",
		1.0 means "all packets arrive in backwards (e.g. 10, 9, 8, etc)". This measure is proportional to the standard deviation of the packet order
		*/
		virtual const double GetServerOutOfOrderEstimative() const;

		/*! \brief Utility service to get an estimative of how packets arrive "out of order" from the server
		\param id the unique identifier of the peer we are interested to estimate the out-of-order factor
		\return an estimative of how packets arrive out of order from the server (0.0 means "all packets arrive properly (e. g., 1, 2, 3, etc)",
		1.0 means "all packets arrive in backwards (e.g. 10, 9, 8, etc)". This measure is proportional to the standard deviation of the packet order
		*/
		virtual const double GetRemotePeerOutOfOrderEstimative(const TClientID iClientId) /*const*/;

	private:
		/// Get my group
		CGroupAbstract< CRemotePeer >* GetGroup();

		/// Create Group
		template<class T>
		CGroupAbstract< CRemotePeer >* CreateGroup(const TGroupID id, station_c& oServer);

		/// Get Limbo Group
		CGroupLimboClient& GetLimboGroup();

		void ProcessGroupSync(buffer_c& oBuffer);

		void ProcessPeerJoin(buffer_c& oBuffer);

		void ProcessPeerLeft(buffer_c& oBuffer);

		/// Socket callbacks
		virtual void msocket_incoming_unreg(address_c &addr, const buffer_c &pkt);

		virtual void msocket_read_error(NLsocket &sock);

		virtual bool msocket_accept_station(address_c &addr, const buffer_c& connection_data, buffer_c& answer);

		virtual void msocket_station_connected(station_c &st, const buffer_c& oConnectionData);

		virtual void msocket_closed();

		virtual void station_incoming_data(buffer_c& pkt, int packet_id);

		virtual void station_connected(buffer_c& connection_pkt, const bool forced);

		virtual void station_connection_timed_out();

		virtual void station_connection_refused(buffer_c& reject_pkt);

		virtual void station_disconnected(bool remote, buffer_c& reason);

	private:
		struct SGroup
		{
			SGroup(CGroupLimboClient& oLimbo): m_oLimbo(oLimbo), m_poActual(NULL) {}
			///Limbo Group
			CGroupLimboClient& m_oLimbo;
			///Actual Group
			CGroupAbstract< CRemotePeer >* m_poActual;
		};
		/// Group member
		SGroup m_sGroup;

		/// My group ID
		TGroupID m_iGroupID;

		///Socket
		msocket_c m_oSocket;
		///Client-Server Station
		station_c* m_poStation;

		///Console
		console_c* m_poConsole;

		/// Client id
		TClientID m_iClientID;

		/// Event interface
		CClientEventInterface* m_poInterface;

		/// List of incoming peers
		map< address_c, TClientID > m_oAddressIDMap;

		map<int, list<buffer_c> > m_oOwnMessagesMap;

		unsigned int m_iStreamIDGenerator;
	};
}

