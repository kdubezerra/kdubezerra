/*
 * Main.cpp
 *
 *  Created on: 17/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#include <iostream>
#include <string>
#include <vector>
#include <cosmmus-msg.h>
#include "../headers/testobject.h"
#include "../headers/testobjfactory.h"
#include "../headers/testserver.h"
#include "../headers/testclient.h"

using namespace optpaxos;
using namespace netwrapper;
using namespace std;

//#define DESIRED_LENGTH 512000
//#define MSG_LENGTH DESIRED_LENGTH-32
#define RPORT0 45000
#define UPORT0 50000

testobject* o1;
testobject* o2;
testobject* o3;

Command* newRandomCommand();
void serverloop(testserver* server, int sid);
void clientloop(testclient* client, int cid, int comcount);

void printstates() {
  cout << endl << "obj1.optstate = " << o1->optState << endl;
  cout << "obj1.constate = " << o1->conState << endl << endl;
  cout << "obj2.optstate = " << o2->optState << endl;
  cout << "obj2.constate = " << o2->conState << endl << endl;
  cout << "obj3.optstate = " << o3->optState << endl;
  cout << "obj3.constate = " << o3->conState << endl << endl;
}

void helpnquit() {
  cout << "usage:\n   pxtest -s <SID> <NSERVERS>\nor pxtest -c <CID> <SID>[<NCOMMANDS>, default: -1 (infinite)]" << endl;
  exit(0);
}

int main(int argc, char* argv[]) {
/* <SERVER/CLIENT> ...
 *    <SERVER> <SID> <NSERVERS>
 *    <CLIENT> <CID> [<NCOMMANDS>, default: -1 (infinite)]
 */

  SDLNet_Init();

  if (argc < 3) helpnquit();
  vector<string> args;
  for (int i = 0 ; i < argc ; i++) args.push_back(argv[i]);

  testobjfactory* myfactory = new testobjfactory();
  Object::setObjectFactory(myfactory);

  o1 = new testobject(1);
  o2 = new testobject(2);
  o3 = new testobject(3);
  Object::indexObject(o1);
  Object::indexObject(o2);
  Object::indexObject(o3);

  if (args[1].compare("-s") == 0) {
    int serverid = atoi(args[2].c_str());
    int nservers = atoi(args[3].c_str());
    Group* grp = new Group(1);
    Group::addGroup(grp);
    grp->addManagedObject(o1->getInfo());
    grp->addManagedObject(o2->getInfo());
    grp->addManagedObject(o3->getInfo());
    for (int i = 0 ; i < nservers ; i++) {
      NodeInfo* sv = new NodeInfo(i+1, SERVER_NODE, new Address("localhost", UPORT0 + i + 1));
      grp->addServer(sv);
      if (i == 0) grp->setCoordinator(sv);
    }
    testserver* server = new testserver();
    server->init(RPORT0 + serverid, UPORT0 + serverid);
    server->joinGroup(grp);
    serverloop(server, serverid);
  }
  else if (args[1].compare("-c") == 0) {
    int clientid = atoi(args[2].c_str());
    int serverid = atoi(args[3].c_str());
    int cmdcount = args.size() > 4 ? atoi(args[4].c_str()) : -1;
    testclient* client = new testclient();
    client->connect("localhost", RPORT0 + serverid);
    clientloop(client, clientid, cmdcount);
  }
  else helpnquit();

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

void serverloop(testserver* server, int cmdcount) {
  while(true) {
    server->checkAll();
    printstates();
    cout << "\nObject 1 delivery:\n" << flush;
    o1->printCmdLists();
    cout << "\nObject 2 delivery:\n" << flush;
    o2->printCmdLists();
    cout << "\nObject 3 delivery:\n" << flush;
    o3->printCmdLists();
  }
}

void clientloop(testclient* client, int clientid, int cmdcount) {
  Command* cmd;
  int cmdmax = 2 * cmdcount;
  long seq = 0;
  int rcvmsgs = 0;
  while (rcvmsgs < cmdmax) {
    if (cmdcount != 0) {
      //SDL_Delay(1000);
      cmd = newRandomCommand();
      if (cmd && cmdcount > 0) cmdcount--;
      if (cmd) client->sendCommand(cmd, seq++, clientid);
      if (cmd && !(seq % 1000)) cerr << "Command " << seq << " issued" << endl;
      if (cmd) delete cmd;
    }
    rcvmsgs += client->checkAll();
    cout << "rcvmsgs = " << rcvmsgs << endl;
  }
  printstates();
  cout << "\nObject 1 delivery:\n" << flush;
  o1->printCmdLists();
  cout << "\nObject 2 delivery:\n" << flush;
  o2->printCmdLists();
  cout << "\nObject 3 delivery:\n" << flush;
  o3->printCmdLists();
}

Command* newRandomCommand() {
  Command* cmd = new Command();
  unsigned char targsBitmap = 1 + rand() % 7;

  cout << endl;

  if (targsBitmap & (unsigned char) 0x1) {
    cmd->addTarget(o1->getInfo());
    cout << "    obj1 is a target" << endl;
  }

  if ((targsBitmap >> 1) & (unsigned char) 0x1) {
    cmd->addTarget(o2->getInfo());
    cout << "    obj2 is a target" << endl;
  }

  if ((targsBitmap >> 2) & (unsigned char) 0x1) {
    cmd->addTarget(o3->getInfo());
    cout << "    obj3 is a target" << endl;
  }

  if (cmd->getTargetList().size() == 0) {
    cout << "newRandomCommand: somehow, the command has no target" << endl;
    delete cmd;
    return NULL;
  }

  cmd->setContent(new Message());
  if (rand() % 2)
    cmd->getContent()->addString("ADD");
  else
    cmd->getContent()->addString("MUL");
  cmd->getContent()->addInt(1 + rand() % 3);
  cout << "Command is " << cmd->getContent()->getString(0) << "(" << cmd->getContent()->getInt(0) << ")" << endl;

  return cmd;
}
