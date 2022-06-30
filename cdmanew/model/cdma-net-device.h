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
** Author(s): Anu Jagannath
** Email:  ajagannath@androcs.com
** File description: Cdma Net Device Class header
** File: model/cdma-net-device.h
*/
#ifndef CDMA_NET_DEVICE_H
#define CDMA_NET_DEVICE_H

#include "ns3/net-device.h"
#include "ns3/pointer.h"
#include "ns3/traced-callback.h"
#include "ns3/node.h"
#include "ns3/node-container.h"
#include "ns3/mac48-address.h"
#include "vector"

namespace ns3 {
class CdmaPhy;
class CdmaMac;
class CdmaChannel;

class CdmaNetDevice : public NetDevice{
public:

  static TypeId GetTypeId (void);

  CdmaNetDevice ();
  virtual ~CdmaNetDevice ();
  virtual void SetIfIndex (const uint32_t index);
  virtual void SetMac (Ptr<CdmaMac> mac);
  virtual void SetPhy (const Ptr<CdmaPhy> phy);
  virtual void SetChannel (Ptr<CdmaChannel> channel);
  virtual Ptr<CdmaMac> GetMac (void) const;
  virtual Ptr<CdmaPhy> GetPhy (void) const;
  virtual Ptr<Node> GetNode (void) const;
  virtual void SetNode (const Ptr<Node> node);
  virtual bool SetMtu (uint16_t mtu);
  virtual bool NeedsArp () const;
  virtual uint32_t GetIfIndex () const;
  virtual Ptr<Channel> GetChannel () const;
  virtual uint16_t GetMtu () const;
  virtual bool IsLinkUp () const;
  virtual void AddLinkChangeCallback (Callback<void> callback);
  virtual void SetReceiveCallback (NetDevice::ReceiveCallback cb);
  virtual bool IsBridge (void) const;
  virtual bool IsPointToPoint (void) const;
  virtual Address GetMulticast (Ipv6Address addr) const;
  virtual Address GetMulticast (Ipv4Address multicastGroup) const;
  virtual bool IsMulticast () const;
  virtual void SetAddress (Address address);
  virtual Address GetAddress (void) const;
  virtual bool IsBroadcast (void) const;
  virtual Address GetBroadcast (void) const;
  virtual void SetPromiscReceiveCallback (PromiscReceiveCallback cb);
  virtual bool SupportsSendFrom (void) const;
  virtual bool Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber);
  virtual bool SendFrom (Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber);
  void Clear (void);
  uint32_t m_ifIndex;

protected:
  virtual void DoDispose (void);
  virtual void DoInitialize (void);

private:
  virtual void ForwardUp (Ptr<Packet> packet, Mac48Address src, Mac48Address dest);
  Ptr<Node> m_node; //!< the node
  Ptr<CdmaPhy> m_phy; //!< the phy
  Ptr<CdmaMac> m_mac;
  Ptr<CdmaChannel> m_channel;

  uint16_t m_mtu;
  bool m_linkup;
  TracedCallback<> m_linkChanges;
  NetDevice::ReceiveCallback m_forwardUp;

  TracedCallback<Ptr<const Packet>, Mac48Address> m_rxLogger;
  TracedCallback<Ptr<const Packet>, Mac48Address> m_txLogger;

  bool m_arp;
};

} //namespace ns3

#endif /* CDMA_NET_DEVICE_H */
