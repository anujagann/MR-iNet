/* ------------------------------------------------------------------------------
* Copyright 2022 ANDRO Computational Solutions, LLC.  All Rights Reserved
 ------------------------------------------------------------------------------
*/
/* ------------------------------------------------------------------------------
This file is part of the MR-iNET software codebase released for public use under 
the [Creative Commons Attribution - NonCommercial - ShareAlike 4.0 (CC BY-NC-SA 4.0) License]
(https://creativecommons.org/licenses/by-nc-sa/4.0/).

This material is based upon work supported by the US Army Contract No. W15P7T-20-C-0006.
Any opinions, findings, and conclusions or recommendation expressed in this material 
are those of the author(s) and do not necessarily reflect the views of the US Army.
* ------------------------------------------------------------------------------
*/
/*
* I-ROAM
** Author(s): Kian Hamedani
** Email:  khamedani@androcs.com
** File: model/cdmanew-helper.h
*/

#ifndef CDMANEW_HELPER_H
#define CDMANEW_HELPER_H

#include <string>
#include "ns3/attribute.h"
#include "ns3/object-factory.h"
#include "ns3/node-container.h"
#include "ns3/net-device-container.h"
#include "ns3/cdma-net-device.h"

namespace ns3 {

class CdmaChannel;
class CdmaNetDevice;


class CdmaHelper
{
public:
  CdmaHelper ();           //!< Default constructor.
  virtual ~CdmaHelper ();  //!< Destructor.

  /**
   * Set MAC attributes.
   *
   * \param type The type of ns3::CdmaMac to create.
   * \param n0 The name of the attribute to set.
   * \param v0 The value of the attribute to set.
   * \param n1 The name of the attribute to set.
   * \param v1 The value of the attribute to set.
   * \param n2 The name of the attribute to set.
   * \param v2 The value of the attribute to set.
   * \param n3 The name of the attribute to set.
   * \param v3 The value of the attribute to set.
   * \param n4 The name of the attribute to set.
   * \param v4 The value of the attribute to set.
   * \param n5 The name of the attribute to set.
   * \param v5 The value of the attribute to set.
   * \param n6 The name of the attribute to set.
   * \param v6 The value of the attribute to set.
   * \param n7 The name of the attribute to set.
   * \param v7 The value of the attribute to set.
   *
   * All the attributes specified in this method should exist
   * in the requested mac.
   */
  void SetMac (std::string type,
               std::string n0 = "", const AttributeValue &v0 = EmptyAttributeValue (),
               std::string n1 = "", const AttributeValue &v1 = EmptyAttributeValue (),
               std::string n2 = "", const AttributeValue &v2 = EmptyAttributeValue (),
               std::string n3 = "", const AttributeValue &v3 = EmptyAttributeValue (),
               std::string n4 = "", const AttributeValue &v4 = EmptyAttributeValue (),
               std::string n5 = "", const AttributeValue &v5 = EmptyAttributeValue (),
               std::string n6 = "", const AttributeValue &v6 = EmptyAttributeValue (),
               std::string n7 = "", const AttributeValue &v7 = EmptyAttributeValue ());


  /**
   * Set PHY attributes.
   *
   * \param phyType The type of ns3::CdmaPhy to create.
   * \param n0 The name of the attribute to set.
   * \param v0 The value of the attribute to set.
   * \param n1 The name of the attribute to set.
   * \param v1 The value of the attribute to set.
   * \param n2 The name of the attribute to set.
   * \param v2 The value of the attribute to set.
   * \param n3 The name of the attribute to set.
   * \param v3 The value of the attribute to set.
   * \param n4 The name of the attribute to set.
   * \param v4 The value of the attribute to set.
   * \param n5 The name of the attribute to set.
   * \param v5 The value of the attribute to set.
   * \param n6 The name of the attribute to set.
   * \param v6 The value of the attribute to set.
   * \param n7 The name of the attribute to set.
   * \param v7 The value of the attribute to set.
   *
   * All the attributes specified in this method should exist
   * in the requested Phy.
   */
  void SetPhy (std::string phyType,
               std::string n0 = "", const AttributeValue &v0 = EmptyAttributeValue (),
               std::string n1 = "", const AttributeValue &v1 = EmptyAttributeValue (),
               std::string n2 = "", const AttributeValue &v2 = EmptyAttributeValue (),
               std::string n3 = "", const AttributeValue &v3 = EmptyAttributeValue (),
               std::string n4 = "", const AttributeValue &v4 = EmptyAttributeValue (),
               std::string n5 = "", const AttributeValue &v5 = EmptyAttributeValue (),
               std::string n6 = "", const AttributeValue &v6 = EmptyAttributeValue (),
               std::string n7 = "", const AttributeValue &v7 = EmptyAttributeValue ()
);


  /**
   * Set the transducer attributes.
   *
   * \param type The type of ns3::Transducer to create.
   * \param n0 The name of the attribute to set.
   * \param v0 The value of the attribute to set.
   * \param n1 The name of the attribute to set.
   * \param v1 The value of the attribute to set.
   * \param n2 The name of the attribute to set.
   * \param v2 The value of the attribute to set.
   * \param n3 The name of the attribute to set.
   * \param v3 The value of the attribute to set.
   * \param n4 The name of the attribute to set.
   * \param v4 The value of the attribute to set.
   * \param n5 The name of the attribute to set.
   * \param v5 The value of the attribute to set.
   * \param n6 The name of the attribute to set.
   * \param v6 The value of the attribute to set.
   * \param n7 The name of the attribute to set.
   * \param v7 The value of the attribute to set.
   *
   * All the attributes specified in this method should exist
   * in the requested transducer.
   */


  NetDeviceContainer Install (NodeContainer c) const;
  
  /**
   * For each of the input nodes, a new ns3::CdmaNetDevice is attached
   * to the shared input channel. Each ns3::CdmaNetDevice is also
   * configured with an ns3::CdmaTransducerHd, a ns3::CdmaMac, and ns3::CdmaPhy,
   * all of which are created based on the user-specified attributes
   * CdmaHelper::SetMac, and
   * CdmaHelper::SetPhy.
   *
   * \param c A set of nodes.
   * \param channel A channel to use.
   * \return The installed netdevices.
   */
  NetDeviceContainer Install (NodeContainer c, Ptr<CdmaChannel> channel) const;

  /**
   * Create a default an stack.
   *
   * Channel, physical layer,and mac layer are added to the
   * CdmaNetDevice and then added to the node.
   *
   * \param node A node where to install the cdma components.
   * \param channel A channel to use.
   * \return The installed CdmaNetDevice.
   */
  Ptr<CdmaNetDevice> Install (Ptr<Node> node, Ptr<CdmaChannel> channel) const;

  /**
  * Assign a fixed random variable stream number to the random variables
  * used by this model. Return the number of streams (possibly zero) that
  * have been assigned. The Install() method should have previously been
  * called by the user.
  *
  * \param c NetDeviceContainer of the set of net devices for which the 
  *          CdmaNetDevice should be modified to use a fixed stream.
  * \param stream First stream index to use.
  * \return The number of stream indices assigned by this helper.
  */

private:
  ObjectFactory m_device;      //!< The device.
  ObjectFactory m_mac;         //!< The MAC layer.
  ObjectFactory m_phy;         //!< The PHY layer.


};


} // end namespace ns3

#endif /* CDMANEW_HELPER_H */

