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
** File description: MAC layer of CDMA module
** File: model/cdma-mac.cc
*/
#include "ns3/simulator.h"
#include "ns3/attribute.h"
#include "ns3/uinteger.h"
#include "ns3/boolean.h"
#include "ns3/double.h"
#include "ns3/nstime.h"
#include "ns3/random-variable-stream.h"
#include "ns3/log.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/rng-seed-manager.h"
#include "cdma-mac.h"
#include "cdma-mac-header.h"
#include "cdma-phy.h"
#include "cdma-net-device.h"
#include "ns3/node.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

NS_LOG_COMPONENT_DEFINE ("CdmaMac");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (CdmaMac);

CdmaMac::CdmaMac ()
:m_pktDataTx(0),
  m_phy(0)
{
  m_throughputAll = 0;
  m_sequence = 0;
  m_buffer = 0;
  m_n_acks_received = 0;
  m_max_buffer = 100;
  m_guard = NanoSeconds(1e5);

  NS_LOG_FUNCTION (this);
}

CdmaMac::~CdmaMac ()
{
  //NS_LOG_FUNCTION (this);
}

TypeId
CdmaMac::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::CdmaMac")
    .SetParent<Object> ()
    .SetGroupName ("Cdma")
    .AddConstructor<CdmaMac> ()
    .AddAttribute ("QueueLimit",
                   "Maximum packets to queue at MAC",
                   UintegerValue (10000),
                   MakeUintegerAccessor (&CdmaMac::m_queueLimit),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("FrameLength",
                   "Actual packet length at the MAC layer",
                   UintegerValue (5),
                   MakeUintegerAccessor (&CdmaMac::m_FrameLength),
                   MakeUintegerChecker<uint16_t> ())
    .AddTraceSource ("SendDataDone",
                     "Trace Hookup for sending a data",
                     MakeTraceSourceAccessor (&CdmaMac::m_traceSendDataDone),
                     "ns3::CdmaMac::SendDataDoneTracedCallback")
    .AddTraceSource ("Enqueue",
                     "Trace Hookup for enqueue a data",
                     MakeTraceSourceAccessor (&CdmaMac::m_traceEnqueue),
                     "ns3::CdmaMac::TimeTracedCallback")
    .AddTraceSource ("Throughput",
                     "Trace Hookup for Throughput",
                     MakeTraceSourceAccessor (&CdmaMac::m_traceThroughput),
                     "ns3::CdmaMac::ThroughputTracedCallback")
  ;
  return tid;
}
// ------------------------ Set Functions -----------------------------
  void
  CdmaMac::Clear ()
  {
      m_pktQueue.clear ();
      m_seqList.clear ();
      m_throughput = 0;
      m_throughputAll = 0;
      m_addr = 0;

  }
  // get broadcast addr
  Mac48Address
  CdmaMac::GetBroadcast (void) const
  {
    return Mac48Address::GetBroadcast ();
  }

bool
CdmaMac::Enqueue (Ptr<Packet> packet, Mac48Address dest)
{
  NS_LOG_DEBUG ("---------------------------------------------------------------------------------------------------");
  NS_LOG_FUNCTION ("Time: " << Simulator::Now () << " at node: " << m_addr << "queue size" << m_pktQueue.size () << " dest:" << dest);

  
  if (packet->GetSize () == 64)
    {
      return true;
    }

  if (m_pktQueue.size () >= m_queueLimit)
    {
      return false;
    }


  m_traceEnqueue (m_device->GetNode ()->GetId (), m_device->GetIfIndex ());
  ++m_sequence;
  NS_LOG_DEBUG ("enqueued seq: " << m_sequence);
  CdmaMacHeader header = CdmaMacHeader (m_addr, dest, CDMA_PKT_TYPE_DATA, 0); // dest_id change
  header.SetSequence (m_sequence);
  packet->AddHeader (header);
  m_pktQueue.push_back (packet);

  PktTx ota; //to keep track of tstart and retries for each packet
  ota.retry = 0;
  ota.destination = dest;
  ota.packet = packet;
  ota.sequence = m_sequence;
  ota.tstart = Simulator::Now ();

  m_pktTx.push_back (ota);
  TransmitFirstPacket ();
  return true;
}

  //
  void
  CdmaMac::SetForwardUpCb (Callback<void, Ptr<Packet>, Mac48Address, Mac48Address> cb)
  {

    m_forwardUpCb = cb;
  }
  // transmit first packet
  void
  CdmaMac::TransmitFirstPacket ()
  {

    NS_LOG_DEBUG ("Time: " << Simulator::Now () << " at node: " << m_addr << " queue size: " << m_pktQueue.size ());
    
    if (m_pktQueue.empty ())
      {
        return;
      }
    m_pktDataTx = m_pktQueue.front ();
    SendData(m_pktDataTx);
  }

  // send data
  void
  CdmaMac::SendData (Ptr<Packet> packet)
  {
    NS_LOG_FUNCTION ("CdmaMac::SendData");

    // Set appropriate tx power
    m_phy->SetTxPower(m_phy->GetDataTxPower());

    CdmaMacHeader header;
    packet->RemoveHeader (header);
    NS_LOG_FUNCTION ( "Time: " << Simulator::Now () << " at node: " << m_addr << " to: " << header.GetDestination ());
    
    if (header.GetDestination () == GetBroadcast ()) // Broadcast
      {
        header.SetDuration (Seconds (0));
        packet->AddHeader (header);
        SendPacket (packet);

        return;
      }

    if (header.GetDestination () != GetBroadcast ()) // Unicast
      {
        header.SetDuration (Seconds (0));
        packet->AddHeader (header);
        SendPacket (packet); 
        return;
      }
    NS_LOG_FUNCTION ("# dest" << header.GetDestination () << "seq" << m_sequence << "q-size" << m_pktQueue.size ());
  }
