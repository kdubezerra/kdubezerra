#include "ServerListenerTest.h"
#include "ServerInterface.h"
//#include "Server.h"
#include "ServerEventInterface.h"
//#include "Client.h"
#include "ClientListenerTest.h"
#include "ClientInterface.h"
#include "ClientEventInterface.h"
#include "fake_console.h"

#include "zig/utils.h"
#include "zig/zig.h"

#include <vector>
#include <iostream>
using std::vector;
using namespace P2PSEL0;

const unsigned long nPeers = 4;
fake_console_c oConsole(&std::cerr);

const bool Tick(const float fTime)
{
  //LARGE_INTEGER stTime, stFrequency;
  //QueryPerformanceFrequency(&stFrequency);
  //QueryPerformanceCounter(&stTime);
  //const double get_time_crapped = (stTime.QuadPart/(double)stFrequency.QuadPart);

  static double fLate = get_time();//_crapped;
  const double fNow = get_time();//_crapped;
  const double fDiff = fNow - fLate;
  if (fDiff >= fTime)
  {
    fLate = fNow + (fDiff - fTime);
    return true;
  }
  return false;
}

void NetworkLoop(CServerInterface& oPublisher, vector< CClientInterface* > oPeerLocal)
{
  for (long i = 0; i < (long) oPeerLocal.size(); i++)
  {
		/*
	if (oPeerLocal[i]->HasSomethingToSend()) {
		oConsole.printf("peer %d vai mandar pacote\n", oPeerLocal[i]->GetID());
		*/
		oPeerLocal[i]->SendPacket();
	//}
	oPeerLocal[i]->ProcessNonBlocking();
  }
  //if (oPublisher.HasSomethingToSend()) {
	//oConsole.printf("server vai mandar pacote\n");
	oPublisher.Dispatch();
  //}
  oPublisher.ProcessNonBlocking();
  SLEEP(0);
}

void StreamCheck(CServerInterface& oPublisher, vector< CClientInterface* > oPeerLocal)
{
  const unsigned long iMessageSize = 64 * 64;
  const unsigned long iUseStreamCount = 1;
  vector< long > oMessage[iUseStreamCount];
  vector< long > oReceivedMessage[nPeers][iUseStreamCount];
  for (unsigned long i = 0; i < iUseStreamCount; i++)
  {
    oMessage[i].reserve(iMessageSize);
    for (unsigned long j = 0; j < iMessageSize; j++)
      oMessage[i].push_back(rand());
  }

  NetworkLoop(oPublisher, oPeerLocal);

  unsigned long iSend[iUseStreamCount];
  memset(&iSend, 0, sizeof(long) * iUseStreamCount);
  while (true)
  {
    if (Tick(0.01f))
    {
      if (iSend[0] < iMessageSize)
      {
        buffer_c oSend;
        for (unsigned long j = 0; j < iUseStreamCount; j++)
        {
          for (unsigned long i = 0; i < nPeers; i++)
          {
            oSend.clear();
            oSend << oMessage[j][iSend[j]];
            oPeerLocal[i]->SendMessageToServer(oSend, j, true);
            {
              oConsole.xprintf(3, "%d(%d): send %d\n", i, j, oMessage[j][iSend[j]]);
            }
          }
          iSend[j]++;
        }
      }
    }
    NetworkLoop(oPublisher, oPeerLocal);
    for (unsigned long i = 0; i < nPeers; i++)
    {
      for (unsigned long j = 0; j < iUseStreamCount; j++)
      {
        buffer_c oReceive;
        if (oPublisher.ReceiveMessageFromClient(oReceive, oPeerLocal[i]->GetID(), j))
        {
          long iTemp;
          oReceive >> iTemp;
          oReceivedMessage[i][j].push_back(iTemp);
          if (oReceivedMessage[i][j].back() != oMessage[j][oReceivedMessage[i][j].size() - 1])
          {
            oConsole.xprintf(3, "Expected %d, Received %d\n", oMessage[j][oReceivedMessage[i][j].size() - 1], oReceivedMessage[i][j].back());
          }
          else
          {
            oConsole.xprintf(3, "Ok %d\n", oReceivedMessage[i][j].back());
          }
        }
      }
    }
    if (oReceivedMessage[0][iUseStreamCount - 1].size() == iMessageSize)
      break;
  }
}

