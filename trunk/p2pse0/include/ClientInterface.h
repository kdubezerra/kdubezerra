/*! \file ClientInterface.h
 */

#pragma once

#include <string>
using std::string;

#include "config.h"
#include "ClientEventInterface.h"

/*! \brief All classes in P2PSE layer 0 are scoped by P2PSEL0 namespace. This is very useful
to avoid name collision between different layers.
*/
namespace P2PSEL0
{

	/*! \brief This interface is the access point to client-side services of P2PSE layer 0.
	It groups as many services for peer-to-peer communication as client-server services.
	The 'real' client class is not visible to the next layer, and the unique way to access it
	is through this facade
	*/
	class CClientInterface
	{
	public:

		/*! \brief Virtual destrutor (it permits the next layer to delete the layer 0 client object). */
		virtual ~CClientInterface() {}

		/*! \brief Utility service to get the identifier of the client current group.
		\return the current group identifier where the client is.
		*/
		virtual const TGroupID GetGroupID() const = 0;

		/*! \brief Utility service to check if the client's current group is a social group (without peer-to-peer
		connectivity).
		\return true if the current group is a social group.
		*/
		virtual const bool IsGroupSocial() = 0;

		/*! \brief Utility service to check if the client's current group is an action group (with peer-to-peer
		connectivity).
		\return true if the current group is an action group.
		*/
		virtual const bool IsGroupAction() = 0;

		/*! \brief Utility service to get the current client identifier. This identifier is unique and its
		lifetime corresponds to the session duration between client and server.
		\return the current client identifier
		*/
		virtual const TClientID GetID() const = 0;

		/*! \brief This service connect the client from the server. 
		The connection protocol is ASSYNCHRONOUS, it means, the next layer that calls this service CANNOT infers
		that the client was successfully connected after this method return. The caller layer is informed about
		the success or failure of the connection protocol through the callbacks that it must implement of the
		ClientEventInterface.
		\return true if no error in lower level connection protocol appears
		*/ 
		virtual const bool Connect(address_c& oAddress) = 0;

		/*! \brief This service disconnect the client from the server. 
		If the current group of this client is an action group, this call also disconnect all remote peers from it.
		The disconnection protocol is SYNCHRONOUS, it means, after the method return the caller of this service
		can infers that the client is disconnected from server and (possible) remote peers.
		\return true if no error in lower level disconnection protocol appears
		*/ 
		virtual const bool Disconnect() = 0;

		/*! \brief This service makes the client ready to connect to the server
		Actually, this service calls the correspondent service in the lower level, that will try to open an
		UDP communication port to send and receive packets.
		The parameters determine the ports interval where the lower layer will try to bind an opened socket.
		This service will fail if it was not possibly to the lower level to bind the socket in any port in the range
		\param startPort the minimum port value where the lower level can bind the socket
		\param endPort the maximum port value where the lower level can bind the socket
		\return true if underlying level could open an communication point (an UDP port)
		*/
		virtual const bool Start(const unsigned short startPort, const unsigned short endPort) = 0;

		/*! \brief This service closes the network communication capabilities of client in the lower level,
		that will closes the UDP socket. If the Disconnect service was not called before this service invocation,
		then this service will first Disconnect from server and all connected peers before its execution.
		This service will fail if it was not possibly to the lower level to correctly finish the network.
		\return true if underlying level could closes the communication point
		*/
		virtual const bool Stop() = 0;

		/*! \brief Utility service to get the current number of connected peers with this client.
		\return the current number of connected peers
		*/
		virtual const int GetNumberOfRemotePeers() = 0;

		/*! \brief Utility service to get the client identifier of an specified connected peer. To use this method,
		it is needed to call before the service GetNumberOfRemotePeers, to discover the number of connected peers
		and so the maximum value of iIndex.
		\param iIndex index of the queried client ( between the interval [0, GetNumberOfRemotePeers()[ )
		\return the client identifier of the queried peer
		*/
		virtual const TClientID GetRemotePeerID(const int iIndex) = 0;