// ack timeout
  void
  CdmaMac::AckTimeout (uint16_t sequence)
  {

   std::cout << "AckTimeout \n";

    NS_LOG_DEBUG ("---------------------------------------------------------------------------------------------------");
    NS_LOG_FUNCTION ("now" << Simulator::Now ());
    NS_LOG_DEBUG ("!!! ACK timeout !!! for packet: " << sequence << " at node: " << m_device->GetNode ()->GetId ());
    m_traceAckTimeout (m_device->GetNode ()->GetId (), m_device->GetIfIndex ());

    std::list<AckTimeouts>::iterator ait = m_ackTimeouts.begin ();
    for (; ait != m_ackTimeouts.end (); )
      {
        if (ait->sequence == sequence)
          {
            ait = m_ackTimeouts.erase (ait);
            break;
          }
        else
          {
            ++ait;
          }
      }

    std::list<PktTx>::iterator it = m_pktTx.begin ();
    for (; it != m_pktTx.end (); ++it)
      {
        if (it->sequence == sequence)
          {
            NS_LOG_DEBUG ("retry: " << it->retry << " for packet:" << it->sequence);
            it->retry = it->retry + 1;
            NS_LOG_DEBUG ("retry: " << it->retry);
            if (it->retry >= m_dataRetryLimit)
              {
                SendDataDone (false, it->packet);
                return;
              }
            return;
          }
      }
  }
 // get duration of ctrl packet
  Time
  CdmaMac::GetCtrlDuration (uint16_t type)
  {
    Mac48Address m_addr2 = Mac48Address::ConvertFrom (m_addr);   
    CdmaMacHeader header = CdmaMacHeader (m_addr2, m_addr2, type, 0); // dest_id change
    return m_phy->GetPktDuration (header.GetSize ());
  }
 // get duration of data packet
  Time
  CdmaMac::GetDataDuration (Ptr<Packet> p)
  {
    return m_phy->GetPktDuration (p->GetSize ());
  }

 // attach phy to the mac
  void
  CdmaMac::AttachPhy (Ptr<CdmaPhy> phy)
  {
    m_phy = phy;
  }
  // set mac addr
  void
  CdmaMac::SetAddress (Mac48Address addr)
  {
   this->m_addr = addr;
  }
  // set net device 
  void
  CdmaMac::SetDevice (Ptr<CdmaNetDevice> dev)
  {
    this->m_device = dev;
  }
  Ptr<CdmaNetDevice>
  CdmaMac::GetDevice (void) const 
  {
     return this->m_device;
  }
  // Get mac addr
  Mac48Address
  CdmaMac::GetAddress () const
  {
    return this->m_addr;
  }

 // Receive packet
  void
  CdmaMac::ReceivePacket (Ptr<CdmaPhy> phy, Ptr<Packet> packet)
  {
    NS_LOG_FUNCTION ("at node: " << this->m_device->GetNode ()->GetId ());
  }

  /*
  Calls mac->ReceiveData. The call to this function can be scheduled with a delay, where
  the delay acts as the guard. 
  This function is outside the CdmaMac class to avoid issues of using Simulator::Schedule to call private
  member function.
  */
  void OnGuard(Ptr<CdmaMac> mac, Ptr<Packet> packet)
  {
    mac->ReceiveData (packet); 
  }

  // Receive pkt done
  void
  CdmaMac::ReceivePacketDone (Ptr<CdmaPhy> phy, Ptr<Packet> packet, Ptr<CdmaPhy> sender)
  {

    CdmaMacHeader header;
    packet->PeekHeader (header);
    switch (header.GetType ())
      {
      case CDMA_PKT_TYPE_DATA:
        Simulator::Schedule(m_guard, &OnGuard, this, packet);

        break;
      case CDMA_PKT_TYPE_ACK:
        ReceiveAck (packet);
        break;
       // }
      default:
        break;
      }
  }
  // receive data
  void
  CdmaMac::ReceiveData (Ptr<Packet> packet)
  {
    NS_LOG_DEBUG ("---------------------------------------------------------------------------------------------------");
    NS_LOG_FUNCTION ("Time: " << Simulator::Now () << " at node: " << m_addr);
    CdmaMacHeader header;
    packet->PeekHeader (header);
    
    m_forwardUpCb (packet, header.GetSource (), header.GetDestination ());
    SendAck(header.GetDestination (), header.GetSource(), 0);
    
    return;
  }

 // send ack
  void
  CdmaMac::SendAck (Mac48Address source, Mac48Address dest, uint16_t sequence)
  {
    NS_LOG_FUNCTION ("CdmaMac::SendAck");
    NS_LOG_DEBUG ("Time: " << Simulator::Now () << " at node: " << m_addr << " to: " << dest);
  
    Ptr<Packet> packet = Create<Packet> ();
    CdmaMacHeader ackHeader = CdmaMacHeader (source, dest, CDMA_PKT_TYPE_ACK, 0); // dest_id
    ackHeader.SetDuration (Seconds (0));
    ackHeader.SetSequence (sequence);
    packet->AddHeader (ackHeader);

    // Set appropriate tx power
    m_phy->SetTxPower(m_phy->GetAckTxPower());

    bool sent = SendPacket (packet);
    NS_LOG_FUNCTION ("Sent: " << sent);
  }

  // send packet
  bool
  CdmaMac::SendPacket (Ptr<Packet> packet)
  {
    NS_LOG_FUNCTION ("CdmaMac::SendPacket");
    CdmaMacHeader header;
    packet->PeekHeader (header);
    NS_LOG_FUNCTION ("Time: " << Simulator::Now () << " at node: " << m_addr << " to: " << header.GetDestination ());

    NS_LOG_INFO ("sequence" << header.GetSequence ());

    if (m_phy->SendPacket (packet))
    {
      SendPacketDone (packet);
      NS_LOG_DEBUG ("packet header is : " << header );
      return true;
    }

    return false;
  }
 // send pkt done
  void
  CdmaMac::SendPacketDone (Ptr<Packet> packet)
  {
    std::list<PktTx>::iterator it = m_pktTx.begin ();
    for (; it != m_pktTx.end (); ++it)
      {
        if (it->packet == packet)
          {
            CdmaMacHeader header;
            packet->PeekHeader (header);
            NS_LOG_DEBUG ("data packet: " << header.GetSequence () << " dest: " << header.GetDestination () << " has been transmitted into channel");
          }
      }

     m_phy->SendPacketDone(packet);

    return;
  }
 // send data done
  void
  CdmaMac::SendDataDone (bool success, Ptr<Packet> packet)
  {
    // Putting Dequeue calls from the front of m_pktTx here for now
    Dequeue (m_pktTx.front().packet);
    m_pktTx.pop_front();

    CdmaMacHeader header;
    packet->PeekHeader (header);
    NS_LOG_FUNCTION ("        Time: " << Simulator::Now () << " at node: " << m_addr << " to: " << header.GetDestination ());
    std::list<PktTx>::iterator dit = m_pktTx.begin ();
    for (; dit != m_pktTx.end (); ++dit)
      {
        Ptr<Packet> data = dit->packet;
        CdmaMacHeader dataHeader;
        data->PeekHeader (dataHeader);
        NS_LOG_DEBUG ("data dest: " << dataHeader.GetDestination () << " acked pack dest: " << header.GetDestination ());
        if (dataHeader.GetSequence () > header.GetSequence () && dataHeader.GetDestination () == header.GetDestination ())
          {
            NS_LOG_DEBUG ("same destination");
            break;
          }
      }

    m_tend = Simulator::Now ();
    std::list<PktTx>::iterator it = m_pktTx.begin ();
    for (; it != m_pktTx.end (); )
      {

        if (it->sequence == header.GetSequence ())
          {
            if (success)
              {
                NS_LOG_FUNCTION ("Success to transmit packet: " << it->sequence << "! at node: " << m_addr);
                m_traceSendDataDone (m_device->GetNode ()->GetId (), m_device->GetIfIndex (), true);
                m_timeRec = (m_tend - it->tstart);
                m_throughput = it->packet->GetSize () * 8 / m_timeRec.GetSeconds ();
                m_throughputAll += m_throughput;
                m_ite += 1;
                m_throughputavg = m_throughputAll / (m_ite);
                m_traceThroughput (m_throughputavg);
                NS_LOG_DEBUG (it->packet->GetSize () << " bytes successfully transmitted during " << m_timeRec.GetSeconds () << " Seconds");
                NS_LOG_DEBUG ("  throughput : " << m_throughput);
                NS_LOG_DEBUG ("  overall throughput : " << m_throughputAll);
                NS_LOG_DEBUG ("  average throughput : " << m_throughputavg);
                NS_LOG_UNCOND (" discarded packets: " << m_discarded << " successful packets: " << m_ite << " throughput: " << m_throughput << " average throughput: " << m_throughputavg << " at node: " << m_addr);
                /*----------------------------------------------------------------------------------------
                * enable the result printing in a .txt file by uncommenting the content below
                *----------------------------------------------------------------------------------------*/
                /*std::ofstream myfile;
                myfile.open ("nano_2way_sucessful.txt", std::ofstream::out | std::ios::app);
                myfile << m_device->GetNode ()->GetId () << "  " << m_timeRec.GetSeconds () << "   " << it->sequence << std::endl;
                myfile.close ();*/
                Dequeue (it->packet);
                it = m_pktTx.erase (it);
              }
            else
              {
                /*std::ofstream myfile;
                myfile.open ("nano_2way_discarded.txt", std::ofstream::out | std::ios::app);
                myfile << m_device->GetNode ()->GetId () << "  " << 1 << std::endl;
                myfile.close ();*/
                m_discarded += 1;
                NS_LOG_UNCOND (" discarded packets : " << m_discarded << "! at node: " << m_addr);
                NS_LOG_FUNCTION ("Fail to transmit packet: " << it->sequence << "! at node: " << m_device->GetNode ()->GetId ());
                m_traceSendDataDone (m_device->GetNode ()->GetId (), m_device->GetIfIndex (), false);
                m_pktQueue.remove (it->packet);
                it = m_pktTx.erase (it);
              }
          }
        else
          {
            ++it;
          }
      }

  }
 // receive ack
  void
  CdmaMac::ReceiveAck (Ptr<Packet> packet)
  {
    m_n_acks_received += 1;

    NS_LOG_DEBUG ("---------------------------------------------------------------------------------------------------");
    NS_LOG_FUNCTION ("Time: " << Simulator::Now () << " at node: " << m_addr);

    CdmaMacHeader header;
    packet->PeekHeader (header);
    //NS_LOG_INFO ("sequence" << header.GetSequence () << " at node: " << m_device->GetNode ()->GetId ());
    if (header.GetDestination () == m_addr || header.GetDestination () == Mac48Address::GetBroadcast ())
      {
        SendDataDone (false, packet);
        return;
      }
    else
      {
        NS_LOG_INFO ("ACK not for me");
        return;
      }
  }
 // check if its new pkt sequence
  bool
  CdmaMac::IsNewSequence (Mac48Address addr, uint16_t seq)
  {
    std::list<std::pair<Mac48Address, uint16_t> >::iterator it = m_seqList.begin ();
    for (; it != m_seqList.end (); ++it)
      {
        if (it->first == addr)
          {
            if (it->second == 65536 && seq < it->second)
              {
                it->second = seq;
                return true;
              }
            else if (seq > it->second)
              {
                it->second = seq;
                return true;
              }
            else
              {
                return false;
              }
          }
      }
    std::pair<Mac48Address, uint16_t> newEntry;
    newEntry.first = addr;
    newEntry.second = seq;
    m_seqList.push_back (newEntry);
    return true;
  }
// dequeue 
void
CdmaMac::Dequeue (Ptr<Packet> packet)
{
  //NS_LOG_FUNCTION (m_pktQueue.size ());
  m_pktQueue.remove (packet);
  m_buffer -= 1;

  if (m_buffer_queue.size() > 0) 
  {
    m_buffer_queue.pop_front();
  }
}

}
