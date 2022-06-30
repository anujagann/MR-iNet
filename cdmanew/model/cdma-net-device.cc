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
** Author(s):Anu Jagannath
** Email:  ajagannath@androcs.com
** File description: Cdma Net Device Class
** File: model/cdma-net-device.cc
*/
#include "ns3/trace-source-accessor.h"
#include "ns3/traced-callback.h"
#include "ns3/assert.h"
#include "ns3/channel.h"
#include "ns3/pointer.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/uinteger.h"
//#include "ns3/net-device.h"
#include "ns3/llc-snap-header.h"
#include "cdma-net-device.h"
#include "cdma-phy.h"
#include "cdma-mac.h"
#include "cdma-channel.h"
#include "ns3/mac48-address.h"
#include "cdma-mac-header.h"
#include <string>
namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CdmaNetDevice");

NS_OBJECT_ENSURE_REGISTERED (CdmaNetDevice);

CdmaNetDevice::CdmaNetDevice (): NetDevice ()
{
  NS_LOG_FUNCTION_NOARGS ();
    m_mtu = 1400;
    m_arp = true;

}

CdmaNetDevice::~CdmaNetDevice ()
{
  NS_LOG_FUNCTION_NOARGS ();
}
void
CdmaNetDevice::DoInitialize (void)
{
  m_phy->Initialize ();
  m_mac->Initialize ();
  m_channel->Initialize ();
  NetDevice::DoInitialize ();
}
void
CdmaNetDevice::DoDispose (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_node = 0;
  if (m_phy)
    {
      m_phy->Clear ();
      m_phy = 0;
    }

  NetDevice::DoDispose ();
}

void
CdmaNetDevice::Clear ()
{
  m_node = 0;
  if (m_mac)
    {
      m_mac->Clear ();
      m_mac = 0;
    }
  if (m_phy)
    {
      m_phy->Clear ();
      m_phy = 0;
    }
  if (m_channel)
    {
      m_channel->Clear ();
      m_channel = 0;
    }
}

/* Get the type ID. */
TypeId
CdmaNetDevice::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::CdmaNetDevice")
    .SetParent<NetDevice> ()
    .SetGroupName ("Cdma")
    .AddAttribute ("Channel", "The channel attached to this device.",
                   PointerValue (),
                   MakePointerAccessor (&CdmaNetDevice::GetChannel, &CdmaNetDevice::SetChannel),
                   MakePointerChecker<CdmaChannel> ())
    .AddAttribute ("Phy", 
                   "The PHY layer attached to this device.",
                   PointerValue (),
                   MakePointerAccessor (&CdmaNetDevice::GetPhy, &CdmaNetDevice::SetPhy),
                   MakePointerChecker<CdmaPhy> ())
     .AddAttribute ("Mac", "The MAC layer attached to this device.",
                   PointerValue (),
                   MakePointerAccessor (&CdmaNetDevice::GetMac, &CdmaNetDevice::SetMac),
                   MakePointerChecker<CdmaMac> ())
    .AddTraceSource ("Rx",
                     "Received payload from the MAC layer.",
                     MakeTraceSourceAccessor (&CdmaNetDevice::m_rxLogger),
                     "ns3::Packet::Mac48AddressTracedCallback")
    .AddTraceSource ("Tx",
                     "Send payload to the MAC layer.",
                     MakeTraceSourceAccessor (&CdmaNetDevice::m_txLogger),
                     "ns3::Packet::Mac48AddressTracedCallback")
  ;
  return tid;
}

/* Attach to phy layer*/
void
CdmaNetDevice::SetPhy (const Ptr<CdmaPhy> phy)
{
    if (phy != 0)
    {

      m_phy = phy;
      m_phy->SetDevice (Ptr<CdmaNetDevice> (this));
      NS_LOG_DEBUG ("Set PHY");
      if (m_mac != 0)
        {
          m_mac->AttachPhy (phy);
          m_phy->SetMac (m_mac);
          NS_LOG_DEBUG ("Attached PHY to MAC");
        }

    }
}

Ptr<CdmaPhy>
CdmaNetDevice::GetPhy (void) const
{
  return m_phy;
}

void
CdmaNetDevice::SetIfIndex (uint32_t index)
{
  m_ifIndex = index;
}
void
CdmaNetDevice::SetAddress (Address address)
{
  NS_ASSERT_MSG (NULL != m_mac, "Tried to set MAC address with no MAC");
  m_mac->SetAddress (Mac48Address::ConvertFrom (address));
}
Address
CdmaNetDevice::GetAddress () const
{
  return m_mac->GetAddress ();
}

/* Attach to mac layer*/
void
CdmaNetDevice::SetMac (Ptr<CdmaMac> mac)
{
  if (mac != 0)
    {
      m_mac = mac;
      NS_LOG_DEBUG ("Set MAC in Netdevice");

      if (m_phy != 0)
        {
          m_phy->SetMac (m_mac);
          m_mac->AttachPhy (m_phy);
          m_mac->SetDevice (this);
          NS_LOG_DEBUG ("Attached MAC to PHY in NetDevice");
        }
      m_mac->SetForwardUpCb (MakeCallback (&CdmaNetDevice::ForwardUp, this));
  
    }
}

  Ptr<CdmaMac>
  CdmaNetDevice::GetMac () const
  {
    return m_mac;
  }

