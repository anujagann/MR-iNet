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
** Author(s): Anu Jagannath, Collin Farquhar, Kian Hamedani
** Email:  ajagannath@androcs.com, cfarquhar@androcs.com, khamedani@androcs.com
** File description: Channel class is expected to be used in tandem with the CdmaPhy
** File: model/cdma-channel.cc
*/
#include "ns3/simulator.h"
#include "ns3/log.h"
#include "ns3/pointer.h"
#include "ns3/double.h"
#include "ns3/integer.h"
#include "ns3/mobility-model.h"
#include "ns3/node.h"
#include "cdma-channel.h"
#include "cdma-phy.h"
#include "ns3/packet.h"
#include "ns3/net-device.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/propagation-delay-model.h"
#include <fstream>
#include <iostream>
#include "cdma-phy-derived.h"
#include "cdma-mac-header.h"


NS_LOG_COMPONENT_DEFINE ("CdmaChannel");
namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (CdmaChannel);
/****************************************************************
 *       The actual CdmaChannel class
 ****************************************************************/

TypeId
CdmaChannel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::CdmaChannel")
    .SetParent<Channel> ()
    .SetGroupName ("Cdma")
    .AddAttribute ("PropagationLossModel", "A pointer to the propagation loss model attached to this channel.",
                   PointerValue (),
                   MakePointerAccessor (&CdmaChannel::m_loss),
                   MakePointerChecker<PropagationLossModel> ())
    .AddAttribute ("PropagationDelayModel", "A pointer to the propagation delay model attached to this channel.",
                   PointerValue (),
                   MakePointerAccessor (&CdmaChannel::m_delay),
                   MakePointerChecker<PropagationDelayModel> ())
  ;
  return tid;
}

CdmaChannel::CdmaChannel ()
{
  //NS_LOG_FUNCTION (this);
}
CdmaChannel::~CdmaChannel ()
{
  //NS_LOG_FUNCTION (this);
}

void
CdmaChannel::Clear ()
{
  // Not implmented
  //m_devList.clear ();
}

/*Set propagation loss model*/
void 
CdmaChannel::SetPropagationLossModel(ns3::Ptr<ns3::PropagationLossModel> loss){
	//NS_LOG_FUNCTION(this<<loss);
	this->m_loss = loss;
}

/*Get prop loss*/
ns3::Ptr<ns3::PropagationLossModel> 
CdmaChannel::GetPropagationLossModel(){
	//NS_LOG_FUNCTION(this);
	return this->m_loss;
}

/*Set prop delay*/
void 
CdmaChannel::SetPropagationDelayModel(ns3::Ptr<ns3::PropagationDelayModel> delay){
	//NS_LOG_FUNCTION(this<<delay); 
	this->m_delay = delay;
}

ns3::Ptr<ns3::PropagationDelayModel> 
CdmaChannel::GetPropagationDelayModel(){
	//NS_LOG_FUNCTION(this);
	return this->m_delay;
}

std::size_t
CdmaChannel::GetNDevices (void) const
{
  return m_devList.size();
}

Ptr<NetDevice>
CdmaChannel::GetDevice (std::size_t i) const
{
  return m_devList[i]->GetDevice ()->GetObject<NetDevice> ();
}

/*
Identifies other nodes that transmitted between the receivers RX start time and the RX
end time (the current simulation time).
Returns a vector of tuples: [(CdmaPhy of interferer, TxPower), ...]
*/
// Original
std::vector<std::tuple<Ptr<CdmaPhy>, double>>
CdmaChannel::GetInterferingNodes(Ptr <CdmaPhy> receiver, Ptr <CdmaPhy> sender)
{
  std::vector<std::tuple<Ptr<CdmaPhy>, double>> interferingNodesAndTxPower;

  Time currentTime = Simulator::Now();
  Time rxStartTime = receiver->m_reception.back().rxStartTime;

  uint32_t sender_id = sender->GetDevice()->GetNode()->GetId(); // Id of sender (not an interferer)
  uint32_t receiver_id = receiver->GetDevice()->GetNode()->GetId(); 

  // For all nodes
  for (uint32_t nodeId = 0; nodeId < m_devList.size(); nodeId++)
  {
    // If the nodes is not the sender or receiver
    if (nodeId != sender_id && nodeId != receiver_id)
    {
      Ptr<CdmaPhy> potentialInterferer = m_devList.at(nodeId);
      // Only transmissions on the same frequency can be considered as interferers
      if (potentialInterferer->m_frequency_id == sender->m_frequency_id) 
      {
        auto it = potentialInterferer->m_transmission.rbegin(); // iterate backwards

        // For all transmissions of the node (in reverse chronological order)
        for (; it != potentialInterferer->m_transmission.rend(); ++it)
        {
          Time txStartTime = it->txStartTime;
          Time txEndTime = it->txEndTime;

          // If the node has a transmission schedule for the future don't count it yet. Continue.
          if (txStartTime >= currentTime)
          {
            continue;
          }
          // If the node transmitted while the receiver was receiving, it is an interferer.
          else if (txEndTime > rxStartTime)
          {
            double interfererTxPower = potentialInterferer->m_transmission.back().txPower; 
            interferingNodesAndTxPower.push_back(std::make_tuple(potentialInterferer, interfererTxPower));
          }
          // The node has no other transmissions in the reception window.
          else
          {
            break;
          }
        }
      }
    }
  }

  return interferingNodesAndTxPower;
}