		virtual const bool GetRemotePeerAddress(const TClientID iRemotePeerID, address_c& oAddress) = 0;

		/*! \brief Utility service to set an artificial loss of sent packets to the server and all possibly
		connected peers. By default, the loss emulation is zero.
		\param dLoss the probability of loss a packet (if dLoss = 0.0 then there aren`t packet loss, if it is 1.0 ALL packets are dropped)
		*/
		virtual void SetPacketLossEmulation(double dLoss) = 0;

		/*! \brief Utility service to set an artificial delay to sent packets to the server and all possibly
		connected peers. By default, the delay emulation is zero.
		\param dBaseLatency the minimum amout of seconds before send a packet
		\param dDeltaLatency the maximum difference between the minimum delay and the maximum delay to send a packet
		*/
		virtual void SetPacketLatencyEmulation(double dBaseLatency, double dDeltaLatency) = 0;

		/*! \brief Utility service to set an artificial loss of sent packets to a specific peer.
		By default, the loss emulation is zero.
		\param id the unique identifier of the peer we are interested to emulate outgoing packet loss.
		\param dLoss the probability of loss a packet (if dLoss = 0.0 then there aren`t packet loss,
		if it is 1.0 ALL packets are dropped)
		*/
		virtual void SetRemotePacketLossEmulation(const TClientID id, double dLoss) = 0;

		/*! \brief Utility service to set an artificial delay to send packets to a specific peer.
		By default, the delay emulation is zero.
		\param id the unique identifier of the peer we are interested to emulate outgoing packet latency.
		\param dBaseLatency the minimum amout of seconds before send a packet
		\param dDeltaLatency the maximum difference between the minimum delay and the maximum delay to send a packet
		*/
		virtual void SetRemotePacketLatencyEmulation(const TClientID id, double dBaseLatency, double dDeltaLatency) = 0;

		/*! \brief Utility service to get an estimative of the round-trip time between the client and the server
		\return an estimative of the latency between client and server (in seconds)
		*/
		virtual const double GetServerRoundTripTimeEstimative() = 0;

		/*! \brief Utility service to get an estimative of the round-trip time between the client and a specific peer
		\param id the unique identifier of the peer we are interested to estimate round-trip time
		\return an estimative of the latency between client and server (in seconds)
		*/
		virtual const double GetRemotePeerRoundTripTimeEstimative(const TClientID id) = 0;

		/*! \brief Send a message to a specified peer in the client's current group, using the stream specified
		by iStream. This method DOES NOT send any packet in the network, but only "schedule" the message to be sent
		in the next Dispatch() service call.
		\param id the session unique identifier of the peer for who the client will sent the message
		\param oBuffer message to be sent to the peer.
		\param iStream identifier of the stream where the message must be sent.
		\param bReliable defines if the message must be sent in a reliable fashion (with acknowledgement from remote
		side) or in a unreliable way (no ack). This parameter is only useful for the STREAM_CUMULATIVE_ACK stream
		type, that can mix reliable and unreliable messages. For the other streams types (STREAM_INDIVIDUAL_ACK and
		STREAM_NO_ACK), this parameter is ignored.
		\return true if message was correctly scheduled to be sent.
		*/
		virtual const bool SendUnicastMessage(TClientID id, buffer_c& oBuffer, const int iStream, const bool bReliable) = 0;

		/*! \brief Send a message to all peers in the client's current group, using the stream specified by the
		parameter iStream. This method DOES NOT send any packet in the network, but only "schedule" the message to be
		sent in the next Dispatch() service call.
		\param oBuffer message to be sent to all peers in the current group.
		\param iStream identifier of the stream where the message must be sent.
		\param bReliable defines if the message must be sent in a reliable fashion (with acknowledgement from remote
		side) or in a unreliable way (no ack). This parameter is only useful for the STREAM_CUMULATIVE_ACK stream
		type, that can mix reliable and unreliable messages. For the other streams types (STREAM_INDIVIDUAL_ACK and
		STREAM_NO_ACK), this parameter is ignored.
		\return true if message was correctly scheduled to be sent.
		*/
		virtual const bool SendBroadcastMessage(buffer_c& oBuffer, const int iStream, const bool bReliable) = 0;

