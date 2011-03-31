/*
 * Main.cpp
 *
 *  Created on: 17/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include <iostream>
#include <string>
#include <cosmmus-msg.h>
#include "../headers/testobject.h"
#include "../headers/testobjfactory.h"
#include "../headers/testserver.h"

using namespace optpaxos;
using namespace netwrapper;
using namespace std;

//#define DESIRED_LENGTH 512000
//#define MSG_LENGTH DESIRED_LENGTH-32
#define RPORT0 40000
#define UPORT0 50000

testobject* o1;
testobject* o2;
testobject* o3;

Command* newRandomCommand();

void serverloop(testserver* server, int sid, int cc);

int main(int argc, char* argv[]) {
  int sid;
  int cmdcount = -1;
  testserver* server = new testserver();
  testobjfactory* myfactory = new testobjfactory();
  Object::setObjectFactory(myfactory);
  if (!argv[1]) return 1;
  string arg = argv[1];
  o1 = new testobject(1);
  o2 = new testobject(2);
  o3 = new testobject(3);
  Object::indexObject(o1);
  Object::indexObject(o2);
  Object::indexObject(o3);
  Group* grp = new optpaxos::Group(1);
  Group::addGroup(grp);
  grp->addManagedObject(o1->getInfo());
  grp->addManagedObject(o2->getInfo());
  grp->addManagedObject(o3->getInfo());
  NodeInfo* sv1 = new NodeInfo(1, SERVER_NODE, new Address("localhost", UPORT0 + 1));
  grp->addServer(sv1);
  grp->setCoordinator(sv1);

  if (arg.compare("COOSOLO") == 0) {
    cout << ",,..::SOLO::..,, Coordinator starting..." << endl;
    srand(1);
    server->init(RPORT0 + 1, UPORT0 + 1),
    server->joinGroup(grp);
    if (argc > 2) {
      cmdcount = atoi(argv[2]);
    }
    serverloop(server, 1, cmdcount);

    cout << endl << "obj1.optstate = " << o1->optState << endl;
    cout << "obj1.constate = " << o1->conState << endl << endl;
    cout << "obj2.optstate = " << o2->optState << endl;
    cout << "obj2.constate = " << o2->conState << endl << endl;
    cout << "obj3.optstate = " << o3->optState << endl;
    cout << "obj3.constate = " << o3->conState << endl << endl;

    return 0;
  }

  NodeInfo* sv2 = new NodeInfo(2, SERVER_NODE, new Address("localhost", UPORT0 + 2));
  grp->addServer(sv2);
  NodeInfo* sv3 = new NodeInfo(3, SERVER_NODE, new Address("localhost", UPORT0 + 3));
  grp->addServer(sv3);

  if (arg.compare("COO") == 0) {
    sid = 1;
    cout << "Coordinator starting..." << endl;
    srand(1);
    server->init(RPORT0 + 1, UPORT0 + 1),
    //server->getNodeInfo()->setNodeId(1);
    server->joinGroup(grp);
  }
  else if (arg.compare("REP") == 0) {
    sid = atoi(argv[2]);
    cout << "Replica " << sid << " starting..." << endl;

    srand((unsigned) sid);
    server->init(RPORT0 + sid, UPORT0 + sid),
    //server->getNodeInfo()->setNodeId(sid);
    server->joinGroup(grp);
  }
  else {
    cout << "Wrong argument (COO or REP). Exiting." << endl;
    return 2;
  }
  if (sid == 1 && argc > 2) {
    cmdcount = atoi(argv[2]);
  }
  serverloop(server, sid, cmdcount);
  cout << endl << "obj1.optstate = " << o1->optState << endl;
  cout << "obj1.constate = " << o1->conState << endl << endl;

  cout << "obj2.optstate = " << o2->optState << endl;
  cout << "obj2.constate = " << o2->conState << endl << endl;

  cout << "obj3.optstate = " << o3->optState << endl;
  cout << "obj3.constate = " << o3->conState << endl << endl;
  /*
  FIFOReliableServer* server = new FIFOReliableServer();
  FIFOReliableClient* client = new FIFOReliableClient();

  server->init(45678);
  client->connect("localhost", 45678);
  server->checkConnections();
  Message* msg = new Message();
  char buff[MSG_LENGTH];
  msg->setArbitraryData(MSG_LENGTH, buff);
  /\*
  client->sendMessage(msg);
  client->sendMessage(msg);
  client->sendMessage(msg);
  client->sendMessage(msg);
  *\/
  client->sendMessage(msg);
  client->disconnect();
  //for (int i = 0; i < 100 ; i++)
  //while (server->checkNewMessages());
  while(server->checkNewMessages());

  /\*
  client->connect("127.0.0.1", 45678);
  server->checkConnections();
  server->checkNewMessages();
  server->checkNewMessages();
  server->send(msg, server->getClientList().front());
  while(client->checkNewMessages());
  client->disconnect();
  while(server->checkNewMessages());
  *\/

  UnreliablePeer* p1 = new UnreliablePeer();
  UnreliablePeer* p2 = new UnreliablePeer();
  p1->init(45654);
  p2->init(34543);

  Address* p2addr = new Address();
  IPaddress p2ip;
  SDLNet_ResolveHost(&p2ip, "192.168.68.134", 34543);
  p2addr->setAddress(p2ip);

  p1->sendMessage(msg, p2addr);
  for (long long l = 0 ; l < 0xFFFFFF ; l++);
  p2->checkNewMessages();

  cout << "sizeof(long long) = " << sizeof(long long) << endl;
  long long ll = 0x7fffffffffffffff;
  double d = *(reinterpret_cast<double*>(&ll));

  cout << "ll = " << ll << endl;
  cout << "d = " << d << endl;
  cout << "d = " << *(reinterpret_cast<long long*>(&d)) << endl;
  */
  return 0;
}

