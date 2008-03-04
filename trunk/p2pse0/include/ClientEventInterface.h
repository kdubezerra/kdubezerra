/*! \file ClientEventInterface.h
 */

#pragma once

#include "config.h"
#include "zig/buffer.h"

/*! \brief All classes in P2PSE layer 0 are scoped by P2PSEL0 namespace. This is very useful
to avoid name collision between different layers.
*/
namespace P2PSEL0
{
	/*! \brief This interface is the access point to client-side callbacks generated by P2PSE layer 0.
	It groups as many group related as client-server related callbacks.
	The next layer must implement all the below callbacks in order to be told about all events of layer 0
	*/
	class CClientEventInterface
	{
	public:

		/*! \brief Virtual destrutor (unuseful) */
		virtual ~CClientEventInterface() {}

		/*! \brief Callback that signalizes that the client has properly connect to the server.
		\param id the unique session identifier assigned to this client by the server.
		\param oMessage message sent by the server in the connection acknowledgment
		*/
		virtual void Connected(const TClientID id, buffer_c& oMessage) = 0;

		/*! \brief Callback that signalizes that the connection betwwen client and server has finished
		(and also the possible peers connections)
		\param oMessage if disconnection was initiated by the server side, the reason sent by the server to the disconnection
		*/
		virtual void Disconnected(buffer_c& oMessage) = 0;

		/*! \brief Callback that signalizes that the client has successfully joined a group
		\param gid the identifier of the group where the client has joined
		*/
		virtual void GroupJoined(const TGroupID gid) = 0;

		/*! \brief Callback that signalizes that the client has left a group
		\param gid the identifier of the group where the client has left
		*/
		virtual void GroupLeft(const TGroupID gid) = 0;

		/*! \brief Callback that signalizes that one client has joined the current group of the client
		\param id the unique session identifier of the client that has joined the client group
		*/
		virtual void PeerJoined(const TClientID id) = 0;

		/*! \brief Callback that signalizes that one peer has joined the current group of the client
		\param id the unique session identifier of the peer that has joined the client group
		*/
		virtual void PeerLeft(const TClientID id) = 0;

		/*! \brief Callback that signalizes that an attempt to connect to the server has failed (due to timeout or other factor)
		\param oMessage the possible reason to the connection attempt failure, sent by the server (this buffer is empty in case of timeout)
		*/
		virtual void ConnectionFailed(buffer_c& oMessage) = 0;

		/*! \brief Callback that signalizes that a packet with data has arrived from a connected peer
		\param id identifier of the peer that has sent data
		\param oUnreliable buffer with the unreliable block sent in the received packet (layer 1 must call the ReceiveMessage... services to get messages sent inside streams)
		*/
		virtual void IncomingPeerData(const TClientID id, buffer_c& oUnreliable) = 0;

		/*! \brief Callback that signalizes that a packet with data has arrived from the server
		\param oUnreliable buffer with the unreliable block sent in the received packet (layer 1 must call the ReceiveMessage... services to get messages sent inside streams)
		*/
		virtual void IncomingServerData(buffer_c& oUnreliable) = 0;
	};
}

