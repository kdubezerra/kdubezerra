#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <list>

#define STD_MSGS 1000
#define TIME_INTERVAL 100
#define MIN_DELAY 10
#define MAX_DELAY 500
#define WP 255
#define DISCARD false

using namespace std;

class Message {
  public:
    Message() {}
    
    Message(int _id, int _ts, int _arrival) {
      this->id = _id;
      this->ts = _ts;
      this->arrival = _arrival;
    }
    
    Message(Message* other) {
      this->id = other->id;
      this->ts = other->ts;
      this->arrival = other->arrival;
    }
    
    ~Message() {}
    
    int id;
    int ts;
    int arrival;
};

bool compareMsgsTS(Message* m1, Message* m2);
bool compareMsgsID(Message* m1, Message* m2);
bool compareMsgsARRIVAL(Message* m1, Message* m2);
bool step(list<Message*> &toarrive, list<Message*> &optpending, list<Message*> &delved, int &clock, int &last_delved);
int count_mistakes(list<Message*> &cd, list<Message*> &rd);
void printMsgList(list<Message*> &msglist);

int mistakes = 0;
int nmsgs = STD_MSGS;
int interval = TIME_INTERVAL;
int min_delay = MIN_DELAY;
int max_delay = MAX_DELAY;
int wp = WP;
bool discard = DISCARD;

int main(int argc, char** argv) {
  //cout << "time: " << time(NULL) << endl;
  srand(time(NULL));

  if (argc > 1) {
    nmsgs = atoi(argv[1]);
    interval = atoi(argv[2]);
    min_delay = atoi(argv[3]);
    max_delay = atoi(argv[4]);
    wp = atoi(argv[5]);
    discard = (bool) atoi(argv[6]);
  }
  
  list<Message*> coord, coord_optlist, coord_delved, replica, replica_optlist, replica_delved;
  
  //srand(time(0));
  
  for (int i = 0 ; i < nmsgs ; i++) {
    int newts = i * interval;
    int coordarr = (int) (((double) newts + (double) (min_delay + max_delay) * (double) ((rand() >> 8)%100)) / 100.0f);
    //cout << "coord arrival time for message " << i << " = " << coordarr << endl;
    int replicaarr = (int) (((double) newts + (double) (min_delay + max_delay) * (double) ((rand() >> 8)%100)) / 100.0f);
    //cout << "replica arrival time for message " << i << " = " << replicaarr << endl << endl;
    coord.push_back(new Message(i, newts, coordarr));
    replica.push_back(new Message(i, newts, replicaarr));
  }
  int coord_clock, replica_clock, c_ld, r_ld;
  coord_clock = replica_clock = 0;
  c_ld = r_ld = -1;
  coord.sort(compareMsgsARRIVAL);  
  replica.sort(compareMsgsARRIVAL);
  //printMsgList(coord);
  //printMsgList(replica);
  //cout << "Stepping coord" << endl;
  while(step(coord, coord_optlist, coord_delved, coord_clock, c_ld));
  //cout << "Stepping replica" << endl;
  while(step(replica, replica_optlist, replica_delved, replica_clock, r_ld));
  //cout << "Counting mistakes" << endl;
  //printMsgList(coord_delved);
  //printMsgList(replica_delved);
  int dropped = nmsgs - (int) coord_delved.size();
  cout << wp << " " << nmsgs << " " << count_mistakes(coord_delved, replica_delved) << " " << dropped << flush;
  cout << endl;
}



int count_mistakes(list<Message*> &cd, list<Message*> &rd) {
  int m = 0;
  while (!cd.empty()) {

    if (rd.empty()) {
      m += (int) cd.size();
      break;
    }    
    if (cd.front()->id != rd.front()->id) {
      m++;
      //cout << "error msg = (" << cd.front()->id << "," << cd.front()->ts << "," << cd.front()->arrival << ")" << endl;
      if (discard && cd.front()->ts > rd.front()->ts) {
        delete rd.front();
        rd.pop_front();
      }        
    }
    list<Message*>::iterator it;
    for (it = rd.begin() ; it != rd.end() && (*it)->id != cd.front()->id ; it++);
    if (it != rd.end()) rd.erase(it);
    delete cd.front();
    cd.pop_front();
  }
  return m;
}




bool step(list<Message*> &toarrive, list<Message*> &optpending, list<Message*> &delved, int &clock, int &last_delved) {
  bool mustcontinue = true;
  
  optpending.sort(compareMsgsTS);
  
  while (optpending.empty() == false && optpending.front()->ts < clock - wp) {
    last_delved = optpending.front()->ts;
    delved.push_back(new Message(optpending.front()));
    delete optpending.front();
    optpending.pop_front();
  }
  
  while (toarrive.empty() == false && toarrive.front()->arrival < clock) {
    if (!discard || toarrive.front()->ts >= last_delved)
      optpending.push_back(new Message(toarrive.front()));
    delete toarrive.front();
    toarrive.pop_front();
  }

  clock++;
  
  if (optpending.empty() && toarrive.empty())
    return false;
  else
    return true;
}

bool compareMsgsTS(Message* m1, Message* m2) {
  return m1->ts < m2->ts;
}

bool compareMsgsID(Message* m1, Message* m2) {
  return m1->id < m2->id;
}

bool compareMsgsARRIVAL(Message* m1, Message* m2) {
  return m1->arrival < m2->arrival;
}

void printMsgList(list<Message*> &msglist) {
  cout << endl;
  for (list<Message*>::iterator it = msglist.begin() ; it != msglist.end() ; it++) {
    cout << "message " << (*it)->id << " with ts " << (*it)->ts <<", arriving time " << (*it)->arrival << endl;
  }
  cout << endl;
}