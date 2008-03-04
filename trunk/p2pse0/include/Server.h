#pragma once

#if !defined (__P2PSE0_COMPILATION__)
#error "Bad, bad BigNose, no donut for you. You cannot include directly \"Server.h\""
#endif

// ZIG include files.
#include <zig/station.h>
#include <zig/msocket.h>
#include <zig/buffer.h>
#include <zig/leetnet.h>
//#include <zig/typereg.h>
#include <zig/address.h>
#include "idgen.h"
#include "config.h"
#include <map>
#include "GroupAbstract.h"
#include "RemoteClient.h"
#include "ServerEventInterface.h"
#include "ServerInterface.h"

namespace P2PSEL0
{
	class CServer: private msocket_callback_i, public CServerInterface
	{
		/// Private callbacks implementation
		friend class msocket_c;

		/// Hack against BigNose ;)
		friend class CServerInterface;

	private:
		/// Constructor
		CServer(CServerEventInterface& oEventInterface);
		/// Destructor
		~CServer();

	public:
		
		/*! \brief This service makes the server ready to accept client connections
		Actually, this service calls the correspondent service in the lower level, that will try to open an
		UDP communication port to send and receive packets.
		The parameters determine the ports interval where the lower layer will try to bind an opened socket.
		This service will fail if it was not possibly to the lower level to bind the socket in any port in the range
		\param startPort the minimum port value where the lower level can bind the socket
		\param endPort the maximum port value where the lower level can bind the socket
		\return true if underlying level could open an communication point (an UDP port)
		*/
		const bool Start(const int iMinPort, const int iMaxPort);

		/*! \brief This service closes the network communication capabilities of server in the lower level,
		that will closes the UDP socket. If the DisconnectClient service was not called before this service invocation for ALL connected clients,
		then this service will first Disconnect all connected clients before its execution.
		This service will fail if it was not possibly to the lower level to correctly finish the network.
		\return true if underlying level could closes the communication point
		*/
		const bool Stop();

		/*! \brief This service will create an group without peer-to-peer connectivity
		(all clients inside this group are only connected with server, and all broadcast/unicast messages will be relayed in the server to achieve the target(s)
		\param nMaxPeer not utilized
		\return the unique identifier of the created group, or INVALID_ID in case of failure
		*/
		const TGroupID CreateSocialGroup(const int nMaxPeer);

		/*! \brief This service will create an group with peer-to-peer connectivity
		(all clients inside this group must be directly connect with ALL other clients
		at the group).
		\param nMaxPeer not utilized
		\return the unique identifier of the created group, or INVALID_ID in case of failure
		*/
		const TGroupID CreateActionGroup(const int nMaxPeer);

		/*! \brief This service will destroy a group and move all its components to the default
		social group (it cannot be destroyed!). There is also other special group that cannot be
		destroyed, internally called "Limbo", that is the place where clients are placed during
		group transitions.
		\param gid the unique identifier of the group to be destroyed
		\return true if there was a group associated if the specified group identifier and
		if it was successfully destroyed
		*/
		const bool DestroyGroup(const TGroupID gid);

		/*! \brief This service change a client from a group to another one. 
		The changement protocol is ASSYNCHRONOUS, it means, the next layer that calls this service CANNOT infers
		that the client was successfully connected after this method return. The caller layer is informed about
		the success or failure of the changement protocol through the callbacks that it must implement of the
		ServerEventInterface.
		\param id the session unique identifier of the client we want to move to another group
		\param gid the unique identifier of the group we want to move the specified client
		\return true if client and target group specified in the parameters are valid groups
		*/ 
		const bool MoveToGroup(const TClientID id, const TGroupID gid);

		/*! \brief Utility service to get the current number of existent groups
		\return the current number of groups in the server
		*/
		const int GetNumberOfGroups();
		
		/*! \brief Utility service to get the group identifier of an specific group. To use this method,
		it is needed to call before the service GetNumberOfGroups, to discover the maximum value of iIndex.
		\param iIndex index of the queried group ( between the interval [0, GetNumberOfGroups()[ )
		\return the group identifier of the queried group
		*/
		const TGroupID GetGroupIDByIndex(const unsigned int iIndex);

		/*! \brief This service disconnect a client from the server. 
		If the current group of this client is an action group, this call also disconnect all remote peers from it.
		The disconnection protocol is SYNCHRONOUS, it means, after the method return the caller of this service
		can infers that the client is disconnected from server and (possible) remote peers.
		\return true if no error in lower level disconnection protocol appears
		*/ 
		const bool DisconnectClient(const TClientID id);

		/// Get number of clients
		const unsigned int GetNumberOfClients(const TGroupID gid);

		/// Get max number of clients in the specified group
		const unsigned int GetMaxNumberOfClients(const TGroupID gid);

		/// Get client id by index
		const TClientID GetClientByIndex(const TGroupID gid, const unsigned int iIndex);

		//verifica se existe cliente com o id do parâmetro
		const bool HasClient(const TClientID clientId);

		//verifica se existe grupo com o id do parâmetro
		const bool HasGroup(const TGroupID& groupId);

		/// Send broadcast
		const bool SendBroadcast(const buffer_c& oBuffer, const TGroupID gid, const int iStream, const bool bReliable);
		/// Send unicast
		const bool SendUnicast(const buffer_c& oBuffer, const TClientID id, const int iStream, const bool bReliable);
		