void PrintGroupStatus(CServerInterface& oPublisher)
{
  oConsole.xprintf(3, "---------Group Status--------\n");
  char c[1024];
  for (int i = 0; i < oPublisher.GetNumberOfGroups(); i++)
  {
    const int gid = oPublisher.GetGroupIDByIndex(i);
    sprintf(c, "Group ID: %d\n", gid);
    oConsole.xprintf(3, c);
    for (unsigned int j = 0; j < oPublisher.GetNumberOfClients(gid); j++)
    {
      const int id = oPublisher.GetClientByIndex(gid, j);
      sprintf(c, "id: %d\n", id);
      oConsole.xprintf(3, c);
    }
  }
}

void GroupCheck(CServerInterface& oPublisher, vector< CClientInterface* > oPeerLocal)
{
  for (unsigned int i = 0; i < oPeerLocal.size(); i++)
  {
    oPeerLocal[i]->SetPacketLatencyEmulation(0.1f, 0.5f);
    oPeerLocal[i]->SetPacketLossEmulation(0.005f);
  }

#ifdef _DEBUG
  const float fMult = 5.0f;
#else
  const float fMult = 4.0f;
#endif
  //IGNORED!
  while (!Tick(2.0f * fMult)) //Connection delay
    NetworkLoop(oPublisher, oPeerLocal);

  PrintGroupStatus(oPublisher);

  TGroupID gid = oPublisher.CreateActionGroup(100);
  //oPeerLocal[0].

  PrintGroupStatus(oPublisher);

  for (unsigned long i = 0; i < nPeers; i++)
  {
    oPublisher.MoveToGroup(oPeerLocal[i]->GetID(), gid);
  }
  CClientInterface* poTemp = oPeerLocal.back();
  //oPeerLocal.back()->SetPacketLossEmulation(1);
  //oPeerLocal.pop_back();

  while (!Tick(2.0))
    NetworkLoop(oPublisher, oPeerLocal);

  PrintGroupStatus(oPublisher);

  for (unsigned long i = 0; i < oPeerLocal.size(); i++)
  {
    const long nConnectedPeers = oPeerLocal[i]->GetNumberOfRemotePeers();
    oConsole.xprintf(3, "Connections = %d, GroupID %d, ClientID %d\n", nConnectedPeers, oPeerLocal[i]->GetGroupID(), oPeerLocal[i]->GetID());
  }

	/*
  oPeerLocal.back()->SetPacketLossEmulation(0);
  oPeerLocal.push_back(poTemp);

  while (!Tick(12.5))
    NetworkLoop(oPublisher, oPeerLocal);

  PrintGroupStatus(oPublisher);

  for (unsigned long i = 0; i < oPeerLocal.size(); i++)
  {
    const long nConnectedPeers = oPeerLocal[i]->GetNumberOfRemotePeers();
    oConsole.xprintf(3, "Connections = %d, GroupID %d, ClientID %d\n", nConnectedPeers, oPeerLocal[i]->GetGroupID(), oPeerLocal[i]->GetID());
  }
	*/

/*
  buffer_c oBuffer;
  oBuffer << "CRAP!";
  oPeerLocal[0]->SendBroadcastMessage(oBuffer, 0, false);

  while (!Tick(0.5))
    NetworkLoop(oPublisher, oPeerLocal);

  for (unsigned long i = 0; i < nPeers; i++)
  {
    oPublisher.MoveToGroup(oPeerLocal[i]->GetID(), oPublisher.GetDefaultSocialGroupID());
  }

  while (!Tick(0.5))
    NetworkLoop(oPublisher, oPeerLocal);

  PrintGroupStatus(oPublisher);

  for (unsigned long i = 0; i < nPeers; i++)
  {
    const long nConnectedPeers = oPeerLocal[i]->GetNumberOfRemotePeers();
    oConsole.xprintf(3, "Connections = %d, GroupID %d, ClientID %d\n", nConnectedPeers, oPeerLocal[i]->GetGroupID(), oPeerLocal[i]->GetID());
  }

  while (!Tick(0.5))
    NetworkLoop(oPublisher, oPeerLocal);

  for (unsigned long i = 0; i < nPeers; i++)
  {
    oConsole.printf("Move client %d = %d\n", oPeerLocal[i]->GetID(), oPublisher.MoveToGroup(oPeerLocal[i]->GetID(), gid) == true ? 1 : 0);
  }

  while (!Tick(0.5))
    NetworkLoop(oPublisher, oPeerLocal);

  PrintGroupStatus(oPublisher);

  for (unsigned long i = 0; i < nPeers; i++)
  {
    const long nConnectedPeers = oPeerLocal[i]->GetNumberOfRemotePeers();
    oConsole.xprintf(3, "Connections = %d, GroupID %d, ClientID %d\n", nConnectedPeers, oPeerLocal[i]->GetGroupID(), oPeerLocal[i]->GetID());
  }


  while (!Tick(5.0))
    NetworkLoop(oPublisher, oPeerLocal);

  for (unsigned long i = 0; i < nPeers; i++)
  {
    oConsole.printf("Move client %d = %d\n", oPeerLocal[i]->GetID(), oPublisher.MoveToGroup(oPeerLocal[i]->GetID(), gid) == true ? 1 : 0);
  }

  while (!Tick(0.5))
    NetworkLoop(oPublisher, oPeerLocal);

  PrintGroupStatus(oPublisher);

  for (unsigned long i = 0; i < nPeers; i++)
  {
    const long nConnectedPeers = oPeerLocal[i]->GetNumberOfRemotePeers();
    oConsole.xprintf(3, "Connections = %d, GroupID %d, ClientID %d\n", nConnectedPeers, oPeerLocal[i]->GetGroupID(), oPeerLocal[i]->GetID());
  }
*/
  while (!Tick(5.0))
    NetworkLoop(oPublisher, oPeerLocal);

  PrintGroupStatus(oPublisher);

  for (unsigned long i = 0; i < oPeerLocal.size(); i++)
  {
    const long nConnectedPeers = oPeerLocal[i]->GetNumberOfRemotePeers();
    oConsole.xprintf(3, "Connections = %d, GroupID %d, ClientID %d\n", nConnectedPeers, oPeerLocal[i]->GetGroupID(), oPeerLocal[i]->GetID());
  }

  /*
  while (!Tick(2.0f * fMult))
    NetworkLoop(oPublisher, oPeerLocal, nPeers);

  buffer_c oSend;
  oSend << "My Crap!";
  oPeerLocal[3].SendBroadcastMessage(oSend, 10, true);

  while (!Tick(2.0f * fMult))
    NetworkLoop(oPublisher, oPeerLocal, nPeers);

  buffer_c oReceive;
  for (unsigned long i = 0; i < nPeers; i++)
  {
    if (oPeerLocal[i].ReceiveFromPeer(oReceive, oPeerLocal[i].GetID(), 10))
    {
      string oData;
      oReceive >> oData;
      oConsole.xprintf(3, "Peer %d, message: %s\n", i, oData.c_str());
    }
    else
    {
      oConsole.xprintf(3, "Peer %d, no messages\n", i);
    }
  }
  */
}