		/*! \brief Send a message to all peers in the client's current group, using the stream reserved for chat.
		\param oMessage string to be sent for all connected peers.
		\return true if message was correctly scheduled to be sent.
		*/
		virtual const bool Chat(const string& oMessage) = 0;

		/*! \brief This service creates a communication channel ('stream') to a specific connected peer
		\param id the unique identifier of the client which we want to create a communication channel
		\param iType the type of this channel (STREAM_INDIVIDUAL_ACK, STREAM_CUMULATIVE_ACK, STREAM_NO_ACK).
		They offers different services in relation with order and reliability. If you do not know what we must
		set here, try STREAM_INDIVIDUAL_ACK, that guarantees order and reliability with "medium" traffic.
		See zig.h for more details.
		\param oPolicy the send policy of this channel. If you do not know what you must set here, try DefaultPolicy.
		See station.h for more details.
		\return the unique identifier of the created stream, or INVALID_ID if some parameter is incorrect
		*/
		virtual const int CreateStream(const TClientID id, int iType, policy_c oPolicy) = 0;

		/*! \brief This service creates a communication channel ('stream') to the server
		\param iType the type of this channel (STREAM_INDIVIDUAL_ACK, STREAM_CUMULATIVE_ACK, STREAM_NO_ACK).
		They offers different services in relation with order and reliability. If you do not know what we must
		set here, try STREAM_INDIVIDUAL_ACK, that guarantees order and reliability with "medium" traffic.
		See zig.h for more details.
		\param oPolicy the send policy of this channel. If you do not know what you must set here, try DefaultPolicy.
		See station.h for more details.
		\return the unique identifier of the created stream, or INVALID_ID if some parameter is incorrect
		*/
		virtual const int CreateStreamToServer(int iType, policy_c oPolicy) = 0;

		/*! \brief This service must be called periodically in order to the lower level sends packets
		to network. It sends packets to server and to possible connected peers (only in action groups)
		\return true if no error in lower level SendPacket services.
		*/
		virtual const bool SendPacket() = 0;

		/*! \brief This service must be called periodically in order to the lower level receives packets
		from network
		\return true if no error in lower level ProcessNonBlocking service.
		*/
		virtual const bool ProcessNonBlocking() = 0;

		/*! \brief Utility service to check if the client is connected with a server.
		\return true if client is connected with a server.
		*/
		virtual const bool IsConnected() const = 0;

		/*! \brief Send a message to server, using the stream specified by the parameter iStream.
		This method DOES NOT send any packet in the network, but only "schedule" the message to be sent in the
		next Dispatch() service call.
		\param oBuffer message to be sent to the server.
		\param iStream identifier of the stream where the message must be sent.
		\param bReliable defines if the message must be sent in a reliable fashion (with acknowledgement from remote
		side) or in a unreliable way (no ack). This parameter is only useful for the STREAM_CUMULATIVE_ACK stream
		type, that can mix reliable and unreliable messages. For the other streams types (STREAM_INDIVIDUAL_ACK and
		STREAM_NO_ACK), this parameter is ignored.
		\return true if message was correctly scheduled to be sent.
		*/
		virtual const bool SendMessageToServer(buffer_c& oBuffer, const int iStream, const bool bReliable) = 0;

		virtual const bool SendOwnMessage(buffer_c& oBuffer, const int iStream) = 0;

		/*! \brief This service verifies if there are new received chat messages from group that are not already
		delivered. It will check only avaliable messages in the stream reserved for chat. If there are new messages,
		the oMessage string object will be fit with the first not delivered chat message and the chat message copy
		owned by the lower level will be discarded (a chat messagee is not delivered twice).
		\param iClientID object where will be filled the identifier of the sender of the (possible)
		received chat message.
		\param oChatMessage object where (possible) received chat message will be stocked
		\return true if there were chat messages in the lower level (so oMessage contains a copy of a valid
		chat message).
		*/
		virtual const bool ReceiveChat(string& oChatMessage, TClientID& iClientID) = 0;

