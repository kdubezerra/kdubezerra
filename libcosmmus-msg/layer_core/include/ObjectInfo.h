/*
 * ObjectInfo.h
 *
 *  Created on: 11/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef OBJECTINFO_H_
#define OBJECTINFO_H_

/*
 *
 */
namespace optpaxos {

class ObjectInfo {
  public:
    ObjectInfo();
    virtual ~ObjectInfo();

    int getId();
    void setId(int _id);

  private:
    int objectId;
};

}

#endif /* OBJECTINFO_H_ */
