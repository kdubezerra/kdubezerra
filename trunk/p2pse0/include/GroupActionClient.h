#pragma once

#include "GroupAbstract.h"
#include "RemotePeer.h"

namespace P2PSEL0
{
	class CGroupActionClient: public CGroupAbstract< CRemotePeer >
	{
	public:
		/// Default constructor
		CGroupActionClient(): CGroupAbstract<CRemotePeer>::CGroupAbstract() {}
		/// Constructor
		CGroupActionClient(TGroupID id, station_c& oServer, CClientEventInterface& oClientEventInteface): CGroupAbstract<CRemotePeer>::CGroupAbstract(id, 0, oServer), m_poClientEventInterface(&oClientEventInteface) {}
		/// Chat message
		virtual const bool Chat(const string& oMessage);
		/// Send group
		virtual const bool SendMessage(buffer_c& oData, const int iStream, const bool bReliable = true);

	private:
		/// Callback Interface
		CClientEventInterface* m_poClientEventInterface;
	};
}