void serverloop(testserver* server, int sid, int cmdcount) {
  //SDL_Delay(5678);
  Command* cmd;
  long seq = 0;
  bool printed = false;
  while(true) {
    if (sid == 1 && cmdcount != 0) {
      cmd = newRandomCommand();
      if (cmd && cmdcount > 0) cmdcount--;
      if (cmd) server->sendCommand(cmd,seq++,1);
      if (cmd && !(seq % 1000)) cerr << "Command " << seq << " issued" << endl;
      if (cmd) delete cmd;
    }
    server->checkAll();
    if (cmdcount == 0 && !printed) {      
      cout << endl << "obj1.optstate = " << o1->optState << endl;
      cout << "obj1.constate = " << o1->conState << endl << endl;
      cout << "obj2.optstate = " << o2->optState << endl;
      cout << "obj2.constate = " << o2->conState << endl << endl;
      cout << "obj3.optstate = " << o3->optState << endl;
      cout << "obj3.constate = " << o3->conState << endl << endl;
      printed = true;
    }
  }
}

Command* newRandomCommand() {
  if (rand() % 2)
    return NULL;
  Command* cmd = new Command();

  cout << endl;

  if (rand() % 2) {
    cmd->addTarget(o1->getInfo());
    cout << "    obj1 is a target" << endl;
  }

  if (rand() % 2) {
    cmd->addTarget(o2->getInfo());
    cout         << "    obj2 is a target" << endl;
  }

  if (rand() % 2) {
    cmd->addTarget(o3->getInfo());
    cout         << "    obj3 is a target" << endl;
  }

  if (cmd->getTargetList().size() == 0) {
    delete cmd;
    return NULL;
  }
  cmd->setContent(new Message());
  if (rand() % 2)
    cmd->getContent()->addString("ADD");
  else
    cmd->getContent()->addString("MUL");
  //cout << "newRandomCommand: cmd->getContent()->getString(0) = \"" << cmd->getContent()->getString(0) << "\"" << endl;
  cmd->getContent()->addInt(1 + rand() % 3);
  cout << "Command is " << cmd->getContent()->getString(0) << "(" << cmd->getContent()->getInt(0) << ")" << endl;

  return cmd;
}
