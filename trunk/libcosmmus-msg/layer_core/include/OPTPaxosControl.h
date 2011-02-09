/*
 * OPTPaxosControl.h
 *
 *  Created on: 09/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef OPTPAXOSCONTROL_H_
#define OPTPAXOSCONTROL_H_

/*
 *
 */
namespace optpaxos {

class OPTPaxosControl {
  public:
    OPTPaxosControl();
    virtual ~OPTPaxosControl();

    static void setObjectFactory(ObjectFactory* _factory);

  protected:
    static ObjectFactory* objFactory;
};

}

#endif /* OPTPAXOSCONTROL_H_ */
