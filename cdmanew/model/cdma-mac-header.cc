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
** File description: MAC Header Class
** File: model/cdma-mac-header.cc
*/
#include "ns3/address-utils.h"
#include "cdma-mac-header.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("CdmaMacHeader");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (CdmaMacHeader);

CdmaMacHeader::CdmaMacHeader ()
{
}

CdmaMacHeader::CdmaMacHeader (const Mac48Address srcAddr, const Mac48Address dstAddr, uint8_t type, uint32_t dest_id)
  : Header (),
  m_srcAddr (srcAddr),
  m_dstAddr (dstAddr),
  m_type (type)
{
  m_dest_id = dest_id; // Added w/ CdmaMacHeader Deserialized changes 
  m_sequence = 0; // Added w/ CdmaMacHeader Deserialized changes. Testing sequence.

}

TypeId
CdmaMacHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::CdmaMacHeader")
    .SetParent<Header> ()
    .SetGroupName ("Cdma")
    .AddConstructor<CdmaMacHeader> ()
  ;
  return tid;
}

TypeId
CdmaMacHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

CdmaMacHeader::~CdmaMacHeader ()
{
}

/* set src addr*/
void
CdmaMacHeader::SetSource (Mac48Address addr)
{
  m_srcAddr = addr;
}
/*set dest addr*/
void
CdmaMacHeader::SetDestination (Mac48Address addr)
{
  m_dstAddr = addr;
}
/*set pkt type*/
void
CdmaMacHeader::SetType (uint8_t type)
{
  m_type = type;
}
/*Set duration and convert to us*/
void
CdmaMacHeader::SetDuration (Time duration)
{
  int64_t duration_us = duration.GetMicroSeconds ();
  m_duration = static_cast<uint16_t> (duration_us);
}
// set sequence number
void
CdmaMacHeader::SetSequence (uint16_t seq)
{
  m_sequence = seq;
}
// get src addr
Mac48Address
CdmaMacHeader::GetSource (void) const
{
  return m_srcAddr;
}
//get dst addr
Mac48Address
CdmaMacHeader::GetDestination (void) const
{
  return m_dstAddr;
}
// get pkt type
uint8_t
CdmaMacHeader::GetType (void) const
{
  return m_type;
}
// Get dest id
uint32_t
CdmaMacHeader::GetDestinationId (void) const
{
  return m_dest_id;
}

// get duration
Time
CdmaMacHeader::GetDuration (void) const
{
  return MicroSeconds (m_duration);
}
// get pkt size
uint32_t
CdmaMacHeader::GetSize (void) const
{
  // Note: This function only seems to work after initialization.
  // E.g., after the constructore for a CdmaMacHeader object has been called, it has size 21.
  // Before the constructor has been called, it has size 4. 
  uint32_t size = 0;
  switch (m_type)
    {
    case CDMA_PKT_TYPE_ACK:
      size = sizeof(m_type) + sizeof(m_duration) + sizeof(Mac48Address) * 2 + sizeof(m_sequence);
      break;
    case CDMA_PKT_TYPE_DATA:
      size = sizeof(m_type) + sizeof(m_duration) + sizeof(Mac48Address) * 2 + sizeof(m_sequence);
      break;
    }
  
  // with dest_id 
  size += sizeof(m_dest_id);

  return size; 
}

// inherited method
uint32_t
CdmaMacHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  
  // Original implemenation
  m_type = i.ReadU8 ();
  m_duration = i.ReadLsbtohU16 ();
  switch (m_type)
  {
    case CDMA_PKT_TYPE_ACK:
      ReadFrom (i, m_srcAddr);
      ReadFrom (i, m_dstAddr);
      m_sequence = i.ReadU16 ();
      m_dest_id = i.ReadU32 ();
      break;
    case CDMA_PKT_TYPE_DATA:
      ReadFrom (i, m_srcAddr);
      ReadFrom (i, m_dstAddr);
      m_sequence = i.ReadU16 ();
      m_dest_id = i.ReadU32 ();
      break;
  }

  return i.GetDistanceFrom (start);
}

// inherited method
void
CdmaMacHeader::Serialize (Buffer::Iterator i) const
{
  i.WriteU8 (m_type);
  i.WriteHtolsbU16 (m_duration);
  switch (m_type)
    {
    case CDMA_PKT_TYPE_ACK:
      WriteTo (i, m_srcAddr);
      WriteTo (i, m_dstAddr);
      i.WriteU16 (m_sequence);
      i.WriteU32 (m_dest_id);
      break;
    case CDMA_PKT_TYPE_DATA:
      WriteTo (i, m_srcAddr);
      WriteTo (i, m_dstAddr);
      i.WriteU16 (m_sequence);
      i.WriteU32 (m_dest_id);
      break;
    }
}
// inherited method
void
CdmaMacHeader::Print (std::ostream &os) const
{
  os << "CdmaMacHeader src=" << m_srcAddr << " dest=" << m_dstAddr << " type=" << (uint32_t) m_type << 
  " sequence=" << m_sequence << " dest_id=" << m_dest_id << "\n";
}
// get pkt sequence number
uint16_t
CdmaMacHeader::GetSequence (void) const
{
  return m_sequence;
}

// Inherrited method

uint32_t
CdmaMacHeader::GetSerializedSize (void) const
{
  return GetSize ();
}

void CdmaMacHeader::SetDestId(uint32_t id) // new!
{
  m_dest_id = id;
}

}