		/*! \brief This service verifies if there are new received messages from server that are not already
		delivered. It will check only avaliable messages in the stream specified by the iStream identifier.
		If there are new messages, the oBuffer object will be fit with the first message and the message copy owned
		by the lower level will be discarded (a message is not delivered twice).
		\param oBuffer object where (possible) received message will be stocked
		\param iStream identifier of the stream to be listed
		\return true if there were messages in the lower level (so oBuffer contains a copy of one valid message).
		*/
		virtual const bool ReceiveFromServer(buffer_c& oBuffer, const int iStream) = 0; //Not implemented

		/*! \brief This service verifies if there are new received messages from a server that are not already
		delivered. It will check avaliable messages in all the avaliables streams in the lower level.
		If there are new messages, the oBuffer object will be filled with the first message and the message
		copy owned by the lower level will be discarded (a message is not delivered twice).
		\param oBuffer object where (possible) received message will be stocked
		\param iStream object where will be filled the identifier of the stream of a (possible) received message.
		\return true if there were messages in the lower layer (so oBuffer contains a copy of one valid message
		and iStream contains the identifier of its stream).
		*/
		virtual const bool ReceiveFromServerAnyStream(buffer_c& oBuffer, int& iStream) = 0; //Not implemented

		/// Receive own message
		virtual const bool ReceiveOwn(buffer_c& oBuffer, const int iStream) = 0;
		/// Receive own message
		virtual const bool ReceiveOwnAnyStream(buffer_c& oBuffer, int& iStream) = 0;

		/*! \brief This service verifies if there are new received messages from a specific peer that are not
		already delivered. It will check avaliable messages in the stream specified by the iStream identifier.
		If there are new messages, the oBuffer object will be filled with the first message
		and the message copy owned by the lower level will be discarded (a message is not delivered twice).
		\param oBuffer object where (possible) received message will be stocked
		\param id identifier of the peer to be listed
		\param iStream identifier of the stream to be listed.
		\return true if there were messages in the lower layer (so oBuffer contains a copy of one valid message).
		*/
		virtual const bool ReceiveFromPeer(buffer_c& oBuffer, const TClientID id, const int iStream) = 0;

		/*! \brief This service verifies if there are new received messages from a specific peer that are not
		already delivered. It will check avaliable messages in all the avaliables streams of this specified peer
		in the lower level. If there are new messages, the oBuffer object will be filled with the first message
		and the message copy owned by the lower level will be discarded (a message is not delivered twice).
		\param oBuffer object where (possible) received message will be stocked
		\param id identifier of the peer to be listed
		\param iStream object where will be filled the identifier of the stream of a (possible) received message.
		\return true if there were messages in the lower layer (so oBuffer contains a copy of one valid message
		and iStream contains the identifier of its stream).
		*/
		virtual const bool ReceiveFromPeerAnyStream(buffer_c& oBuffer, const TClientID id, int& iStream) = 0;

		/*! \brief This service verifies if there are new received messages from any peer that are not
		already delivered. It will check avaliable messages in the specified stream by th iStream identifier of
		all connected peers in the lower level. If there are new messages, the oBuffer object will be filled
		with the first message and the message copy owned by the lower level will be discarded (a message is
		not delivered twice).
		\param oBuffer object where (possible) received message will be stocked
		\param id object where will be filled the identifier of the sender of the (possible) received message.
		\param iStream identifier of the stream to be listed.
		\return true if there were messages in the lower layer (so oBuffer contains a copy of one valid message
		and id contains the identifier of the message sender).
		*/
		virtual const bool ReceiveFromAnyPeer(buffer_c& oBuffer, TClientID& id, const int iStream) = 0;

