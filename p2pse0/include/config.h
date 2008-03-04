#pragma once

/// Forward declaration
class buffer_c;
class policy_c;
class address_c;
class console_c;

/// Type identifier for groups
typedef unsigned short TGroupID;
/// Type identifier for clients
typedef unsigned short TClientID;

/// ID Inválido
#define INVALID_ID 0

namespace P2PSEL0
{
	class CGlobalInformation
	{
	public:
		static int m_iChatStream;
		static int m_iGroupEventProtocolStream;
		static double m_dTimeoutValue;
		static double m_dGroupTimeoutValue;
		static double m_dBannedTimeout;
	};

	enum EGroupType
	{
		/// Social group
		GROUP_SOCIAL_TYPE = 0,
		/// Social group
		GROUP_LIMBO_TYPE = 1,
		/// Action group
		GROUP_ACTION_TYPE = 2 
	};

	EGroupType GetGroupType(TGroupID id);

	enum
	{
		///Peer has joined group
		///group id
		///peer id
		PROTOCOL_EVENT_PEER_JOINED, 
		///Peer has left group
		///group id
		///peer id
		PROTOCOL_EVENT_PEER_LEFT, 
		///Send message to sync all peers in group when someone want to join or left the group
		///Message contains group id and list of peers ids in group (group id needed?)
		PROTOCOL_MAN_GROUP_SYNC, 
		///peers inside group and connecting peers inform that are connected through station (happy)
		///client id
		PROTOCOL_MAN_GROUP_CONNECTED 
	};
}