/* 
* Returns interference caused to the receiver by all other transmitting nodes. 
* Returns the sum of interferences in Watts.
*/
double
CdmaChannel::GetInterference (Ptr <CdmaPhy> receiver, Ptr <CdmaPhy> sender)
{
  std::vector<std::tuple<Ptr<CdmaPhy>, double>> interferingNodes = GetInterferingNodes(receiver, sender);
  auto interfererIterator = interferingNodes.begin();

  Ptr<MobilityModel> receiverMobility = receiver->GetMobility ()->GetObject<MobilityModel> ();

  double total_interference = 0.0; // Watts
  Ptr<CdmaPhy> interferer;
  double txPower;
  for (; interfererIterator != interferingNodes.end(); ++interfererIterator)
  {
    std::tie(interferer, txPower) = *interfererIterator;
    Ptr<MobilityModel> interfererMobility = interferer->GetMobility ()->GetObject<MobilityModel> ();
    total_interference += DbmToW(m_loss->CalcRxPower (txPower, interfererMobility, receiverMobility));
  }

  return total_interference; 
}

/*
* Returns a vector of interference caused by the sender to all other nodes other than the sender and intended receiver.
* The elements inferenceCaused vector corresponding to the sender and reciever are set to zero.
*
* Adds to the CdmaPhy member interference_sensed_sum for ns3-gym.
*/
std::vector<double>
CdmaChannel::GetInterferenceCaused (uint32_t sender_id, uint32_t receiver_id)
{ 
  std::vector<double> interferenceCaused;
  double interfCaused; 

  Ptr<MobilityModel> interefrerMobility = m_devList[sender_id]->GetMobility ();
  for (uint32_t i = 0; i < m_devList.size(); i++)
  {
    if (i != sender_id && i != receiver_id)
    {

      Ptr<MobilityModel> receiverMobility = m_devList[i]->GetMobility ();
      interfCaused = DbmToW(m_loss->CalcRxPower (m_devList[sender_id]->GetTxPower() , interefrerMobility, receiverMobility));
      interferenceCaused.push_back(interfCaused);
      m_devList[i]->interference_sensed_sum += interfCaused; // Update for ns3-gym
    }

    else
    {
      interferenceCaused.push_back(0);
    }
  }

  return interferenceCaused;
}

bool 
CdmaChannel::SendPacket (Ptr<Packet> txPkt, Ptr<CdmaPhy> sender, double txPowerDbm, Time txDuration) 
{
  // Establish the destination node ID
  Ptr<Packet> copypkt = txPkt->Copy ();
  CdmaMacHeader header;
  copypkt->PeekHeader(header);
  u_int32_t destNodeId;

  // Data transmission
  if (header.GetType() == 1)
  {
    uint8_t buf[6];
    header.GetDestination().CopyTo(buf);

    // If in "broadcast mode", default to old assumption that TX's communicate with RX's such that
    // TX node ID - 1 = RX node ID. 
    // Note, this is still unicast transmissions, and not really broadcast.
    if ((int)buf[5] == 255) 
    {
      destNodeId = sender->GetDevice ()->GetNode ()->GetId () - 1;
    }
    else // Get the RX node ID from the header
    {
      destNodeId = (int)buf[5] - 1; 
    }
  }
  // Ack transmission
  else if (header.GetType() == 0)
  {
    CdmaDeviceList::const_iterator i = m_devList.begin ();
    for (i = m_devList.begin (); i != m_devList.end (); i++)
    {
      if (sender != (*i))
      {
        if ((*i)->GetDevice()->GetMac()->GetAddress() == header.GetDestination())
        {
          destNodeId = (*i)->GetDevice()->GetNode()->GetId();
        }
      }
    }
  }

  Ptr<MobilityModel> senderMobility = sender->GetMobility ()->GetObject<MobilityModel> ();
  Ptr<MobilityModel> receiverMobility = m_devList[destNodeId]->GetMobility ()->GetObject<MobilityModel> ();
  Time delay = m_delay->GetDelay (senderMobility, receiverMobility);
 
  double rxPowerDbm = m_loss->CalcRxPower (txPowerDbm, senderMobility, receiverMobility); 

  Simulator::ScheduleWithContext (destNodeId, delay, &CdmaChannel::ReceivePacket, this, destNodeId,
                                  copypkt, txDuration, rxPowerDbm, sender);

  return true;
}

void
CdmaChannel::AddDevice (Ptr<CdmaPhy> phy)
{
  m_devList.push_back ( phy);
}

/* Receive Packet from channel*/
void
CdmaChannel::ReceivePacket (uint32_t RxNodeId, Ptr<Packet> rxPkt, Time txDuration, 
                            double rxPowerDbm, Ptr<CdmaPhy> sender)
{
  NS_LOG_FUNCTION ("CdmaChannel::ReceivePacket: " << "rxPkt " << rxPkt << 
                   "rxPowerDbm" << rxPowerDbm);
                        
  m_devList[RxNodeId]->ReceivePacket (rxPkt, txDuration, rxPowerDbm, sender);
  Simulator::Schedule (txDuration, &CdmaChannel::ReceivePacketDone, this, RxNodeId, rxPkt, rxPowerDbm, sender);
}

/* Receive Packet Done from channel*/
void
CdmaChannel::ReceivePacketDone (uint32_t RxNodeId, Ptr<Packet> rxPkt, double rxPowerDbm, Ptr<CdmaPhy> sender)
{
  NS_LOG_FUNCTION ("CdmaChannel: ReceivePacketDone");
  m_devList[RxNodeId]->ReceivePacketDone (rxPkt, rxPowerDbm, sender);
}

/*Dbm to W*/
double
CdmaChannel::DbmToW (double dbm)
{
  double mw = pow (10.0,dbm / 10.0);
  return mw / 1000.0;
}

}