		/*! \brief This service verifies if there are new received messages from any peer that are not
		already delivered. It will check avaliable messages in all the avaliables streams of all connected peers
		in the lower level. If there are new messages, the oBuffer object will be filled with the first message
		and the message copy owned by the lower level will be discarded (a message is not delivered twice).
		\param oBuffer object where (possible) received message will be stocked
		\param id object where will be filled the identifier of the sender of the (possible) received message.
		\param iStream object where will be filled the identifier of the stream of a (possible) received message.
		\return true if there were messages in the lower layer (so oBuffer contains a copy of one valid message,
		id contains the identifier of the message sender and iStream contains the identifier of its stream).
		*/
		virtual const bool ReceiveFromAnyPeerAnyStream(buffer_c& oBuffer, TClientID& id, int& iStream) = 0; //Not implemented

		/*! \brief Utility service to verify if there is a stream associated with an specific stream identifier.
		\param iStream the stream identifier to be tested
		\return true if there is a stream associated with the stream identifier
		*/
		virtual const bool IsValidStream(int iStream) = 0; //Not implemented

		/*! \brief Utility service to get the address of the server where the client is connected.
		If the server is not yet connected with a server, it returns false.
		\param oAddress the object where this service will grab the server address.
		\return true if client is connected (and so the object oAddress contains the server address).
		*/
		virtual const bool GetServerAddress(address_c& oAddress) = 0;

		/*! \brief Utility service to set the console class charged to log messages in P2PSE layer 0 and lower levels in the client-side.
		\param oConsole an console object that will be used to log messages.
		*/
		virtual void SetDebuggingConsole(console_c& oConsole) = 0;

		/*! \brief Utility method to verify if a specified peer is present in the client's current group
		\param id the unique session identifier of the peer to be tested
		\return true if the specified peer is present at the client's current group
		*/
		virtual const bool IsClientPresent(const TClientID id) = 0;

    
		/*! \brief Utility service to verify if there is some scheduled message to be sent to server or to the possibly connected peers
		\return true if there is some scheduled message to be sent (some useful packet will be sent if we
		call the Dispatch service)
		*/
		virtual const bool HasSomethingToSend() = 0;

		/*! \brief Utility service to get an estimative of the packet loss from the server to the client
		\return an estimative of probability of packet loss from the server to the client (0.0 means "no packet loss", 1.0 means "all packets were lost")
		*/
		virtual const double GetServerLossEstimative() const = 0;

		/*! \brief Utility service to get an estimative of the packet loss between the client and a specific peer
		\param id the unique identifier of the peer we are interested to estimate the packet loss
		\return an estimative of probability of packet loss from the specified remote peer (0.0 means "no packet loss", 1.0 means "all packets were lost")
		1.0 means "all packets were lost")
		*/
		virtual const double GetRemotePeerLossEstimative(const TClientID iClientId) /*const*/ = 0;

		/*! \brief Utility service to get an estimative of how packets arrive "out of order" from the server
		\return an estimative of how packets arrive out of order from the server (0.0 means "all packets arrive properly (e. g., 1, 2, 3, etc)",
		1.0 means "all packets arrive in backwards (e.g. 10, 9, 8, etc)". This measure is proportional to the standard deviation of the packet order
		*/
		virtual const double GetServerOutOfOrderEstimative() const = 0;

		/*! \brief Utility service to get an estimative of how packets arrive "out of order" from the server
		\param id the unique identifier of the peer we are interested to estimate the out-of-order factor
		\return an estimative of how packets arrive out of order from the server (0.0 means "all packets arrive properly (e. g., 1, 2, 3, etc)",
		1.0 means "all packets arrive in backwards (e.g. 10, 9, 8, etc)". This measure is proportional to the standard deviation of the packet order
		*/
		virtual const double GetRemotePeerOutOfOrderEstimative(const TClientID iClientId) /*const*/ = 0;

		/*! \brief Static service that creates a 'real' client object and returns the facade to higher level caller.
 		\param oListener an interface where layer 0 client-side will inform events to the next layer.
		\return the service facade of the 'real' client object.
		*/
		static CClientInterface* GetNewClient(CClientEventInterface& oListener);
	};
}

