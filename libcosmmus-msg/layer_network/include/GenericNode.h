/*
 * GenericNode.h
 *
 *  Created on: 11/02/2011
 *      Author: Carlos Eduardo B. Bezerra - carlos.bezerra@usi.ch
 */

#ifndef GENERICNODE_H_
#define GENERICNODE_H_

/*
 *
 */
namespace netwrapper {

class GenericNode {
  public:
    GenericNode();
    virtual ~GenericNode();

    Address* getAdress();
    void setAddress (Adress* address);

  private:
    Address* address;
};

}

#endif /* GENERICNODE_H_ */