void ChatTest(CServerInterface& oPublisher, vector< CClientInterface* > oPeerLocal)
{
  for (unsigned long i = 0; i < nPeers; i++)
  {
    oPublisher.MoveToGroup(oPeerLocal[i]->GetID(), oPublisher.GetDefaultSocialGroupID());
  }

  while (!Tick(2.0))
    NetworkLoop(oPublisher, oPeerLocal);

  oPeerLocal[0]->Chat("Ei, Inter/Vai toma no cu!");
  oConsole.printf("Enviei mensagem pro server: \"Ei, Inter/Vai toma no cu!\"\n");

  while (!Tick(2.0))
    NetworkLoop(oPublisher, oPeerLocal);

  TGroupID gid = oPublisher.CreateActionGroup(100);
  for (unsigned long i = 0; i < nPeers; i++)
  {
    oPublisher.MoveToGroup(oPeerLocal[i]->GetID(), gid);
  }

  while (!Tick(2.0))
    NetworkLoop(oPublisher, oPeerLocal);

  oPeerLocal[0]->Chat("Ei, Inter/Vai toma no cu!");
  oConsole.printf("Estou num grupo de acao, enviei mensagem diretamente pros peers: \"Ei, Inter/Vai toma no cu!\"\n");

  while (!Tick(2.0))
    NetworkLoop(oPublisher, oPeerLocal);
}

