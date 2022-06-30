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
** File description: MAC Header 
** File: model/cdma-mac-header.h
*/
#ifndef CDMA_MAC_HEADER_H
#define CDMA_MAC_HEADER_H

#include "ns3/header.h"
#include "ns3/nstime.h"
#include "ns3/simulator.h"
#include "ns3/mac48-address.h"

#define CDMA_PKT_TYPE_ACK   0
#define CDMA_PKT_TYPE_DATA  1

namespace ns3 {

class CdmaMacHeader : public Header
{
public:
  CdmaMacHeader ();

  CdmaMacHeader (const Mac48Address srcAddr, const Mac48Address dstAddr, uint8_t type, uint32_t dest_id);
  virtual ~CdmaMacHeader ();

  static TypeId GetTypeId (void);

  void SetSource (Mac48Address addr);

  void SetDestination (Mac48Address addr);

  void SetType (uint8_t type);

  void SetDuration (Time duration);

  void SetSequence (uint16_t seq);

  Mac48Address GetSource () const;

  Mac48Address GetDestination () const;

  uint8_t GetType () const;

  uint32_t GetDestinationId() const; // Added w/ CdmaMacHeader Deserialized changes 

  Time GetDuration () const;

  uint32_t GetSize () const;

  uint16_t GetSequence () const;

  // Inherrited methods
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;
  virtual TypeId GetInstanceTypeId (void) const;
  
  // Adding
  void SetDestId(uint32_t id);
  uint32_t m_dest_id; // Added w/ CdmaMacHeader Deserialized changes 

private:
  Mac48Address m_srcAddr;
  Mac48Address m_dstAddr;
  uint8_t m_type;
  uint16_t m_duration;
  uint16_t m_sequence;
};

}

#endif // CDMA_MAC_HEADER_H