		/// Receive any message
		const bool ReceiveMessageAnyClientAnyStream(buffer_c& oBuffer, TClientID& id, int& iStream);
		/// Receive message
		const bool ReceiveMessageAnyClient(buffer_c& oBuffer, TClientID& id, const int iStream);
		/// Receive from client any stream
		const bool ReceiveMessageFromClientAnyStream(buffer_c& oBuffer, const TClientID id, int& iStream);
		/// Receive from client any stream
		const bool ReceiveMessageFromClient(buffer_c& oBuffer, const TClientID id, const int iStream);


		/*! \brief This service verifies if there are new received messages from any peer from a specific group
		that are not already delivered. It will check avaliable messages in all avaliable streams of all clients
		at the group. If there are new messages, the oBuffer object will be filled with the first message and the
		message copy owned by the lower level will be discarded (a message is not delivered twice).
		\param oBuffer object where (possible) received message will be stocked
		\param gid unique identifier of the group to be listed.
		\param id object where will be filled the identifier of the sender of the (possible) received message.
		\param iStream object where will be filled the identifier of the stream of a (possible) received message.
		\return true if there were messages in the lower layer (so oBuffer contains a copy of one valid message,
		id contains the identifier of the message sender and iStream contains the identifier of its stream).
		*/
		const bool ReceiveMessageFromGroupAnyStream(buffer_c& oBuffer, const TGroupID gid, TClientID& id, int& iStream);

		/*! \brief This service verifies if there are new received messages from any peer from a specific group
		that are not already delivered. It will check avaliable messages in the specified stream by th iStream
		identifier of all clients at the group. If there are new messages, the oBuffer object will be filled
		with the first message and the message copy owned by the lower level will be discarded (a message is not
		delivered twice).
		\param oBuffer object where (possible) received message will be stocked
		\param gid unique identifier of the group to be listed.
		\param id object where will be filled the identifier of the sender of the (possible) received message.
		\param iStream object where will be filled the identifier of the stream of a (possible) received message.
		\param iStream identifier of the stream to be listed.
		\return true if there were messages in the lower layer (so oBuffer contains a copy of one valid message
		and id contains the identifier of the message sender).
		*/
		const bool ReceiveMessageFromGroup(buffer_c& oBuffer, const TGroupID gid, TClientID& id, const int iStream);

		/*! \brief Utility service to verify if there is some scheduled message to be sent to some connected client
		\return true if there is some scheduled message to be sent (some useful packet will be sent if we
		call the Dispatch service)
		*/
		const bool HasSomethingToSend();

		/*! \brief This service must be called periodically in order to the lower level sends packets
		to network. It sends packets to all connected clients
		\return true if no error in lower level SendPacket services.
		*/
		const bool Dispatch();

		/*! \brief This service must be called periodically in order to the lower level receives packets
		from network
		\return true if no error in lower level ProcessNonBlocking service.
		*/
		void ProcessNonBlocking();

		/*! \brief Utility service to return the current group of a specific client
		\param id the unique identifier of the client we want to know the current group.
		\return the unique identifier of the current group where the specified client is.
		*/
		const TGroupID GetClientGroupID(const TClientID id);

		/*! \brief This service creates a communication channel ('stream') to a specific client
		\param id the unique identifier of the client which we want to create a communication channel
		\param iType the type of this channel (STREAM_INDIVIDUAL_ACK, STREAM_CUMULATIVE_ACK, STREAM_NO_ACK).
		They offers different services in relation with order and reliability. If you do not know what we must
		set here, try STREAM_INDIVIDUAL_ACK, that guarantees order and reliability with "medium" traffic.
		See zig.h for more details.
		\param oPolicy the send policy of this channel. If you do not know what you must set here, try DefaultPolicy.
		See station.h for more details.
		\return the unique identifier of the created stream
		*/
		const int CreateStream(const TClientID id, const int iType, const policy_c& oPolicy);

		/*! \brief Utility service to set the server console and also the lower level console
		\param oConsole the console to be set to the server
		*/
		void SetDebuggingConsole(console_c& oConsole);

		console_c* GetConsole() const { return m_poConsole; }

		/*! \brief Utility service to get the identifier of a 'special' social group, that is the first group of all clients when they connect to the server, and also is the group where clients are moved when a group is destroyed/a changement of group fails
		\return the unique identifier of the default social group
		*/
		const TGroupID GetDefaultSocialGroupID();

	private:
		/// Create Group
		template<class T>
			const TGroupID CreateGroup(const int nMaxPeer);

		/// Get Group
		CGroupAbstract< CRemoteClient >* GetGroup(const TGroupID id);

		/// Get client group
		CGroupAbstract< CRemoteClient >* GetClientGroup(const TClientID id);

		/// Socket callbacks
		void msocket_incoming_unreg(address_c &addr, const buffer_c &pkt) {}

		void msocket_read_error(NLsocket &sock) {}

		bool msocket_accept_station(address_c &addr, const buffer_c& connection_data, buffer_c& answer);

		void msocket_station_connected(station_c &st, const buffer_c& oConnectionData);

		void msocket_closed() {}

		///Group Id Social Generator
		CIdGenerator< TGroupID > m_oGroupIDSocialGenerator;
		///Group Id Action Generator
		CIdGenerator< TGroupID > m_oGroupIDActionGenerator;
		///Client Id Generator
		CIdGenerator< TClientID > m_oClientIDGenerator;
		///Map id to group
		std::map< TGroupID, CGroupAbstract< CRemoteClient >* > m_oGroupMap;
		///Social Group
		TGroupID m_iSocialGroup;
		///Limbo Group
		TGroupID m_iLimboGroup;
		/// Event interface
		CServerEventInterface* m_poInterface;

		///Socket
		msocket_c m_oSocket;

		///Console
		console_c* m_poConsole;
	};
}