void CrazyTest(CServerInterface& oPublisher, vector< CClientInterface* > oPeerLocal)
{
  for (unsigned int i = 0; i < oPeerLocal.size(); i++)
  {
    oPeerLocal[i]->SetPacketLatencyEmulation(0.5f, 1.5f);
    oPeerLocal[i]->SetPacketLossEmulation(0.1f);
  }

  const int nGroup = 20;
  vector < TGroupID > oGroup;
  for (int i = 0; i < nGroup; i++)
  {
    switch (rand() % 3)
    {
    case 0:
    case 1:
      oGroup.push_back(oPublisher.CreateActionGroup(100));
      break;
    case 2:
      oGroup.push_back(oPublisher.CreateSocialGroup(100));
      break;
    }
  }

  for (int i = 0; i < 100; i++)
  {
    int nPeerOperation = rand() % nPeers;
    for (int j = 0; j < nPeerOperation; j++)
    {
      oPublisher.MoveToGroup(oPeerLocal[(rand() % nPeers)]->GetID(), oGroup[rand() % nGroup]);
    }
    
    while (!Tick(1.5))
      NetworkLoop(oPublisher, oPeerLocal);

    PrintGroupStatus(oPublisher);
  }

  while (!Tick(30.0))
    NetworkLoop(oPublisher, oPeerLocal);

  PrintGroupStatus(oPublisher);
}

int main()
{
  zig_init();

  srand(0);
  oConsole.set_xprintf_level(1);
  CServerListenerTest oServerListener;
  CClientListenerTest oClientListener[nPeers];

  vector< CClientInterface* > oClientList;

  {
    CServerInterface* poPublisher = CServerInterface::GetNewServer(oServerListener);
    for (unsigned int i = 0; i < nPeers; i++)
    {
      CClientInterface* poClient = CClientInterface::GetNewClient(oClientListener[i]);
	oClientListener[i].SetClient0(poClient);
      oClientList.push_back(poClient);
    }
    poPublisher->SetDebuggingConsole(oConsole);
    poPublisher->Start(1400, 1400);
    for (unsigned long i = 0; i < nPeers; i++)
    {
      address_c oAddress("127.0.0.1:1400");
      oClientList[i]->SetDebuggingConsole(oConsole);
      oClientList[i]->Start(1024 + i, 9999 + i);
      const bool bConnected = oClientList[i]->Connect(oAddress);
	  
	  oConsole.printf("Connect() result = %d\n", bConnected? 1 : 0);
	  
      while (!Tick(1.0f))
        NetworkLoop(*poPublisher, oClientList);

      if (oClientList[i]->IsConnected())
      {
        oConsole.xprintf(3, "Connected\n");
      }
      else
        oConsole.xprintf(3, "Failed\n");
      assert(bConnected);
    }

	//ChatTest(*poPublisher, oClientList);
	for (long i = 0; i < 20; i++)
	{
		for (long i = 0; i < nPeers; i++)
		{
			oClientList[i]->SetPacketLossEmulation((rand() % 80) / 100.0);
			oClientList[i]->SetPacketLatencyEmulation(0.1, (rand() % 200) / 100.0);
		}
		GroupCheck(*poPublisher, oClientList);
	}
//	CrazyTest(*poPublisher, oClientList);

	char cTemp[128];
	printf("Terminou!!!\n");
	sscanf("%d\n",cTemp);
  }
  //int a;
  //std::cin >> a;
}

