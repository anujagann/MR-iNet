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
** File: model/cdma-phy.cc
*/

#include "cdma-phy.h"

namespace ns3 {
NS_OBJECT_ENSURE_REGISTERED (CdmaPhy);

TypeId CdmaPhy::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::CdmaPhy")
    .SetParent<Object> ()
    .SetGroupName ("Cdma")
    .AddTraceSource ("PhyTxBegin",
                     "Trace source indicating a packet has "
                     "begun transmitting over the channel medium.",
                     MakeTraceSourceAccessor (&CdmaPhy::m_phyTxBeginTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("PhyTxEnd",
                     "Trace source indicating a packet has "
                     "been completely transmitted over the channel.",
                   MakeTraceSourceAccessor (&CdmaPhy::m_phyTxEndTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("PhyTxDrop",
                     "Trace source indicating a packet has "
                     "been dropped by the device during transmission.",
                     MakeTraceSourceAccessor (&CdmaPhy::m_phyTxDropTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("PhyRxBegin",
                     "Trace source indicating a packet has "
                     "begun being received from the channel medium by the device.",
                     MakeTraceSourceAccessor (&CdmaPhy::m_phyRxBeginTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("PhyRxEnd",
                     "Trace source indicating a packet has "
                     "been completely received from the channel medium by the device.",
                     MakeTraceSourceAccessor (&CdmaPhy::m_phyRxEndTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("PhyRxDrop",
                     "Trace source indicating a packet has "
                     "been dropped by the device during reception.",
                     MakeTraceSourceAccessor (&CdmaPhy::m_phyRxDropTrace),
                     "ns3::Packet::TracedCallback")
  ;
  return tid;
}


void
CdmaPhy::NotifyTxBegin (Ptr<const Packet> packet)
{
  m_phyTxBeginTrace (packet);
}

void
CdmaPhy::NotifyTxEnd (Ptr<const Packet> packet)
{
  m_phyTxEndTrace (packet);
}

void
CdmaPhy::NotifyTxDrop (Ptr<const Packet> packet)
{
  m_phyTxDropTrace (packet);
}

void
CdmaPhy::NotifyRxBegin (Ptr<const Packet> packet)
{
  m_phyRxBeginTrace (packet);
}

void
CdmaPhy::NotifyRxEnd (Ptr<const Packet> packet)
{
  m_phyRxEndTrace (packet);
}

void
CdmaPhy::NotifyRxDrop (Ptr<const Packet> packet)
{
  m_phyRxDropTrace (packet);
}
} // namespace ns3
