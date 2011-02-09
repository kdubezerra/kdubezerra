/*
 * NodeInfo.h
 *
 *  Created on: 09/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef NODEINFO_H_
#define NODEINFO_H_

/*
 *
 */
namespace optpaxos {

class NodeInfo {
  public:
    NodeInfo();
    virtual ~NodeInfo();

    std::string getAddress() const;
    int getId() const;
    unsigned getPort() const;
    void setAddress(std::string _address);
    void setId(int _id);
    void setPort(unsigned  _port);

    static netwrapper::Message* packToNetwork(NodeInfo* _obj);
    static netwrapper::Message* packNodeListToNetwork(std::list<NodeInfo*> _objList);
    static NodeInfo* unpackFromNetwork(netwrapper::Message* _msg);
    static std::list<NodeInfo*> unpackNodeListFromNetwork(netwrapper::Message* _msg);

  private:
    int id;
    std::string address;
    unsigned port;
};

}


#endif /* NODEINFO_H_ */
