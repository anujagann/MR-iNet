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
** File: model/cdmanew-helper.cc
*/
#include "cdmanew-helper.h"
#include "ns3/cdma-net-device.h"
#include "ns3/cdma-mac.h"
#include "ns3/cdma-phy.h"
#include "ns3/cdma-channel.h"
#include "ns3/cdma-mac-header.h"
#include "ns3/log.h"
#include "ns3/config.h"
#include "ns3/simulator.h"
#include "ns3/cdma-phy-derived.h"
#include <sstream>
#include <string>
namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CdmaHelper");



CdmaHelper::CdmaHelper ()
{
  m_mac.SetTypeId ("ns3::CdmaMac");
  m_phy.SetTypeId ("ns3::CdmaPhyDerived");

}

CdmaHelper::~CdmaHelper ()
{

}


void
CdmaHelper::SetMac (std::string macType,
                   std::string n0, const AttributeValue &v0,
                   std::string n1, const AttributeValue &v1,
                   std::string n2, const AttributeValue &v2,
                   std::string n3, const AttributeValue &v3,
                   std::string n4, const AttributeValue &v4,
                   std::string n5, const AttributeValue &v5,
                   std::string n6, const AttributeValue &v6,
                   std::string n7, const AttributeValue &v7)
{
  m_mac = ObjectFactory ();
  m_mac.SetTypeId (macType);
  m_mac.Set (n0, v0);
  m_mac.Set (n1, v1);
  m_mac.Set (n2, v2);
  m_mac.Set (n3, v3);
  m_mac.Set (n4, v4);
  m_mac.Set (n5, v5);
  m_mac.Set (n6, v6);
  m_mac.Set (n7, v7);
}

void
CdmaHelper::SetPhy (std::string phyType,
                   std::string n0, const AttributeValue &v0,
                   std::string n1, const AttributeValue &v1,
                   std::string n2, const AttributeValue &v2,
                   std::string n3, const AttributeValue &v3,
                   std::string n4, const AttributeValue &v4,
                   std::string n5, const AttributeValue &v5,
                   std::string n6, const AttributeValue &v6,
                   std::string n7, const AttributeValue &v7)
{
  m_phy = ObjectFactory ();
  m_phy.SetTypeId (phyType);
  m_phy.Set (n0, v0);
  m_phy.Set (n1, v1);
  m_phy.Set (n2, v2);
  m_phy.Set (n3, v3);
  m_phy.Set (n4, v4);
  m_phy.Set (n5, v5);
  m_phy.Set (n6, v6);
  m_phy.Set (n7, v7);

}


NetDeviceContainer
CdmaHelper::Install (NodeContainer c) const
{
  Ptr<CdmaChannel> channel = CreateObject<CdmaChannel> ();
  channel->SetPropagationLossModel (CreateObject<MatrixPropagationLossModel> ());
  channel->SetPropagationDelayModel (CreateObject <ConstantSpeedPropagationDelayModel> ());


  return Install (c, channel);
}

NetDeviceContainer
CdmaHelper::Install (NodeContainer c, Ptr<CdmaChannel> channel) const
{
  NetDeviceContainer devices;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); i++)
    {
      Ptr<Node> node = *i;

      Ptr<CdmaNetDevice> device = Install (node, channel);

      devices.Add (device);
      NS_LOG_DEBUG ("node=" << node << ", mob=" << node->GetObject<MobilityModel> ());
    }
  return devices;
}

Ptr<CdmaNetDevice>
CdmaHelper::Install (Ptr<Node> node, Ptr<CdmaChannel> channel) const
{
  Ptr<CdmaNetDevice> device = CreateObject<CdmaNetDevice> ();
  Ptr<CdmaMac> mac = m_mac.Create<CdmaMac> ();
  Ptr<CdmaPhy> phy = m_phy.Create<CdmaPhy> ();
   mac->SetAddress (Mac48Address::Allocate ());
   mac->SetDevice(device);


  device->SetMac (mac);

  device->SetPhy (phy);

  device->SetChannel (channel);


  node->AddDevice (device);

  return device;
}


} // end namespace ns3