/* Attach to channel*/
void
CdmaNetDevice::SetChannel (Ptr<CdmaChannel> channel)
{
  if (channel != 0)
    {
      m_channel = channel;
      NS_LOG_DEBUG ("Set CHANNEL");
      if (m_phy != 0)
        {
          m_channel->AddDevice (m_phy);
          m_phy->SetChannel (channel);
          NS_LOG_DEBUG ("Attach Channel to PHY");
        }
    }
}
bool
CdmaNetDevice::Send (Ptr<Packet> packet, const Address &dest, uint16_t protocolNumber)
{
  NS_LOG_FUNCTION (this << packet << dest << protocolNumber);
  Mac48Address realTo = Mac48Address::ConvertFrom (dest);
  LlcSnapHeader llc;
  llc.SetType (protocolNumber);
  packet->AddHeader (llc);
  m_mac->Enqueue (packet, realTo); // 
  return true;   
}

bool
CdmaNetDevice::SendFrom (Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber)
{
  
  return false;
}
void
CdmaNetDevice::SetPromiscReceiveCallback (PromiscReceiveCallback cb)
{
  // Not implemented yet
  NS_ASSERT_MSG (0, "Not yet implemented");
}
bool
CdmaNetDevice::SupportsSendFrom (void) const
{
  return false;
}

/*set MTU (Maximum Transmission Unit) value, in bytes, to set for the device*/
bool
CdmaNetDevice::SetMtu (uint16_t mtu)
{
  m_mtu = mtu;
  return true;
}

/* Called by higher-layers to check if this NetDevice requires ARP to be used.*/
bool
CdmaNetDevice::NeedsArp () const
{
  return m_arp;
}
/* return index ifIndex of the device */
uint32_t
CdmaNetDevice::GetIfIndex () const
{
  return m_ifIndex;
}
/* return the channel this NetDevice is connected to.*/
Ptr<Channel>
CdmaNetDevice::GetChannel () const
{
  return m_channel;
}

/* the link MTU in bytes for this interface. This value is typically used by the IP layer to perform
  IP fragmentation when needed. */
uint16_t
CdmaNetDevice::GetMtu () const
{
  return m_mtu;
}
/* return true if link is up; false otherwise */
bool
CdmaNetDevice::IsLinkUp () const
{
  return  (m_linkup && (m_phy != 0));
}

void
CdmaNetDevice::ForwardUp (Ptr<Packet> packet, Mac48Address src, Mac48Address dest)
{
  NS_LOG_FUNCTION (this << packet << src << dest);
  CdmaMacHeader llc; 
  packet->RemoveHeader (llc);
  LlcSnapHeader llc1;
  packet->RemoveHeader (llc1);
  enum NetDevice::PacketType type;
  if (dest.IsBroadcast ())
  {
    type = NetDevice::PACKET_BROADCAST;
  }
  else if (dest.IsGroup ())
  {
    type = NetDevice::PACKET_MULTICAST;
  }
  else if (dest == m_mac->GetAddress ())
  {
    type = NetDevice::PACKET_HOST;
  }
  else
  {
    type = NetDevice::PACKET_OTHERHOST;
  }

  if (type != NetDevice::PACKET_OTHERHOST)
  {
    m_rxLogger (packet, src);
    m_forwardUp (this, packet, llc1.GetType (), src);
  }
}

/* Return true if the net device is on a point-to-point link. */
bool
CdmaNetDevice::IsPointToPoint (void) const
{
  return false;
}
bool
CdmaNetDevice::IsBroadcast () const
{
  return true;
}
Address
CdmaNetDevice::GetBroadcast () const
{

  return  Mac48Address::GetBroadcast ();
}
bool
CdmaNetDevice::IsMulticast () const
{
  return true;
}

/* \return The MAC multicast Address used to send packets to the provided multicast group.*/
Address
CdmaNetDevice::GetMulticast (Ipv4Address multicastGroup) const
{
 // m_mac-> GetMulticast (Ipv4Address multicastGroup)
 // NS_FATAL_ERROR ("CdmaNetDevice does not support multicast");
  return Mac48Address::GetMulticast (multicastGroup);
}
/*return the MAC multicast address*/
Address
CdmaNetDevice::GetMulticast (Ipv6Address addr) const
{
  NS_FATAL_ERROR ("CdmaNetDevice does not support multicast");
  return m_mac->GetBroadcast ();
}

/*Return true if the net device is acting as a bridge.*/
bool
CdmaNetDevice::IsBridge (void) const
{
  return false;
}
/* callback to invoke whenever a packet has been received and must be forwarded to the higher layers.*/
void
CdmaNetDevice::SetReceiveCallback (NetDevice::ReceiveCallback cb)
{

  m_forwardUp = cb;
}
/*Add a callback invoked whenever the link status changes to UP. This callback is typically used by the IP/ARP layer
  to flush the ARP cache and by IPv6 stack to flush NDISC cache whenever the link goes up.*/
void
CdmaNetDevice::AddLinkChangeCallback (Callback<void> callback)
{
  m_linkChanges.ConnectWithoutContext (callback);
}
/* returns the node base class which contains this network interface.*/
Ptr<Node>
CdmaNetDevice::GetNode (void) const
{
  return m_node;
}

/** \param node the node associated to this netdevice. 
 This method is called from ns3::Node::AddDevice.*/
void
CdmaNetDevice::SetNode (const Ptr<Node> node)
{
  m_node = node;
  NS_LOG_FUNCTION (m_node->GetId ());
}


} //namespace ns3
